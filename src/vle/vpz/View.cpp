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
        out << " />\n";
    } else {
        out << ">"
            << "<![CDATA[\n"
            << m_data
            << "]]>\n"
            << "</view>\n";
    }
}

void View::setEventView(const std::string& output,
                        const std::string& library)
{
    set_type(EVENT);
    set_output(output);
    set_library(library);
}

void View::setTimedView(double timestep,
                        const std::string& output,
                        const std::string& library)
{
    set_type(TIMED);
    set_timestep(timestep);
    set_output(output);
    set_library(library);
}

void View::set_timestep(double time)
{
    Assert(utils::SaxParserError, time > 0.0,
           (boost::format("Bad time step %1% for view %2%")
            % time % m_name));

    m_timestep = time;
}

}} // namespace vle vpz
