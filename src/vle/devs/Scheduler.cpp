/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2016 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2016 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2016 INRA http://www.inra.fr
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

#include <vle/devs/Simulator.hpp>
#include <vle/devs/Scheduler.hpp>
#include <memory>
#include <vector>
#include <cassert>

namespace vle { namespace devs {

void Scheduler::init(Time time)
{
    m_current_bag.clear();
    m_next_bag.clear();
    m_current_time = time;

    while (not m_scheduler.empty()
           and m_scheduler.top().m_time <= m_current_time) {
        Simulator *sim = m_scheduler.top().m_simulator;

        auto it = std::find(
            std::begin(m_current_bag),
            std::end(m_current_bag),
            sim);

        if (it == m_current_bag.end())
            m_current_bag.emplace_back(sim);

        sim->setInternalEvent();
        sim->resetHandle();
        m_scheduler.pop();
    }
}

void Scheduler::addInternal(Simulator *simulator, Time time)
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

void Scheduler::addExternal(Simulator *simulator,
                            std::shared_ptr<value::Value> values,
                            const std::string& portname)
{
    auto it = std::find(std::begin(m_next_bag),
                        std::end(m_next_bag),
                        simulator);

    if (it == m_next_bag.end())
        m_next_bag.emplace_back(simulator);

    simulator->addExternalEvents(simulator, values, portname);

    //
    // If an external event exists in the scheduler and not for the next
    // bag, we erase this event.
    //

    if (simulator->haveHandle() and simulator->getTn() > m_current_time) {
        m_scheduler.erase(simulator->handle());
        simulator->resetHandle();
    }
}

void Scheduler::delSimulator(Simulator *simulator)
{
     {
         auto it = std::find(std::begin(m_current_bag),
                             std::end(m_current_bag),
                             simulator);

         if (it != m_current_bag.end())
             m_current_bag.erase(it);
     }

     {
         auto it = std::find(std::begin(m_next_bag),
                             std::end(m_next_bag),
                             simulator);

         if (it != m_next_bag.end())
             m_next_bag.erase(it);
     }

     if (simulator->haveHandle()) {
         m_scheduler.erase(simulator->handle());
         simulator->resetHandle();
     }
}

Time Scheduler::getNextTime() const noexcept
{
    if (not m_next_bag.empty())
        return m_current_time;

    if (not m_scheduler.empty())
        return m_scheduler.top().m_time;

    return infinity;
}

Bag& Scheduler::getCurrentBag() noexcept
{
    return m_current_bag;
}

void Scheduler::makeNextBag()
{
    m_current_time = getNextTime();

    //
    // First step, we get all external events generated in the previous
    // current_bag.
    //

    m_current_bag.clear();
    if (not m_next_bag.empty())
        m_current_bag.swap(m_next_bag);

    //
    // We need to browse throug this new current_bag to swap all external
    // events.
    //

    for (auto& elem : m_current_bag)
        elem->swapExternalEvents();

    //
    // Finally, we merge all simulator that tn equal current time.
    //

    while (not m_scheduler.empty()
           and m_scheduler.top().m_time == m_current_time) {

        Simulator *sim = m_scheduler.top().m_simulator;

        auto it = std::find(
            std::begin(m_current_bag),
            std::end(m_current_bag),
            sim);

        if (it == m_current_bag.end())
            m_current_bag.emplace_back(sim);

        sim->setInternalEvent();
        sim->resetHandle();
        m_scheduler.pop();
    }
}

}} // namespace vle devs
