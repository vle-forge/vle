/** 
 * @file vpz/Classes.cpp
 * @brief 
 * @author The vle Development Team
 * @date lun, 27 fév 2006 17:27:56 +0100
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

#include <vle/vpz/Classes.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {
    
void Classes::write(std::ostream& out) const
{
    if (not empty()) {
        out << "<classes>\n";
        for (const_iterator it = begin(); it != end(); ++it) {
            out << "<class name=\""
                << (*it).first
                << "\" >"
                << (*it).second
                << "</class>";
        }
        out << "</classes>\n";
    }
}

Class& Classes::add(const Class& c)
{
    const_iterator it = find(c.name());
    Assert(utils::SaxParserError, it != end(),
           (boost::format("Class %1% already exist") % c.name()));

    return (*insert(std::make_pair < std::string, Class >(
                c.name(), c)).first).second;
}

void Classes::del(const std::string& name)
{
    erase(name);
}

const Class& Classes::get(const std::string& name) const
{
    const_iterator it = find(name);
    Assert(utils::SaxParserError, it != end(),
           (boost::format("Unknow class '%1%'") % name));

    return it->second;
}

Class& Classes::get(const std::string& name)
{
    iterator it = find(name);
    Assert(utils::SaxParserError, it != end(),
           (boost::format("Unknow class '%1%'") % name));

    return it->second;
}

}} // namespace vle vpz
