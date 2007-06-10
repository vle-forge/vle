/**
 * @file devs/EventList.hpp
 * @author The VLE Development Team.
 * @brief A std::vector container to store template of event. We define two
 * class ExternalEventList and InstantaneousEventList.
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

#ifndef DEVS_EVENTLIST_HPP
#define DEVS_EVENTLIST_HPP

#include <deque>

namespace vle { namespace devs {

    /**
     * @brief A std::vector container to store template of event. We define two
     * class ExternalEventList and InstantaneousEventList.
     *
     */
    template < class Class >
    class EventList
    {   
    public:
        typedef typename std::deque < Class >::iterator iterator;
        typedef typename std::deque < Class >::const_iterator const_iterator;

	EventList() { }

	EventList(Class event)
        { m_eventList.push_back(event); }

        inline ~EventList()
        { clear(false); }

        inline void addEvent(Class event)
        { m_eventList.push_back(event); }

        inline size_t size() const
        { return m_eventList.size(); }

        inline Class at(size_t i) const
        { return m_eventList[i]; }

        inline Class front()
        { return m_eventList[0]; }

        inline const_iterator begin() const
        { return m_eventList.begin(); }

        inline const_iterator end() const
        { return m_eventList.end(); }

        inline iterator begin()
        { return m_eventList.begin(); }

        inline iterator end()
        { return m_eventList.end(); }

        inline void erase(size_t i)
        { m_eventList.erase(m_eventList.begin() + i); }

        bool empty() const
        { return m_eventList.empty(); }

        void clear(bool erase = false)
        {
            if (erase) {
                const size_t sz = m_eventList.size();
                for (size_t i = 0; i < sz; ++i) {
                    delete m_eventList[i];
                }
            }
            m_eventList.clear();
        }

    private:
        std::deque < Class > m_eventList;
    };

}} // namespace vle devs

#endif
