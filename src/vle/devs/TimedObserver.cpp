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

TimedObserver::TimedObserver(const std::string& p_name,
			     Stream* p_stream,
			     double p_timeStep) :
    Observer(p_name,p_stream),
    m_timeStep(p_timeStep),
    m_counter(0)
{
}

StateEventList TimedObserver::init()
{
    m_counter=0;
    m_valueList.clear();
    return Observer::init();
}

StateEvent*
TimedObserver::processStateEvent(StateEvent* p_event)
{
    value::Value v_value = p_event->getAttributeValue(p_event->getPortName());

    if (v_value) {
	m_valueList[StreamModelPort(p_event->getModel(),
				    p_event->getPortName())] =
	    v_value->clone();
	m_counter++;
        if (m_counter == m_size) {
	    m_stream->writeValues(p_event->getTime(),
				  m_valueList,getObservableList());
	    m_counter = 0;


            //FIXME
            //StreamModelPortValue::iterator it = m_valueList.begin();
            //while (it != m_valueList.end()) {
                //delete (*it).second;
                //++it;
                //}
	    m_valueList.clear();
	}
    }

    return new StateEvent(p_event->getTime()+m_timeStep,
			  p_event->getModel(),
			  getName(),
			  p_event->getPortName());
}

}} // namespace vle devs
