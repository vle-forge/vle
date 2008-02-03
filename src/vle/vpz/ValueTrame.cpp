/**
 * @file vle/vpz/ValueTrame.cpp
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


#include <vle/vpz/ValueTrame.hpp>
#include <vle/utils/Debug.hpp>



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
