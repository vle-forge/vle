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

Map::Map(xmlpp::Element* root)
{
    xmlpp::Node::NodeList lst = root->get_children("VALUE");
    for (xmlpp::Node::NodeList::iterator it = lst.begin();
         it != lst.end(); ++it) {

        xmlpp::Node::NodeList lst2 = ((xmlpp::Element*)*it)->get_children();
        for (xmlpp::Node::NodeList::iterator jt = lst2.begin();
             jt != lst2.end(); ++jt) {
            if (vle::utils::xml::is_element(*jt)) {
                addValue(vle::utils::xml::get_attribute(
                    ((xmlpp::Element*)*it), "NAME"),
                     Value::getValue((xmlpp::Element*)*jt));
            }
        }
    }
}

Map::~Map()
{
    for (MapValue::iterator it = m_value.begin();
         it != m_value.end(); ++it)
        delete (*it).second;
}

void Map::addValue(const std::string& name, Value* value)
{
    Assert(vle::utils::InternalError, value,
           boost::format("Push null value '%1%' into Map\n") % name);
    
    MapValue::iterator it = m_value.find(name);
    if (it == m_value.end()) {
        m_value[name] = value;
    } else {
        delete (*it).second;
        (*it).second = value;
    }
}

Value* Map::getValue(const std::string& name) const
{
    MapValue::const_iterator it = m_value.find(name);

    Assert(vle::utils::InternalError, it != m_value.end(),
           boost::format("Map Value have no value name '%1%'\n") % name);

    return (*it).second;
}

const std::string& Map::getStringValue(const std::string& name) const
{
    Value* val = getValue(name);

    Assert(vle::utils::InternalError, val->getType() == Value::STRING,
           boost::format("Value is not a String '%1%'\n") % name);

    return ((String*)val)->stringValue();
}
    
bool Map::getBooleanValue(const std::string& name) const
{
    Value* val = getValue(name);

    Assert(vle::utils::InternalError, val->getType() == Value::BOOLEAN,
           boost::format("Value is not a Boolean '%1%'\n") % name);

    return ((Boolean*)val)->boolValue();
}
    
long Map::getLongValue(const std::string& name) const
{
    Value* val = getValue(name);

    Assert(vle::utils::InternalError, val->getType() == Value::INTEGER,
           boost::format("Value is not a Integer '%1%'\n") % name);

    return ((Integer*)val)->longValue();
}
    
double Map::getDoubleValue(const std::string& name) const
{
    Value* val = getValue(name);

    Assert(vle::utils::InternalError, val->getType() == Value::DOUBLE,
           boost::format("Value is not a Double '%1%'\n") % name);

    return ((Double*)val)->doubleValue();
}
    
Map* Map::getMapValue(const std::string& name) const
{
    Value* val = getValue(name);

    Assert(vle::utils::InternalError, val->getType() == Value::MAP,
           boost::format("Value is not a Map '%1%'\n") % name);

    return ((Map*)val);
}
    
Set* Map::getSetValue(const std::string& name) const
{
    Value* val = getValue(name);

    Assert(vle::utils::InternalError, val->getType() == Value::SET,
           boost::format("Value is not a Set '%1%'\n") % name);

    return ((Set*)val);
}

void Map::clear()
{
    for (MapValue::iterator it = m_value.begin(); it != m_value.end(); ++it) {
        delete (*it).second;
        (*it).second = 0;
    }

    m_value.clear();
}
    
Value* Map::clone() const
{
    Map* r = new Map();
    for (MapValue::const_iterator it = m_value.begin();
         it != m_value.end(); ++it)
	r->addValue((*it).first, (*it).second->clone());

    return r;
}

std::string Map::toFile() const
{
    std::string s;
    MapValue::const_iterator it = m_value.begin();

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

std::string Map::toString() const
{
    std::string s;
    MapValue::const_iterator it = m_value.begin();

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

std::string Map::toXML() const
{
    std::string s="<MAP>";
    MapValue::const_iterator it = m_value.begin();

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
