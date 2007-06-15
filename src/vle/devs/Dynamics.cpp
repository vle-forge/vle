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
#include <vle/devs/Simulator.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/String.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/XML.hpp>


namespace vle { namespace devs {

ExternalEvent* Dynamics::buildEvent(const std::string& portName) const
{
  return new ExternalEvent(portName);
}

ExternalEvent* Dynamics::buildEventWithADouble(
    const std::string & portName,
    const std::string & attributeName,
    double attributeValue) const
{
    ExternalEvent* event = new ExternalEvent(portName);
    event->putAttribute(attributeName,
                        value::DoubleFactory::create(attributeValue));
    return event;
}

ExternalEvent* Dynamics::buildEventWithAInteger(
    const std::string & portName,
    const std::string & attributeName,
    long attributeValue) const
{
    ExternalEvent* event = new ExternalEvent(portName);

    event->putAttribute(attributeName,
                          value::IntegerFactory::create(attributeValue));
    return event;
}

ExternalEvent* Dynamics::buildEventWithABoolean(
    const std::string & portName,
    const std::string & attributeName,
    bool attributeValue) const
{
    ExternalEvent* event = new ExternalEvent(portName);
   
    event->putAttribute(attributeName,
                        value::BooleanFactory::create(attributeValue));
    return event;
}

ExternalEvent* Dynamics::buildEventWithAString(
    const std::string & portName,
    const std::string & attributeName,
    const std::string & attributeValue) const
{
    ExternalEvent* event = new ExternalEvent(portName);

    event->putAttribute(attributeName,
                        value::StringFactory::create(attributeValue));
    return event;
}

}} // namespace vle devs
