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


#ifndef DEVS_INTERNALEVENT_HPP
#define DEVS_INTERNALEVENT_HPP

#include <vle/DllDefines.hpp>
#include <vle/devs/Time.hpp>
#include <vector>

namespace vle { namespace devs {

class Simulator;

/**
 * The @e InternalEvent represents internal events in VLE.
 *
 * The @e InternalEvent is only used by the scheduler of VLE. To
 * speed-up the simulation an @e InternalEvent can be invalidate.
 */
class VLE_API InternalEvent
{
public:
    /**
     * InternalEvent constructor.
     *
     * @param time The @e time when wake up the model.
     * @param simualtor The @e simulator associated.
     */
    InternalEvent(const Time& time, Simulator* simulator)
        : m_simulator(simulator), m_time(time), m_isvalid(true)
    {
    }

    /**
     * InternalEvent descructor.
     *
     */
    ~InternalEvent()
    {
    }

    /**
     * Get a pointer to the simulator.
     *
     * @return A pointer to the simulator.
     */
    Simulator* getModel() const
    { return m_simulator; }

    /**
     * Get the wake up time.
     *
     * @return A time.
     */
    inline const Time& getTime() const
    { return m_time; }

    /**
     * Inferior comparator.
     *
     * @param event Event to test, no test on validity.
     *
     * @return true if this Event is inferior to @e event.
     */
    inline bool operator<(const InternalEvent *event) const
    { return m_time < event->m_time; }

    /**
     * Superior comparator.
     *
     * @param event Event to test, no test on validity.
     *
     * @return true if this Event is superior to @e event.
     */
    inline bool operator>(const InternalEvent *event) const
    { return m_time > event->m_time; }

    /**
     * Inferior comparator.
     *
     * @param event Event to test, no test on validity.
     *
     * @return true if this Event is inferior to @e event.
     */
    inline bool operator<=(const InternalEvent *event) const
    { return m_time <= event->m_time; }

    /**
     * Superior comparator.
     *
     * @param event Event to test, no test on validity.
     *
     * @return true if this Event is superior to @e event.
     */
    inline bool operator>=(const InternalEvent *event) const
    { return m_time >= event->m_time; }

    /**
     * Equality comparator.
     *
     * @param event Event to test, no test on validity.
     *
     * @return true if this Event is equal to @e event.
     */
    inline bool operator==(const InternalEvent *event) const
    { return m_time == event->m_time; }

    /**
     * Invalidate this @e InternalEvent.
     *
     */
    inline void invalidate()
    { m_isvalid = false; }

    /**
     * Check if this @e InternalEvent is valid or not.
     *
     *
     * @return true if this InternalEvent is valid false otherwise.
     */
    inline bool isValid() const
    { return m_isvalid; }

private:
    InternalEvent(const InternalEvent&);
    InternalEvent& operator=(const InternalEvent&);

    Simulator *m_simulator;     /**< A pointer to the simulator. */
    Time       m_time;          /**< The time to wake-up the simulator. */
    bool       m_isvalid;       /**< Is this InternalEvent valid? */
};

/**
 * @brief Define a vector of pionter of InternalEvent.
 */
typedef std::vector < InternalEvent* > InternalEventList;

}} // namespace vle devs

#endif
