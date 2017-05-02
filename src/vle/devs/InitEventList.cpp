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

#include <vle/devs/InitEventList.hpp>
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

inline vle::devs::InitEventList::container_type::const_iterator
pp_get(const vle::devs::InitEventList::container_type& m,
       const std::string& name)
{
    auto it = m.find(name);

    if (it == m.end())
        throw vle::utils::ArgError(
          (vle::fmt(_("Map: the key '%1%' does not exist")) % name).str());

    return it;
}

inline const vle::value::Value&
pp_get_value(const vle::devs::InitEventList::container_type& m,
             const std::string& name)
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
pp_add(vle::devs::InitEventList::container_type& m,
       const std::string& name,
       Args&&... args)
{
    auto value = std::shared_ptr<const vle::value::Value>(
      new T(std::forward<Args>(args)...));
    auto* ret = static_cast<T*>(value.get());

    std::swap(m[name], value);

    return *ret;
}
}

namespace vle {
namespace devs {

void
InitEventList::add(const std::string& name,
                   std::shared_ptr<const value::Value> value)
{
    std::swap(m_value[name], value);
}

const std::shared_ptr<const value::Value>& InitEventList::operator[](
  const std::string& name) const
{
    return ::pp_get(m_value, name)->second;
}

const std::shared_ptr<const value::Value>&
InitEventList::get(const std::string& name) const
{
    return ::pp_get(m_value, name)->second;
}

const value::Map&
InitEventList::getMap(const std::string& name) const
{
    return ::pp_get_value(m_value, name).toMap();
}

const value::Set&
InitEventList::getSet(const std::string& name) const
{
    return ::pp_get_value(m_value, name).toSet();
}

const value::Matrix&
InitEventList::getMatrix(const std::string& name) const
{
    return ::pp_get_value(m_value, name).toMatrix();
}

const std::string&
InitEventList::getString(const std::string& name) const
{
    return ::pp_get_value(m_value, name).toString().value();
}

bool
InitEventList::getBoolean(const std::string& name) const
{
    return ::pp_get_value(m_value, name).toBoolean().value();
}

int32_t
InitEventList::getInt(const std::string& name) const
{
    return ::pp_get_value(m_value, name).toInteger().value();
}

double
InitEventList::getDouble(const std::string& name) const
{
    return ::pp_get_value(m_value, name).toDouble().value();
}

const std::string&
InitEventList::getXml(const std::string& name) const
{
    return ::pp_get_value(m_value, name).toXml().value();
}

const value::Table&
InitEventList::getTable(const std::string& name) const
{
    return ::pp_get_value(m_value, name).toTable();
}

const value::Tuple&
InitEventList::getTuple(const std::string& name) const
{
    return ::pp_get_value(m_value, name).toTuple();
}
}
} // namespace vle devs
