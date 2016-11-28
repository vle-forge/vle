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

#ifndef VLE_DEVS_SCHEDULER_HPP
#define VLE_DEVS_SCHEDULER_HPP

#include <boost/heap/fibonacci_heap.hpp>
#include <map>
#include <unordered_set>
#include <vector>
#include <vle/DllDefines.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/ViewEvent.hpp>

namespace vle {
namespace devs {

class Simulator;

/**
 * Compare two events with the \e getTime() function as comparator.
 *
 * \param e1 first event to compare.
 * \param e2 second event to compare.
 * \return true if Time e1 is more recent than devs::Time e2.
 */
template <typename EventT>
bool EventCompare(const EventT &e1, const EventT &e2) noexcept
{
    return e1.mTime > e2.mTime;
}

/**
 * Pop an element from the \e scheduler.
 *
 * \param scheduler where to remove element.
 */
template <typename SchedulerT> void pop(SchedulerT &scheduler) noexcept
{
    using event_type = typename SchedulerT::value_type;

    if (scheduler.empty())
        return;

    std::pop_heap(
        scheduler.begin(), scheduler.end(), EventCompare<event_type>);

    scheduler.pop_back();
}

/**
 * Push a new element build inplace in the \e scheduler.
 *
 * \param scheduler where to push element.
 * \param args list of argument to build inplace the element to push into
 * the scheduler.
 */
template <typename SchedulerT, typename... Args>
void push(SchedulerT &scheduler, Args &&... args)
{
    using event_type = typename SchedulerT::value_type;

    scheduler.emplace_back(std::forward<Args>(args)...);

    std::push_heap(
        scheduler.begin(), scheduler.end(), EventCompare<event_type>);
}

struct HeapElementCompare {
    template <typename HeapElementT>
    bool operator()(const HeapElementT &lhs, const HeapElementT &rhs) const
        noexcept
    {
        return lhs.m_time >= rhs.m_time;
    }
};

struct HeapElement {
    HeapElement(Time time, Simulator *Simulator)
        : m_time(time)
        , m_simulator(Simulator)
    {
    }

    Time m_time;
    Simulator *m_simulator;
};

using Heap =
    boost::heap::fibonacci_heap<HeapElement,
                                boost::heap::compare<HeapElementCompare>>;

using HandleT = Heap::handle_type;

/**
 * @brief Bag stores \e Simulator that need to be call in this bag.
 *
 */
struct Bag {
    std::vector<Simulator *> dynamics;
    std::vector<Simulator *> executives;

    //
    // \e unique_simulators is used to ensures that \e dynamics and \e
    // executives vectors have unique pointer through a \e simulator object.
    //
    std::unordered_set<Simulator *> unique_simulators;
};

class VLE_LOCAL Scheduler {
public:
    Scheduler()
        : m_current_time(negativeInfinity)
    {
    }

    ~Scheduler() = default;

    Scheduler(const Scheduler &) = delete;
    Scheduler &operator=(const Scheduler &) = delete;
    Scheduler(Scheduler &&) = delete;
    Scheduler &operator=(Scheduler &&) = delete;

    void init(Time time);

    void addInternal(Simulator *simulator, Time time);
    void addExternal(Simulator *simulator,
                     std::shared_ptr<value::Value> values,
                     const std::string &portname);
    void delSimulator(Simulator *simulator);

    Bag &getCurrentBag() noexcept { return m_current_bag; }

    Time getCurrentTime() const noexcept { return m_current_time; }

    Time getNextTime() const noexcept
    {
        if (not m_scheduler.empty())
            return m_scheduler.top().m_time;

        return infinity;
    }

    void makeNextBag();

private:
    Bag m_current_bag;
    Heap m_scheduler;
    Time m_current_time;
};

class VLE_LOCAL TimedObservationScheduler {
    std::vector<ViewEvent> m_observation;

public:
    void add(View *ptr, Time time, Time timestep)
    {
        assert(not isInfinity(time) && "addObservation: infinity time");
        assert(not isNegativeInfinity(time) &&
               "addObservation: negative infinity time");

        push(m_observation, ptr, time, timestep);
    }

    bool haveObservationAtTime(Time time) noexcept
    {
        if (m_observation.empty())
            return false;

        return m_observation[0].mTime < time;
    }

    void finalize(Time time)
    {
        for (auto &elem : m_observation)
            elem.run(time);

        m_observation.clear();
    }

    std::vector<ViewEvent> getObservationAtTime(Time time)
    {
        std::vector<ViewEvent> ret;

        while (not m_observation.empty() and m_observation[0].mTime < time) {
            ret.push_back(std::move(m_observation[0]));
            pop(m_observation);
        }

        return ret;
    }
};
}
} // namespace vle devs

#endif
