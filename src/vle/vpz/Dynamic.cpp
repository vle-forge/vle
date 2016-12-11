/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2016 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2016 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2016 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <vle/vpz/Dynamic.hpp>

namespace vle {
namespace vpz {

void Dynamic::write(std::ostream &out) const
{
    out << "<dynamic "
        << "name=\"" << m_name.c_str() << "\" "
        << "library=\"" << m_library.c_str() << "\" ";

    if (not m_package.empty()) {
        out << "package=\"" << m_package.c_str() << "\" ";
    }

    if (not m_language.empty()) {
        out << "language=\"" << m_language.c_str() << "\" ";
    }

    out << " />";
}

bool Dynamic::operator==(const Dynamic &dynamic) const
{
    return m_name == dynamic.name() and m_library == dynamic.library() and
           m_language == dynamic.language() and
           m_ispermanent == dynamic.isPermanent();
}
}
} // namespace vle vpz
