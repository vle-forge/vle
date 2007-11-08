/** 
 * @file vpz/Conditions.cpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 31 jan 2006 17:29:08 +0100
 */

/*
 * Copyright (C) 2006 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <vle/vpz/Conditions.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/value/Value.hpp>

namespace vle { namespace vpz {

using namespace vle::utils;

Conditions::Conditions() :
    Base()
{
}

void Conditions::write(std::ostream& out) const
{
    if (not m_list.empty()) {
        out << "<conditions>\n";

        for (ConditionList::const_iterator it = m_list.begin(); 
             it != m_list.end(); ++it) {
            out << it->second;
        }

        out << "</conditions>\n";
    }
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
