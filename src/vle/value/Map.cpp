/**
 * @file value/Map.cpp
 * @author The VLE Development Team.
 * @brief Map Value a container to a pair of (std::string,  Value class).
 */

/*
 * Copyright (c) 2005 The vle Development Team
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

#include <vle/value/Map.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Exception.hpp>

namespace vle { namespace value {

MapFactory::MapFactory(const MapFactory& mapfactory) :
    ValueBase(mapfactory)
{
    for (MapValueConstIt it = mapfactory.m_value.begin();
         it != mapfactory.m_value.end(); ++it) {
        // addValue((*it).first, (*it).second->clone());
        addValue((*it).first, (*it).second);
    }
}

Map MapFactory::create()
{
    return Map(new MapFactory());
}

Value MapFactory::clone() const
{
    return Value(new MapFactory(*this));
}

void MapFactory::addValue(const std::string& name, Value value)
{
    Assert(vle::utils::InternalError, value,
           boost::format("Push null value '%1%' into Map\n") % name);
    
    MapValueIt it = m_value.find(name);
    if (it == m_value.end()) {
        m_value[name] = value;
    } else {
        (*it).second = value;
    }
}

Value MapFactory::getValue(const std::string& name) const
{
    MapValueConstIt it = m_value.find(name);

    Assert(vle::utils::InternalError, it != m_value.end(),
           boost::format("Map Value have no value name '%1%'\n") % name);

    return (*it).second;
}

const std::string& MapFactory::getStringValue(const std::string& name) const
{
    Value val = getValue(name);
    return to_string(val)->stringValue();
}
    
bool MapFactory::getBooleanValue(const std::string& name) const
{
    Value val = getValue(name);
    return to_boolean(val)->boolValue();
}
    
long MapFactory::getLongValue(const std::string& name) const
{
    Value val = getValue(name);
    return to_integer(val)->longValue();
}

int MapFactory::getIntValue(const std::string& name) const
{
    Value val = getValue(name);
    return to_integer(val)->intValue();
}
    
double MapFactory::getDoubleValue(const std::string& name) const
{
    Value val = getValue(name);
    return to_double(val)->doubleValue();
}
    
Map MapFactory::getMapValue(const std::string& name) const
{
    Value val = getValue(name);
    return to_map(val);
}
    
Set MapFactory::getSetValue(const std::string& name) const
{
    Value val = getValue(name);
    return to_set(val);
}

void MapFactory::clear()
{
    m_value.clear();
}
    
std::string MapFactory::toFile() const
{
    std::string s;
    MapValueConstIt it = m_value.begin();

    while (it != m_value.end()) {
        s += "(";
        s += (*it).first;
        s += ", ";
        s += (*it).second->toFile();
        s += ")";
        ++it;
        if (it != m_value.end()) {
            s += " ";
        }
    }
    return s;
}

std::string MapFactory::toString() const
{
    std::string s;
    MapValueConstIt it = m_value.begin();

    while (it != m_value.end()) {
        s += "(";
        s += (*it).first;
        s += ", ";
        s += (*it).second->toFile();
        s += ")";
        ++it;
        if (it != m_value.end()) {
            s += " ";
        }
    }
    return s;
}

std::string MapFactory::toXML() const
{
    std::string s="<MAP>";
    MapValueConstIt it = m_value.begin();

    while (it != m_value.end()) {
        s += "<VALUE NAME=\"";
        s += (*it).first;
        s += "\">";
        s += (*it).second->toXML();
        s += "</VALUE>";
	++it;
    }
    s += "</MAP>";
    return s;
}

}} // namespace vle value
