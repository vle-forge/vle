/**
 * @file src/vle/vpz/Conditions.cpp
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




#include <vle/vpz/Conditions.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/value/Value.hpp>
#include <algorithm>

namespace vle { namespace vpz {

Conditions::Conditions() :
    Base()
{
}

void Conditions::write(std::ostream& out) const
{
    if (not m_list.empty()) {
        out << "<conditions>\n";

        std::transform(m_list.begin(), m_list.end(),
                       std::ostream_iterator < Condition >(out),
                       ConditionValue());

        out << "</conditions>\n";
    }
}

void Conditions::conditionnames(std::list < std::string >& lst) const
{
    lst.resize(m_list.size());
    std::transform(m_list.begin(), m_list.end(), lst.begin(), ConditionName());
}

void Conditions::portnames(const std::string& condition,
                           std::list < std::string >& lst) const
{
    const Condition& cnd(get(condition));
    cnd.portnames(lst);
}

void Conditions::add(const Conditions& cdts)
{
    std::for_each(cdts.conditionlist().begin(), cdts.conditionlist().end(),
                  AddCondition(*this));
}

Condition& Conditions::add(const Condition& condition)
{
    ConditionList::const_iterator it = m_list.find(condition.name());
    Assert(utils::InternalError, it == m_list.end(),
           boost::format("The condition %1% already exist") %
           condition.name());

    return (*m_list.insert(
            std::make_pair < std::string, Condition >(
                condition.name(), condition)).first).second;
}

void Conditions::del(const std::string& condition)
{
    m_list.erase(condition);
}

const Condition& Conditions::get(const std::string& condition) const
{
    ConditionList::const_iterator it = m_list.find(condition);
    Assert(utils::InternalError, it != m_list.end(),
           boost::format("The condition %1% not exist") %
           condition);

    return it->second;
}

Condition& Conditions::get(const std::string& condition)
{
    ConditionList::iterator it = m_list.find(condition);
    Assert(utils::InternalError, it != m_list.end(),
           boost::format("The condition %1% not exist") %
           condition);

    return it->second;
}
    
void Conditions::cleanNoPermanent()
{
    ConditionList::iterator prev = m_list.begin();
    ConditionList::iterator it = m_list.begin();
    
    while (it != m_list.end()) {
        if (not it->second.isPermanent()) {
            if (prev == it) {
                m_list.erase(it);
                prev = m_list.begin();
                it = m_list.begin();
            } else {
                m_list.erase(it);
                it = prev;
            }
        } else {
            ++prev;
            ++it;
        }
    }
}

void Conditions::rebuildValueSet()
{
    std::for_each(m_list.begin(), m_list.end(), RebuildValueSet());
}

}} // namespace vle vpz
