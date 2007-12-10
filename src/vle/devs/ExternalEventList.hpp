/**
 * @file devs/ExternalEventList.hpp
 * @author The VLE Development Team.
 * @brief A EventList container to store devs::ExternalEvent. 
 */

/*
 * Copyright (c) 2005 The VLE Development Team.
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

#ifndef VLE_DEVS_EXTERNALEVENTLIST_HPP
#define VLE_DEVS_EXTERNALEVENTLIST_HPP

#include <vle/devs/EventList.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/vpz/Condition.hpp>

namespace vle { namespace devs {

    typedef EventList < ExternalEvent > ExternalEventList;
    typedef vpz::ValueList InitEventList;

    inline std::ostream& operator<<(std::ostream& o,
                                    const ExternalEventList& evts)
    {
        for (ExternalEventList::const_iterator it = evts.begin();
             it != evts.end(); ++it) {
            o << " [" << *(*it) << "]";
        }
        return o;
    }

}} // namespace vle devs

#endif
