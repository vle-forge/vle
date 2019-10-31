/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2003-2018 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2018 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2018 INRA http://www.inra.fr
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

#include <vle/utils/Algo.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
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
#include <vle/vpz/Condition.hpp>

#include "utils/i18n.hpp"

#include <algorithm>
#include <ostream>
#include <string>
#include <utility>

#include <cassert>

namespace vle {
namespace vpz {

Condition::Condition(const std::string& name)
  : Base()
  , m_name(std::move(name))
{}

Condition::Condition(const Condition& cnd)
  : Base(cnd)
  , m_name(cnd.m_name)
  , m_last_port(cnd.m_last_port)
  , m_ispermanent(cnd.m_ispermanent)
{
    for (auto& elem : cnd.m_list) {
        auto& init = m_list[elem.first];
        init = std::move(value::clone(elem.second));
    }
}

Condition&
Condition::operator=(const Condition& cnd)
{
    Condition tmp(cnd);

    std::swap(m_list, tmp.m_list);
    std::swap(m_name, tmp.m_name);
    std::swap(m_last_port, tmp.m_last_port);
    std::swap(m_ispermanent, tmp.m_ispermanent);

    return *this;
}

void
Condition::write(std::ostream& out) const
{
    out << "<condition name=\"" << m_name.c_str() << "\" >\n";

    for (const auto& elem : m_list) {
        out << " <port "
            << "name=\"" << elem.first.c_str() << "\" "
            << ">\n";
        auto& v = elem.second;
        if (v.get()) {
            v->writeXml(out);
            out << '\n';
        }
        out << "</port>\n";
    }
    out << "</condition>\n";
}

std::vector<std::string>
Condition::portnames() const
{
    std::vector<std::string> lst(m_list.size());

    std::transform(m_list.begin(),
                   m_list.end(),
                   lst.begin(),
                   [](const value_type& v) { return v.first; });

    return lst;
}

bool
Condition::exist(const std::string& portname) const
{
    return m_list.find(portname) != m_list.end();
}

void
Condition::add(const std::string& portname)
{
    m_list[portname] = std::shared_ptr<value::Value>(nullptr);
    m_last_port.assign(portname);
}

void
Condition::del(const std::string& portname)
{
    m_list.erase(portname);
}

void
Condition::setValueToPort(const std::string& portname,
                          std::shared_ptr<value::Value> value)
{
    auto& v = m_list[portname];
    v = value;
    m_last_port.assign(portname);
}

void
Condition::clearValueOfPort(const std::string& portname)
{
    auto it = m_list.find(portname);

    if (it == m_list.end())
        throw utils::ArgError(
          _("Condition %s have no port %s"), m_name.c_str(), portname.c_str());
    it->second.reset();
}

const std::shared_ptr<value::Value>&
Condition::valueOfPort(const std::string& portname) const
{
    auto it = m_list.find(portname);

    if (it == m_list.end()) {
        throw utils::ArgError(
          _("Condition %s have no port %s"), m_name.c_str(), portname.c_str());
    }
    return it->second;
}

std::shared_ptr<value::Value>&
Condition::lastAddedPort()
{
    auto it = m_list.find(m_last_port);

    if (it == m_list.end()) {
        throw utils::ArgError(_("Condition %s have no port %s"),
                              m_name.c_str(),
                              m_last_port.c_str());
    }
    return it->second;
}

}
} // namespace vle vpz
