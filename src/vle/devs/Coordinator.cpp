/**
 * @file devs/Coordinator.cpp
 * @author The VLE Development Team.
 * @brief Represent the DEVS Coordinator class. This class provide a non
 * hierarchical DEVS coordinator ie. all models are in the same coupled model.
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

#include <vle/devs/Coordinator.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/Simulator.hpp>
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

Coordinator::Coordinator(const vpz::Vpz& vp) :
    m_vpz(vp),
    m_experiment(vp.project().experiment()),
    m_duration(0.0),
    m_currentTime(0)
{
    m_modelFactory = new ModelFactory(*this, vp.project().dynamics(),
                                      vp.project().classes());

    addModels(vp.project().model());
    parseExperiment();
}

Coordinator::~Coordinator()
{
    delete m_modelFactory;

    {
        std::map < graph::AtomicModel*, Simulator* >::iterator it;
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

void Coordinator::addModels(const vpz::Model& model)
{
    graph::AtomicModelVector atomicmodellist;
    graph::Model* mdl = model.model();

    if (mdl->isAtomic()) {
        atomicmodellist.push_back((graph::AtomicModel*)mdl);
    } else {
        graph::Model::getAtomicModelList(mdl, atomicmodellist);
    }

    m_modelFactory->createModels(atomicmodellist, m_modelList);
}

void Coordinator::addCondition(const std::string& modelName,
                             const std::string& portName,
                             value::Value* value)
{
    m_conditionList[modelName].
        push_back(pair < string , value::Value* >(portName, value));
}

void Coordinator::addObserver(Observer* p_observer)
{
    m_observerList[p_observer->getName()] = p_observer;
    if (p_observer->isTimed())
        m_timedObserverList.push_back((TimedObserver*)p_observer);
    else
        m_eventObserverList[p_observer->getFirstModel()->getName()].
            push_back((EventObserver*)p_observer);
}

graph::Model* Coordinator::createModels(graph::CoupledModel* parent,
                                      const std::string& className,
                                      const std::string& xmlDynamics,
                                      const std::string& xmlInit)
{
    SimulatorList lst;

    graph::Model* top = m_modelFactory->createModels(className, lst,
                                                     m_modelList);
    parent->addModel(top);
    top->setParent(parent);

    if (not xmlDynamics.empty())
        applyDynamics(lst, xmlDynamics);

    for (SimulatorList::iterator it = lst.begin(); it != lst.end(); ++it) {
        dispatchInternalEvent((*it)->init());
    }

    if (not xmlInit.empty())
        applyInits(lst, xmlInit);

    //xmlpp::Document doc;
    //xmlpp::Element* root = doc.create_root_node("test");
    //parent->writeXML(root); FIXME FIXME FIXME

    return top;
}

void Coordinator::delAtomicModel(graph::CoupledModel* parent,
                               graph::AtomicModel* atom)
{
    if (parent and atom) {
        SimulatorMap::iterator it = m_modelList.find(atom);
        if (it != m_modelList.end()) {
            Simulator* satom = (*it).second;
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

void Coordinator::delCoupledModel(graph::CoupledModel* parent,
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

bool Coordinator::delModel(graph::CoupledModel* parent,
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

void Coordinator::finish()
{
    {
        SimulatorMap::iterator it;
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

void Coordinator::init()
{
    utils::Rand::rand().set_seed(m_experiment.seed());

    const vpz::AtomicModelList& atoms(m_vpz.project().model().atomicModels());
    const vpz::Conditions& cnds(m_experiment.conditions());

    for (vpz::AtomicModelList::const_iterator it = atoms.begin(); it !=
         atoms.end(); ++it) {

        if (it->first->isAtomic()) {
            graph::AtomicModel* atom(graph::Model::toAtomic(it->first));
            Simulator* satom(getModel(atom));
            const vpz::Condition& cnd(cnds.get(it->second.conditions()));
            satom->processInitEvents(cnd.firstValues());
        }
    }

    for (SimulatorMap::iterator satom = m_modelList.begin();
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

void Coordinator::dispatchExternalEvent(ExternalEventList& eventList,
                                        CompleteEventBagModel& bags)
{
    const size_t sz = eventList.size();
    size_t i = 0;

    while (i < sz) {
        ExternalEvent* event = eventList.at(i);

        vector < graph::TargetPort* > v_targetPortList =
            getTargetPortList(event->getModel()->getStructure(),
                              event->getPortName());

        vector < graph::TargetPort* >::iterator it2 =
            v_targetPortList.begin();

        while (it2 != v_targetPortList.end()) {
            graph::TargetPort *v_port = *it2;
            Simulator* dst = m_modelList[v_port->getModel()];

            if (event->isInstantaneous()) {
                bags.addInstantaneous(
                    dst, new InstantaneousEvent(event, dst,
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
    eventList.clear(true);
}

void Coordinator::dispatchInternalEvent(InternalEvent* event)
{
    if (event)
        m_eventTable.putInternalEvent(event);
}

void Coordinator::dispatchStateEvent(StateEvent* event)
{
    if (event)
        m_eventTable.putStateEvent(event);
}

Simulator* Coordinator::getModel(graph::AtomicModel* model) const
{
    SimulatorMap::const_iterator it = m_modelList.find(model);
    return (it == m_modelList.end()) ? 0 : it->second;
}

Simulator* Coordinator::getModel(const std::string& name) const
{
    SimulatorMap::const_iterator it = m_modelList.begin();
    while (it != m_modelList.end()) {
        if ((*it).first->getName() == name) {
            return (*it).second;
        }
        ++it;
    }
    return 0;
}

const Time& Coordinator::getNextTime()
{
    if (m_duration < m_currentTime.getValue())
        return Time::infinity;

    return m_eventTable.topEvent();
}

Observer* Coordinator::getObserver(const std::string& p_name) const
{
    map < string , Observer* >::const_iterator it =
        m_observerList.find(p_name);

    return (it == m_observerList.end())?NULL:it->second;
}

vector < graph::TargetPort* > Coordinator::getTargetPortList(
    graph::Port* /* port */)
{
    //if (m_model != NULL) { FIXME pas d'influence ?
    //return m_model->getStructure()->getTargetPortList(port);
    //} else {
        return vector < graph::TargetPort* > ();
        //}
}

