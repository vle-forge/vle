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
    if (not empty()) {
        out << "<conditions>\n";

        for (const_iterator it = begin(); it != end(); ++it) {
            out << it->second;
        }

        out << "</conditions>\n";
    }
}

void Conditions::add(const Conditions& cdts)
{
    for (const_iterator it = cdts.begin(); it != cdts.end(); ++it)
        add(it->second);
}

Condition& Conditions::add(const Condition& condition)
{
    const_iterator it = find(condition.name());
    Assert(utils::InternalError, it == end(),
           boost::format("The condition %1% already exist") %
           condition.name());

    return (*insert(
            std::make_pair < std::string, Condition >(
                condition.name(), condition)).first).second;
}

void Conditions::del(const std::string& condition)
{
    erase(condition);
}

const Condition& Conditions::get(const std::string& condition) const
{
    const_iterator it = find(condition);
    Assert(utils::InternalError, it != end(),
           boost::format("The condition %1% not exist") %
           condition);

    return it->second;
}

Condition& Conditions::get(const std::string& condition)
{
    iterator it = find(condition);
    Assert(utils::InternalError, it != end(),
           boost::format("The condition %1% not exist") %
           condition);

    return it->second;
}
    
void Conditions::clean_no_permanent()
{
    iterator prev = begin();
    iterator it = begin();
    
    while (it != end()) {
        if (not it->second.is_permanent()) {
            if (prev == it) {
                erase(it);
                prev = begin();
                it = begin();
            } else {
                erase(it);
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
    for (iterator it = begin(); it != end(); ++it) {
        it->second.rebuildValueSet();
    }
}

}} // namespace vle vpz
