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

Conditions::Conditions()
{
}

void Conditions::write(std::ostream& out) const
{
    if (not m_conditions.empty()) {
        out << "<experimental_conditions>";

        std::copy(m_conditions.begin(), m_conditions.end(),
                  std::ostream_iterator < Condition >(out, "\n"));

        out << "</experimental_conditions>";
    }
}

void Conditions::addConditions(const Conditions& c)
{
    std::unique_copy(c.conditions().begin(), c.conditions().end(),
                     m_conditions.begin());
}

void Conditions::addCondition(const Condition& c)
{
    ConditionList::const_iterator it =
        std::find_if(m_conditions.begin(), m_conditions.end(),
                     ConditionHasNames(c.modelname(), c.portname()));

    Assert(utils::InternalError, it == m_conditions.end(),
           boost::format(
               "Already defined condition model '%1%' on port '%2%'\n") %
           c.modelname() % c.portname());

    m_conditions.push_back(c);
}

void Conditions::clear()
{
    m_conditions.clear();
}

void Conditions::delCondition(const std::string& modelname,
                              const std::string& portname)
{
    std::remove_if(m_conditions.begin(), m_conditions.end(),
                   ConditionHasNames(modelname, portname));
}

Condition& Conditions::find(const std::string& modelname,
                            const std::string& portname)
{
    ConditionList::iterator it =
        std::find_if(m_conditions.begin(), m_conditions.end(),
                     ConditionHasNames(modelname, portname));

    Assert(utils::InternalError, it != m_conditions.end(),
           boost::format(
               "Unknow condition model name '%1%' on port '%2%'\n") % modelname
           % portname);

    return *it;
}

const Condition& Conditions::find(const std::string& modelname,
                                  const std::string& portname) const
{
    ConditionList::const_iterator it =
        std::find_if(m_conditions.begin(), m_conditions.end(),
                     ConditionHasNames(modelname, portname));

    Assert(utils::InternalError, it != m_conditions.end(),
           boost::format(
               "Unknow condition model name '%1%' on port '%2%'\n") % modelname
           % portname);

    return *it;
}

}} // namespace vle vpz
