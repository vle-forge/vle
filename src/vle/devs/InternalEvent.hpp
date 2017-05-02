/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#ifndef DEVS_INTERNALEVENT_HPP
#define DEVS_INTERNALEVENT_HPP

#include <vector>
#include <vle/DllDefines.hpp>
#include <vle/devs/Time.hpp>

namespace vle {
namespace devs {

class Simulator;

/**
 * The \e InternalEvent represents DEVS internal events and is stored in
 * scheduller ie. \e EventTable. The \e InternalEvent is only used by the
 * scheduler of VLE. To speed-up the simulation an \e InternalEvent can be
 * invalidate.
 */
class VLE_LOCAL InternalEvent
{
public:
    /**
     * InternalEvent constructor.
     *
     * @param time The @e time when wake up the model.
     * @param simualtor The @e simulator associated.
     */
    InternalEvent(Time time, Simulator* simulator, std::int32_t id)
      : m_simulator(simulator)
      , m_time(time)
      , m_id(id)
    {
    }

    InternalEvent() = default;
    InternalEvent(const InternalEvent&) = default;
    InternalEvent(InternalEvent&&) = default;
    InternalEvent& operator=(const InternalEvent&) = default;
    InternalEvent& operator=(InternalEvent&&) = default;
    ~InternalEvent() = default;

    /**
     * Get a pointer to the simulator.
     *
     * @return A pointer to the simulator.
     */
    Simulator* getModel() const noexcept
    {
        return m_simulator;
    }

    /**
     * Get the wake up time.
     *
     * @return A time.
     */
    Time getTime() const noexcept
    {
        return m_time;
    }

    /**
     * Get the identifier of the internal event.
     * \return The identifier.
     */
    std::int32_t getId() const noexcept
    {
        return m_id;
    }

    /**
     * Check if the \e id is equal to the underlying \id provided in
     * constructor.
     *
     * \param id The identifier  to compare.
     * \return true if \e id equals \e m_id.
     */
    bool isCurrentId(std::int32_t id) const noexcept
    {
        return id == m_id;
    }

private:
    Simulator* m_simulator; /**< A pointer to the simulator. */
    Time m_time;            /**< The time to wake-up the simulator. */
    int32_t m_id;           /**< An identifier to the indicate elemet
                             * status. */
};

/**
 * Inferior comparator.
 *
 * @param event Event to test, no test on validity.
 * @return true if this Event is inferior to @e event.
 */
inline bool
operator<(const InternalEvent& lhs, const InternalEvent& rhs) noexcept
{
    return lhs.getTime() < rhs.getTime();
}

/**
 * Superior comparator.
 *
 * @param event Event to test, no test on validity.
 * @return true if this Event is superior to @e event.
 */
inline bool
operator>(const InternalEvent& lhs, const InternalEvent& rhs) noexcept
{
    return lhs.getTime() > rhs.getTime();
}

/**
 * Inferior comparator.
 *
 * @param event Event to test, no test on validity.
 * @return true if this Event is inferior to @e event.
 */
inline bool
operator<=(const InternalEvent& lhs, const InternalEvent& rhs) noexcept
{
    return lhs.getTime() <= rhs.getTime();
}

/**
 * Superior comparator.
 *
 * @param event Event to test, no test on validity.
 * @return true if this Event is superior to @e event.
 */
inline bool
operator>=(const InternalEvent& lhs, const InternalEvent& rhs) noexcept
{
    return lhs.getTime() >= rhs.getTime();
}

/**
 * Equality comparator.
 *
 * @param event Event to test, no test on validity.
 * @return true if this Event is equal to @e event.
 */
inline bool
operator==(const InternalEvent& lhs, const InternalEvent& rhs) noexcept
{
    return lhs.getTime() == rhs.getTime();
}

/**
 * @brief Define a vector of pionter of InternalEvent.
 */
using InternalEventList = std::vector<InternalEvent>;
}
} // namespace vle devs

#endif
