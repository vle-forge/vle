/**
 * @file devs/Dynamics.cpp
 * @author The VLE Development Team.
 * @brief Dynamics class represent a part of the DEVS simulator. This class
 * must be inherits to build simulation components.
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

#include <vle/devs/Event.hpp>
#include <vle/devs/InternalEvent.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/EventList.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/sAtomicModel.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/String.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/XML.hpp>


namespace vle { namespace devs {

Event::EventType Dynamics::processConflict(
                                const InternalEvent&,
                                const ExternalEventList&,
                                const InstantaneousEventList&)
{
    return Event::INTERNAL;
}

bool Dynamics::processConflict(const InternalEvent&,
                               const ExternalEventList&)
{
    return true;
}

bool Dynamics::processConflict(const InternalEvent&,
                               const InstantaneousEventList&)
{
    return true;
}

bool Dynamics::processConflict(const ExternalEventList&,
                               const InstantaneousEventList&)
{
    return true;
}

size_t Dynamics::processConflict(const ExternalEventList&)
{
    return 0;
}

ExternalEventList* Dynamics::buildEvent(const std::string& portName,
                                        const Time& currentTime) const
{
    ExternalEventList* v_eventList = new ExternalEventList();
    v_eventList->addEvent(
	new ExternalEvent(portName, currentTime, getModel()));
    return v_eventList;
}

ExternalEventList* Dynamics::buildEventWithADouble(
    const std::string & portName,
    const std::string & attributeName,
    double attributeValue,
    const Time& currentTime) const
{
    ExternalEventList* v_eventList = new ExternalEventList();
    ExternalEvent* v_event = new ExternalEvent(portName, currentTime,
					       getModel());

    v_event->putAttribute(attributeName,
                          value::DoubleFactory::create(attributeValue));
    v_eventList->addEvent(v_event);
    return v_eventList;
}

ExternalEventList* Dynamics::buildEventWithAInteger(
    const std::string & portName,
    const std::string & attributeName,
    long attributeValue,
    const Time& currentTime) const
{
    ExternalEventList* v_eventList = new ExternalEventList();
    ExternalEvent* v_event = new ExternalEvent(portName, currentTime,
					       getModel());

    v_event->putAttribute(attributeName,
                          value::IntegerFactory::create(attributeValue));
    v_eventList->addEvent(v_event);
    return v_eventList;
}

ExternalEventList* Dynamics::buildEventWithABoolean(
    const std::string & portName,
    const std::string & attributeName,
    bool attributeValue,
    Time const & currentTime) const
{
    ExternalEventList* v_eventList = new ExternalEventList();
    ExternalEvent* v_event = new ExternalEvent(portName, currentTime,
					       getModel());

    v_event->putAttribute(attributeName,
                          value::BooleanFactory::create(attributeValue));
    v_eventList->addEvent(v_event);
    return v_eventList;
}

ExternalEventList* Dynamics::buildEventWithAString(
    const std::string & portName,
    const std::string & attributeName,
    const std::string & attributeValue,
    Time const & currentTime) const
{
    ExternalEventList* v_eventList = new ExternalEventList();
    ExternalEvent* v_event = new ExternalEvent(portName, currentTime,
					       getModel());

    v_event->putAttribute(attributeName,
                          value::StringFactory::create(attributeValue));
    v_eventList->addEvent(v_event);
    return v_eventList;
}

ExternalEventList* Dynamics::getOutputFunction(const Time& /* time */)
{
    return new ExternalEventList();
}

ExternalEventList* Dynamics::processInstantaneousEvent(
            InstantaneousEvent* /* event */) const
{
    return noEvent();
}

vle::value::Value Dynamics::processStateEvent(StateEvent* /* event */) const
{
    return value::ValueBase::empty;
}

void Dynamics::processInitEvent(InitEvent* /* event */)
{
}

void Dynamics::processInternalEvent(InternalEvent* /* event */)
{
}

void Dynamics::processExternalEvent(ExternalEvent* /* event */)
{
}

}} // namespace vle devs
