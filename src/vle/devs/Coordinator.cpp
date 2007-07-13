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

Coordinator::Coordinator(const vpz::Vpz& vpz, vpz::Model& mdls) :
    m_experiment(vpz.project().experiment()),
    m_currentTime(0)
{
    m_modelFactory = new ModelFactory(*this,
                                      vpz.project().dynamics(),
                                      vpz.project().classes(),
                                      vpz.project().experiment(),
                                      vpz.project().model().atomicModels());

    buildViews();
    addModels(mdls);
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
        std::map < std::string , devs::View* >::iterator it;
        for (it = m_viewList.begin(); it != m_viewList.end(); ++it) {
            delete it->second;
        }
    }
}

void Coordinator::init()
{
    utils::Rand::rand().set_seed(m_experiment.seed());
}

const Time& Coordinator::getNextTime()
{
    return m_eventTable.topEvent();
}

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
                        processInternalEvent(mdl, bag);
                        break;

                    case Event::EXTERNAL:
                        processExternalEvents(mdl, bag);
                        break;
                    }
                } else {
                    processInternalEvent(mdl, bag);
                }
            } else {
                if (not bag.emptyExternal()) {
                    processExternalEvents(mdl, bag);
                } else {
                    processInstantaneousEvents(mdl, bag);
                }
            }
        }
        if (mdl == nextEvent.topBagModel())
            nextEvent.popBag();
    }

    processStateEvents(nextEvent);
    AssertI(nextEvent.empty());
    return 0;
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
        ViewList::iterator it;
        for (it = m_viewList.begin(); it != m_viewList.end(); ++it) {
            (*it).second->finish();
        }
    }
}

//
///
//// Functions use by Executive models to manage DsDevs simulation.
///
//

void Coordinator::addPermanent(const vpz::Dynamic& dynamics)
{
    m_modelFactory->addPermanent(dynamics);
}

void Coordinator::addPermanent(const vpz::Condition& condition)
{
    m_modelFactory->addPermanent(condition);
}

void Coordinator::addPermanent(const vpz::Observable& observable)
{
    m_modelFactory->addPermanent(observable);
}

Simulator* Coordinator::createModel(graph::AtomicModel* model,
                                    const std::string& dynamics,
                                    const std::string& condition,
                                    const std::string& observable)
{
    return m_modelFactory->createModel(model, dynamics, condition, observable,
                                       m_modelList);
}

