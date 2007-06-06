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
    if (not m_conditions.empty()) {
        out << "<experimental_conditions>";

        for (const_iterator it = m_conditions.begin(); it != m_conditions.end();
             ++it) {
            out << it->second;
        }

        out << "</experimental_conditions>";
    }
}

void Conditions::addConditions(const Conditions& conditions)
{
    const ConditionList& cdts(conditions.conditions());
    for (const_iterator it = cdts.begin(); it != cdts.end(); ++it)
        addCondition(it->second);
}

void Conditions::addCondition(const Condition& condition)
{
    const_iterator it = m_conditions.find(condition.name());
    
    Assert(utils::InternalError, it == m_conditions.end(),
           boost::format("The condition %1% already exist") %
           condition.name());

    m_conditions[condition.name()] = condition;
}

void Conditions::clear()
{
    m_conditions.clear();
}

void Conditions::delCondition(const std::string& condition)
{
    m_conditions.erase(condition);
}

const Condition& Conditions::find(const std::string& condition) const
{
    const_iterator it = m_conditions.find(condition);

    Assert(utils::InternalError, it != m_conditions.end(),
           boost::format("The condition %1% not exist") %
           condition);

    return it->second;
}

Condition& Conditions::find(const std::string& condition)
{
    iterator it = m_conditions.find(condition);

    Assert(utils::InternalError, it != m_conditions.end(),
           boost::format("The condition %1% not exist") %
           condition);

    return it->second;
}

}} // namespace vle vpz
