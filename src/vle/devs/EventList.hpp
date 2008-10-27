/**
 * @file vle/devs/EventList.hpp
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


#ifndef VLE_DEVS_EVENTLIST_HPP
#define VLE_DEVS_EVENTLIST_HPP

#include <vector>
#include <algorithm>
#include <boost/checked_delete.hpp>

namespace vle { namespace devs {

    /**
     * @brief A std::vector container to store template of External and
     * Instantaneout event. We define two class devs::ExternalEventList and
     * devs::InstantaneousEventList.
     *
     */
    template < class Class >
    class EventList : public std::vector < Class* >
    {
    public:
        /**
         * @brief Constructor to build and empty EventList.
         */
	EventList() { }

        /**
         * @brief This constructor is an helper function to build an EventList
         * and add a first event in it.
         * @param event the event to push.
         */
        EventList(Class* event)
        { addEvent(event); }

        /**
         * @brief Add an event to the EventList. This function check if the
         * event is not null before add it.
         * @param event the event to push to the EventList.
         */
        inline void addEvent(Class* event)
        {
            if (event) {
                push_back(event);
            }
        }

        /**
         * @brief Delete all the event stored in the event list and clear the
         * list after.
         */
        void deleteAndClear()
        {
            std::for_each(EventList < Class >::begin(),
                          EventList < Class >::end(),
                          boost::checked_deleter < Class >());

            EventList < Class >::clear();
        }

    };

}} // namespace vle devs

#endif
