/**
 * @file src/vle/devs/Event.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */




#include <vle/devs/Event.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/String.hpp>
#include <vle/utils/Debug.hpp>



namespace vle { namespace devs {

Event::Event() :
    m_source(0),
    m_valid(true),
    m_attributes(value::MapFactory::create())
{ 
}

Event::Event(Simulator* model) :
    m_source(model),
    m_valid(true),
    m_attributes(value::MapFactory::create())
{
}

Event::Event(const Event& event) :
    m_source(event.m_source),
    m_valid(event.m_valid),
    m_attributes(event.m_attributes)
{
}

Event::~Event()
{
}

const std::string Event::getSourceModelName() const
{
    return m_source->getName();
}

void Event::putAttribute(vle::value::Map mp)
{
    for (value::MapValue::iterator it = mp->getValue().begin();
         it != mp->getValue().end(); ++it) {
        putAttribute((*it).first, (*it).second);
    }
}

}} // namespace vle devs
