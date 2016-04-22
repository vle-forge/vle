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


#include <vle/value/Set.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/XML.hpp>
#include <vle/value/Null.hpp>
#include <fstream>
#include <sstream>

namespace vle { namespace value {

Set::Set(const size_type& size)
    : m_value(size)
{}

Set::Set(const Set& setfactory)
    : Value(setfactory)
{
    m_value.reserve(setfactory.size());

    for (const auto & elem : setfactory.m_value)
        m_value.emplace_back(
            std::unique_ptr<Value>(elem.get() ? elem->clone() : nullptr));
}

void Set::writeFile(std::ostream& out) const
{
    for (auto it = m_value.begin(); it != m_value.end(); ++it) {
        if (it != m_value.begin()) {
            out << ",";
        }
        if (*it) {
            (*it)->writeFile(out);
        } else {
            out << "NA";
        }
    }
}

void Set::writeString(std::ostream& out) const
{
    out << "(";

    for (auto it = m_value.begin(); it != m_value.end(); ++it) {
        if (it != m_value.begin()) {
            out << ",";
        }
        if (*it) {
            (*it)->writeString(out);
        } else {
            out << "NA";
        }
    }

    out << ")";
}

void Set::writeXml(std::ostream& out) const
{
    out << "<set>";

    for (const auto & elem : m_value) {
        if (elem) {
            (elem)->writeXml(out);
        } else {
            out << "<null />";
        }
    }

    out << "</set>";
}

Set& Set::addSet()
{
    return pp_add<Set>();
}

Map& Set::addMap()
{
    return pp_add<Map>();
}

Matrix& Set::addMatrix()
{
    return pp_add<Matrix>();
}

Set& Set::getSet(const size_type& i)
{
    return pp_get_value(i).toSet();
}

Map& Set::getMap(const size_type& i)
{
    return pp_get_value(i).toMap(); 
}

Matrix& Set::getMatrix(const size_type& i)
{
    return pp_get_value(i).toMatrix();
}

const Set& Set::getSet(const size_type& i) const
{
    return pp_get_value(i).toSet();
}

const Map& Set::getMap(const size_type& i) const
{
    return pp_get_value(i).toMap();
}

const Matrix& Set::getMatrix(const size_type& i) const
{
    return pp_get_value(i).toMatrix();
}

void Set::resize(size_type newSize, const Value& fill)
{
    if (newSize == size())
        return;

    if (newSize > size()) {
        auto old_size = size();
        auto to_fill = newSize - size();
        m_value.resize(newSize);

        for (size_t i = 0; i != to_fill; ++i)
            m_value[old_size + i] = fill.clone();
        
        return;
    }
    
    m_value.resize(newSize);
}

}} // namespace vle value
