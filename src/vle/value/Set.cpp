/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#include <fstream>
#include <sstream>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/XML.hpp>

namespace {

inline void
pp_check_index(const vle::value::Set& s, std::size_t i)
{
#ifndef VLE_FULL_OPTIMIZATION
    if (i >= s.size())
        throw vle::utils::ArgError(
          (vle::fmt(_("Set: too big index '%1%' (max %2%)")) % i % s.size())
            .str());
#else
    (void)s;
    (void)i;
#endif
}

inline vle::value::Set::iterator
pp_get(vle::value::Set& s, std::size_t i)
{
    pp_check_index(s, i);
    return s.begin() + i;
}

inline vle::value::Set::const_iterator
pp_get(const vle::value::Set& s, std::size_t i)
{
    pp_check_index(s, i);
    return s.begin() + i;
}

inline vle::value::Value&
pp_get_value(vle::value::Set& s, std::size_t i)
{
    auto it = pp_get(s, i);

    if (not it->get())
        throw vle::utils::ArgError(
          (vle::fmt(_("Set: empty or null value at '%1%'")) % i).str());

    return *it->get();
}

inline const vle::value::Value&
pp_get_value(const vle::value::Set& s, std::size_t i)
{
    auto it = pp_get(s, i);

    if (not it->get())
        throw vle::utils::ArgError(
          (vle::fmt(_("Set: empty or null value at '%1%'")) % i).str());

    return *it->get();
}

template <typename T, typename... Args>
T&
pp_add(vle::value::Set& s, Args&&... args)
{
    auto value =
      std::unique_ptr<vle::value::Value>(new T(std::forward<Args>(args)...));
    auto* ret = static_cast<T*>(value.get());

    s.value().emplace_back(std::move(value));

    return *ret;
}
}

namespace vle {
namespace value {

Set::Set(size_type size)
  : m_value(size)
{
}

Set::Set(const Set& setfactory)
  : Value(setfactory)
{
    m_value.reserve(setfactory.size());

    for (const auto& elem : setfactory.m_value)
        m_value.emplace_back(
          std::unique_ptr<Value>(elem.get() ? elem->clone() : nullptr));
}

Value::type
Set::getType() const
{
    return Value::SET;
}

void
Set::writeFile(std::ostream& out) const
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

void
Set::writeString(std::ostream& out) const
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

void
Set::writeXml(std::ostream& out) const
{
    out << "<set>";

    for (const auto& elem : m_value) {
        if (elem) {
            (elem)->writeXml(out);
        } else {
            out << "<null />";
        }
    }

    out << "</set>";
}

void
Set::set(size_type i, std::unique_ptr<Value> val)
{
    pp_check_index(*this, i);
    m_value[i] = std::move(val);
}

const std::unique_ptr<Value>&
Set::get(size_type i) const
{
    pp_check_index(*this, i);
    return m_value[i];
}

const std::unique_ptr<Value>& Set::operator[](size_type i) const
{
    pp_check_index(*this, i);
    return m_value[i];
}

std::unique_ptr<Value>
Set::give(size_type i)
{
    auto it = pp_get(*this, i);
    return std::move(*it);
}

Set&
Set::addSet()
{
    return pp_add<Set>(*this);
}

Map&
Set::addMap()
{
    return pp_add<Map>(*this);
}

Matrix&
Set::addMatrix()
{
    return pp_add<Matrix>(*this);
}

Set&
Set::getSet(size_type i)
{
    return pp_get_value(*this, i).toSet();
}

Map&
Set::getMap(size_type i)
{
    return pp_get_value(*this, i).toMap();
}

Matrix&
Set::getMatrix(size_type i)
{
    return pp_get_value(*this, i).toMatrix();
}

const Set&
Set::getSet(size_type i) const
{
    return pp_get_value(*this, i).toSet();
}

const Map&
Set::getMap(size_type i) const
{
    return pp_get_value(*this, i).toMap();
}

const Matrix&
Set::getMatrix(size_type i) const
{
    return pp_get_value(*this, i).toMatrix();
}

void
Set::add(std::unique_ptr<Value> value)
{
    m_value.push_back(std::move(value));
}

Null&
Set::addNull()
{
    return ::pp_add<Null>(*this);
}

Boolean&
Set::addBoolean(bool value)
{
    return ::pp_add<Boolean>(*this, value);
}

bool
Set::getBoolean(size_type i) const
{
    return ::pp_get_value(*this, i).toBoolean().value();
}

bool&
Set::getBoolean(size_type i)
{
    return ::pp_get_value(*this, i).toBoolean().value();
}

Double&
Set::addDouble(double value)
{
    return ::pp_add<Double>(*this, value);
}

double
Set::getDouble(size_type i) const
{
    return ::pp_get_value(*this, i).toDouble().value();
}

double&
Set::getDouble(size_type i)
{
    return ::pp_get_value(*this, i).toDouble().value();
}

Integer&
Set::addInt(int32_t value)
{
    return ::pp_add<Integer>(*this, value);
}

int32_t
Set::getInt(size_type i) const
{
    return ::pp_get_value(*this, i).toInteger().value();
}

int32_t&
Set::getInt(size_type i)
{
    return ::pp_get_value(*this, i).toInteger().value();
}

String&
Set::addString(const std::string& value)
{
    return ::pp_add<String>(*this, value);
}

const std::string&
Set::getString(size_type i) const
{
    return ::pp_get_value(*this, i).toString().value();
}

std::string&
Set::getString(size_type i)
{
    return ::pp_get_value(*this, i).toString().value();
}

Xml&
Set::addXml(const std::string& value)
{
    return ::pp_add<Xml>(*this, value);
}

const std::string&
Set::getXml(size_type i) const
{
    return ::pp_get_value(*this, i).toXml().value();
}

std::string&
Set::getXml(size_type i)
{
    return ::pp_get_value(*this, i).toXml().value();
}

Table&
Set::addTable(size_t width, size_t height)
{
    return ::pp_add<Table>(*this, width, height);
}

const Table&
Set::getTable(size_type i) const
{
    return ::pp_get_value(*this, i).toTable();
}

Table&
Set::getTable(size_type i)
{
    return ::pp_get_value(*this, i).toTable();
}

Tuple&
Set::addTuple(size_t width, double value)
{
    return ::pp_add<Tuple>(*this, width, value);
}

const Tuple&
Set::getTuple(size_type i) const
{
    return ::pp_get_value(*this, i).toTuple();
}

Tuple&
Set::getTuple(size_type i)
{
    return ::pp_get_value(*this, i).toTuple();
}

void
Set::remove(size_type i)
{
    ::pp_check_index(*this, i);
    m_value.erase(begin() + i);
}

void
Set::resize(size_type newSize, const Value& fill)
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
}
} // namespace vle value
