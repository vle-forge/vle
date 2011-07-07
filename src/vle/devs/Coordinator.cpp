/*
 * @file vle/devs/Coordinator.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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
#include <vle/utils/Path.hpp>
#include <vle/value/Value.hpp>

using std::vector;
using std::map;
using std::string;
using std::pair;

namespace vle { namespace devs {

Coordinator::Coordinator(ModelFactory& modelfactory) :
    m_currentTime(0),
    m_modelFactory(modelfactory),
    m_isStarted(false)
{
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

void Coordinator::init(const vpz::Model& mdls, const Time& current,
                       const Time& duration)
{
    m_currentTime = current;
    m_durationTime = duration;
    buildViews();
    addModels(mdls);
    m_toDelete = 0;
    m_isStarted = true;
}

const Time& Coordinator::getNextTime()
{
    return m_eventTable.topEvent();
}

ExternalEventList* Coordinator::run()
{
    DTraceDevs(_("-------- BAG --------"));
    SimulatorList::size_type oldToDelete(m_toDelete);

    CompleteEventBagModel& bags = m_eventTable.popEvent();
    if (not bags.empty()) {
        updateCurrentTime(m_eventTable.getCurrentTime());
    }

    while (not bags.emptyBag()) {
        std::map < Simulator*, EventBagModel >::value_type& bag(bags.topBag());
        if (not bag.second.emptyInternal()) {
            if (not bag.second.emptyExternal()) {
                processConflictEvents(bag.first, bag.second);
            } else {
                processInternalEvent(bag.first, bag.second);
            }
        } else {
            if (not bag.second.emptyExternal()) {
                processExternalEvents(bag.first, bag.second);
            }
        }
        if (not bag.second.emptyRequest()) {
            processRequestEvents(bag.first, bag.second);
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
            m_obsEventBuffer.insert(bags.states().begin(),
                                    bags.states().end());
            bags.states().clear();
        } else {
            processViewEvents(bags.states());
            processViewEvents(m_obsEventBuffer);
            bags.states().erase();
            m_obsEventBuffer.erase();
        }
    } else if (not m_obsEventBuffer.empty()) {
        if (getNextTime() != m_obsEventBuffer.front()->getTime()) {
            processViewEvents(m_obsEventBuffer);
            m_obsEventBuffer.erase();
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
                              const std::vector < std::string >& conditions,
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

    if (it == m_viewList.end()) {
        throw utils::InternalError(fmt(_(
            "The view '%1%' is unknow of coordinator view list")) % view);
    }

    Simulator* simulator = getModel(model);

    if (not simulator) {
        throw utils::InternalError(fmt(_(
                "The simulator of the model '%1%' does not exist")) %
            simulator->getStructure()->getName());
    }

    View* obs = it->second;

    obs->addObservable(simulator, portname, m_currentTime);
}

void Coordinator::delModel(graph::CoupledModel* parent,
                           const std::string& modelname)
{
    graph::Model* mdl = parent->findModel(modelname);

    if (not mdl) {
        throw utils::InternalError(fmt(_(
                "Cannot delete an unknow model '%1%'")) % modelname);
    }

    if (mdl->isCoupled()) {
        delCoupledModel(parent, (graph::CoupledModel*)mdl);
        parent->delAllConnection(mdl);
        parent->delModel(mdl);
    } else {
        delAtomicModel(parent, (graph::AtomicModel*)mdl);
        parent->delModel(mdl);
    }
}

void Coordinator::getSimulatorsSource(
    graph::Model* model,
    std::vector < std::pair < Simulator*, std::string > >& lst)
{
    if (m_isStarted) {
        graph::ConnectionList& inputs(model->getInputPortList());
        for (graph::ConnectionList::iterator it = inputs.begin(); it !=
             inputs.end(); ++it) {

            const std::string& port(it->first);
            getSimulatorsSource(model, port, lst);
        }
    }
}

void Coordinator::getSimulatorsSource(
    graph::Model* model,
    const std::string& port,
    std::vector < std::pair < Simulator*, std::string > >& lst)
{
    if (m_isStarted) {
        graph::ModelPortList result;
        model->getAtomicModelsSource(port, result);

        for (graph::ModelPortList::iterator jt = result.begin();
             jt != result.end(); ++jt) {
            lst.push_back(
                std::make_pair(
                    getModel(
                        reinterpret_cast < graph::AtomicModel* >(jt->first)),
                    jt->second));
        }
    }
}

void Coordinator::updateSimulatorsTarget(
    std::vector < std::pair < Simulator*, std::string > >& lst)
{
    if (m_isStarted) {
        for (std::vector < std::pair < Simulator*, std::string > >::iterator
             it = lst.begin(); it != lst.end(); ++it) {
            if (it->first != 0) {
                it->first->updateSimulatorTargets(it->second, m_modelList);
            }
        }
    }
}

void Coordinator::addSimulatorTargetPort(graph::AtomicModel* model,
                                         const std::string& port)
{
    getModel(model)->addTargetPort(port);
}

void Coordinator::removeSimulatorTargetPort(graph::AtomicModel* model,
                                            const std::string& port)
{
    getModel(model)->removeTargetPort(port);
}

// / / / /
//
// Some usefull functions.
//
// / / / /

void Coordinator::addModel(graph::AtomicModel* model, Simulator* simulator)
{
    std::pair < SimulatorMap::iterator, bool > r =
        m_modelList.insert(std::make_pair(model, simulator));

    if (not r.second) {
        throw utils::InternalError(fmt(_(
                    "The Atomic model node '%1% have already a simulator"))
            % model->getName());
    }
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
    if (not parent or not atom) {
        throw utils::DevsGraphError(_(
            "Cannot delete an atomic model without parent"));
    }

    SimulatorMap::iterator it = m_modelList.find(atom);
    if (it == m_modelList.end()) {
        throw utils::ModellingError(_(
            "Cannot delete an unknow atomic model"));
    }

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

    m_obsEventBuffer.remove(satom);

    ++m_toDelete;
}

void Coordinator::delCoupledModel(graph::CoupledModel* parent,
                                  graph::CoupledModel* mdl)
{
    if (not parent or not mdl) {
        throw utils::DevsGraphError(_(
            "Cannot delete an atomic model without parent"));
    }

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

void Coordinator::dispatchExternalEvent(ExternalEventList& eventList,
                                        Simulator* sim)
{
    for (ExternalEventList::iterator it = eventList.begin(); it !=
         eventList.end(); ++it) {
        (*it)->setModel(sim);

        std::pair < Simulator::iterator, Simulator::iterator > x;
        x = sim->targets((*it)->getPortName(), m_modelList);

        if (x.first == x.second or x.first->second.first == 0) {
            (*it)->deleter(); // it must delete allocated values, else
        } else {              // the first newly external manage values.
            for (Simulator::iterator jt = x.first; jt != x.second; ++jt) {
                Simulator* dst = jt->second.first;
                const std::string& port(jt->second.second);

                if ((*it)->isRequest()) {
                    m_eventTable.putRequestEvent(
                        new RequestEvent(
                            reinterpret_cast < RequestEvent* >((*it)),
                            dst, port, jt == x.first));
                } else {
                    m_eventTable.putExternalEvent(
                        new ExternalEvent((*it), dst, port,
                                          jt == x.first));
                }
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
            TimedView* v = new TimedView(it->second.name(), stream,
                                         it->second.timestep());
            m_timedViewList[it->second.name()] = v;
            obs = v;
            m_eventTable.putObservationEvent(
                new ViewEvent(obs, m_currentTime));
        } else if (it->second.type() == vpz::View::EVENT) {
            EventView* v = new EventView(it->second.name(), stream);
            m_eventViewList[it->second.name()] = v;
            obs = v;
        } else if (it->second.type() == vpz::View::FINISH) {
            FinishView* v = new devs::FinishView(it->second.name(), stream,
                                                 m_durationTime);
            m_finishViewList[it->second.name()] = v;
            obs = v;
            m_eventTable.putObservationEvent(
                new ViewEvent(obs, m_durationTime));
        }
        m_viewList[it->second.name()] = obs;
        stream->setView(obs);
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

    std::string file((fmt("%1%_%2%") %
                      m_modelFactory.experiment().name() %
                      view.name()).str());
    stream->open(output.plugin(), output.package(), output.location(), file,
                 (output.data()) ? output.data()->clone() : 0, m_currentTime);

    return stream;
}

void Coordinator::processInternalEvent(
    Simulator* sim,
    const EventBagModel& modelbag)
{
    const InternalEvent* ev = modelbag.internal();

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

    processEventView(sim);
}

void Coordinator::processExternalEvents(
    Simulator* sim,
    const EventBagModel& modelbag)
{
    const ExternalEventList& lst(modelbag.externals());

    {
        InternalEvent* internal(sim->externalTransition(lst, m_currentTime));
        if (internal) {
            m_eventTable.putInternalEvent(internal);
        }
    }

    processEventView(sim);
}

void Coordinator::processConflictEvents(
    Simulator* sim,
    const EventBagModel& modelbag)
{
    {
        ExternalEventList result;
        sim->output(m_currentTime, result);
        dispatchExternalEvent(result, sim);
    }

    InternalEvent* internal = sim->confluentTransitions(
        *modelbag.internal(), modelbag.externals());

    processEventView(sim);

    if (internal) {
        m_eventTable.putInternalEvent(internal);
    }
}

void Coordinator::processRequestEvents(
    Simulator* sim,
    const EventBagModel& modelbag)
{
    const RequestEventList& lst(modelbag.Request());

    ExternalEventList result;
    for (RequestEventList::const_iterator it = lst.begin(); it != lst.end();
         ++it) {
        sim->request(**it, m_currentTime, result);
        dispatchExternalEvent(result, sim);
        result.clear();
    }
}

void Coordinator::processEventView(Simulator* model)
{
    for (EventViewList::iterator it = m_eventViewList.begin(); it !=
         m_eventViewList.end(); ++it) {

        if (it->second->exist(model)) {
            it->second->run(m_currentTime);
        }
    }
}

void Coordinator::processViewEvents(ViewEventList& bag)
{
    for (ViewEventList::iterator it = bag.begin(); it != bag.end(); ++it) {
        (*it)->run(m_currentTime);
        (*it)->update(m_currentTime);

        m_eventTable.putObservationEvent(
            new ViewEvent((*it)->getView(), (*it)->getTime()));
    }
}

}} // namespace vle devs
