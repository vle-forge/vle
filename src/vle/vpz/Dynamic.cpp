/**
 * @file src/vle/vpz/Dynamic.cpp
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

#include <vle/vpz/Dynamic.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {

void Dynamic::write(std::ostream& out) const
{
    out << "<dynamic "
        << "name=\"" << m_name << "\" "
        << "library=\"" << m_library << "\" "
        << "model=\"" << m_model << "\" ";

    if (not m_language.empty()) {
        out << "language=\"" << m_language << "\" ";
    }

    if (m_type == LOCAL) {
        out << "type=\"local\" ";
    } else {
        out << "type=\"distant\"  location=\"" << m_location << "\"";
    }

    out << " />";
}

void Dynamic::setDistantDynamics(const std::string& host, int port)
{
    Assert(utils::ArgError, port > 0 and port < 65535, boost::format(
            "Error in TCP/IP port for Dynamics distant model %1% (%2%:%3%)") %
        m_name % host % port);

    m_location = (boost::format("%1%:%2%") % host % port).str();
    m_type = DISTANT;
}

void Dynamic::setLocalDynamics()
{
    m_location.clear();
    m_type = LOCAL;
}

}} // namespace vle vpz
