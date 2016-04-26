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

#include <vle/devs/Simulator.hpp>
#include <vle/devs/Scheduler.hpp>
#include <memory>
#include <vector>
#include <cassert>

namespace vle { namespace devs {

void Scheduler::addInternal(Simulator *simulator, Time time)
{
    assert(not isInfinity(time) && "addInternal: infinity time?");
    assert(not isNegativeInfinity(time) && "addInternal: infinity time?");

    if (simulator->haveHandle()) {
        (*simulator->handle()).m_time = time;
        m_scheduler.update(simulator->handle());
    } else {
        HandleT handle = m_scheduler.emplace(time, simulator);
        simulator->setHandle(handle);
    }
}

void Scheduler::addObservation(View *ptr, Time time)
{
    assert(not isInfinity(time) && "addObservation: infinity time?");
    assert(not isNegativeInfinity(time) && "addObservation: infinity time?");

    push(m_observation, ptr, time);
}

void Scheduler::addExternal(Simulator *simulator,
                            std::shared_ptr<value::Value> values,
                            const std::string& portname)
{
    auto it = std::find_if(m_next_bag.begin(), m_next_bag.end(),
            [simulator](const std::pair<Simulator*, BagModel>& p)
            {
                return p.first == simulator;
            });

    if (it == m_next_bag.end()) {
        m_next_bag.emplace_back();
        m_next_bag.back().first = simulator;
        m_next_bag.back().second.external_events.emplace_back(
            simulator, values, portname);
    } else {
        it->second.external_events.emplace_back(simulator, values, portname);
    }


    // auto & bagmodel = m_next_bag[simulator];
    // bagmodel.external_events.emplace_back(simulator, values, portname);

    if (simulator->haveHandle() and
        simulator->getTn() != m_current_time) {
        m_scheduler.erase(simulator->handle());
        simulator->resetHandle();
    }
}

void Scheduler::delSimulator(Simulator *simulator)
{
     {
         auto it = std::find_if(m_current_bag.begin(), m_current_bag.end(),
                 [simulator](const std::pair<Simulator*, BagModel>& v)
                 {
                     return v.first == simulator;
                 });
         if (it != m_current_bag.end())
             m_current_bag.erase(it);
     }

     {
         auto it = std::find_if(m_next_bag.begin(), m_next_bag.end(),
                 [simulator](const std::pair<Simulator*, BagModel>& v)
                 {
                     return v.first == simulator;
                 });
         if (it != m_next_bag.end())
             m_next_bag.erase(it);
     }

     if (simulator->haveHandle()) {
         m_scheduler.erase(simulator->handle());
         simulator->resetHandle();
     }
}

Time Scheduler::getNextTime() noexcept
{
    if (not m_next_bag.empty())
        return m_current_time;

    if (not m_scheduler.empty()) {
        if (not m_observation.empty())
            return std::min(m_scheduler.top().m_time,
                            m_observation[0].getTime());

        return m_scheduler.top().m_time;
    }

    if (not m_observation.empty())
        return m_observation[0].getTime();

    return infinity;
}

Bag& Scheduler::getCurrentBag() noexcept
{
    return m_current_bag;
}

bool Scheduler::haveNextBagAtTime() noexcept
{
    if (not m_next_bag.empty())
        return true;

    if (not m_scheduler.empty())
        return m_scheduler.top().m_time == m_current_time;

    return false;
}

bool Scheduler::haveObservationEventAtTime() noexcept
{
    if (m_observation.empty())
        return false;

    if (m_observation[0].getTime() < m_scheduler.top().m_time)
        return true;

    return false;
}

std::vector<ViewEvent> Scheduler::getCurrentObservationBag() noexcept
{
    std::vector<ViewEvent> ret;

    while (not m_observation.empty() and
           m_observation[0].getTime() < m_scheduler.top().m_time) {
        ret.push_back(std::move(m_observation[0]));
        pop(m_observation);
    }

    return ret;
}

void Scheduler::makeNextBag()
{
    m_current_time = getNextTime();

    m_current_bag.clear();
    if (not m_next_bag.empty())
        std::swap(m_current_bag, m_next_bag);

    while (not m_scheduler.empty()
           and m_scheduler.top().m_time == m_current_time) {

        Simulator *sim = m_scheduler.top().m_simulator;

        auto it = std::find_if(m_next_bag.begin(), m_next_bag.end(),
                            [sim](const Bag::value_type& p)
                            {
                                return p.first == sim;
                            });


        // m_current_bag[m_scheduler.top().m_simulator].internal_event = true;

        if (it == m_next_bag.end()) {
            m_next_bag.emplace_back();
            m_next_bag.back().first = sim;
            m_next_bag.back().second.internal_event = true;
        } else {
            it->second.internal_event = true;
        }

        m_scheduler.top().m_simulator->resetHandle();
        m_scheduler.pop();
    }
}

}} // namespace vle devs
