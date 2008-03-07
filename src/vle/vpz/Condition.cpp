/**
 * @file src/vle/vpz/Condition.cpp
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




#include <vle/vpz/Condition.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Set.hpp>

namespace vle { namespace vpz {

Condition::Condition() :
    Base()
{
}

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
    for (ConditionValues::const_iterator it = cnd.m_list.begin();
         it != cnd.m_list.end(); ++it) {
        m_list[it->first] = toSetValue(it->second->clone());
    }
}

value::Value ValueList::get(const std::string& name)
{
    iterator it;
    if ((it = find(name)) == end()) {
        Throw(utils::InternalError, boost::format(
                "Condition '%1%' does not exist") % name);
    }
    return it->second;
}

bool ValueList::exist(const std::string& name) const
{
    return find(name) != end();
}

const value::Value& ValueList::get(const std::string& name) const
{
    const_iterator it;
    if ((it = find(name)) == end()) {
        Throw(utils::InternalError, boost::format(
                "Condition '%1%' does not exist") % name);
    }
    return it->second;
}

void Condition::write(std::ostream& out) const
{
    out << "<condition name=\"" << m_name << "\" >\n";

    for (ConditionValues::const_iterator it = m_list.begin(); it !=
         m_list.end();
         ++it) {
        out << " <port "
            << "name=\"" << it->first << "\" "
            << ">\n";

        const value::VectorValue& val(value::toSet(it->second));
        for (value::VectorValue::const_iterator jt = val.begin();
             jt != val.end(); ++jt) {
            out << (*jt)->toXML() << "\n";
        }

        out << "</port>\n";
    }

    out << "</condition>\n";
}

void Condition::portnames(std::list < std::string >& lst) const
{
    lst.resize(m_list.size());
    std::transform(m_list.begin(), m_list.end(), lst.begin(), PortName());
}

void Condition::add(const std::string& portname)
{
    if (m_list.find(portname) == m_list.end()) {
        m_list.insert(std::make_pair < std::string, value::Set >(
                portname, value::SetFactory::create()));
    }
    m_last_port.assign(portname);
}

void Condition::del(const std::string& portname)
{
    m_list.erase(portname);
}

void Condition::addValueToPort(const std::string& portname,
                               const value::Value& value)
{
    ConditionValues::iterator it = m_list.find(portname);

    if (it == m_list.end()) {
        value::Set newset = value::SetFactory::create();
        newset->addValue(value);

        m_list.insert(std::make_pair < std::string, value::Set >(
                portname, newset));
        m_last_port.assign(portname);
    } else {
        it->second->addValue(value);
    }
}

void Condition::setValueToPort(const std::string& portname,
                               const value::Value& value)
{
    ConditionValues::iterator it = m_list.find(portname);
    Assert(utils::InternalError, it != m_list.end(),
           boost::format("Condition %1% have no port %2%") %
           m_name % portname);

    it->second->clear();
    it->second->addValue(value);
}

void Condition::clearValueOfPort(const std::string& portname)
{
    ConditionValues::iterator it = m_list.find(portname);
    Assert(utils::InternalError, it != m_list.end(),
           boost::format("Condition %1% have no port %2%") %
           m_name % portname);

    it->second->clear();
}

ValueList Condition::firstValues() const
{
    ValueList result;

    for (ConditionValues::const_iterator it = m_list.begin(); it !=
         m_list.end();
         ++it) {
        if (it->second->size() > 0) {
            result[it->first] = it->second->getValue(0);
        } else {
            Throw(utils::SaxParserError, (boost::format(
                        "Build a empty first values for condition %1%.") %
                    m_name));
        }
    }
    return result;
}

const value::Set& Condition::getSetValues(const std::string& portname) const
{
    ConditionValues::const_iterator it = m_list.find(portname);

    Assert(utils::InternalError, it != m_list.end(),
           boost::format("Condition %1% have no port %2%") %
           m_name % portname);

    return it->second;
}

const value::Value& Condition::firstValue(const std::string& portname) const
{
    return getSetValues(portname)->getValue(0);
}

const value::Value& Condition::nValue(const std::string& portname,
                                      size_t i) const
{
    return getSetValues(portname)->getValue(i);
}

value::Set& Condition::lastAddedPort()
{
    ConditionValues::iterator it = m_list.find(m_last_port);

    Assert(utils::InternalError, it != m_list.end(),
           boost::format("Condition %1% have no port %2%") %
           m_name % m_last_port);

    return it->second;
}

void Condition::rebuildValueSet()
{
    for (ConditionValues::iterator it = m_list.begin(); it != m_list.end();
         ++it) {
        it->second = value::SetFactory::create();
    }
}

}} // namespace vle vpz
