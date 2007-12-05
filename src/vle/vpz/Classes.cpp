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
    if (not m_lst.empty()) {
        out << "<classes>\n";

        std::transform(begin(), end(), 
                       std::ostream_iterator < Class >(out),
                       ClassValue());

        out << "</classes>\n";
    }
}

Class& Classes::add(const std::string& name)
{
    Assert(utils::SaxParserError, not exist(name),
           (boost::format("Class %1% already exist") % name));

    return (*m_lst.insert(std::make_pair < std::string, Class >(
                name, Class(name))).first).second;
}

void Classes::del(const std::string& name)
{
    m_lst.erase(name);
}

const Class& Classes::get(const std::string& name) const
{
    const_iterator it = m_lst.find(name);
    Assert(utils::SaxParserError, it != end(),
           (boost::format("Unknow class '%1%'") % name));

    return it->second;
}

Class& Classes::get(const std::string& name)
{
    iterator it = m_lst.find(name);
    Assert(utils::SaxParserError, it != end(),
           (boost::format("Unknow class '%1%'") % name));

    return it->second;
}

}} // namespace vle vpz

