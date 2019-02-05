/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2003-2018 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2018 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2018 INRA http://www.inra.fr
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

#include <vle/devs/Dynamics.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/ExternalEventList.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/vpz/BaseModel.hpp>
#include <vle/vpz/CoupledModel.hpp>

#include "devs/Coordinator.hpp"
#include "devs/InternalEvent.hpp"
#include "devs/Simulator.hpp"
#include "devs/Thread.hpp"
#include "utils/ContextPrivate.hpp"
#include "utils/i18n.hpp"

#include <boost/bind.hpp>

#include <functional>
#include <memory>

using std::map;
using std::pair;
using std::string;
using std::vector;

namespace {

/** Compute the depth of the hierarchy.
 *
 * @return @e depth returns 0 if executive is in the top model otherwise a
 * internet less than 0.
 */
inline int
depth(const std::unique_ptr<vle::devs::Dynamics>& mdl) noexcept
{
    int ret = 0;

    const vle::vpz::CoupledModel* parent = mdl->getModel().getParent();
    while (parent != nullptr) {
        parent = parent->getParent();
        --ret;
    }

    return ret;
}
}

namespace vle {
namespace devs {

Coordinator::Coordinator(utils::ContextPtr context,
                         const vpz::Dynamics& dyn,
                         const vpz::Classes& cls,
                         const vpz::Experiment& experiment)
  : m_context(context)
  , m_currentTime(0.0)
  , m_simulators_thread_pool(m_context)
  , m_modelFactory(context, m_eventViewList, dyn, cls, experiment)
  , m_isStarted(false)
{}

void
Coordinator::init(const vpz::Model& mdls,
                  Time current,
                  Time duration,
                  long instance)
{
    m_currentTime = current;
    m_durationTime = duration;
    buildViews(instance);
    addModels(mdls);
    m_isStarted = true;

    m_eventTable.init(current);
}

void
Coordinator::run()
{
    Bag& bag = m_eventTable.getCurrentBag();
    if (not bag.dynamics.empty() or not bag.executives.empty())
        m_currentTime = m_eventTable.getCurrentTime();
    else
        m_context->debug(_("Coordinator::run bag is empty...\n"));

    m_context->debug(_("-------- BAG [%f] --------\n"), m_currentTime);

    //
    // Call output functions for all executives and dynamics models then
    // dispatches external events for all executives and dynamics.
    //

    const std::size_t nb_dynamics = bag.dynamics.size();
    const std::size_t nb_executive = bag.executives.size();

    if (nb_dynamics > 0) {
        for (std::size_t i = 0; i != nb_dynamics; ++i)
            bag.dynamics[i]->output(m_currentTime);

        dispatchExternalEvent(bag.dynamics, nb_dynamics);
    }

    if (nb_executive > 0) {
        for (std::size_t i = 0; i != nb_executive; ++i)
            bag.executives[i]->output(m_currentTime);

        dispatchExternalEvent(bag.executives, nb_executive);
    }

    //
    // First we sort executives models according to the depth of the executive
    // model into the structure of the models.
    //
    if (not bag.executives.empty()) {
        std::sort(bag.executives.begin(),
                  bag.executives.end(),
                  [](const Simulator* lhs, const Simulator* rhs) {
                      return ::depth(lhs->dynamics()) <
                             ::depth(rhs->dynamics());
                  });
    }

    //
    // Compute internal, confluent or external transition for dynamics models.
    // If parallelization is available, use it otherwise, compute transition
    // linearly.
    //
    if (m_simulators_thread_pool.parallelize()) {
        m_simulators_thread_pool.for_each(bag.dynamics, m_currentTime);
    } else {
        for (auto& elem : bag.dynamics) {
            if (elem->haveInternalEvent()) {
                if (not elem->haveExternalEvents())
                    elem->internalTransition(m_currentTime);
                else
                    elem->confluentTransitions(m_currentTime);
            } else {
                elem->externalTransition(m_currentTime);
            }
        }
    }

    for (auto& elem : bag.dynamics) {
        auto tn = elem->getTn();
        if (not isInfinity(tn))
            m_eventTable.addInternal(elem, tn);
    }

    for (auto& elem : bag.executives) {
        if (elem->haveInternalEvent()) {
            if (not elem->haveExternalEvents())
                elem->internalTransition(m_currentTime);
            else
                elem->confluentTransitions(m_currentTime);
        } else {
            elem->externalTransition(m_currentTime);
        }
    }

    for (auto& elem : bag.executives) {
        auto tn = elem->getTn();
        if (not isInfinity(tn))
            m_eventTable.addInternal(elem, tn);
    }

    //
    // Finally, we go through simulators and executive to get all observation
    // and dispatch to output plug-in.
    //
    for (auto& elem : bag.dynamics) {
        auto& observations = elem->getObservations();
        for (auto& obs : observations)
            obs.view->run(elem->dynamics().get(),
                          m_currentTime,
                          obs.portname,
                          std::move(obs.value));

        observations.clear();
    }

    for (auto& elem : bag.executives) {
        auto& observations = elem->getObservations();
        for (auto& obs : observations)
            obs.view->run(elem->dynamics().get(),
                          m_currentTime,
                          obs.portname,
                          std::move(obs.value));

        observations.clear();
    }

    //
    // Process observation event if the next bag is scheduled for a different
    // date than \e m_currentTime.
    //
    auto next = m_eventTable.getNextTime();
    if (next > m_currentTime) {

        //
        // Scheduler is empty. We eat all timed view until the duration time
        //
        auto eatuntil = std::min(next, m_durationTime);

        while (m_timed_observation_scheduler.haveObservationAtTime(eatuntil)) {
            auto obs =
              m_timed_observation_scheduler.getObservationAtTime(eatuntil);

            if (not obs.empty()) {
                m_currentTime = obs.back().mTime;

                for (auto& elem : obs) {
                    elem.run();
                    elem.update();

                    if (not isInfinity(elem.mTime))
                        m_timed_observation_scheduler.add(
                          elem.mView, elem.mTime, elem.mTimestep);
                }
            }
        }

        if (isInfinity(next) or next > m_durationTime) {
            //
            // For all Timed view, process a final observation and clear the
            // scheduler.
            //
            m_currentTime = m_durationTime;
            m_timed_observation_scheduler.finalize(m_currentTime);
        }
    }

    //
    // Finally, we destroy model and simulator if one executive delete a model
    //
    if (not m_delete_model.empty())
        dynamic_deletion();

    m_eventTable.makeNextBag();
    m_currentTime = m_eventTable.getCurrentTime();
}

void
Coordinator::createModel(vpz::AtomicModel* model,
                         const vpz::Conditions& experiment_conditions,
                         const std::string& dynamics,
                         const std::vector<std::string>& conditions,
                         const std::string& observable)
{
    m_modelFactory.createModel(
      *this, experiment_conditions, model, dynamics, conditions, observable);
}

vpz::BaseModel*
Coordinator::createModelFromClass(const std::string& classname,
                                  vpz::CoupledModel* parent,
                                  const std::string& modelname,
                                  const vpz::Conditions& conditions,
                                  const std::vector<std::string>& inputs,
                                  const std::vector<std::string>& outputs)
{
    return m_modelFactory.createModelFromClass(
      *this, parent, classname, modelname, conditions, inputs, outputs);
}

void
Coordinator::addObservableToView(vpz::AtomicModel* model,
                                 const std::string& portname,
                                 const std::string& view)
{
    assert(model);
    assert(model->get_simulator());

    Simulator* simulator = model->get_simulator();

    auto event_it = m_eventViewList.find(view);
    auto timed_it = m_timedViewList.find(view);

    if (event_it != m_eventViewList.end())
        event_it->second.addObservable(
          simulator->dynamics().get(), portname, m_currentTime);
    else if (timed_it != m_timedViewList.end())
        timed_it->second.addObservable(
          simulator->dynamics().get(), portname, m_currentTime);
}

void
Coordinator::prepare_dynamic_deletion(vpz::BaseModel* model)
{
    assert(model);

    m_delete_model.emplace_back(model);
}

void
Coordinator::dynamic_deletion()
{
    std::vector<std::pair<Simulator*, std::string>> toupdate;
    std::vector<Simulator*> lst;

    for (auto& elem : m_delete_model) {
        getSimulatorsSource(elem, toupdate);

        auto* parent = elem->getParent();

        if (elem->isCoupled())
            delCoupledModel(static_cast<vpz::CoupledModel*>(elem), lst);
        else
            delAtomicModel(static_cast<vpz::AtomicModel*>(elem), lst);

        if (parent)
            parent->delModel(elem);

        updateSimulatorsTarget(toupdate);
        toupdate.clear();
    }

    m_delete_model.clear();

    for (auto& elem : lst) {
        m_eventTable.delSimulator(elem);

        for (auto it = m_simulators.begin(), et = m_simulators.end(); it != et;
             ++it) {
            if (it->get() == elem) {
                elem->finish();
                auto& observations = elem->getObservations();
                for (auto& obs : observations)
                    obs.view->run(elem->dynamics().get(),
                                  m_currentTime,
                                  obs.portname,
                                  std::move(obs.value));

                observations.clear();
                m_simulators.erase(it);
                break;
            }
        }
    }
}

void
Coordinator::getSimulatorsSource(
  vpz::BaseModel* model,
  std::vector<std::pair<Simulator*, std::string>>& lst)
{
    if (m_isStarted) {
        vpz::ConnectionList& inputs(model->getInputPortList());
        for (auto& input : inputs) {
            const std::string& port(input.first);
            getSimulatorsSource(model, port, lst);
        }
    }
}

void
Coordinator::getSimulatorsSource(
  vpz::BaseModel* model,
  const std::string& port,
  std::vector<std::pair<Simulator*, std::string>>& lst)
{
    if (m_isStarted) {
        vpz::ModelPortList result;
        model->getAtomicModelsSource(port, result);

        for (auto& elem : result)
            lst.emplace_back(
              static_cast<vpz::AtomicModel*>(elem.first)->get_simulator(),
              elem.second);
    }
}

void
Coordinator::updateSimulatorsTarget(
  std::vector<std::pair<Simulator*, std::string>>& lst)
{
    if (m_isStarted) {
        for (auto& elem : lst) {
            if (elem.first != nullptr) {
                elem.first->updateSimulatorTargets(elem.second);
            }
        }
    }
}

void
Coordinator::removeSimulatorTargetPort(vpz::AtomicModel* model,
                                       const std::string& port)
{
    model->get_simulator()->removeTargetPort(port);
}

Simulator*
Coordinator::addModel(vpz::AtomicModel* model)
{
    assert(model && "Coordinator: nullptr model to add?");

    m_simulators.emplace_back(std::make_unique<Simulator>(model));

    return m_simulators.back().get();
}

///
/// Private functions.
///

void
Coordinator::delAtomicModel(vpz::AtomicModel* atom,
                            std::vector<Simulator*>& to_delete)
{
    assert(atom && "Cannot delete undefined atomic model");

    Simulator* satom = atom->get_simulator();

    for (auto& elem : m_eventViewList)
        elem.second.removeObservable(satom->dynamics().get());

    for (auto& elem : m_timedViewList)
        elem.second.removeObservable(satom->dynamics().get());

    to_delete.emplace_back(satom);
}

void
Coordinator::delCoupledModel(vpz::CoupledModel* mdl,
                             std::vector<Simulator*>& to_delete)
{
    assert(mdl && "Cannot delete undefined coupled model");

    std::vector<vpz::AtomicModel*> lst;
    vpz::BaseModel::getAtomicModelList(mdl, lst);

    for (auto& elem : lst)
        delAtomicModel(elem, to_delete);
}

void
Coordinator::addModels(const vpz::Model& model)
{
    m_modelFactory.createModels(*this, model);
}

void
Coordinator::dispatchExternalEvent(std::vector<Simulator*>& simulators,
                                   const std::size_t number)
{
    for (std::size_t i = 0; i != number; ++i) {
        if (simulators[i]->result().empty())
            continue;

        auto& eventList = simulators[i]->result();
        for (auto& elem : eventList) {
            auto x = simulators[i]->targets(elem.getPortName());

            if (x.first != x.second and x.first->second.first) {
                for (auto jt = x.first; jt != x.second; ++jt)
                    m_eventTable.addExternal(
                      jt->second.first, elem.attributes(), jt->second.second);
            }
        }

        simulators[i]->clear_result();
    }
}

void
Coordinator::buildViews(long instance)
{
    const vpz::Outputs& outs(m_modelFactory.outputs());
    const vpz::Views& views(m_modelFactory.views());
    const vpz::ViewList& viewlist(views.viewlist());

    for (const auto& elem : viewlist) {
        if (elem.second.is_enable()) {
            std::string file;

            file =
              instance >= 0
                ? utils::format("%s_%s-%ld",
                                m_modelFactory.experiment().name().c_str(),
                                elem.first.c_str(),
                                instance)
                : utils::format("%s_%s",
                                m_modelFactory.experiment().name().c_str(),
                                elem.first.c_str());

            const auto& output = outs.get(elem.second.output());

            if (elem.second.type() == vpz::View::TIMED) {
                View& v = m_timedViewList[elem.second.name()];

                v.open(m_context,
                       elem.second.name(),
                       output.plugin(),
                       output.package(),
                       output.location(),
                       file,
                       m_currentTime,
                       (output.data()) ? output.data()->clone() : nullptr);

                m_timed_observation_scheduler.add(
                  &v, m_currentTime, elem.second.timestep());
            } else {
                auto& v = m_eventViewList[elem.second.name()];

                v.open(m_context,
                       elem.second.name(),
                       output.plugin(),
                       output.package(),
                       output.location(),
                       file,
                       m_currentTime,
                       (output.data()) ? output.data()->clone() : nullptr);
            }
        }
    }
}

void
Coordinator::processInit(Simulator* simulator)
{
    Time tn = simulator->init(m_currentTime);

    if (not isInfinity(tn)) {
        m_eventTable.addInternal(simulator, tn);
    }
}

std::unique_ptr<value::Map>
Coordinator::finish()
{
    for (auto& elem : m_simulators) {
        assert(elem.get());
        elem->finish();
        auto& observations = elem->getObservations();
        for (auto& obs : observations)
            obs.view->run(elem->dynamics().get(),
                          m_currentTime,
                          obs.portname,
                          std::move(obs.value));

        observations.clear();
    }

    std::unique_ptr<value::Map> result;
    for (auto& elem : m_timedViewList) {
        auto matrix = elem.second.finish(m_currentTime);
        if (matrix) {
            if (not result)
                result = std::make_unique<value::Map>();

            result->add(elem.first, std::move(matrix));
        }
    }

    for (auto& elem : m_eventViewList) {
        auto matrix = elem.second.finish(m_currentTime);
        if (matrix) {
            if (not result)
                result = std::make_unique<value::Map>();
            result->add(elem.first, std::move(matrix));
        }
    }

    return result;
}

std::unique_ptr<value::Map>
Coordinator::getMap() const
{
    std::unique_ptr<value::Map> result;

    for (const auto& elem : m_timedViewList) {
        auto matrix = elem.second.matrix();

        if (matrix) {
            if (not result)
                result = std::make_unique<value::Map>();

            result->add(elem.first, std::move(matrix));
        }
    }

    for (const auto& elem : m_eventViewList) {
        auto matrix = elem.second.matrix();

        if (matrix) {
            if (not result)
                result = std::make_unique<value::Map>();

            result->add(elem.first, std::move(matrix));
        }
    }

    return result;
}
}
} // namespace vle devs
