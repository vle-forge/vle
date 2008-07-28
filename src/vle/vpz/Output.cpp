/**
 * @file src/vle/vpz/Output.cpp
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

#include <vle/vpz/Output.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {

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
        out << ">\n<![CDATA[\n" << m_data << "\n]]>\n</output>\n";
    } else {
        out << "/>\n";
    }
}

void Output::setLocalStream(const std::string& location,
                            const std::string& plugin)
{
    Assert(utils::ArgError, not plugin.empty(), boost::format(
            "Output '%1%' have not plugin defined") % m_name);

    m_location.assign(location);
    m_plugin.assign(plugin);
    m_format = Output::LOCAL;
}

void Output::setDistantStream(const std::string& location,
                              const std::string& plugin)
{
    Assert(utils::ArgError, not plugin.empty(), boost::format(
            "Output '%1%' have not plugin defined") % m_name);

    m_location.assign(location);
    m_plugin.assign(plugin);
    m_format = Output::DISTANT;
}

void Output::setData(const std::string& data)
{
    m_data.assign(data);
}

}} // namespace vle vpz
