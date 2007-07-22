/** 
 * @file ValueTrame.cpp
 * @brief 
 * @author The vle Development Team
 * @date 2007-07-15
 */

/*
 * Copyright (C) 2007 - The vle Development Team
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

#include <vle/vpz/ValueTrame.hpp>



namespace vle { namespace vpz {

void ValueTrame::add(const std::string& simulator,
                     const std::string& parent,
                     const std::string& port,
                     const std::string& view)
{
    ModelTrame mdl(simulator, parent, port, view);
    m_list.push_back(mdl);
}

void ValueTrame::add(const value::Value& value)
{
    Assert(utils::SaxParserError, not m_list.empty(),
           "ValueTrame have not model trame.");
    m_list.back().setValue(value);
}

void ValueTrame::write(std::ostream& out) const
{
    out << "<trame type=\"value\" date=\"" << m_time << "\" >";

    for (ModelTrameList::const_iterator it = m_list.begin();
         it != m_list.end(); ++it) {
        out << "<modeltrame"
            << " name=\"" << it->simulator() << "\" "
            << " parent=\"" << it->parent() << "\" "
            << " port=\"" << it->port() << "\" "
            << " view=\"" << it->view() << "\" >";

        out << it->value()->toXML();

        out << "</modeltrame>";
    }
    
    out << "</trame>";
}

}} // namespace vle vpz
