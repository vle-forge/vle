/** 
 * @file vpz/View.cpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 31 jan 2006 17:31:58 +0100
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

#include <vle/vpz/View.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {

using namespace vle::utils;

View::View()
{
}

void View::write(std::ostream& out) const
{
    out << "<view "
        << "name=\"" << m_name << "\" "
        << "output=\"" << m_output << "\" "
        << "library=\"" << m_library << "\" ";

    switch (m_type) {
    case View::EVENT:
        out << "type=\"event\"";
        break;

    case View::TIMED:
        out << "type=\"timed\" "
            << "time_step=\"" << m_timestep << "\"";
        break;
    }

    if (m_data.empty()) {
        out << " />";
    } else {
        out << ">"
            << "<![CDATA[\n"
            << m_data
            << "]]>\n"
            << "</view>";
    }
}

void View::setEventView(Type type,
                        const std::string& output,
                        const std::string& library
                        const std::string& data)
{
    set_type(type);
    set_output(output);
    set_library(library);
    set_data(data);
}

void View::setTimedView(Type type,
                        double timestep,
                        const std::string& output,
                        const std::string& library,
                        const std::string& data)
{
    set_type(type);
    set_output(output);
    set_library(library);
    set_data(data);
}

}} // namespace vle vpz