vector < graph::TargetPort* > Coordinator::getTargetPortList(
                    graph::AtomicModel* model,
                    const std::string & portName)
{
    Simulator* smodel = getModel(model);
    if (model) {
        graph::Port *port = smodel->getStructure()->getOutPort(portName);
        return smodel->getStructure()->getParent()->getTargetPortList(port);
    }
    else return vector < graph::TargetPort* >();
}

void Coordinator::applyDynamics(SimulatorList& /* lst */,
                              const std::string& /* xmlDynamics */)
{
    /* FIXME que faire avec cette fonction
    xmlpp::DomParser dom;
    dom.parse_memory(xmlDynamics);
    xmlpp::Element* root = utils::xml::get_root_node(dom, "MODELS");
    xmlpp::Node::NodeList lt = root->get_children("MODEL");
    for (xmlpp::Node::NodeList::iterator it = lt.begin(); it != lt.end();
         ++it) {
        xmlpp::Element* mdl = (xmlpp::Element*)(*it);
        Glib::ustring name = utils::xml::get_attribute(mdl, "NAME");
        xmlpp::Element* dynamics = utils::xml::get_children(mdl, "DYNAMICS");

        for (SimulatorList::iterator jt = lst.begin(); jt != lst.end();
             ++jt) {
            if ((*jt)->getName() == name) {
                (*jt)->parse(dynamics);
                break;
            }
        }
    }
    */
}

