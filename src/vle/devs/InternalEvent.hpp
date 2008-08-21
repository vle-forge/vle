/**
 * @file vle/devs/InternalEvent.hpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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


#ifndef DEVS_INTERNALEVENT_HPP
#define DEVS_INTERNALEVENT_HPP

#include <vle/devs/Event.hpp>

namespace vle { namespace devs {

    class Simulator;

    class InternalEvent : public Event
    {
    public:
	InternalEvent(const Time& time, Simulator* model) :
            Event(model),
            m_time(time)
	{ }

	virtual ~ InternalEvent()
	{ }

        virtual bool isInternal() const
        { return true; }

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
        inline bool operator<(const InternalEvent* event) const
        { return m_time < event->m_time; }

        /**
         * Superior comparator use Time as key.
         *
         * @param event Event to test, no test on validity.
         *
         * @return true if this Event is superior to event.
         */
        inline bool operator>(const InternalEvent * event) const
        { return m_time > event->m_time; }

        /**
         * Equality comparator use Time as key.
         *
         * @param event Event to test, no test on validity.
         *
         * @return true if this Event is equal to  event.
         */
        inline bool operator==(const InternalEvent * event) const
        { return m_time == event->m_time; }

    private:
        Time        m_time;
    };

}} // namespace vle devs

#endif
