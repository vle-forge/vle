/**
 * @file DevsTypes.hpp
 * @author The VLE Development Team.
 * @brief Just an include to define all typedef use in DEVS library.
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

#ifndef DEVS_DEVSTYPES_HPP
#define DEVS_DEVSTYPES_HPP

#include <deque>
#include <vector>
#include <map>
#include <string>

namespace vle { namespace value {

    class Value;

}} // namespace vle value

namespace vle { namespace devs {

    class Event;
    class StateEvent;
    class ExternalEvent;
    class InternalEvent;
    class InstantaneousEvent;
    class sAtomicModel;

    typedef std::deque < StateEvent* >    StateEventList;
    typedef std::vector < InternalEvent* > InternalEventList;
    typedef std::pair < sAtomicModel*, std::string > StreamModelPort;
    typedef std::map < StreamModelPort, vle::value::Value* > StreamModelPortValue;

}} // namespace vle devs

#endif
