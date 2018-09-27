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

#include <cassert>
#include <memory>
#include <vector>
#include <vle/devs/Scheduler.hpp>
#include <vle/devs/Simulator.hpp>

namespace vle {
namespace devs {

void
Scheduler::init(Time time)
{
    m_current_time = time;

    m_current_bag.dynamics.clear();
    m_current_bag.executives.clear();
    m_current_bag.unique_simulators.clear();

    while (not m_scheduler.empty() and
           m_scheduler.top().m_time <= m_current_time) {

        Simulator* sim = m_scheduler.top().m_simulator;

        //
        // Add the simulator pointer into the unordered_set and into the
        // vector. In std::unordered_set to be sure that only one pointer is
        // available after the Coordinator::dispatchExternalEvents' call and in
        // std::vector to starts directly without merge vectors with
        // unordered_set.
        //

        if (sim->dynamics()->isExecutive())
            m_current_bag.executives.emplace_back(sim);
        else
            m_current_bag.dynamics.emplace_back(sim);

        m_current_bag.unique_simulators.emplace(sim);

        sim->setInternalEvent();
        sim->resetHandle();
        m_scheduler.pop();
    }
}

void
Scheduler::addInternal(Simulator* simulator, Time time)
{
    assert(not isInfinity(time) && "addInternal: infinity time?");
    assert(not isNegativeInfinity(time) && "addInternal: infinity time?");
    assert(time >= m_current_time && "addInternal: time < m_current_time?");

    if (simulator->haveHandle()) {
        (*simulator->handle()).m_time = time;
        m_scheduler.update(simulator->handle());
    } else {
        HandleT handle = m_scheduler.emplace(time, simulator);
        simulator->setHandle(handle);
    }
}

void
Scheduler::addExternal(Simulator* simulator,
                       std::shared_ptr<value::Value> values,
                       const std::string& portname)
{
    //
    // Tries to insert the simulator into the std::unordered_set. If insertion
    // success, (simulator does not exists with an internal transition), we add
    // it into the appropriate std::vector.
    //

    auto result = m_current_bag.unique_simulators.emplace(simulator);
    if (result.second == true) {
        if (simulator->dynamics()->isExecutive())
            m_current_bag.executives.emplace_back(simulator);
        else
            m_current_bag.dynamics.emplace_back(simulator);
    }

    simulator->addExternalEvents(values, portname);

    //
    // If an external event exists in the scheduler and not for the next
    // bag, we erase this event.
    //

    if (simulator->haveHandle() and simulator->getTn() > m_current_time) {
        m_scheduler.erase(simulator->handle());
        simulator->resetHandle();
        assert(not simulator->haveInternalEvent() && "Bad scheduler");
    }
}

void
Scheduler::delSimulator(Simulator* simulator)
{
    //
    // Tries to delete the simulator from the \c Bag objects (\c std::vector
    // and/or \c std::unordered_set
    //

    if (simulator->dynamics()->isExecutive())
        m_current_bag.executives.erase(
          std::remove(m_current_bag.executives.begin(),
                      m_current_bag.executives.end(),
                      simulator),
          m_current_bag.executives.end());
    else
        m_current_bag.dynamics.erase(
          std::remove(m_current_bag.dynamics.begin(),
                      m_current_bag.dynamics.end(),
                      simulator),
          m_current_bag.dynamics.end());

    m_current_bag.unique_simulators.erase(simulator);

    if (simulator->haveHandle()) {
        m_scheduler.erase(simulator->handle());
        simulator->resetHandle();
    }
}

void
Scheduler::makeNextBag()
{
    m_current_time = getNextTime();

    m_current_bag.dynamics.clear();
    m_current_bag.executives.clear();
    m_current_bag.unique_simulators.clear();

    while (not m_scheduler.empty() and
           m_scheduler.top().m_time == m_current_time) {

        Simulator* sim = m_scheduler.top().m_simulator;

        //
        // Add the simulator pointer into the unordered_set and into the
        // vector. In std::unordered_set to be sure that only one pointer is
        // available after the Coordinator::dispatchExternalEvents' call and in
        // std::vector to starts directly without merge vectors with
        // unordered_set.
        //

        if (sim->dynamics()->isExecutive())
            m_current_bag.executives.emplace_back(sim);
        else
            m_current_bag.dynamics.emplace_back(sim);

        m_current_bag.unique_simulators.emplace(sim);

        sim->setInternalEvent();
        sim->resetHandle();
        m_scheduler.pop();
    }
}
}
} // namespace vle devs