void Coordinator::addObservableToView(graph::AtomicModel* model,
                                      const std::string& portname,
                                      const std::string& view)
{
    ViewList::iterator it = m_viewList.find(view);
    Assert(utils::InternalError, it != m_viewList.end(), boost::format(
            "The view %1% is unknow of coordinator view list") % view);

    Simulator* simulator = getModel(model);
    Assert(utils::InternalError, simulator, boost::format(
            "The simulator of the model %1% does not exist") %
        simulator->getStructure()->getName());

    View* obs = it->second;
    StateEvent* evt = obs->addObservable(simulator, portname, m_currentTime);
    if (evt != 0) {
        m_eventTable.putStateEvent(evt);
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


//
///
//// Some usefull functions.
///
//


Simulator* Coordinator::getModel(const graph::AtomicModel* model) const
{
    SimulatorMap::const_iterator it =
        m_modelList.find( const_cast < graph::AtomicModel* >(model));
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

View* Coordinator::getView(const std::string& name) const
{
    ViewList::const_iterator it = m_viewList.find(name);
    return (it == m_viewList.end()) ? 0 : it->second;
}

//
///
//// Private functions.
///
//

void Coordinator::delAtomicModel(graph::CoupledModel* parent,
                                 graph::AtomicModel* atom)
{
    if (parent and atom) {
        SimulatorMap::iterator it = m_modelList.find(atom);
        if (it != m_modelList.end()) {
            Simulator* satom = (*it).second;
            m_modelList.erase(it);

            std::map < std::string , View* >::iterator it2;
            for (it2 = m_viewList.begin(); it2 != m_viewList.end();
                 ++it2) {
                View* View = (*it2).second;
                View->removeObservable(satom);
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
        graph::ModelList& lst = mdl->getModelList();
        for (graph::ModelList::iterator it = lst.begin(); it != lst.end();
             ++it) {
            if (it->second->isAtomic()) {
                delAtomicModel(mdl, (graph::AtomicModel*)(it->second));
            } else if (it->second->isCoupled()) {
                delCoupledModel(mdl, (graph::CoupledModel*)(it->second));
            }
        }
        parent->delAllConnection(mdl);
        parent->delModel(mdl);
    }
}


void Coordinator::addModels(vpz::Model& model)
{
    graph::AtomicModelVector atomicmodellist;
    graph::Model* mdl = model.model();
    model.set_model(0);

    if (mdl->isAtomic()) {
        atomicmodellist.push_back((graph::AtomicModel*)mdl);
    } else {
        graph::Model::getAtomicModelList(mdl, atomicmodellist);
    }

    const vpz::AtomicModelList& atoms(m_modelFactory->atomics());
    for (graph::AtomicModelVector::iterator it = atomicmodellist.begin();
         it != atomicmodellist.end(); ++it) {
        const vpz::AtomicModel& atom(atoms.get(*it));
        createModel(*it, atom.dynamics(), atom.conditions(),
                    atom.observables());
    }
}

void Coordinator::addView(View* view)
{
    Assert(utils::InternalError, view, boost::format("Empty reference"));

    ViewList::iterator it = m_viewList.find(view->getName());
    if (it == m_viewList.end()) {
        m_viewList[view->getName()] = view;
        if (view->isTimed()) {
            m_timedViewList.push_back(
                reinterpret_cast < TimedView*>(view));
        } else {
            m_eventViewList[
                view->getFirstModel()->getName()].push_back(
                    reinterpret_cast < EventView* >(view));
        }
    }
}

void Coordinator::dispatchExternalEvent(ExternalEventList& eventList,
                                        Simulator* sim)
{
    const size_t sz = eventList.size();
    size_t i = 0;

    while (i < sz) {
        ExternalEvent* event = eventList.at(i);
        event->setModel(sim);

        graph::TargetModelList out;
        getTargetPortList(sim->getStructure(), event->getPortName(), out);

        graph::TargetModelList::iterator it2 = out.begin();
        while (it2 != out.end()) {
            AssertI(it2->model()->isAtomic());
            Simulator* dst = getModel(
                static_cast < graph::AtomicModel* >(it2->model()));
            const std::string& port(it2->port());

            if (event->isInstantaneous()) {
                m_eventTable.putInstantaneousEvent(
                    new InstantaneousEvent(
                        reinterpret_cast < InstantaneousEvent* >(event),
                        dst, port));
            } else {
                m_eventTable.putExternalEvent(
                    new ExternalEvent(event, dst, port));
            }
            ++it2;
        }
        ++i;
    }
    eventList.clear(true);
}

void Coordinator::getTargetPortList(
    graph::AtomicModel* model,
    const std::string& portName,
    graph::TargetModelList& out)
{
    if (model) {
        model->getTargetPortList(portName, out);
    }
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

       View* obs = 0;
       if (m.type() == vpz::Measure::TIMED) {
       obs = new devs::TimedView(measurename, stream, m.timestep());
       } else if (m.type() == vpz::Measure::EVENT) {
       obs = new devs::EventView(measurename, stream);
       }
       stream->setView(obs);

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
       addView(obs);

*/
}

void Coordinator::startLocalStream()
{
    const vpz::Views& views(m_modelFactory->views());
    vpz::Views::const_iterator it;
    for (it = views.begin(); it != views.end(); ++it) {
        Stream* stream = 0;
        const vpz::Output& o(views.outputs().get(it->second.output()));

        if (o.format() == vpz::Output::TEXT 
            or o.format() == vpz::Output::SDML) {
            std::string file((boost::format("%1%_%2%") % m_experiment.name() %
                             it->second.name()).str());

            stream = getStreamPlugin(o);
            stream->init(o.plugin(), file, o.location(), o.data());

            View* obs = 0;
            if (it->second.type() == vpz::View::TIMED) {
                obs = new devs::TimedView(
                    it->second.name(), stream, it->second.timestep());
            } else if (it->second.type() == vpz::View::EVENT) {
                obs = new devs::EventView(it->second.name(), stream);
            }
            stream->setView(obs);
            addView(obs);
        }
    }
}

void Coordinator::buildViews()
{
    startEOVStream();
    startNetStream();
    startLocalStream();
}

void Coordinator::processEventView(Simulator& model, Event* event)
{
    if (m_eventViewList.find(model.getName()) !=
        m_eventViewList.end()) {
        vector < EventView* > v_list =
            m_eventViewList[model.getName()];
        vector < EventView* >::iterator it =
            v_list.begin();

        while (it != v_list.end()) {
            const std::vector < Observable >& v_list2(
                (*it)->getObservableList());
            std::vector < Observable >::const_iterator it2 = v_list2.begin();

            while (it2 != v_list2.end()) {
                StateEvent* event3 = 0;
                if (event == 0 or not event->isInternal()) {
                    event3 = new StateEvent(m_currentTime, &model,
                                            (*it)->getName(),
                                            (*it2).portname());
                } else if (event and event->isInternal()) {
                    event3 = new StateEvent(((InternalEvent*)event)->getTime(),
                                            &model, (*it)->getName(),
                                            (*it2).portname());
                }
                StateEvent* v_event = model.processStateEvent(*event3);

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
    EventBagModel& modelbag)
{
    InternalEvent* ev = modelbag.internal();
    modelbag.delInternal();

    {
        ExternalEventList result;
        sim->getOutputFunction(m_currentTime, result);
        dispatchExternalEvent(result, sim);
    }

    {
        InternalEvent* internal(sim->processInternalEvent(*ev));
        if (internal) {
            m_eventTable.putInternalEvent(internal);
        }
    }

    processEventView(*sim, ev);
    delete ev;
}

void Coordinator::processExternalEvents(
    Simulator* sim,
    EventBagModel& modelbag)
{
    ExternalEventList& lst(modelbag.externals());

    {
        InternalEvent* internal(sim->processExternalEvents(lst, m_currentTime));
        if (internal) {
            m_eventTable.putInternalEvent(internal);
        }
    }
    
    modelbag.delExternals();
    processEventView(*sim, 0);
}

void Coordinator::processInstantaneousEvents(
    Simulator* sim,
    EventBagModel& modelbag)
{
    InstantaneousEventList& lst(modelbag.instantaneous());

    ExternalEventList result;
    for (InstantaneousEventList::iterator it = lst.begin(); it != lst.end();
         ++it) {
        sim->processInstantaneousEvent(**it, m_currentTime, result);
        dispatchExternalEvent(result, sim);
        result.clear();
    }
    
    modelbag.delInstantaneous();
}

void Coordinator::processStateEvents(CompleteEventBagModel& bag)
{
    while (not bag.emptyStates()) {
        Simulator* model(bag.topStateEvent()->getModel());
        StateEvent* event(model->processStateEvent(*bag.topStateEvent()));

        if (event) {
            View* View(getView(event->getViewName()));
            StateEvent* event2(View->processStateEvent(event));
            delete event;

            if (event2) {
                m_eventTable.putStateEvent(event2);
            }
        }
        delete bag.topStateEvent();
        bag.popState();
    }
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
