/*
 * @file vle/devs/ObservationEvent.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef VLE_DEVS_OBSERVATION_EVENT_HPP
#define VLE_DEVS_OBSERVATION_EVENT_HPP 1

#include <vle/devs/DllDefines.hpp>
#include <vle/devs/Event.hpp>
#include <vector>

namespace vle { namespace devs {

/**
 * @brief State event use to get information from graph::Model using
 * TimedView or EventView.
 *
 */
class VLE_DEVS_EXPORT ObservationEvent : public Event
{
public:
    ObservationEvent(const Time& time,
                     Simulator* model,
                     const std::string& viewname,
                     const std::string& portName) :
        Event(model),
        m_time(time),
        m_viewName(viewname),
        m_portName(portName)
    {
        deleter();
    }

    ObservationEvent(const ObservationEvent& event) :
        Event(event),
        m_time(event.m_time),
        m_viewName(event.m_viewName),
        m_portName(event.m_portName)
    {
        deleter();
    }

    virtual ~ObservationEvent()
    {}

    inline const std::string& getViewName() const
    { return m_viewName; }

    inline const std::string& getPortName() const
    { return m_portName; }

    virtual bool isObservation() const
    { return true; }

    inline bool onPort(std::string const& portName) const
    { return m_portName == portName; }

    /**
     * @return arrived time.
     */
    inline const Time& getTime() const
    { return m_time; }

    /**
     * Inferior comparator use Time as key.
     *
     * @param event Event to test, no test on validity.
     * @return true if this Event is inferior to event.
     */
    inline bool operator<(const ObservationEvent* event) const
    { return m_time < event->m_time; }

    /**
     * Superior comparator use Time as key.
     *
     * @param event Event to test, no test on validity.
     *
     * @return true if this Event is superior to event.
     */
    inline bool operator>(const ObservationEvent* event) const
    { return m_time > event->m_time; }

    /**
     * Equality comparator use Time as key.
     *
     * @param event Event to test, no test on validity.
     *
     * @return true if this Event is equal to  event.
     */
    inline bool operator==(const ObservationEvent * event) const
    { return m_time == event->m_time; }

private:
    Time        m_time;
    std::string m_viewName;
    std::string m_portName;
};

inline std::ostream& operator<<(std::ostream& o, const ObservationEvent& evt)
{
    return o << "from: '" << evt.getViewName()
        << "' port: '" << evt.getPortName() << "'";
}

/**
 * @brief Define a vector pointer of ObservationEvent.
 */
typedef std::vector < ObservationEvent* > ObservationEventList;

}} // namespace vle devs

#endif
