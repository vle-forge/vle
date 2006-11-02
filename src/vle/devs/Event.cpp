/**
 * @file devs/Event.cpp
 * @author The VLE Development Team.
 * @brief The Event base class. Define the base for internal, initialisation,
 * state and external events.
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
#include <vle/devs/sAtomicModel.hpp>
#include <vle/value/ConstPointer.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/String.hpp>
#include <vle/utils/Debug.hpp>
#include <iostream>



namespace vle { namespace devs {

Event::Event(const Time& time, sAtomicModel* model) :
    m_time(time),
    m_source(model),
    m_valid(true)
{
}

Event::Event(const Event& event) :
    m_time(event.m_time),
    m_source(event.m_source),
    m_valid(event.m_valid)
{
    for (vle::value::Map::MapValue::const_iterator it =
         event.m_attributes.getValue().begin();
         it != event.m_attributes.getValue().end(); ++it) {
        m_attributes.addValue((*it).first, (*it).second->clone());
    }
}

Event::~Event()
{
}

const std::string& Event::getSourceModelName() const
{
    return m_source->getName();
}

const void* Event::getConstPointerAttributeValue(const std::string& name) const
{
    const vle::value::ConstPointer* d = dynamic_cast < const
        vle::value::ConstPointer* >(getAttributeValue(name));
    Assert(utils::InternalError, d, "Bad conversion");
    return d->ref();
}

void Event::putAttribute(vle::value::Map* mp)
{
    for (vle::value::Map::MapValue::iterator it = mp->getValue().begin();
         it != mp->getValue().end(); ++it) {
        putAttribute((*it).first, (*it).second);
    }
}

double Event::getDoubleAttributeValue(const std::string& name) const
{
    const vle::value::Double* d = dynamic_cast < const vle::value::Double*
        >(getAttributeValue(name));
    Assert(utils::InternalError, d, "Bad conversion");
    return d->doubleValue();
}

long Event::getIntegerAttributeValue(const std::string& name) const
{
    const vle::value::Integer* d = dynamic_cast < const vle::value::Integer*
        >(getAttributeValue(name));
    Assert(utils::InternalError, d, "Bad conversion");
    return d->longValue();
}

bool Event::getBooleanAttributeValue(const std::string& name) const
{
    const vle::value::Boolean* d = dynamic_cast < const vle::value::Boolean*
        >(getAttributeValue(name));
    Assert(utils::InternalError, d, "Bad conversion");
    return d->boolValue();
}

const std::string& Event::getStringAttributeValue(const std::string& name) const
{
    const vle::value::String* d = dynamic_cast < const vle::value::String*
        >(getAttributeValue(name));
    Assert(utils::InternalError, d, "Bad conversion");
    return d->stringValue();
}

}} // namespace vle devs
