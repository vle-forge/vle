/**
 * @file TimedObserver.hpp
 * @author The VLE Development Team.
 * @brief Define a Timed observer base on devs::Observer class. This class
 * build state event with timed clock.
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

#ifndef DEVS_TIMED_OBSERVER_HPP
#define DEVS_TIMED_OBSERVER_HPP

#include <vle/devs/Observer.hpp>
#include <vle/devs/StateEvent.hpp>
#include <vle/devs/DevsTypes.hpp>
#include <vle/devs/Stream.hpp>

namespace vle { namespace devs {

    /**
     * @brief Define a Timed observer base on devs::Observer class. This class
     * build state event with timed clock.
     *
     */
    class TimedObserver : public Observer
    {
    public:
	TimedObserver(const std::string& name, Stream* stream, double timeStep);

	virtual ~TimedObserver() { }

	virtual StateEventList init();

	virtual bool isEvent() const { return false; }

	virtual bool isTimed() const { return true; }

	virtual devs::StateEvent* processStateEvent(devs::StateEvent* event);

    private:
	double               m_timeStep;
	size_t               m_counter;
	StreamModelPortValue m_valueList;
    };

}} // namespace vle devs

#endif