void Coordinator::applyInits(SimulatorList& /* lst */,
                           const std::string& /* xmlInits */)
{
    //xmlpp::DomParser dom;
    //dom.parse_memory(xmlInits);
    //xmlpp::Element* root =
    //utils::xml::get_root_node(dom, "EXPERIMENTAL_CONDITIONS");
    //xmlpp::Node::NodeList lt = root->get_children("CONDITION");
    //for (xmlpp::Node::NodeList::iterator it = lt.begin(); it != lt.end();
    //++it) {
    //xmlpp::Element* cdt = (xmlpp::Element*)(*it);
    //Glib::ustring name = utils::xml::get_attribute(cdt, "MODEL_NAME");
    //Glib::ustring port = utils::xml::get_attribute(cdt, "PORT_NAME");

    //FIXME
    //std::vector < value::Value > result = value::Value::getValues(cdt);
    //vle::value::Value init;
    //if (not result.empty()) {
    //init = result.front();
    //std::vector < value::Value* >::iterator jt = result.begin();
    //++jt;
    //for (;jt != result.end(); ++jt)
    //delete (*jt);
    //}
    //
    //if (init) {
    //for (SimulatorList::iterator jt = lst.begin(); jt != lst.end();
    //++jt) {
    //if ((*jt)->getName() == name) {
    //InitEvent* evt = new InitEvent(m_currentTime, (*jt), port);
    //evt->putAttribute(port, init);
    //(*jt)->processInitEvent(evt);
    //delete evt;
    //break;
    //}
    //}
    //}
    //}
}

void Coordinator::startEOVStream()
{
    /*
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
            value::Value v =
                value::ValueBase::getValue(dom.get_document()->get_root_node());

            if (v->isMap()) {
                value::MapFactory::MapValue& mp = value::to_map(v)->getValue();
                for (value::MapFactory::MapValue::const_iterator it = mp.begin();
                     it != mp.end(); ++it) {
                    if ((*it).second->isInteger()) {
                        std::string output = (*it).first;
                        int port = value::to_integer((*it).second)->intValue();

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
    Glib::usleep(1000000); // FIXME pose obligatoire ?!?
    */
}

void Coordinator::startNetStream()
{
    /*
    const vpz::Measures& measures = m_experiment.measures();
    const vpz::Outputs& outs = measures.outputs();

    for (vpz::Outputs::OutputList::const_iterator it = outs.outputs().begin();
         it != outs.outputs().end(); ++it) {
        if (it->second.format() == vpz::Output::NET) {

            for (vpz::Measures::MeasureList::const_iterator jt = measures.begin();
                 jt != measures.end(); ++jt) {

                if (jt->second.output() == it->second.name()) {
                    startNetStream(it->second, jt->second);
                }
            }
        }
        }
        */
}

void Coordinator::startNetStream(const vpz::View& /* measure */,
                                 const vpz::Output& /* output */)
{
    /*

    Stream* stream(getStreamPlugin(output));
    stream->init(ouput.plugin(), m_experiment.name(), output.location(),
                 o.data());

    DTRACE1(boost::format("measurename : %1%.\n") % measure.name());

    Observer* obs = 0;
    if (m.type() == vpz::Measure::TIMED) {
        obs = new devs::TimedObserver(measurename, stream, m.timestep());
    } else if (m.type() == vpz::Measure::EVENT) {
        obs = new devs::EventObserver(measurename, stream);
    }
    stream->setObserver(obs);

    const vpz::Measure::ObservableList& lst(measure.observable());
    const vpz::AtomicModelList& atoms(m_vpz.project().model().atomicModels());

    for (vpz::Measure::ObservableList::const_iterator it = lst.begin();
         it != lst.end(); ++it) {
        for (vpz::AtomicModelList::const_iterator jt = atoms.begin(); 
             jt != atoms.end(); ++jt) {
            if (it->second.name() == (*jt).observables()) {
                const vpz::ObservablePort& ports(jt->
                for (vpz::ObservableList::const_iterator kt = 
                obs->addObservable(jt->first,
                                   (

    const vpz::Measure::ObservableList& obslst(m.observables());
    for (vpz::Measure::ObservableList::const_iterator it = obslst.begin(); 
         it != obslst.end(); ++it) {
        obs->addObservable(getModel((*it).modelname()), (*it).portname(),
                           (*it).group(), (*it).());
    }
    addObserver(obs);

    */
}

