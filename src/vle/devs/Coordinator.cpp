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
#include <vle/devs/StreamWriter.hpp>
#include <vle/devs/LocalStreamWriter.hpp>
#include <vle/devs/NetStreamWriter.hpp>
#include <vle/graph/Model.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Path.hpp>
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
        m_modelList.clear();
    }

    {
        ViewList::iterator it;
        for (it = m_viewList.begin(); it != m_viewList.end(); ++it) {
            (*it).second->finish(m_currentTime);
        }
        m_viewList.clear();
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
    Assert(utils::InternalError, view, "Empty reference");

    ViewList::iterator it = m_viewList.find(view->getName());
    if (it == m_viewList.end()) {
        const std::string& name = view->getName();
        m_viewList[name] = view;
        if (view->isTimed()) {
            m_timedViewList[name] = reinterpret_cast < TimedView*>(view);
        } else {
            m_eventViewList[name] = reinterpret_cast < EventView*>(view);
        }
    }
}

void Coordinator::dispatchExternalEvent(ExternalEventList& eventList,
                                        Simulator* sim)
{
    for (ExternalEventList::iterator it = eventList.begin(); it !=
         eventList.end(); ++it) {
        (*it)->setModel(sim);

        graph::TargetModelList out;
        getTargetPortList(sim->getStructure(), (*it)->getPortName(), out);

        for (graph::TargetModelList::iterator jt = out.begin();
             jt != out.end(); ++jt) {
            AssertI(jt->model()->isAtomic());
            Simulator* dst = getModel(
                reinterpret_cast < graph::AtomicModel* >(jt->model()));
            const std::string& port(jt->port());

            if ((*it)->isInstantaneous()) {
                m_eventTable.putInstantaneousEvent(
                    new InstantaneousEvent(
                        reinterpret_cast < InstantaneousEvent* >((*it)),
                        dst, port));
            } else {
                m_eventTable.putExternalEvent(
                    new ExternalEvent((*it), dst, port));
            }
        }
        delete (*it);
    }
    eventList.clear();
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

void Coordinator::buildViews()
{
    std::map < std::string, StreamWriter* > result;

    vpz::Outputs outs(m_modelFactory->outputs());
    for (vpz::Outputs::iterator it = outs.begin(); it != outs.end(); ++it) {
        StreamWriter* stream = 0;
        switch (it->second.format()) {
        case vpz::Output::LOCAL:
            stream = new LocalStreamWriter();
            break;
        case vpz::Output::DISTANT:
            stream = new NetStreamWriter();
            break;
        }
        
        std::string file((boost::format("%1%_%2%.dat") % m_experiment.name() %
                          it->second.name()).str());
        stream->open(it->second.plugin(), it->second.location(), file, 
                     it->second.data(), m_currentTime); 
        result[it->first] = stream;
    }

    vpz::Views views(m_modelFactory->views());
    for (vpz::Views::iterator it = views.begin(); it != views.end(); ++it) {
        std::map < std::string, StreamWriter* >::iterator jt;
        jt = result.find(it->second.output());

        Assert(utils::InternalError, jt != result.end(), boost::format(
                "The output %1% does not exist for view %2%") %
            it->second.output() % it->first);

        StreamWriter* stream = jt->second;
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

void Coordinator::processEventView(Simulator& model, Event* event)
{
    std::list < std::string > lst;
    for (EventViewList::iterator it = m_eventViewList.begin();
         it != m_eventViewList.end(); ++it) {
        lst.clear();
        lst = it->second->get(&model);

        for (std::list < std::string >::iterator jt = lst.begin();
             jt != lst.end(); ++jt) {
            
            StateEvent* newevent = 0;
            if (event == 0 or not event->isInternal()) {
                newevent = new StateEvent(m_currentTime, &model,
                                          it->second->getName(), *jt);
            } else if (event and event->isInternal()) {
                newevent = new StateEvent(((InternalEvent*)event)->getTime(),
                                          &model, it->second->getName(), *jt);
            }
            StateEvent* eventvalue = model.processStateEvent(*newevent);
            delete newevent;
            it->second->processStateEvent(eventvalue);
            delete eventvalue;
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
    
    lst.deleteAndClear();
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
    
    lst.deleteAndClear();
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

}} // namespace vle devs
