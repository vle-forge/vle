/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/value/Map.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/utils/Algo.hpp>
#include <boost/checked_delete.hpp>

namespace vle { namespace value {

Map::Map(const Map& orig)
    : Value(orig)
{
    for (const_iterator it = orig.begin(); it != orig.end(); ++it) {
        if ((*it).second) {
            m_value.insert(std::make_pair((*it).first,
                                          ((*it).second)->clone()));
        } else {
            m_value.insert(std::make_pair((*it).first, (value::Value*)0));
        }
    }
}

void Map::writeFile(std::ostream& out) const
{
    for (const_iterator it = begin(); it != end(); ++it) {
        if (it != begin()) {
            out << " ";
        }
        out << "(" << (*it).first.c_str() << ", ";
        (*it).second->writeFile(out);
        out << ")";
    }
}

void Map::writeString(std::ostream& out) const
{
    for (const_iterator it = begin(); it != end(); ++it) {
        if (it != begin()) {
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

    for (const_iterator it = begin(); it != end(); ++it) {
        out << "<key name=\"" << (*it).first.c_str() << "\">";
        (*it).second->writeXml(out);
        out << "</key>";
    }
    out << "</map>";
}

Matrix& Map::addMatrix(const std::string& name)
{
    value::Matrix* value = new Matrix();

    add(name, value);

    return *value;
}

Set& Map::addSet(const std::string& name)
{
    value::Set* value = new Set();

    add(name, value);

    return *value;
}

Map& Map::addMap(const std::string& name)
{
    value::Map* value = new Map();

    add(name, value);

    return *value;
}

const Value* Map::operator[](const std::string& name) const
{
    const_iterator it = find(name);

    if (it == end()) {
        throw utils::ArgError(fmt(_(
                "Map: the key '%1%' does not exist")) % name);
    }

    return it->second;
}

Value* Map::operator[](const std::string& name)
{
    iterator it = find(name);

    if (it == end()) {
        throw utils::ArgError(fmt(_(
                "Map: the key '%1%' does not exist")) % name);
    }

    return it->second;
}

Value* Map::give(const std::string& name)
{
    iterator it = find(name);

    if (it == end()) {
        throw utils::ArgError(fmt(_(
                "Map: the key '%1%' does not exist")) % name);
    }

    Value* result = it->second;
    it->second = 0;
    m_value.erase(it);

    return result;
}

const Map& Map::getMap(const std::string& name) const
{
    return value::toMapValue(value::reference(get(name)));
}

const Set& Map::getSet(const std::string& name) const
{
    return value::toSetValue(value::reference(get(name)));
}

const Matrix& Map::getMatrix(const std::string& name) const
{
    return value::toMatrixValue(value::reference(get(name)));
}

Map& Map::getMap(const std::string& name)
{
    return value::toMapValue(value::reference(get(name)));
}

Set& Map::getSet(const std::string& name)
{
    return value::toSetValue(value::reference(get(name)));
}

Matrix& Map::getMatrix(const std::string& name)
{
    return value::toMatrixValue(value::reference(get(name)));
}

void Map::clear()
{
    vle::utils::forEach(begin(), end(), boost::checked_deleter < Value >());

    m_value.clear();
}

Value* Map::getPointer(const std::string& name)
{
    iterator it = find(name);

    return it == end() ? 0 : it->second;
}

const Value* Map::getPointer(const std::string& name) const
{
    const_iterator it = find(name);

    return it == end() ? 0 : it->second;
}

}} // namespace vle value
