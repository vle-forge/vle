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

inline vle::value::MapValue::iterator
pp_get(vle::value::MapValue& m, const std::string& name)
{
    auto it = m.find(name);

    if (it == m.end())
        throw vle::utils::ArgError(
          (vle::fmt(_("Map: the key '%1%' does not exist")) % name).str());

    return it;
}

inline vle::value::MapValue::const_iterator
pp_get(const vle::value::MapValue& m, const std::string& name)
{
    auto it = m.find(name);

    if (it == m.end())
        throw vle::utils::ArgError(
          (vle::fmt(_("Map: the key '%1%' does not exist")) % name).str());

    return it;
}

inline vle::value::Value&
pp_get_value(vle::value::MapValue& m, const std::string& name)
{
    auto it = pp_get(m, name);

    if (not it->second)
        throw vle::utils::ArgError(
          (vle::fmt(_("Map: the key '%1%' have empty (null) value")) % name)
            .str());

    return *it->second.get();
}

inline const vle::value::Value&
pp_get_value(const vle::value::MapValue& m, const std::string& name)
{
    auto it = pp_get(m, name);

    if (not it->second)
        throw vle::utils::ArgError(
          (vle::fmt(_("Map: the key '%1%' have empty (null) value")) % name)
            .str());

    return *it->second.get();
}

template <typename T, typename... Args>
T&
pp_add(vle::value::MapValue& m, const std::string& name, Args&&... args)
{
    auto value =
      std::unique_ptr<vle::value::Value>(new T(std::forward<Args>(args)...));
    auto* ret = static_cast<T*>(value.get());

    std::swap(m[name], value);

    return *ret;
}
}

