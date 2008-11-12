/**
 * @file vle/value/Map.cpp
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


#include <vle/value/Map.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/XML.hpp>
#include <vle/value/Deleter.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Algo.hpp>
#include <boost/utility.hpp>

namespace vle { namespace value {

Map::Map(const Map& orig) :
    Value(orig)
{
    for (MapValue::const_iterator it = orig.m_value.begin();
         it != orig.m_value.end(); ++it) {
        m_value[(*it).first] = ((*it).second)->clone();
    }
}

Map::~Map()
{
    clear();
}

void Map::writeFile(std::ostream& out) const
{
    for (MapValue::const_iterator it = m_value.begin();
         it != m_value.end(); ++it) {
        if (it != m_value.begin()) {
            out << " ";
        }
        out << "(" << (*it).first.c_str() << ", ";
        (*it).second->writeFile(out);
        out << ")";
    }
}

void Map::writeString(std::ostream& out) const
{
    for (MapValue::const_iterator it = m_value.begin();
         it != m_value.end(); ++it) {
        if (it != m_value.begin()) {
            out << " ";
        }
        out << "(" << (*it).first.c_str() << ", ";
        (*it).second->writeString(out);
        out << ")";
    }
}

void Map::writeXml(std::ostream& out) const
{
    out << "<map>";

    for (MapValue::const_iterator it = m_value.begin();
         it != m_value.end(); ++it) {
        out << "<key name=\"" << (*it).first.c_str() << "\">";
        (*it).second->writeXml(out);
        out << "</key>";
    }
    out << "</map>";
}

void Map::add(const std::string& name, Value* value)
{
    Assert(utils::ArgError, value, "Map: add empty value");

    iterator it = m_value.find(name);
    if (it != m_value.end()) {
        delete it->second;
        it->second = value;
    } else {
        m_value[name] = value;
    }
}

void Map::addClone(const std::string& name, const Value& value)
{
    iterator it = m_value.find(name);
    if (it != m_value.end()) {
	delete it->second;
        it->second = value.clone();
    } else {
        m_value[name] = value.clone();
    }
}

void Map::addClone(const std::string& name, const Value* value)
{
    Assert(utils::ArgError, value, (boost::format(
           "Map: add empty value with key '%1%'") % name));

    iterator it = m_value.find(name);
    if (it != m_value.end()) {
        delete it->second;
        it->second = value->clone();
    } else {
        m_value[name] = value->clone();
    }
}

const Value& Map::operator[](const std::string& name) const
{
    const_iterator it = m_value.find(name);
    Assert(utils::ArgError, it != m_value.end(),
	    (boost::format("Map: the key '%1%' does not exist") % name));
    assert(it->second);
    return *it->second;
}

Value& Map::operator[](const std::string& name)
{
    iterator it = m_value.find(name);
    Assert(utils::ArgError, it != m_value.end(),
	    (boost::format("Map: the key '%1%' does not exist") % name));

    assert((*it).second);
    return *it->second;
}

const Value& Map::get(const std::string& name) const
{
    const_iterator it = m_value.find(name);
    Assert(utils::ArgError, it != m_value.end(),
           (boost::format("Map: the key '%1%' does not exist") % name));

    assert((*it).second);
    return *(*it).second;
}

Value& Map::get(const std::string& name)
{
    iterator it = m_value.find(name);
    Assert(utils::ArgError, it != m_value.end(),
            (boost::format("Map: the key '%1%' does not exist") % name));

    assert((*it).second);
    return *(*it).second;
}

Value* Map::give(const std::string& name)
{
    iterator it = m_value.find(name);
    Assert(utils::ArgError, it != m_value.end(),
            (boost::format("Map: the key '%1%' does not exist") % name));

    Value* result = it->second;
    it->second = 0;
    m_value.erase(it);

    return result;
}

const std::string& Map::getString(const std::string& name) const
{
    return value::toString(get(name));
}

void Map::addString(const std::string& name, const std::string& value)
{
    add(name, String::create(value));
}

bool Map::getBoolean(const std::string& name) const
{
    const Value* r = getPointer(name);
    return value::toBoolean(r);
}

void Map::addBoolean(const std::string& name, bool value)
{
    add(name, Boolean::create(value));
}

long Map::getLong(const std::string& name) const
{
    return value::toLong(get(name));
}

void Map::addLong(const std::string& name, long value)
{
    add(name, Integer::create(value));
}

int Map::getInt(const std::string& name) const
{
    return value::toInteger(get(name));
}

void Map::addInt(const std::string& name, int value)
{
    add(name, Integer::create(value));
}

double Map::getDouble(const std::string& name) const
{
    return value::toDouble(get(name));
}

void Map::addDouble(const std::string& name, double value)
{
    add(name, Double::create(value));
}

const std::string& Map::getXml(const std::string& name) const
{
    return value::toXml(get(name));
}

void Map::addXml(const std::string& name, const std::string& value)
{
    add(name, Xml::create(value));
}

const Map& Map::getMap(const std::string& name) const
{
    return value::toMapValue(get(name));
}

const Set& Map::getSet(const std::string& name) const
{
    return value::toSetValue(get(name));
}

void Map::clear()
{
    std::stack < Value* > composite;

    for (iterator it = begin(); it != end(); ++it) {
        if (it->second) {
            if (isComposite(it->second)) {
                composite.push(it->second);
            } else {
                delete it->second;
                it->second = 0;
            }
        }
    }

    deleter(composite);
    m_value.clear();
}

Value* Map::getPointer(const std::string& name)
{
    iterator it = m_value.find(name);
    return it == m_value.end() ? 0 : it->second;
}

const Value* Map::getPointer(const std::string& name) const
{
    const_iterator it = m_value.find(name);
    return it == m_value.end() ? 0 : it->second;
}

}} // namespace vle value

BOOST_CLASS_EXPORT(vle::value::Map)

