/**
 * @file EventObserver.cpp
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

#include <vle/devs/EventObserver.hpp>
#include <vle/value/Double.hpp>
#include <iostream>



namespace vle { namespace devs {

EventObserver::EventObserver(const std::string& name, Stream* p_stream) :
    Observer(name, p_stream)
{
}

StateEventList EventObserver::init()
{
    m_lastTime = Time(0);

    std::vector < Observable >::const_iterator it = getObservableList().begin();

  while (it != getObservableList().end()) {
      m_valueList[StreamModelPort((*it).model, (*it).portName)] =
	  new value::Double(0.0);
      ++it;
  }

  return Observer::init();
}


StateEvent* EventObserver::processStateEvent(StateEvent* p_event)
{
    value::Value* v_value = p_event->getAttributeValue(p_event->getPortName());

    if (v_value) {
	if (m_lastTime < p_event->getTime()) {
	    m_stream->writeValues(p_event->getTime(),
				  m_valueList, getObservableList());
	    m_lastTime = p_event->getTime();
	}

	StreamModelPortValue::iterator it =
	    m_valueList.find(StreamModelPort(p_event->getModel(),
					     p_event->getPortName()));

	if (it != m_valueList.end())
	    delete it->second;

	m_valueList[StreamModelPort(p_event->getModel(),
				    p_event->getPortName())] =
	    v_value->clone();
    }
    return 0;
}

}} // namespace vle devs
