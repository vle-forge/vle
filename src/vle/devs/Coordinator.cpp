/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/devs/Coordinator.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/InternalEvent.hpp>
#include <vle/devs/ExternalEventList.hpp>
#include <vle/devs/StreamWriter.hpp>
#include <vle/vpz/BaseModel.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/utils/Trace.hpp>
#include <functional>
#include <boost/bind.hpp>

using std::vector;
using std::map;
using std::string;
using std::pair;

namespace vle { namespace devs {

Coordinator::Coordinator(const utils::ModuleManager& modulemgr,
                         const vpz::Dynamics& dyn,
                         const vpz::Classes& cls,
                         const vpz::Experiment& experiment,
                         RootCoordinator& root)
    : m_currentTime(0.0), m_modelFactory(modulemgr, dyn, cls, experiment, root),
      m_modulemgr(modulemgr), m_isStarted(false)
{
}

Coordinator::~Coordinator()
{
    std::for_each(m_modelList.begin(),
                  m_modelList.end(),
                  boost::bind(
                      boost::checked_deleter < Simulator >(),
                      boost::bind(&SimulatorMap::value_type::second, _1)));

    std::for_each(m_viewList.begin(),
                  m_viewList.end(),
                  boost::bind(
                      boost::checked_deleter < View >(),
                      boost::bind(&ViewList::value_type::second, _1)));
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

void Coordinator::run()
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
}

void Coordinator::finish()
{
    std::for_each(m_modelList.begin(), m_modelList.end(),
                  boost::bind(
                      &Simulator::finish,
                      boost::bind(&SimulatorMap::value_type::second, _1)));

    std::for_each(m_viewList.begin(), m_viewList.end(),
                  boost::bind(
                      &View::finish,
                      boost::bind(&ViewList::value_type::second, _1),
                      m_currentTime));
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

void Coordinator::createModel(vpz::AtomicModel* model,
                              const std::string& dynamics,
                              const std::vector < std::string >& conditions,
                              const std::string& observable)
{
    m_modelFactory.createModel(*this, model, dynamics, conditions,
                               observable);
}

vpz::BaseModel* Coordinator::createModelFromClass(const std::string& classname,
                                                vpz::CoupledModel* parent,
                                                const std::string& modelname)
{
    return m_modelFactory.createModelFromClass(*this, parent,
                                               classname, modelname);
}

void Coordinator::addObservableToView(vpz::AtomicModel* model,
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

void Coordinator::delModel(vpz::CoupledModel* parent,
                           const std::string& modelname)
{
    vpz::BaseModel* mdl = parent->findModel(modelname);

    if (not mdl) {
        throw utils::InternalError(fmt(_(
                "Cannot delete an unknown model '%1%'")) % modelname);
    }

    if (mdl->isCoupled()) {
        delCoupledModel(static_cast < vpz::CoupledModel* >(mdl));
    } else {
        delAtomicModel(static_cast < vpz::AtomicModel* >(mdl));
    }

    parent->delModel(mdl); // finally, we remove the model from the
                           // vpz::CoupledModel object.
}

void Coordinator::getSimulatorsSource(
    vpz::BaseModel* model,
    std::vector < std::pair < Simulator*, std::string > >& lst)
{
    if (m_isStarted) {
        vpz::ConnectionList& inputs(model->getInputPortList());
        for (vpz::ConnectionList::iterator it = inputs.begin(); it !=
             inputs.end(); ++it) {

            const std::string& port(it->first);
            getSimulatorsSource(model, port, lst);
        }
    }
}

void Coordinator::getSimulatorsSource(
    vpz::BaseModel* model,
    const std::string& port,
    std::vector < std::pair < Simulator*, std::string > >& lst)
{
    if (m_isStarted) {
        vpz::ModelPortList result;
        model->getAtomicModelsSource(port, result);

        for (vpz::ModelPortList::iterator jt = result.begin();
             jt != result.end(); ++jt) {
            lst.push_back(
                std::make_pair(
                    getModel(
                        reinterpret_cast < vpz::AtomicModel* >(jt->first)),
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

void Coordinator::addSimulatorTargetPort(vpz::AtomicModel* model,
                                         const std::string& port)
{
    getModel(model)->addTargetPort(port);
}

void Coordinator::removeSimulatorTargetPort(vpz::AtomicModel* model,
                                            const std::string& port)
{
    getModel(model)->removeTargetPort(port);
}

// / / / /
//
// Some usefull functions.
//
// / / / /

void Coordinator::addModel(vpz::AtomicModel* model, Simulator* simulator)
{
    std::pair < SimulatorMap::iterator, bool > r =
        m_modelList.insert(std::make_pair(model, simulator));

    if (not r.second) {
        throw utils::InternalError(fmt(_(
                    "The Atomic model node '%1% have already a simulator"))
            % model->getName());
    }
}

Simulator* Coordinator::getModel(const vpz::AtomicModel* model) const
{
    SimulatorMap::const_iterator it =
        m_modelList.find( const_cast < vpz::AtomicModel* >(model));
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

///
/// Private functions.
///

void Coordinator::delAtomicModel(vpz::AtomicModel* atom)
{
    if (not atom) {
        throw utils::DevsGraphError(
            _("Cannot delete undefined atomic model"));
    }

    SimulatorMap::iterator it = m_modelList.find(atom);
    if (it == m_modelList.end()) {
        throw utils::ModellingError(
            _("Cannot delete an unknown atomic model"));
    }

    Simulator* satom = (*it).second;
    m_modelList.erase(it);

    std::map < std::string , View* >::iterator it2;
    for (it2 = m_viewList.begin(); it2 != m_viewList.end();
         ++it2) {
        View* View = (*it2).second;
        View->removeObservable(satom);
    }
    m_eventTable.delModelEvents(satom);
    satom->clear();
    m_deletedSimulator.push_back(satom);

    m_obsEventBuffer.remove(satom);

    ++m_toDelete;
}

void Coordinator::delCoupledModel(vpz::CoupledModel* mdl)
{
    if (not mdl) {
        throw utils::DevsGraphError(
            _("Cannot delete undefined coupled model"));
    }

    std::vector < vpz::AtomicModel* > lst;
    lst.reserve(16);

    vpz::BaseModel::getAtomicModelList(mdl, lst);

    std::for_each(lst.begin(), lst.end(),
                  std::bind1st(
                      std::mem_fun(&Coordinator::delAtomicModel), this));
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

        std::pair < Simulator::iterator, Simulator::iterator > x;
        x = sim->targets((*it)->getPortName(), m_modelList);

        if (x.first != x.second and x.first->second.first) {
            for (Simulator::iterator jt = x.first; jt != x.second; ++jt) {
                m_eventTable.putExternalEvent(
                    new ExternalEvent(*(*it),
                                      jt->second.first,
                                      jt->second.second));
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
            FinishView* v = new devs::FinishView(it->second.name(), stream);
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
    StreamWriter* stream = new StreamWriter(m_modulemgr);

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
