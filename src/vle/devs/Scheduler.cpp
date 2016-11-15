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

namespace vle
{
namespace devs
{

void Scheduler::init(Time time)
{
    // m_current_bag.dynamics.reserve(256);
    // m_current_bag.executives.reserve(256);
    m_current_bag.clear();
    m_current_time = time;

    while (not m_scheduler.empty() and
           m_scheduler.top().m_time <= m_current_time) {
        Simulator *sim = m_scheduler.top().m_simulator;

        if (sim->dynamics()->isExecutive()) {
            auto it = std::find(std::begin(m_current_bag.executives),
                                std::end(m_current_bag.executives),
                                sim);

            if (it == m_current_bag.executives.end())
                m_current_bag.executives.emplace_back(sim);
        } else {
            auto it = std::find(std::begin(m_current_bag.dynamics),
                                std::end(m_current_bag.dynamics),
                                sim);

            if (it == m_current_bag.dynamics.end())
                m_current_bag.dynamics.emplace_back(sim);
        }

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
                            const std::string &portname)
{
    if (simulator->dynamics()->isExecutive()) {
        auto it = std::find(std::begin(m_current_bag.executives),
                            std::end(m_current_bag.executives),
                            simulator);

        if (it == m_current_bag.executives.end())
            m_current_bag.executives.emplace_back(simulator);
    } else {
        auto it = std::find(std::begin(m_current_bag.dynamics),
                            std::end(m_current_bag.dynamics),
                            simulator);

        if (it == m_current_bag.dynamics.end())
            m_current_bag.dynamics.emplace_back(simulator);
    }

    simulator->addExternalEvents(simulator, values, portname);

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

void Scheduler::delSimulator(Simulator *simulator)
{
    if (simulator->dynamics()->isExecutive()) {
        auto it = std::find(std::begin(m_current_bag.executives),
                            std::end(m_current_bag.executives),
                            simulator);

        if (it != m_current_bag.executives.end())
            m_current_bag.executives.erase(it);
    } else {
        auto it = std::find(std::begin(m_current_bag.dynamics),
                            std::end(m_current_bag.dynamics),
                            simulator);

        if (it != m_current_bag.dynamics.end())
            m_current_bag.dynamics.erase(it);
    }

    if (simulator->haveHandle()) {
        m_scheduler.erase(simulator->handle());
        simulator->resetHandle();
    }
}

void Scheduler::makeNextBag()
{
    m_current_time = getNextTime();
    m_current_bag.clear();

    while (not m_scheduler.empty() and
           m_scheduler.top().m_time == m_current_time) {

        Simulator *sim = m_scheduler.top().m_simulator;

        if (sim->dynamics()->isExecutive()) {
            auto it = std::find(std::begin(m_current_bag.executives),
                                std::end(m_current_bag.executives),
                                sim);

            if (it == m_current_bag.executives.end())
                m_current_bag.executives.emplace_back(sim);
        } else {
            auto it = std::find(std::begin(m_current_bag.dynamics),
                                std::end(m_current_bag.dynamics),
                                sim);

            if (it == m_current_bag.dynamics.end())
                m_current_bag.dynamics.emplace_back(sim);
        }

        sim->setInternalEvent();
        sim->resetHandle();
        m_scheduler.pop();
    }
}
}
} // namespace vle devs
