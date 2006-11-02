/**
 * @file devs/EventObserver.hpp
 * @author The VLE Development Team.
 * @brief Define a Event observer base on devs::Observer class. This class
 * build state event when event are push.
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

#ifndef DEVS_EVENT_OBSERVER_HPP
#define DEVS_EVENT_OBSERVER_HPP

#include <vle/devs/Observer.hpp>
#include <vle/devs/StateEvent.hpp>
#include <vle/devs/DevsTypes.hpp>
#include <vle/devs/Stream.hpp>

namespace vle { namespace devs {

    /**
     * @brief Define a Event observer base on devs::Observer class. This class
     * build state event when event are push.
     *
     */
    class EventObserver : public Observer
    {
    public:
        EventObserver(const std::string& name, Stream* stream);

        virtual ~EventObserver() { }

        virtual StateEventList init();

        virtual bool isEvent() const { return true; }

        virtual bool isTimed() const { return false; }

        virtual devs::StateEvent* processStateEvent(devs::StateEvent* event);

    private:
        Time                 m_lastTime;
	StreamModelPortValue m_valueList;

    };

}} // namespace vle devs

#endif
