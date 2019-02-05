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
#include <vle/devs/Time.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/vpz/AtomicModel.hpp>

#include "devs/Simulator.hpp"
#include "utils/i18n.hpp"

namespace vle {
namespace devs {

Simulator::Simulator(vpz::AtomicModel* atomic)
  : m_atomicModel(atomic)
  , m_tn(negativeInfinity)
  , m_have_handle(false)
  , m_have_internal(false)
{
    assert(atomic && "Simulator: missing vpz::AtomicMOdel");

    m_atomicModel->m_simulator = this;
}

void
Simulator::updateSimulatorTargets(const std::string& port)
{
    assert(m_atomicModel);

    mTargets.erase(port);

    vpz::ModelPortList result;
    m_atomicModel->getAtomicModelsTarget(port, result);

    if (result.begin() == result.end()) {
        mTargets.emplace(port, TargetSimulator(nullptr, std::string()));
        return;
    }

    for (auto& elem : result)
        mTargets.emplace(
          port,
          TargetSimulator(
            static_cast<vpz::AtomicModel*>(elem.first)->get_simulator(),
            elem.second));
}

std::pair<Simulator::iterator, Simulator::iterator>
Simulator::targets(const std::string& port)
{
    auto x = mTargets.equal_range(port);

    // If the updateSimulatorTargets function was never call, we update
    // the simulator targets and try to retrieve the newest simulator
    // targets.
    if (x.first == x.second) {
        updateSimulatorTargets(port);
        x = mTargets.equal_range(port);
    }

    if (x.first->second.first == nullptr)
        return { mTargets.end(), mTargets.end() };

    return x;
}

void
Simulator::removeTargetPort(const std::string& port)
{
    auto it = mTargets.find(port);

    if (it != mTargets.end()) {
        mTargets.erase(it);
    }
}

void
Simulator::addTargetPort(const std::string& port)
{
    assert(mTargets.find(port) == mTargets.end());

    mTargets.insert(value_type(port, TargetSimulator(nullptr, std::string())));
}

void
Simulator::addDynamics(std::unique_ptr<Dynamics> dynamics)
{
    m_dynamics = std::unique_ptr<Dynamics>(std::move(dynamics));
}

const std::string&
Simulator::getName() const
{
    if (not m_atomicModel)
        throw utils::InternalError(_("Simulator destroyed"));

    return m_atomicModel->getName();
}

void
Simulator::finish()
{
    m_dynamics->finish();
}

void
Simulator::output(Time time)
{
    assert(m_result.empty());

    m_dynamics->output(time, m_result);
}

Time
Simulator::timeAdvance()
{
    Time tn = m_dynamics->timeAdvance();

    if (tn < 0.0)
        throw utils::ModellingError(
          "Negative time advance in '%s' (%f)", getName().c_str(), tn);

    return tn;
}

Time
Simulator::init(Time time)
{
    Time tn = m_dynamics->init(time);

    if (tn < 0.0)
        throw utils::ModellingError(
          "Negative init function in '%s' (%f)", getName().c_str(), tn);

    m_tn = tn + time;
    return m_tn;
}

Time
Simulator::confluentTransitions(Time time)
{
    assert(not m_external_events.empty() and "Simulator d-conf error");
    assert(m_have_internal == true and "Simulator d-conf error");
    m_dynamics->confluentTransitions(time, m_external_events);

    m_external_events.clear();
    m_have_internal = false;

    m_tn = timeAdvance() + time;
    return m_tn;
}

Time
Simulator::internalTransition(Time time)
{
    assert(m_have_internal == true and "Simulator d-int error");
    m_dynamics->internalTransition(time);

    m_have_internal = false;

    m_tn = timeAdvance() + time;
    return m_tn;
}

Time
Simulator::externalTransition(Time time)
{
    assert(not m_external_events.empty() and "Simulator d-ext error");
    m_dynamics->externalTransition(m_external_events, time);

    m_external_events.clear();

    m_tn = timeAdvance() + time;
    return m_tn;
}

std::unique_ptr<value::Value>
Simulator::observation(const ObservationEvent& event) const
{
    return m_dynamics->observation(event);
}
}
} // namespace vle devs
