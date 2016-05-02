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


#include <vle/vpz/Condition.hpp>
#include <vle/utils/Algo.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Set.hpp>

namespace vle { namespace vpz {

Condition::Condition(const std::string& name) :
    Base(),
    m_name(name)
{
}

Condition::Condition(const Condition& cnd) :
    Base(cnd),
    m_name(cnd.m_name),
    m_last_port(cnd.m_last_port),
    m_ispermanent(cnd.m_ispermanent)
{
    for (auto & elem : cnd.m_list) {
        m_list[elem.first] =
            std::unique_ptr<value::Set>(
                elem.second ? new value::Set(*elem.second.get()) : nullptr);
    }
}

void Condition::write(std::ostream& out) const
{
    out << "<condition name=\"" << m_name.c_str() << "\" >\n";

    for (const auto & elem : m_list) {
        out << " <port "
            << "name=\"" << elem.first.c_str() << "\" "
            << ">\n";

        assert(elem.second);
        const value::VectorValue& val(value::toSet(*elem.second));
        for (const auto & val_jt : val) {
            (val_jt)->writeXml(out);
            out << "\n";
        }
        out << "</port>\n";
    }
    out << "</condition>\n";
}

std::vector <std::string> Condition::portnames() const
{
    std::vector <std::string> lst(m_list.size());

    std::transform(m_list.begin(), m_list.end(), lst.begin(),
                   [](const value_type& v)
                   {
                       return v.first;
                   });

    return lst;
}

void Condition::add(const std::string& portname)
{
    m_list.insert(
        std::make_pair(portname,
                       std::unique_ptr<value::Set>(new value::Set())));

    m_last_port.assign(portname);
}

void Condition::del(const std::string& portname)
{
    m_list.erase(portname);
}

void Condition::rename(const std::string& oldportname,
                       const std::string& newportname)
{
    value::Set values = getSetValues(oldportname);
    value::Set::const_iterator it;

    if (m_list.find(oldportname) == m_list.end())
        throw utils::ArgError(
            fmt(_("Condition: can not rename inexistant port '%1%'"))
            % oldportname);

    std::swap(m_list[newportname], m_list[oldportname]);
    del(oldportname);
}

void Condition::addValueToPort(const std::string& portname,
                               std::unique_ptr<value::Value> value)
{
    auto it = m_list.find(portname);

    if (it == m_list.end()) {
        auto inserted = m_list.insert(
            std::make_pair(portname, std::unique_ptr<value::Set>(
                               new value::Set())));
        it = inserted.first;
    } else if (not it->second) {
        it->second = std::unique_ptr<value::Set>(new value::Set());
    }

    it->second->add(std::move(value));
    m_last_port.assign(portname);
}

void Condition::setValueToPort(const std::string& portname,
                               std::unique_ptr<value::Value> value)
{
    auto it = m_list.find(portname);

    if (it == m_list.end()) {
        auto inserted = m_list.insert(
            std::make_pair(portname, std::unique_ptr<value::Set>(
                               new value::Set())));
        it = inserted.first;
    } else if (not it->second) {
        it->second = std::unique_ptr<value::Set>(new value::Set());
    }

    it->second->clear();
    it->second->add(std::move(value));
    m_last_port.assign(portname);
}

void Condition::clearValueOfPort(const std::string& portname)
{
    auto it = m_list.find(portname);

    if (it == m_list.end())
        throw utils::ArgError(fmt(
                _("Condition %1% have no port %2%")) % m_name % portname);

    it->second->clear();
}

void Condition::fillWithFirstValues(value::MapValue& mapToFill) const
{
    mapToFill.clear();

    for (const auto & elem : m_list)
        mapToFill[elem.first] =
            std::unique_ptr<value::Value>(
                elem.second->size() ? elem.second->get(0)->clone() : nullptr);
}

const value::Set& Condition::getSetValues(const std::string& portname) const
{
    auto it = m_list.find(portname);

    if (it == m_list.end()) {
        throw utils::ArgError(fmt(_("Condition %1% have no port %2%")) % m_name
                              % portname);
    }

    assert(it->second);
    return *it->second;
}

value::Set& Condition::getSetValues(const std::string& portname)
{
    auto it = m_list.find(portname);

    if (it == m_list.end()) {
        throw utils::ArgError(fmt(_("Condition %1% have no port %2%")) % m_name
                              % portname);
    }

    assert(it->second);
    return *it->second;
}

const value::Value& Condition::firstValue(const std::string& portname) const
{
    return *getSetValues(portname).get(0);
}

const value::Value& Condition::nValue(const std::string& portname,
                                      size_t i) const
{
    return *getSetValues(portname).get(i);
}

value::Set& Condition::lastAddedPort()
{
    auto it = m_list.find(m_last_port);

    if (it == m_list.end()) {
        throw utils::ArgError(fmt(_("Condition %1% have no port %2%")) % m_name
                              % m_last_port);
    }

    assert(it->second);
    return *it->second;
}


void Condition::deleteValueSet()
{
    for (auto & elem : m_list) {
        elem.second->clear();
    }
}

}} // namespace vle vpz