namespace vle {
namespace value {

Map::Map(const Map& orig)
  : Value(orig)
{
    for (const auto& elem : orig.m_value)
        m_value[elem.first] =
          std::unique_ptr<Value>(elem.second ? elem.second->clone() : nullptr);
}

std::unique_ptr<Value>
Map::clone() const
{
    return std::unique_ptr<Value>(new Map(*this));
}

Value::type
Map::getType() const
{
    return Value::MAP;
}

void
Map::writeFile(std::ostream& out) const
{
    for (auto it = begin(); it != end(); ++it) {
        if (it != begin())
            out << " ";

        out << "(" << (*it).first.c_str() << ", ";
        (*it).second->writeFile(out);
        out << ")";
    }
}

void
Map::writeString(std::ostream& out) const
{
    for (auto it = begin(); it != end(); ++it) {
        if (it != begin())
            out << " ";

        out << "(" << (*it).first.c_str() << ", ";
        (*it).second->writeString(out);
        out << ")";
    }
}

void
Map::writeXml(std::ostream& out) const
{
    out << "<map>";

    for (const auto& elem : *this) {
        out << "<key name=\"" << (elem).first.c_str() << "\">";
        (elem).second->writeXml(out);
        out << "</key>";
    }

    out << "</map>";
}

void
Map::add(const std::string& name, std::unique_ptr<Value> value)
{
    std::swap(m_value[name], value);
}

void
Map::set(const std::string& name, std::unique_ptr<Value> value)
{
    std::swap(m_value[name], value);
}

bool
Map::exist(const std::string& name) const
{
    return m_value.find(name) != m_value.end();
}

const std::unique_ptr<Value>& Map::operator[](const std::string& name) const
{
    return ::pp_get(m_value, name)->second;
}

const std::unique_ptr<Value>&
Map::get(const std::string& name) const
{
    return ::pp_get(m_value, name)->second;
}

std::unique_ptr<Value>
Map::give(const std::string& name)
{
    auto it = ::pp_get(m_value, name);
    return std::move(it->second);
}

Map&
Map::addMap(const std::string& name)
{
    return ::pp_add<Map>(m_value, name);
}

Set&
Map::addSet(const std::string& name)
{
    return ::pp_add<Set>(m_value, name);
}

Matrix&
Map::addMatrix(const std::string& name)
{
    return ::pp_add<Matrix>(m_value, name);
}

Map&
Map::getMap(const std::string& name)
{
    return ::pp_get_value(m_value, name).toMap();
}

Set&
Map::getSet(const std::string& name)
{
    return ::pp_get_value(m_value, name).toSet();
}

Matrix&
Map::getMatrix(const std::string& name)
{
    return ::pp_get_value(m_value, name).toMatrix();
}

const Map&
Map::getMap(const std::string& name) const
{
    return ::pp_get_value(m_value, name).toMap();
}

const Set&
Map::getSet(const std::string& name) const
{
    return ::pp_get_value(m_value, name).toSet();
}

const Matrix&
Map::getMatrix(const std::string& name) const
{
    return ::pp_get_value(m_value, name).toMatrix();
}

Null&
Map::addNull(const std::string& name)
{
    return ::pp_add<Null>(m_value, name);
}

const std::string&
Map::getString(const std::string& name) const
{
    return ::pp_get_value(m_value, name).toString().value();
}

std::string&
Map::getString(const std::string& name)
{
    return ::pp_get_value(m_value, name).toString().value();
}

String&
Map::addString(const std::string& name, const std::string& value)
{
    return ::pp_add<String>(m_value, name, value);
}

bool
Map::getBoolean(const std::string& name) const
{
    return ::pp_get_value(m_value, name).toBoolean().value();
}

bool&
Map::getBoolean(const std::string& name)
{
    return ::pp_get_value(m_value, name).toBoolean().value();
}

Boolean&
Map::addBoolean(const std::string& name, bool value)
{
    return ::pp_add<Boolean>(m_value, name, value);
}

int32_t
Map::getInt(const std::string& name) const
{
    return ::pp_get_value(m_value, name).toInteger().value();
}

int32_t&
Map::getInt(const std::string& name)
{
    return ::pp_get_value(m_value, name).toInteger().value();
}

void
Map::addInt(const std::string& name, const int32_t& value)
{
    ::pp_add<Integer>(m_value, name, value);
}

double
Map::getDouble(const std::string& name) const
{
    return ::pp_get_value(m_value, name).toDouble().value();
}

double&
Map::getDouble(const std::string& name)
{
    return ::pp_get_value(m_value, name).toDouble().value();
}

Double&
Map::addDouble(const std::string& name, const double& value)
{
    return ::pp_add<Double>(m_value, name, value);
}

const std::string&
Map::getXml(const std::string& name) const
{
    return ::pp_get_value(m_value, name).toXml().value();
}

std::string&
Map::getXml(const std::string& name)
{
    return ::pp_get_value(m_value, name).toXml().value();
}

Xml&
Map::addXml(const std::string& name, const std::string& value)
{
    return ::pp_add<Xml>(m_value, name, value);
}

const Table&
Map::getTable(const std::string& name) const
{
    return ::pp_get_value(m_value, name).toTable();
}

Table&
Map::getTable(const std::string& name)
{
    return ::pp_get_value(m_value, name).toTable();
}

Table&
Map::addTable(const std::string& name,
              Table::size_type width,
              Table::size_type height)
{
    return ::pp_add<Table>(m_value, name, width, height);
}

const Tuple&
Map::getTuple(const std::string& name) const
{
    return ::pp_get_value(m_value, name).toTuple();
}

Tuple&
Map::getTuple(const std::string& name)
{
    return ::pp_get_value(m_value, name).toTuple();
}

Tuple&
Map::addTuple(const std::string& name, std::size_t width, double value)
{
    return ::pp_add<Tuple>(m_value, name, width, value);
}

void
Map::addMultilpleValues(int toadd, const vle::value::Value& fill)
{
    if (toadd <= 0)
        return;

    std::string key;
    bool found;

    for (auto i = 0; i < toadd; ++i) {
        key.assign("NewKey");
        int current = 0;
        found = false;

        while (not found) {
            found = not exist(key);
            if (not found) {
                current++;
                key.assign("NewKey");
                key += "_";
                key += std::to_string(current);
            }
        }
        auto to_add = std::unique_ptr<Value>(fill.clone());

        std::swap(m_value[key], to_add);
    }
}
}
} // namespace vle value
