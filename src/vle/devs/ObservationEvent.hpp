/**
 * @file devs/ObservationEvent.hpp
 * @author The VLE Development Team.
 * @brief State event use to get information from graph::Model using
 * TimedView or EventView.
 */

/*
 * Copyright (c) 2004, 2005 The VLE Development Team.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef DEVS_OBSERVATION_EVENT_HPP
#define DEVS_OBSERVATION_EVENT_HPP

#include <vle/devs/Event.hpp>

namespace vle { namespace devs {

    /**
     * @brief State event use to get information from graph::Model using
     * TimedView or EventView.
     *
     */
    class ObservationEvent : public Event
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
	{ }

	ObservationEvent(const ObservationEvent& event) :
            Event(event),
            m_time(event.m_time),
	    m_viewName(event.m_viewName),
	    m_portName(event.m_portName)
	{ }

        virtual ~ObservationEvent()
        { }

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

}} // namespace vle devs

#endif
