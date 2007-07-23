/** 
 * @file vpz/Output.cpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 31 jan 2006 17:29:47 +0100
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

#include <vle/vpz/Output.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {

using namespace vle::utils;

Output::Output()
{
    setLocalStream("", "");
}

void Output::write(std::ostream& out) const
{
    out << "<output name=\"" << m_name << "\" " << "location=\"" << m_location
        << "\" ";
    
    switch (m_format) {
    case Output::LOCAL:
        out << "format=\"local\"";
        break;
    case Output::DISTANT:
        out << "format=\"distant\"";
        break;
    }

    out << " plugin=\"" << m_plugin << "\" ";

    if (not m_data.empty()) {
        out << ">" << m_data << "</output>\n";
    } else {
        out << "/>\n";
    }
}

void Output::setLocalStream(const std::string& location,
                            const std::string& plugin)
{
    AssertI(not plugin.empty());
    m_location.assign(location);
    m_plugin.assign(plugin);
    m_format = Output::LOCAL;
}

void Output::setDistantStream(const std::string& location,
                              const std::string& plugin)
{
    AssertI(not plugin.empty());
    m_location.assign(location);
    m_plugin.assign(plugin);
    m_format = Output::DISTANT;
}

void Output::setData(const std::string& data)
{
    m_data.assign(data);
}

}} // namespace vle vpz