void Coordinator::startLocalStream()
{
    /* 
    const vpz::Measures& measures = m_experiment.measures();
    vpz::Measures::MeasureList::const_iterator it;
    for (it = measures.measures().begin(); it != measures.measures().end();
         ++it) {
        Stream* stream = 0;
        const vpz::Output& o = measures.outputs().find((*it).output());

        if (o.format() == vpz::Output::TEXT 
	    or o.format() == vpz::Output::SDML) {
            std::string file(m_experiment.name());
            file += "_";
            file += (*it).name();

            stream = getStreamPlugin(o);
            stream->init(o.plugin(), file, o.location(), o.xml());

            Observer* obs = 0;
            if ((*it).type() == vpz::Measure::TIMED) {
                obs = new devs::TimedObserver((*it).name(), stream,
                                              (*it).timestep());
            } else if ((*it).type() == vpz::Measure::EVENT) {
                obs = new devs::EventObserver((*it).name(), stream);
            }
            stream->setObserver(obs);

            const std::list < vpz::Observable >& observables =
                (*it).observables();
            std::list < vpz::Observable >::const_iterator jt;
            for (jt = observables.begin(); jt != observables.end(); ++jt) {
                obs->addObservable(getModel((*jt).modelname()),
                                   (*jt).portname(),
                                   (*jt).group(), (*jt).index());
            }
            addObserver(obs);
        }
        }
        */
}

void Coordinator::parseExperiment()
{
    m_duration = m_experiment.duration();
    startEOVStream();
    startNetStream();
    startLocalStream();
}

void Coordinator::processEventObserver(Simulator* p_model, Event* p_event)
{
    Assert(utils::InternalError, p_model and p_event, "processEventObserver");
    // S'il existe des eventObservers pour le modele
    if (m_eventObserverList.find(p_model->getName()) !=
        m_eventObserverList.end()) {
        vector < EventObserver* > v_list =
            m_eventObserverList[p_model->getName()];
        vector < EventObserver* >::iterator it =
            v_list.begin();

        while (it != v_list.end()) {
            std::vector < Observable > const &
                v_list2 = (*it)->getObservableList();
            std::vector < Observable >::const_iterator it2 = v_list2.begin();

            while (it2 != v_list2.end()) {
                StateEvent* event3 = 0;
                if (p_event->isInternal()) {
                    event3 = new
                        StateEvent(((InternalEvent*)p_event)->getTime(),
                                            p_model, (*it)->getName(),
                                            (*it2).portName);
                } else {
                    event3 = new StateEvent(m_currentTime,
                                            p_model,
                                            (*it)->getName(),
                                            (*it2).portName);
                }
                StateEvent* v_event = p_model->processStateEvent(*event3);

                delete event3;
                (*it)->processStateEvent(v_event);
                ++it2;
            }
            ++it;
        }
    }
}

void Coordinator::processInternalEvent(
    Simulator* sim,
    EventBagModel& modelbag,
    CompleteEventBagModel& bag)
{
    InternalEvent* ev = modelbag.internal();
    modelbag.delInternal();

    {
        ExternalEventList result;
        sim->getOutputFunction(m_currentTime, result);
        dispatchExternalEvent(result, bag);
    }

    {
        InternalEvent* internal(sim->processInternalEvent(*ev));
        if (internal) {
            m_eventTable.putInternalEvent(internal);
        }
    }

    processEventObserver(sim, ev);
    delete ev;
}

