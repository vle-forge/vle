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
#include <vle/utils/ContextPrivate.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/InternalEvent.hpp>
#include <vle/devs/ExternalEventList.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/vpz/BaseModel.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <functional>
#include <boost/bind.hpp>

using std::vector;
using std::map;
using std::string;
using std::pair;

namespace {

/** Compute the depth of the hierarchy.
 *
 * @return @e depth returns 0 if executive is in the top model otherwise a
 * internet less than 0.
 */
inline
int
depth(const std::unique_ptr<vle::devs::Dynamics>& mdl) noexcept
{
    int ret = 0;

    const vle::vpz::CoupledModel *parent = mdl->getModel().getParent();
    while (parent != nullptr) {
        parent = parent->getParent();
        --ret;
    }

    return ret;
}

}

namespace vle { namespace devs {

Coordinator::Coordinator(utils::ContextPtr context,
                         const vpz::Dynamics& dyn,
                         const vpz::Classes& cls,
                         const vpz::Experiment& experiment,
                         RootCoordinator& root)
    : m_context(context)
    , m_currentTime(0.0)
    , m_modelFactory(context, m_eventViewList, dyn, cls, experiment, root)
    , m_isStarted(false)
{
}

Coordinator::~Coordinator()
{
    std::for_each(m_modelList.begin(),
                  m_modelList.end(),
                  [](const SimulatorMap::value_type& pair)
                  {
                      delete pair.second;
                  });
}

void Coordinator::init(const vpz::Model& mdls, Time current, Time duration)
{
    m_currentTime = current;
    m_durationTime = duration;
    buildViews();
    addModels(mdls);
    m_toDelete = 0;
    m_isStarted = true;

    m_eventTable.init(current);
}

void Coordinator::run()
{
    Bag& bags = m_eventTable.getCurrentBag();
    if (not bags.empty())
        m_currentTime = m_eventTable.getCurrentTime();

    vDbg(m_context, _("-------- BAG [%f] --------\n"), m_currentTime);
    SimulatorList::size_type oldToDelete(m_toDelete);

    std::vector<Bag::value_type> executives;
    for (auto & elem: bags) {
        if (elem->dynamics()->isExecutive()) {
            executives.emplace_back(elem);
            continue;
        }

        if (elem->haveInternalEvent()) {
            if (not elem->haveExternalEvents())
                processInternalEvent(elem);
            else
                processConflictEvents(elem);
        } else {
            assert(elem->haveExternalEvents() && "unknown simulator state");
            processExternalEvents(elem);
        }
    }

    if (not executives.empty()) {
        std::sort(executives.begin(), executives.end(),
                  [](const Bag::value_type& lhs,
                     const Bag::value_type& rhs)
                  {
                      return ::depth(lhs->dynamics()) < ::depth(rhs->dynamics());
                  });

        for (auto & elem: executives) {
            if (elem->haveInternalEvent()) {
                if (not elem->haveExternalEvents())
                    processInternalEvent(elem);
                else
                    processConflictEvents(elem);
            } else {
                assert(elem->haveExternalEvents() && "unknow simulator state");
                processExternalEvents(elem);
            }
        }
    }

    // If there is model to delete, we remove models that are destroyed in
    // previous \e run() call.
    if (oldToDelete > 0) {
        auto begin = m_deletedSimulator.begin();
        auto end = m_deletedSimulator.begin() + oldToDelete;

        for (auto it = begin; it != end; ++it) {
            m_eventTable.delSimulator(*it);
            delete *it;
            *it = nullptr;
        }

        m_deletedSimulator.erase(begin, end);
        m_toDelete = m_deletedSimulator.size();
    }

    /* Process observation event if the next bag is scheduled for a
     * different date than \e m_currentTime.
     */
    auto next = m_eventTable.getNextTime();
    if (next > m_currentTime) {
        /* Scheduler is empty. We 'eat' all timed view until the
         * duration time.
         */
        auto eatuntil = std::min(next, m_durationTime);

        while (m_timed_observation_scheduler.haveObservationAtTime(eatuntil)) {
            auto obs = m_timed_observation_scheduler.getObservationAtTime(
                eatuntil);

                if (not obs.empty()) {
                    m_currentTime = obs.back().mTime;

                    for (auto & elem : obs) {
                        elem.run();
                        elem.update();

                        if (not isInfinity(elem.mTime))
                            m_timed_observation_scheduler.add(elem.mView,
                                                              elem.mTime,
                                                              elem.mTimestep);
                }
            }
        }

        if (isInfinity(next) or next > m_durationTime) {
            /* For all Timed view, process a final observation and clear
             * the scheduller.
             */
            m_currentTime = m_durationTime;
            m_timed_observation_scheduler.finalize(m_currentTime);
        }
    }

    m_eventTable.makeNextBag();
    m_currentTime = m_eventTable.getCurrentTime();
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
    Simulator* simulator = getModel(model);

    assert(simulator && "Coordinator: simulator must exist");

    auto event_it = m_eventViewList.find(view);
    auto timed_it = m_timedViewList.find(view);

    if (event_it == m_eventViewList.end() and
        timed_it == m_timedViewList.end())
        throw utils::InternalError(
            (fmt(_("The view '%1%' is unknown")) % view).str());


    if (event_it != m_eventViewList.end())
        event_it->second.addObservable(simulator->dynamics().get(),
                                       portname, m_currentTime);
    else
        timed_it->second.addObservable(simulator->dynamics().get(),
                                       portname, m_currentTime);
}

void Coordinator::delModel(vpz::CoupledModel* parent,
                           const std::string& modelname)
{
    vpz::BaseModel* mdl = parent->findModel(modelname);

    if (not mdl) {
        throw utils::InternalError(
            (fmt(_("Cannot delete an unknown model '%1%'")) % modelname).str());
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
        for (auto & input : inputs) {

            const std::string& port(input.first);
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

        for (auto & elem : result) {
            lst.push_back(
                std::make_pair(
                    getModel(
                        reinterpret_cast < vpz::AtomicModel* >(elem.first)),
                    elem.second));
        }
    }
}

void Coordinator::updateSimulatorsTarget(
    std::vector < std::pair < Simulator*, std::string > >& lst)
{
    if (m_isStarted) {
        for (auto & elem : lst) {
            if (elem.first != nullptr) {
                elem.first->updateSimulatorTargets(elem.second, m_modelList);
            }
        }
    }
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
        throw utils::InternalError(
            (fmt(_("The Atomic model node '%1% have already a simulator"))
             % model->getName()).str());
    }
}

Simulator* Coordinator::getModel(const vpz::AtomicModel* model) const
{
    auto it =
        m_modelList.find( const_cast < vpz::AtomicModel* >(model));
    return (it == m_modelList.end()) ? nullptr : it->second;
}

Simulator* Coordinator::getModel(const std::string& name) const
{
    auto it = m_modelList.begin();
    while (it != m_modelList.end()) {
        if ((*it).first->getName() == name) {
            return (*it).second;
        }
        ++it;
    }
    return nullptr;
}

///
/// Private functions.
///

void Coordinator::delAtomicModel(vpz::AtomicModel* atom)
{
    assert(atom && "Cannot delete undefined atomic model");

    auto it = m_modelList.find(atom);
    assert(it != m_modelList.end() && "Cannot delete an unknown atomic model");

    Simulator* satom = (*it).second;
    m_modelList.erase(it);

    for (auto& elem : m_eventViewList)
        elem.second.removeObservable(satom->dynamics().get());

    for (auto& elem : m_timedViewList)
        elem.second.removeObservable(satom->dynamics().get());

    m_eventTable.delSimulator(satom);
    satom->clear();
    m_deletedSimulator.push_back(satom);

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
    for (auto & elem : eventList) {

        std::pair < Simulator::iterator, Simulator::iterator > x;
        x = sim->targets((elem).getPortName(), m_modelList);

        if (x.first != x.second and x.first->second.first) {
            for (auto jt = x.first; jt != x.second; ++jt)
                m_eventTable.addExternal(jt->second.first,
                                         elem.attributes(),
                                         jt->second.second);
        }
    }

    eventList.clear();
}

void Coordinator::buildViews()
{
    const vpz::Outputs& outs(m_modelFactory.outputs());
    const vpz::Views& views(m_modelFactory.views());
    const vpz::ViewList& viewlist(views.viewlist());

    for (const auto & elem : viewlist) {

        std::string file = utils::format("%s_%s",
                                         m_modelFactory.experiment().name().c_str(),
                                         elem.first.c_str());

        const auto& output = outs.get(elem.second.output());

        if (elem.second.type() == vpz::View::TIMED) {
            View& v = m_timedViewList[elem.second.name()];

            v.open(m_context, elem.second.name(), output.plugin(),
                   output.package(), output.location(), file,
                   m_currentTime,
                   (output.data()) ? output.data()->clone() : nullptr);

            m_timed_observation_scheduler.add(&v, m_currentTime,
                                              elem.second.timestep());
        } else {
            auto& v = m_eventViewList[elem.second.name()];

            v.open(m_context, elem.second.name(), output.plugin(),
                   output.package(), output.location(), file,
                   m_currentTime,
                   (output.data()) ? output.data()->clone() : nullptr);
        }
    }
}

void Coordinator::processInit(Simulator *simulator)
{
    Time tn = simulator->init(m_currentTime);

    if (not isInfinity(tn)) {
        m_eventTable.addInternal(simulator, tn);
    }
}

void Coordinator::processInternalEvent(Bag::value_type& modelbag)
{
    ExternalEventList result; // TODO perhaps use an attribute to cahce
                              // the malloc, realloc, etc.
    modelbag->output(result, m_currentTime);
    dispatchExternalEvent(result, modelbag);

    Time tn = modelbag->internalTransition(m_currentTime);
    if (not isInfinity(tn))
        m_eventTable.addInternal(modelbag, tn);
}

void Coordinator::processExternalEvents(Bag::value_type& modelbag)
{
    Time tn = modelbag->externalTransition(m_currentTime);

    if (not isInfinity(tn))
        m_eventTable.addInternal(modelbag, tn);
}

void Coordinator::processConflictEvents(Bag::value_type& modelbag)
{
    ExternalEventList result; // TODO perhaps use an attribute to cache
                              // the malloc, realloc, etc.
    modelbag->output(result, m_currentTime);
    dispatchExternalEvent(result, modelbag);

    Time tn = modelbag->confluentTransitions(m_currentTime);

    if (not isInfinity(tn))
        m_eventTable.addInternal(modelbag, tn);
}

std::unique_ptr<value::Map> Coordinator::finish()
{
    //delete models
    std::for_each(m_modelList.begin(), m_modelList.end(),
                  boost::bind(
                      &Simulator::finish,
                      boost::bind(&SimulatorMap::value_type::second, _1)));

    //build result views
    std::unique_ptr<value::Map> result;

    for (auto& elem : m_timedViewList) {
        auto matrix = elem.second.finish(m_currentTime);
        if (matrix) {
            if (not result)
                result = std::unique_ptr<value::Map>(new value::Map());

            result->add(elem.first, std::move(matrix));
        }
    }

    for (auto& elem : m_eventViewList) {
        auto matrix = elem.second.finish(m_currentTime);
        if (matrix) {
            if (not result)
                result = std::unique_ptr<value::Map>(new value::Map());
            result->add(elem.first, std::move(matrix));
        }
    }

    return result;
}

std::unique_ptr<value::Map> Coordinator::getMap() const
{
    std::unique_ptr<value::Map> result;

    for (const auto& elem : m_timedViewList) {
        auto matrix = elem.second.matrix();

        if (matrix) {
            if (not result)
                result = std::unique_ptr<value::Map>(new value::Map());

            result->add(elem.first, std::move(matrix));
        }
    }

    for (const auto& elem : m_eventViewList) {
        auto matrix = elem.second.matrix();

        if (matrix) {
            if (not result)
                result = std::unique_ptr<value::Map>(new value::Map());

            result->add(elem.first, std::move(matrix));
        }
    }

    return result;
}

}} // namespace vle devs
