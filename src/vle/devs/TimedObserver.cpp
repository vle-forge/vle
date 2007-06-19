/**
 * @file devs/TimedObserver.cpp
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

#include <vle/devs/TimedObserver.hpp>

using std::string;

namespace vle { namespace devs {

TimedObserver::TimedObserver(const std::string& name,
			     Stream* stream,
			     double timeStep) :
    Observer(name,stream),
    m_timeStep(timeStep),
    m_counter(0)
{
}

StateEventList TimedObserver::init()
{
    m_counter = 0;
    m_valueList.clear();
    return Observer::init();
}

StateEvent*
TimedObserver::processStateEvent(StateEvent* event)
{
    value::Value value = event->getAttributeValue(event->getPortName());

    if (value) {
        m_valueList[StreamModelPort(
            event->getModel(), event->getPortName())] = value;
        m_counter++;
        if (m_counter == m_size) {
	    m_stream->writeValues(event->getTime(),
				  m_valueList,getObservableList());
	    m_counter = 0;
	    m_valueList.clear();
	}
    }

    return new StateEvent(event->getTime() + m_timeStep,
                          event->getModel(),
			  getName(),
			  event->getPortName());
}

}} // namespace vle devs