void Coordinator::processExternalEvents(
    Simulator* sim,
    EventBagModel& modelbag,
    CompleteEventBagModel& /* bag */)
{
    ExternalEventList& lst(modelbag.externals());
    modelbag.delInternal();

    {
        InternalEvent* internal(sim->processExternalEvents(lst, m_currentTime));
        if (internal) {
            m_eventTable.putInternalEvent(internal);
        }
    }

    //processEventObserver(simulator, ev); FIXME
    //delete ev;
}

void Coordinator::processInstantaneousEvents(
    Simulator* sim,
    EventBagModel& modelbag,
    CompleteEventBagModel& bag)
{
    InstantaneousEventList& lst(modelbag.instantaneous());
    modelbag.delInstantaneous();

    ExternalEventList result;
    for (InstantaneousEventList::iterator it = lst.begin(); it != lst.end();
         ++it) {
        sim->processInstantaneousEvent(**it, m_currentTime, result);
        dispatchExternalEvent(result, bag);
        result.clear();
    }
}

void Coordinator::processStateEvents(CompleteEventBagModel& bag)
{
    while (not bag.emptyStates()) {
        Simulator* model(bag.topStateEvent()->getModel());
        StateEvent* event(model->processStateEvent(*bag.topStateEvent()));

        Observer* observer(getObserver(event->getObserverName()));
        StateEvent* event2(observer->processStateEvent(event));
        delete event;

        if (event2) {
            m_eventTable.putStateEvent(event2);
        }

        delete bag.topStateEvent();
        bag.popState();
    }
}

/*
Event* Coordinator::processConflict(EventBagModel& bag, Simulator& mdl)
{
    if (not bag.emptyInternal()) {
        if (not bag.emptyExternal()) {
            switch (mdl.processConflict(*bag.internal(), bag.externals())) {
            case Event::INTERNAL:
                return popInternal(bag);
            case Event::EXTERNAL:
                return popExternal(bag, mdl);
            }
        } else {
            return popInternal(bag);
        }
    } else {
        if (not bag.emptyExternal()) {
            return popExternal(bag, mdl);
        } else {
            Assert(utils::InternalError, not bag.emptyInstantaneous(),
                   boost::format("No internal, external or instantaneous "
                                 "event for model '%1%'\n") % mdl.getName());
            return popInstantaneous(bag);
        }
    }
    return 0;
}*/

ExternalEventList* Coordinator::run()
{
    CompleteEventBagModel& nextEvent = m_eventTable.popEvent();
    if (not nextEvent.empty()) {
        updateCurrentTime(m_eventTable.getCurrentTime());
    }

    while (not nextEvent.emptyBag()) {
        EventBagModel& bag(nextEvent.topBag());
        Simulator* mdl(nextEvent.topBagModel());

        while (not bag.empty()) {
            if (not bag.emptyInternal()) {
                if (not bag.emptyExternal()) {
                    switch (mdl->processConflict(
                            *bag.internal(), bag.externals())) {
                    case Event::INTERNAL:
                        processInternalEvent(mdl, bag, nextEvent);
                        break;
                        
                    case Event::EXTERNAL:
                        processExternalEvents(mdl, bag, nextEvent);
                        break;
                    }
                } else {
                    processInternalEvent(mdl, bag, nextEvent);
                }
            } else {
                if (not bag.emptyExternal()) {
                    processExternalEvents(mdl, bag, nextEvent);
                } else {
                    processInstantaneousEvents(mdl, bag, nextEvent);
                }
            }
        }
        if (mdl == nextEvent.topBagModel()) // FIXME encore utile ?
            nextEvent.popBag();
    }

    processStateEvents(nextEvent);
    AssertI(nextEvent.empty());
    return 0;
}

vle::devs::Stream* Coordinator::getStreamPlugin(const vpz::Output& o)
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
