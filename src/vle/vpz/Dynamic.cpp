/**
 * @file vle/vpz/Dynamic.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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
        << "name=\"" << m_name.c_str() << "\" "
        << "library=\"" << m_library.c_str() << "\" "
        << "model=\"" << m_model.c_str() << "\" ";

    if (not m_package.empty()) {
        out << "package=\"" << m_package.c_str() << "\" ";
    }

    if (not m_language.empty()) {
        out << "language=\"" << m_language.c_str() << "\" ";
    }

    if (m_type == LOCAL) {
        out << "type=\"local\" ";
    } else {
        out << "type=\"distant\"  location=\"" << m_location.c_str() << "\"";
    }

    out << " />";
}

void Dynamic::setDistantDynamics(const std::string& host, int port)
{
    Assert < utils::ArgError >(port > 0 and port < 65535, fmt(_(
            "Error in TCP/IP port for Dynamics distant model %1% (%2%:%3%)"))
        % m_name % host % port);

    m_location = (fmt("%1%:%2%") % host % port).str();
    m_type = DISTANT;
}

void Dynamic::setLocalDynamics()
{
    m_location.clear();
    m_type = LOCAL;
}

bool Dynamic::operator==(const Dynamic& dynamic) const
{
    return m_name == dynamic.name() and m_library == dynamic.library()
	and m_model == dynamic.model() and m_language == dynamic.language()
	and m_location == dynamic.location() and m_type == dynamic.type()
	and m_ispermanent == dynamic.isPermanent();
}

}} // namespace vle vpz
