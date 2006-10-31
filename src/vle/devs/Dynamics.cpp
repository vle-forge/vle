/**
 * @file Dynamics.cpp
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
                          new vle::value::Double(attributeValue));
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

    v_event->putAttribute(attributeName, new
                          vle::value::Integer(attributeValue));
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

    v_event->putAttribute(attributeName, new
                          vle::value::Boolean(attributeValue));
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

    v_event->putAttribute(attributeName, new
                          vle::value::String(attributeValue));
    v_eventList->addEvent(v_event);
    return v_eventList;
}

long Dynamics::getIntegerParameter(
    const std::string & name,
    xmlpp::Element* dyn_elt)
{
    xmlpp::Element* elt = utils::xml::get_children(dyn_elt,name);
    return utils::to_int(utils::xml::get_attribute(elt,"VALUE"));
}

double Dynamics::getDoubleParameter(
    const std::string & name,
    xmlpp::Element* dyn_elt)
{
    xmlpp::Element* elt = utils::xml::get_children(dyn_elt,name);
    return utils::to_double(utils::xml::get_attribute(elt,"VALUE"));
}

bool Dynamics::getBooleanParameter(
    const std::string & name,
    xmlpp::Element* dyn_elt)
{
    xmlpp::Element* elt = utils::xml::get_children(dyn_elt,name);
    return utils::to_boolean(utils::xml::get_attribute(elt,"VALUE"));
}

std::string Dynamics::getStringParameter(
    const std::string & name,
    xmlpp::Element* dyn_elt)
{
    xmlpp::Element* elt = utils::xml::get_children(dyn_elt,name);
    return utils::xml::get_attribute(elt,"VALUE");
}

long Dynamics::getIntegerAttributeValue(
    Event* event,
    const std::string & name) const
{
    return ((vle::value::Integer*)(event->getAttributeValue(name)))->intValue();
}

double Dynamics::getDoubleAttributeValue(
    Event* event,
    const std::string & name) const
{
    return
        ((vle::value::Double*)(event->getAttributeValue(name)))->doubleValue();
}

bool Dynamics::getBooleanAttributeValue(
    Event* event,
    const std::string & name) const
{
    return
        ((vle::value::Boolean*)(event->getAttributeValue(name)))->boolValue();
}

std::string Dynamics::getStringAttributeValue(
    Event* event,
    const std::string & name) const
{
    return ((vle::value::String*)(event->getAttributeValue(name)))->toString();
}

ExternalEventList* Dynamics::processInstantaneousEvent(
                                        InstantaneousEvent*) const
{
    return noEvent();
}

vle::value::Value* Dynamics::processStateEvent(StateEvent*) const
{
    return 0;
}

}} // namespace vle devs
