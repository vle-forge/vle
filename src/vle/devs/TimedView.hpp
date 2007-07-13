/**
 * @file devs/TimedView.hpp
 * @author The VLE Development Team.
 * @brief Define a Timed View base on devs::View class. This class
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

#ifndef VLE_DEVS_TIMED_VIEW_HPP
#define VLE_DEVS_TIMED_VIEW_HPP

#include <vle/devs/View.hpp>
#include <vle/devs/StateEvent.hpp>

namespace vle { namespace devs {

    /**
     * @brief Define a Timed View base on devs::View class. This class
     * build state event with timed clock.
     *
     */
    class TimedView : public View
    {
    public:
        TimedView(const std::string& name, StreamWriter* stream,
                  const Time& timeStep);

	virtual ~TimedView() { }

	virtual bool isEvent() const { return false; }

	virtual bool isTimed() const { return true; }

	virtual devs::StateEvent* processStateEvent(devs::StateEvent* event);

    private:
	Time                    m_timeStep;
        size_t                  m_counter;
    };

    typedef std::map < std::string, devs::TimedView* > TimedViewList;

}} // namespace vle devs

#endif
