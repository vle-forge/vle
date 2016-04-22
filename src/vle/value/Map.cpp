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

namespace vle { namespace value {

Map::Map(const Map& orig)
    : Value(orig)
{
    for (const auto & elem : orig.m_value)
        m_value[elem.first] =
            std::unique_ptr<Value>(
                elem.second ? elem.second->clone() : nullptr);
}

std::unique_ptr<Value> Map::clone() const
{
    return std::unique_ptr <Value>(new Map(*this));
}

Value::type Map::getType() const
{
    return Value::MAP;
}

void Map::writeFile(std::ostream& out) const
{
    for (auto it = begin(); it != end(); ++it) {
        if (it != begin())
            out << " ";

        out << "(" << (*it).first.c_str() << ", ";
        (*it).second->writeFile(out);
        out << ")";
    }
}

void Map::writeString(std::ostream& out) const
{
    for (auto it = begin(); it != end(); ++it) {
        if (it != begin())
            out << " ";

        out << "(" << (*it).first.c_str() << ", ";
        (*it).second->writeString(out);
        out << ")";
    }
}

void Map::writeXml(std::ostream& out) const
{
    out << "<map>";

    for (const auto & elem : *this) {
        out << "<key name=\"" << (elem).first.c_str() << "\">";
        (elem).second->writeXml(out);
        out << "</key>";
    }

    out << "</map>";
}

Map& Map::addMap(const std::string& name)
{
    return pp_add<Map>(name);
}
    
Set& Map::addSet(const std::string& name)
{
    return pp_add<Set>(name);
}

Matrix& Map::addMatrix(const std::string& name)
{
    return pp_add<Matrix>(name);
}

Map& Map::getMap(const std::string& name)
{
    return pp_get_value(name).toMap();
}

Set& Map::getSet(const std::string& name)
{
    return pp_get_value(name).toSet();
}

Matrix& Map::getMatrix(const std::string& name)
{
    return pp_get_value(name).toMatrix();
}

const Map& Map::getMap(const std::string& name) const
{
    return pp_get_value(name).toMap();
}

const Set& Map::getSet(const std::string& name) const
{
    return pp_get_value(name).toSet();
}

const Matrix& Map::getMatrix(const std::string& name) const
{
    return pp_get_value(name).toMatrix();
}


void
Map::addMultilpleValues(int toadd, const vle::value::Value& fill)
{
    if (toadd <= 0)
        return;

    std::string key;
    bool found;
    
    for (auto i=0; i < toadd; ++i) {
        key.assign("NewKey");
        int current = 0;
        found = false;
        
        while (not found) {
            found = not exist(key);
            if (not found) {
                current ++;
                key.assign("NewKey");
                key += "_";
                key += std::to_string(current);
            }
        }
        auto to_add = std::unique_ptr<Value>(fill.clone());
        
        std::swap(m_value[key], to_add);
    }
}

}} // namespace vle value
