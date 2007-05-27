/** 
 * @file vpz/Dynamic.cpp
 * @brief 
 * @author The vle Development Team
 * @date lun, 06 fév 2006 10:34:54 +0100
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

#include <vle/vpz/Dynamic.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/XML.hpp>

namespace vle { namespace vpz {
    
using namespace vle::utils;

void Dynamic::write(std::ostream& out) const
{
    out << "<dynamic "
        << "name=\"" << m_name << "\" "
        << "library=\"" << m_library << "\" "
        << "model=\"" << m_model << "\" "
        << "language=\"" << m_language << "\" ";

    if (m_type == LOCAL)
        out << " type=\"local\"";
    else
        out << " type=\"distant\" location=\"" << m_location << "\"";

    if (m_data.empty())
        out << " />";
    else
        out << ">"
            << m_data
            << "</dynamic>";
}

void Dynamic::setDistantDynamics(const std::string& host, int port)
{
    AssertI(port > 0);
    AssertI(port < 65535);

    m_location = (boost::format("%1%:%2%") % host % port).str();
    m_type = DISTANT;
}

void Dynamic::setLocalDynamics()
{
    m_location.clear();
    m_type = LOCAL;
}

}} // namespace vle vpz
