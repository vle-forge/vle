/**
 * @file Simulator.cpp
 * @author The VLE Development Team.
 * @brief Represent the DEVS Simulator class. This class provide a non
 * hierarchical DEVS simulator ie. all models are in the same coupled model.
 */

/*
 * Copyright (C) 2004, 05, 06 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <vle/devs/Simulator.hpp>
#include <vle/devs/Coordinator.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/devs/sAtomicModel.hpp>
#include <vle/devs/sCoupledModel.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/InternalEvent.hpp>
#include <vle/devs/ExternalEventList.hpp>
#include <vle/devs/StateEvent.hpp>
#include <vle/devs/ModelFactory.hpp>
#include <vle/devs/Observer.hpp>
#include <vle/devs/EventObserver.hpp>
#include <vle/devs/TimedObserver.hpp>
#include <vle/devs/Stream.hpp>
#include <vle/graph/Model.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Rand.hpp>
#include <vle/value/Value.hpp>
#include <libxml++/libxml++.h>
#include <glibmm/spawn.h>
#include <glibmm/timer.h>
#include <iostream>

using std::vector;
using std::map;
using std::string;
using std::pair;

using namespace xmlpp;

namespace vle { namespace devs {

Simulator::Simulator(long simulatorIndex, const vpz::Vpz& vp) :
    m_experiment(vp.project().experiment()),
    m_index(simulatorIndex),
    m_duration(0.0),
    m_currentTime(0),
    m_model(0)
{
    m_modelFactory = new ModelFactory(*this, vp.project().dynamics(),
                                      vp.project().classes());

    addModels(vp.project().model());
    parseExperiment();
}

Simulator::~Simulator()
{
    delete m_modelFactory;
    delete m_model;

    {
        std::map < graph::AtomicModel*, sAtomicModel* >::iterator it;
        for (it = m_modelList.begin(); it != m_modelList.end(); ++it) {
            delete it->second;
        }
    }

    {
        std::map < std::string , devs::Observer* >::iterator it;
        for (it = m_observerList.begin(); it != m_observerList.end(); ++it) {
            delete it->second;
        }
    }
}

void Simulator::addModels(const vpz::Model& model)
{
    graph::AtomicModelVector atomicmodellist;
    graph::Model* mdl = model.model();

    if (mdl->isAtomic()) {
        m_model = 0;
        atomicmodellist.push_back((graph::AtomicModel*)mdl);
    } else {
        m_model = new sCoupledModel((graph::CoupledModel*)mdl, this);
        graph::Model::getAtomicModelList(mdl, atomicmodellist);
    }

    m_modelFactory->createModels(atomicmodellist, m_modelList);
}

void Simulator::addCondition(const string& modelName,
                             const string& portName,
                             value::Value* value)
{
    m_conditionList[modelName].
        push_back(pair < string , value::Value* >(portName, value));
}

void Simulator::addObserver(Observer* p_observer)
{
    m_observerList[p_observer->getName()] = p_observer;
    if (p_observer->isTimed())
        m_timedObserverList.push_back((TimedObserver*)p_observer);
    else
        m_eventObserverList[p_observer->getFirstModel()->getName()].
            push_back((EventObserver*)p_observer);
}

graph::Model* Simulator::createModels(graph::CoupledModel* parent,
                                      const std::string& className,
                                      const std::string& xmlDynamics,
                                      const std::string& xmlInit)
{
    SAtomicModelList lst;

    graph::Model* top = m_modelFactory->createModels(className, lst,
                                                     m_modelList);
    parent->addModel(top);
    top->setParent(parent);

    if (not xmlDynamics.empty())
        applyDynamics(lst, xmlDynamics);

    for (SAtomicModelList::iterator it = lst.begin(); it != lst.end(); ++it) {
        dispatchInternalEvent((*it)->init());
    }

    if (not xmlInit.empty())
        applyInits(lst, xmlInit);

    xmlpp::Document doc;
    xmlpp::Element* root = doc.create_root_node("test");
    parent->writeXML(root);

    return top;
}

void Simulator::delAtomicModel(graph::CoupledModel* parent,
                               graph::AtomicModel* atom)
{
    if (parent and atom) {
        sAtomicModelMap::iterator it = m_modelList.find(atom);
        if (it != m_modelList.end()) {
            sAtomicModel* satom = (*it).second;
            m_modelList.erase(it);

            std::map < std::string , Observer* >::iterator it2;
            for (it2 = m_observerList.begin(); it2 != m_observerList.end();
                 ++it2) {
                Observer* observer = (*it2).second;
                observer->removeObservable(satom);
            }
            m_eventTable.delModelEvents(satom);
            delete satom;
        }
        parent->delAllConnection(atom);
        parent->delModel(atom);
    }
}

void Simulator::delCoupledModel(graph::CoupledModel* parent,
                                graph::CoupledModel* mdl)
{
    if (parent and mdl) {
        graph::VectorModel& lst = mdl->getModelList();
        for (graph::VectorModel::iterator it = lst.begin(); it != lst.end();
             ++it) {
            if ((*it)->isAtomic()) {
                delAtomicModel(mdl, (graph::AtomicModel*)(*it));
            } else if ((*it)->isCoupled()) {
                delCoupledModel(mdl, (graph::CoupledModel*)(*it));
            }
        }
        parent->delAllConnection(mdl);
        parent->delModel(mdl);
    }
}

bool Simulator::delModel(graph::CoupledModel* parent,
                         const std::string& modelname)
{
    graph::Model* mdl = parent->find(modelname);
    if (mdl) {
        if (mdl->isCoupled()) {
            delCoupledModel(parent, (graph::CoupledModel*)mdl);
        } else {
            delAtomicModel(parent, (graph::AtomicModel*)mdl);
        }
        return true;
    }
    return false;
}

void Simulator::finish()
{
    {
        sAtomicModelMap::iterator it;
        for (it = m_modelList.begin(); it != m_modelList.end(); ++it) {
            (*it).second->finish();
        }
    }

    {
        map < string , Observer* >::iterator it;
        for (it = m_observerList.begin(); it != m_observerList.end(); ++it) {
            (*it).second->finish();
        }
    }
}

void Simulator::init()
{
    utils::Rand::rand().set_seed(m_experiment.seed());

    std::list < vpz::Condition >::const_iterator it;
    for (it = m_experiment.conditions().conditions().begin();
         it != m_experiment.conditions().conditions().end(); ++it) {

        sAtomicModel* satom = getModel((*it).modelname());
        Assert(utils::InternalError, satom, boost::format(
                "Unkwnom condition model name '%1%' port '%2%'\n") %
                (*it).modelname() % (*it).portname());

        InitEvent* evt =
            new InitEvent(devs::Time(0), satom, (*it).portname());
        evt->putAttribute((*it).portname(), (*it).firstValue());
        satom->processInitEvent(evt);
        delete evt;
    }

    for (sAtomicModelMap::iterator satom = m_modelList.begin();
         satom != m_modelList.end(); ++satom) {
        dispatchInternalEvent((*satom).second->init());
    }

    std::map < std::string , devs::Observer* >::iterator it3 =
        m_observerList.begin();

    while (it3 != m_observerList.end()) {
        StateEventList v_eventList = it3->second->init();
        StateEventList::iterator it4 = v_eventList.begin();

        while (it4 != v_eventList.end()) {
            dispatchStateEvent(*it4);
            ++it4;
        }
        ++it3;
    }
}

void Simulator::dispatchExternalEvent(ExternalEventList* eventList,
                                      CompleteEventBagModel& bags)
{
    if (eventList) {
        const size_t sz = eventList->size();
        size_t i = 0;

        while (i < sz) {
            ExternalEvent* event = eventList->at(i);

            vector < graph::TargetPort* > v_targetPortList =
                getTargetPortList(event->getModel()->getStructure(),
                                  event->getPortName());

            vector < graph::TargetPort* >::iterator it2 =
                v_targetPortList.begin();

            while (it2 != v_targetPortList.end()) {
                graph::TargetPort *v_port = *it2;
                sAtomicModel* dst = m_modelList[v_port->getModel()];

                if (event->isInstantaneous()) {
                    bags.addInstantaneous(dst,
                        new InstantaneousEvent(event, dst,
                                               v_port->getPortName()));
                } else {
                    m_eventTable.putExternalEvent(
                        new ExternalEvent(event, dst, v_port->getPortName()));
                }
                delete *it2;
                ++it2;
            }
            ++i;
        }
        eventList->clear(true);
    }
}

void Simulator::dispatchInternalEvent(InternalEvent* event)
{
    if (event)
        m_eventTable.putInternalEvent(event);
}

void Simulator::dispatchStateEvent(StateEvent* event)
{
    if (event)
        m_eventTable.putStateEvent(event);
}

sAtomicModel* Simulator::getModel(graph::AtomicModel* model) const
{
    sAtomicModelMap::const_iterator it = m_modelList.find(model);
    return (it == m_modelList.end()) ? 0 : it->second;
}

sAtomicModel* Simulator::getModel(const std::string& name) const
{
    sAtomicModelMap::const_iterator it = m_modelList.begin();
    while (it != m_modelList.end()) {
        if ((*it).first->getName() == name) {
            return (*it).second;
        }
        ++it;
    }
    return 0;
}

const Time& Simulator::getNextTime()
{
    if (m_duration < m_currentTime.getValue())
        return Time::infinity;

    return m_eventTable.topEvent();
}

Observer* Simulator::getObserver(const string& p_name) const
{
    map < string , Observer* >::const_iterator it =
        m_observerList.find(p_name);

    return (it == m_observerList.end())?NULL:it->second;
}

vector < graph::TargetPort* > Simulator::getTargetPortList(graph::Port* port)
{
    if (m_model != NULL) {
        return m_model->getStructure()->getTargetPortList(port);
    } else {
        return vector < graph::TargetPort* > ();
    }
}

vector < graph::TargetPort* > Simulator::getTargetPortList(
                    graph::AtomicModel* model,
                    const string & portName)
{
    sAtomicModel* smodel = getModel(model);
    if (model) {
        graph::Port *port = smodel->getStructure()->getOutPort(portName);
        return smodel->getStructure()->getParent()->getTargetPortList(port);
    }
    else return vector < graph::TargetPort* >();
}

void Simulator::applyDynamics(SAtomicModelList& lst,
                              const std::string& xmlDynamics)
{
    xmlpp::DomParser dom;
    dom.parse_memory(xmlDynamics);
    xmlpp::Element* root = utils::xml::get_root_node(dom, "MODELS");
    xmlpp::Node::NodeList lt = root->get_children("MODEL");
    for (xmlpp::Node::NodeList::iterator it = lt.begin(); it != lt.end();
         ++it) {
        xmlpp::Element* mdl = (xmlpp::Element*)(*it);
        Glib::ustring name = utils::xml::get_attribute(mdl, "NAME");
        xmlpp::Element* dynamics = utils::xml::get_children(mdl, "DYNAMICS");

        for (SAtomicModelList::iterator jt = lst.begin(); jt != lst.end();
             ++jt) {
            if ((*jt)->getName() == name) {
                (*jt)->parseXML(dynamics);
                break;
            }
        }
    }
}

void Simulator::applyInits(SAtomicModelList& lst,
                           const std::string& xmlInits)
{
    xmlpp::DomParser dom;
    dom.parse_memory(xmlInits);
    xmlpp::Element* root =
        utils::xml::get_root_node(dom, "EXPERIMENTAL_CONDITIONS");
    xmlpp::Node::NodeList lt = root->get_children("CONDITION");
    for (xmlpp::Node::NodeList::iterator it = lt.begin(); it != lt.end();
         ++it) {
        xmlpp::Element* cdt = (xmlpp::Element*)(*it);
        Glib::ustring name = utils::xml::get_attribute(cdt, "MODEL_NAME");
        Glib::ustring port = utils::xml::get_attribute(cdt, "PORT_NAME");

        std::vector < value::Value* > result = value::Value::getValues(cdt);
        vle::value::Value* init = 0;
        if (not result.empty()) {
            init = result.front();
            std::vector < value::Value* >::iterator jt = result.begin();
            ++jt;
            for (;jt != result.end(); ++jt)
                delete (*jt);
        }

        if (init) {
            for (SAtomicModelList::iterator jt = lst.begin(); jt != lst.end();
                 ++jt) {
                if ((*jt)->getName() == name) {
                    InitEvent* evt = new InitEvent(m_currentTime, (*jt), port);
                    evt->putAttribute(port, init);
                    (*jt)->processInitEvent(evt);
                    delete evt;
                    break;
                }
            }
        }
    }
}

void Simulator::startEOVStream()
{
    const vpz::EOVs eovs = m_experiment.measures().eovs();
    std::map < std::string, vpz::EOV >::const_iterator it;
    for (it = eovs.eovs().begin(); it != eovs.eovs().end(); ++it) {
        std::string host;
        int port;

        utils::net::explodeStringNet((*it).second.host(), host, port);
        DTRACE1(boost::format("startEOVStream '%1%' '%2%'.\n") % host % port);
        
        if (host == "localhost") {
            std::string cmd("eov -p ");
            cmd += utils::to_string(port);
            Glib::spawn_command_line_async(cmd);
            Glib::usleep(1000000); // FIXME pose obligatoire ?!?
        }

        utils::net::Client* clt = 0;
        try {
            clt = new utils::net::Client(host, port);
            DTRACE1(boost::format("sendEOV: %1%.\n") % (*it).second.xml());
            clt->send_buffer((*it).second.xml());
            Glib::ustring tr(clt->recv_string());
            clt->close();
            delete clt;

            DTRACE2(boost::format("recvEOV: %1%.\n") % tr);
            xmlpp::DomParser dom;
            dom.parse_memory(tr);
            value::Value* v =
                value::Value::getValue(dom.get_document()->get_root_node());

            if (v->isMap()) {
                const value::Map::MapValue& mp = ((value::Map*)v)->getValue();
                for (value::Map::MapValue::const_iterator it = mp.begin();
                     it != mp.end(); ++it) {
                    if ((*it).second->isInteger()) {
                        std::string output = (*it).first;
                        int port = ((value::Integer*)(*it).second)->intValue();

                        std::string outputhost = host + ":" + utils::to_string(port);
                        startNetStream(output, outputhost);
                    }
                }
            }
        } catch(const std::exception& e) {
            delete clt;
            std::string err((boost::format(
                    "Error connecting on eov '%1%' host '%2%' port '%3%': %4%\n") %
                (*it).first % host % port % e.what()).str());
            Throw(utils::InternalError, err);
        }
    }
//    Glib::usleep(1000000); // FIXME pose obligatoire ?!?
}

void Simulator::startNetStream(const std::string& output,
                               const std::string& outputhost)
{
    std::string measurename;
    const vpz::Output& o = m_experiment.measures().outputs().find(output);
    const vpz::Measure& m =
        m_experiment.measures().findMeasureFromOutput(output, measurename);
    Stream* stream = getStreamPlugin(o);
    stream->init(o.plugin(), m_experiment.name(), outputhost, o.xml());

    DTRACE1(boost::format("measurename : %1%.\n") % measurename);

    Observer* obs = 0;
    if (m.type() == vpz::Measure::TIMED) {
        obs = new devs::TimedObserver(measurename, stream, m.timestep());
    } else if (m.type() == vpz::Measure::EVENT) {
        obs = new devs::EventObserver(measurename, stream);
    }
    stream->setObserver(obs);

    const std::list < vpz::Observable >& observables = m.observables();
    std::list < vpz::Observable >::const_iterator it;
    for (it = observables.begin(); it != observables.end(); ++it) {
        obs->addObservable(getModel((*it).modelname()), (*it).portname(),
                               (*it).group(), (*it).index());
    }
    addObserver(obs);
}

void Simulator::startLocalStream()
{
    const vpz::Measures& measures = m_experiment.measures();
    std::map < std::string, vpz::Measure >::const_iterator it;
    for (it = measures.measures().begin(); it != measures.measures().end();
         ++it) {
        Stream* stream = 0;
        const vpz::Output& o = measures.outputs().find((*it).second.output());

        if (o.format() == vpz::Output::TEXT or o.format() == vpz::Output::SDML) {
            std::string file(m_experiment.name());
            file += "_";
            file += (*it).first;
            
            stream = getStreamPlugin(o);
            stream->init(o.plugin(), file, o.location(), o.xml());

            Observer* obs = 0;
            if ((*it).second.type() == vpz::Measure::TIMED) {
                obs = new devs::TimedObserver((*it).first, stream,
                                              (*it).second.timestep());
            } else if ((*it).second.type() == vpz::Measure::EVENT) {
                obs = new devs::EventObserver((*it).first, stream);
            }
            stream->setObserver(obs);

            const std::list < vpz::Observable >& observables =
                (*it).second.observables();
            std::list < vpz::Observable >::const_iterator jt;
            for (jt = observables.begin(); jt != observables.end(); ++jt) {
                obs->addObservable(getModel((*jt).modelname()),
                                   (*jt).portname(),
                                   (*jt).group(), (*jt).index());
            }
            addObserver(obs);
        }
    }
}

void Simulator::parseExperiment()
{
    m_duration = m_experiment.duration();
    startEOVStream();
    startLocalStream();
}

void Simulator::processEventObserver(sAtomicModel* p_model, Event* p_event)
{
    Assert(utils::InternalError, p_model and p_event, "processEventObserver");
    // S'il existe des eventObservers pour le modele
    if (m_eventObserverList.find(p_model->getName()) !=
        m_eventObserverList.end())
    {
        vector < EventObserver* > v_list =
            m_eventObserverList[p_model->getName()];
        vector < EventObserver* >::iterator it =
            v_list.begin();

        while (it != v_list.end())
        {
            std::vector < Observable > const &
                v_list2 = (*it)->getObservableList();
            std::vector < Observable >::const_iterator it2 = v_list2.begin();

            while (it2 != v_list2.end())
            {
                StateEvent * v_event3 =
                    new StateEvent(p_event->getTime(),
                                   p_model,
                                   (*it)->getName(),
                                   (*it2).portName
                                  );
                StateEvent * v_event = p_model->
                    processStateEvent(v_event3);

                delete v_event3;
                (*it)->processStateEvent(v_event);
                ++it2;
            }
            ++it;
        }
    }
}

Event* Simulator::processConflict(EventBagModel& bag, sAtomicModel& mdl)
{
    if (not bag.emptyInternal()) {
        if (not bag.emptyExternal()) {
            if (not bag.emptyInstantaneous()) {
                switch (mdl.processConflict(*bag.internal(), bag.externals(),
                                            bag.instantaneous())) {
                case Event::INTERNAL:
                    return popInternal(bag);
                case Event::EXTERNAL:
                    return popExternal(bag, mdl);
                case Event::INSTANTANEOUS:
                    return popInstantaneous(bag);
                }
            } else {
                if (mdl.processConflict(*bag.internal(), bag.externals())) {
                    return popInternal(bag);
                } else {
                    return popExternal(bag, mdl);
                }
            }
        } else {
            if (not bag.emptyInstantaneous()) {
                if (mdl.processConflict(*bag.internal(), bag.instantaneous())) {
                    return popInternal(bag);
                } else {
                    return popInstantaneous(bag);
                }
            } else {
                return popInternal(bag);
            }
        }
    } else {
        if (not bag.emptyExternal()) {
            if (not bag.emptyInstantaneous()) {
                if (mdl.processConflict(bag.externals(), bag.instantaneous())) {
                    return popExternal(bag, mdl);
                } else {
                    return popInstantaneous(bag);
                }
            } else {
                return popExternal(bag, mdl);
            }
        } else {
            if (not bag.emptyInstantaneous()) {
                return popInstantaneous(bag);
            } else {
                Throw(utils::InternalError, boost::format(
                        "No internal and external events for model '%1%'\n") %
                        mdl.getName());
            }
        }
    }
    return 0;
}

ExternalEventList* Simulator::run()
{
    CompleteEventBagModel& nextEvent = m_eventTable.popEvent();
    InternalEvent* intern = 0;
    Event* current = 0;

    if (not nextEvent.empty()) {
        updateCurrentTime(m_eventTable.getCurrentTime());
    }

    while (not nextEvent.emptyBag()) {
        EventBagModel& eventlist = nextEvent.topBag();
        sAtomicModel* v_model = nextEvent.topBagModel();

        while (not eventlist.empty()) {
            current = processConflict(eventlist, *v_model);

            if (current->isInternal()) {
                ExternalEventList* v_externalEventList;
                v_externalEventList = v_model->getOutputFunction(m_currentTime);
                dispatchExternalEvent(v_externalEventList, nextEvent);
                delete v_externalEventList;
                intern = v_model->processInternalEvent((InternalEvent*)current);
                if (intern)
                    m_eventTable.putInternalEvent(intern);

                processEventObserver(v_model, current);
            } else if (current->isExternal()) {
                if (((ExternalEvent*)current)->isInstantaneous()) {
                    ExternalEventList* lst = v_model->
                        processInstantaneousEvent((InstantaneousEvent*)current);
                    dispatchExternalEvent(lst, nextEvent);
                    delete lst;
                    processEventObserver(v_model, current);
                } else {
                    intern = v_model->
                        processExternalEvent((ExternalEvent*)current);

                    if (intern)
                        m_eventTable.putInternalEvent(intern);

                    processEventObserver(v_model, current);
                }
            }
            delete current;
            current = 0;
        }
        if (v_model == nextEvent.topBagModel())
            nextEvent.popBag();
    }

    while (not nextEvent.emptyStates()) {
        sAtomicModel* v_model = nextEvent.topStateEvent()->getModel();
        StateEvent *v_event = v_model->
            processStateEvent(nextEvent.topStateEvent());
        Observer* v_observer=getObserver(v_event->getObserverName());
        StateEvent* v_event2=v_observer->processStateEvent(v_event);
        delete v_event;

        if (v_event2)
            m_eventTable.putStateEvent(v_event2);

        delete nextEvent.topStateEvent();
        nextEvent.popState();
    }
    AssertI(nextEvent.empty());
    return 0;
}

vle::devs::Stream* Simulator::getStreamPlugin(const vpz::Output& o)
{
    DTRACE1(boost::format("getStreamPlugin: '%1%' '%2%'\n") % o.plugin() %
            o.streamformat());

    std::string file1(Glib::Module::build_path(
        utils::Path::path().getDefaultStreamDir(), o.streamformat()));
    Glib::Module* module = new Glib::Module(file1);

    if (not (*module)) {
        std::string err1(Glib::Module::get_last_error());
        delete module;

        std::string file2(Glib::Module::build_path(
            utils::Path::path().getUserStreamDir(), o.streamformat()));
        module = new Glib::Module(file2);

        if (not (*module)) {
            std::string err2(Glib::Module::get_last_error());
            delete module;

            Throw(utils::FileError, (boost::format(
                        "Error opening stream plugin '%1%'\n"
                        "- from '%2%' with error: %3%\n"
                        "- from '%4%' with error: %5%") % o.streamformat() %
                    file1 % err1 % file2 % err2).str());
        }
    }
    module->make_resident();
    vle::devs::Stream* call = 0;
    void* makeNewStream = 0;

    bool getSymbol = module->get_symbol("makeNewStream", makeNewStream);
    Assert(utils::FileError, getSymbol, boost::format(
            "Error in module '%1%', function makeNewStream not found\n") %
        o.plugin());

    call = ((vle::devs::Stream*(*)())(makeNewStream))();
    Assert(utils::FileError, call, boost::format(
            "Error in module '%1%', function makeNewStream problem allocation "
            "a new plugin: %1%\n") % o.plugin() %
            Glib::Module::get_last_error());

    return call;
}

}} // namespace vle devs
