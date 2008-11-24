/**
 * @file vle/devs/Coordinator.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/devs/Coordinator.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/Executive.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/InternalEvent.hpp>
#include <vle/devs/ExternalEventList.hpp>
#include <vle/devs/ObservationEvent.hpp>
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

using std::vector;
using std::map;
using std::string;
using std::pair;

using namespace xmlpp;

namespace vle { namespace devs {

Coordinator::Coordinator(ModelFactory& modelfactory) :
    m_currentTime(0),
    m_modelFactory(modelfactory)
{
    buildViews();
}

Coordinator::~Coordinator()
{
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

void Coordinator::init(const vpz::Model& mdls)
{
    addModels(mdls);
    m_toDelete = 0;
}

const Time& Coordinator::getNextTime()
{
    return m_eventTable.topEvent();
}

ExternalEventList* Coordinator::run()
{
    SimulatorList::size_type oldToDelete(m_toDelete);

    CompleteEventBagModel& bags = m_eventTable.popEvent();
    if (not bags.empty()) {
        updateCurrentTime(m_eventTable.getCurrentTime());
    }

    while (not bags.emptyBag()) {
        std::map < Simulator*, EventBagModel >::value_type& bag(bags.topBag());
        while (not bag.second.empty()) {
            if (not bag.second.emptyInternal()) {
                if (not bag.second.emptyExternal()) {
                    processConflictEvents(bag.first, bag.second);
                } else {
                    processInternalEvent(bag.first, bag.second);
                }
            } else {
                if (not bag.second.emptyExternal()) {
                    processExternalEvents(bag.first, bag.second);
                } else {
                    processRequestEvents(bag.first, bag.second);
                }
            }
        }
    }

    if (oldToDelete > 0) {
        for (SimulatorList::iterator it = m_deletedSimulator.begin();
             it != m_deletedSimulator.begin() + oldToDelete; ++it) {
            m_eventTable.delModelEvents(*it);
            delete *it;
            *it = 0;
        }

        m_deletedSimulator.erase(m_deletedSimulator.begin(),
                                 m_deletedSimulator.begin() + oldToDelete);

        m_toDelete = m_deletedSimulator.size();
    }

    if (not bags.emptyStates()) {
        if (getNextTime() == bags.topObservationEvent()->getTime()) {
            m_obsEventBuffer.insert(m_obsEventBuffer.begin(),
                                    bags.states().begin(),
                                    bags.states().end());
            bags.popStates();
        } else {
            processTimedObservationEvents(bags.states());
            processTimedObservationEvents(m_obsEventBuffer);
        }
    } else if (not m_obsEventBuffer.empty()) {
        if (getNextTime() != m_obsEventBuffer.front()->getTime()) {
            processTimedObservationEvents(m_obsEventBuffer);
            m_obsEventBuffer.clear();
        }
    }

    bags.clear();
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
        FinishViewList::iterator it;
        for (it = m_finishViewList.begin(); it != m_finishViewList.end(); ++it) {
            ObservableList::const_iterator jt;
            for (jt = it->second->getObservableList().begin();
                 jt != it->second->getObservableList().end(); ++jt) {
                ObservationEvent* evt = new ObservationEvent(
                    m_currentTime, jt->first, it->first, jt->second);
                ObservationEvent* out = jt->first->observation(*evt);
                it->second->processObservationEvent(out);
                delete evt;
                delete out;
            }
        }
    }

    {
        ViewList::iterator it;
        for (it = m_viewList.begin(); it != m_viewList.end(); ++it) {
            (*it).second->finish(m_currentTime);
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
    m_modelFactory.addPermanent(dynamics);
}

void Coordinator::addPermanent(const vpz::Condition& condition)
{
    m_modelFactory.addPermanent(condition);
}

void Coordinator::addPermanent(const vpz::Observable& observable)
{
    m_modelFactory.addPermanent(observable);
}

void Coordinator::createModel(graph::AtomicModel* model,
                              const std::string& dynamics,
                              const vpz::Strings& conditions,
                              const std::string& observable)
{
    m_modelFactory.createModel(*this, model, dynamics, conditions,
                               observable);
}

graph::Model* Coordinator::createModelFromClass(const std::string& classname,
                                                graph::CoupledModel* parent,
                                                const std::string& modelname)
{
    return m_modelFactory.createModelFromClass(*this, parent,
                                               classname, modelname);
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
    ObservationEvent* evt = obs->addObservable(simulator, portname, m_currentTime);
    if (evt != 0) {
        m_eventTable.putObservationEvent(evt);
    }
}

void Coordinator::delModel(graph::CoupledModel* parent,
                           const std::string& modelname)
{
    graph::Model* mdl = parent->findModel(modelname);

    Assert(utils::DevsGraphError, mdl, "Cannt delete an unknow model");

    if (mdl->isCoupled()) {
        delCoupledModel(parent, (graph::CoupledModel*)mdl);
        parent->delAllConnection(mdl);
        parent->delModel(mdl);
    } else {
        delAtomicModel(parent, (graph::AtomicModel*)mdl);
        parent->delModel(mdl);
    }
}


//
///
//// Some usefull functions.
///
//

void Coordinator::addModel(graph::AtomicModel* model, Simulator* simulator)
{
    Assert(utils::InternalError, m_modelList.find(model) == m_modelList.end(),
           boost::format("The Atomic model node '%1% have already a simulator")
           % model->getName());

    m_modelList[model] = simulator;
}

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

oov::OutputMatrixViewList Coordinator::outputs() const
{
    oov::OutputMatrixViewList lst;

    std::for_each(m_viewList.begin(), m_viewList.end(),
                  GetOutputMatrixView(lst));

    return lst;
}

///
/// Private functions.
///

void Coordinator::delAtomicModel(graph::CoupledModel* parent,
                                 graph::AtomicModel* atom)
{
    Assert(utils::DevsGraphError, parent and atom,
           "Cannot delete an atomic model without parent");

    SimulatorMap::iterator it = m_modelList.find(atom);
    Assert(utils::ModellingError, it != m_modelList.end(),
           "Cannot delete an unknow atomic model");

    Simulator* satom = (*it).second;
    m_modelList.erase(it);

    std::map < std::string , View* >::iterator it2;
    for (it2 = m_viewList.begin(); it2 != m_viewList.end();
         ++it2) {
        View* View = (*it2).second;
        View->removeObservable(satom);
    }
    m_eventTable.invalidateModel(satom);
    satom->clear();
    m_deletedSimulator.push_back(satom);

    std::for_each(m_obsEventBuffer.begin(), m_obsEventBuffer.end(),
                  Event::InvalidateSimulator(satom));

    ++m_toDelete;
}

void Coordinator::delCoupledModel(graph::CoupledModel* parent,
                                  graph::CoupledModel* mdl)
{
    Assert(utils::DevsGraphError, parent and mdl,
           "Cannot delete an atomic model without parent");

    graph::ModelList& lst = mdl->getModelList();
    for (graph::ModelList::iterator it = lst.begin(); it != lst.end();
         ++it) {
        if (it->second->isAtomic()) {
            delAtomicModel(mdl, (graph::AtomicModel*)(it->second));
        } else if (it->second->isCoupled()) {
            delCoupledModel(mdl, (graph::CoupledModel*)(it->second));
        }
    }
}


void Coordinator::addModels(const vpz::Model& model)
{
    m_modelFactory.createModels(*this, model);
}

void Coordinator::addView(View* view)
{
    Assert(utils::InternalError, view, "Empty reference");

    ViewList::iterator it = m_viewList.find(view->getName());
    if (it == m_viewList.end()) {
        const std::string& name = view->getName();
        m_viewList[name] = view;
        if (view->isTimed()) {
            m_timedViewList[name] = reinterpret_cast < TimedView* >(view);
        } else if (view->isEvent()) {
            m_eventViewList[name] = reinterpret_cast < EventView* >(view);
        } else {
            m_finishViewList[name] = reinterpret_cast < FinishView* >(view);
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
        sim->getStructure()->getTargetPortList((*it)->getPortName(), out);

        for (graph::TargetModelList::iterator jt = out.begin();
             jt != out.end(); ++jt) {
            assert(jt->model()->isAtomic());
            Simulator* dst = getModel(
                reinterpret_cast < graph::AtomicModel* >(jt->model()));
            const std::string& port(jt->port());

            if ((*it)->isRequest()) {
                m_eventTable.putRequestEvent(
                    new RequestEvent(
                        reinterpret_cast < RequestEvent* >((*it)),
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

void Coordinator::buildViews()
{
    const vpz::Outputs& outs(m_modelFactory.outputs());
    const vpz::Views& views(m_modelFactory.views());
    const vpz::ViewList& viewlist(views.viewlist());

    for (vpz::ViewList::const_iterator it = viewlist.begin();
         it != viewlist.end(); ++it) {

        StreamWriter* stream = buildOutput(
            it->second, outs.get(it->second.output()));

        View* obs = 0;
        if (it->second.type() == vpz::View::TIMED) {
            obs = new devs::TimedView(
                it->second.name(), stream, it->second.timestep());
        } else if (it->second.type() == vpz::View::EVENT) {
            obs = new devs::EventView(it->second.name(), stream);
        } else if (it->second.type() == vpz::View::FINISH) {
            obs = new devs::FinishView(it->second.name(), stream);
        }
        stream->setView(obs);
        addView(obs);
    }
}

StreamWriter* Coordinator::buildOutput(const vpz::View& view,
                                       const vpz::Output& output)
{
    StreamWriter* stream = 0;

    switch (output.format()) {
    case vpz::Output::LOCAL:
        stream = new LocalStreamWriter();
        break;
    case vpz::Output::DISTANT:
        stream = new NetStreamWriter();
        break;
    }

    std::string file((boost::format("%1%_%2%") %
                      m_modelFactory.experiment().name() %
                      view.name()).str());
    stream->open(output.plugin(), output.location(), file,
                 output.data(), m_currentTime);

    return stream;
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

            ObservationEvent* newevent = 0;
            if (event == 0 or not event->isInternal()) {
                newevent = new ObservationEvent(m_currentTime, &model,
                                          it->second->getName(), *jt);
            } else if (event and event->isInternal()) {
                newevent = new ObservationEvent(((InternalEvent*)event)->getTime(),
                                          &model, it->second->getName(), *jt);
            }
            ObservationEvent* eventvalue = model.observation(*newevent);
            delete newevent;
            it->second->processObservationEvent(eventvalue);
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
        sim->output(m_currentTime, result);
        dispatchExternalEvent(result, sim);
    }

    {
        InternalEvent* internal(sim->internalTransition(*ev));
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
        InternalEvent* internal(sim->externalTransition(lst, m_currentTime));
        if (internal) {
            m_eventTable.putInternalEvent(internal);
        }
    }

    lst.deleteAndClear();
    modelbag.delExternals();
    processEventView(*sim, 0);
}

void Coordinator::processConflictEvents(
    Simulator* sim,
    EventBagModel& modelbag)
{
    InternalEvent* internal;

    if (sim->confluentTransitions(*modelbag.internal(), modelbag.externals())
        == Event::INTERNAL) {
        internal = sim->externalTransitionConflit(
            *modelbag.internal(), modelbag.externals());
    } else {
        internal = sim->externalTransitionConflit(
            *modelbag.internal(), modelbag.externals());
    }

    modelbag.delInternal();
    modelbag.externals().deleteAndClear();
    modelbag.delExternals();
    processEventView(*sim, 0);

    if (internal) {
        m_eventTable.putInternalEvent(internal);
    }
}

void Coordinator::processRequestEvents(
    Simulator* sim,
    EventBagModel& modelbag)
{
    RequestEventList& lst(modelbag.Request());

    ExternalEventList result;
    for (RequestEventList::iterator it = lst.begin(); it != lst.end();
         ++it) {
        sim->request(**it, m_currentTime, result);
        dispatchExternalEvent(result, sim);
        result.clear();
    }

    lst.deleteAndClear();
    modelbag.delRequest();
}

void Coordinator::processTimedObservationEvents(ObservationEventList& bag)
{
    for (ObservationEventList::iterator it = bag.begin();
         it != bag.end(); ++it) {
        if ((*it)->isValid()) {
            Simulator* model((*it)->getModel());
            ObservationEvent* event(model->observation(*(*it)));

            if (event) {
                View* view(getView(event->getViewName()));
                ObservationEvent* event2(view->processObservationEvent(event));
                delete event;

                if (event2 and model->getStructure()) {
                    m_eventTable.putObservationEvent(event2);
                }
            }
        }
        delete *it;
    }
    bag.clear();
}

void Coordinator::setCoordinator(Executive& exe)
{
    exe.m_coordinator = this;
}

}} // namespace vle devs
