/**
 * @file vle/vpz/NewObservableTrame.cpp
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


#include <vle/vpz/NewObservableTrame.hpp>

namespace vle { namespace vpz {

void NewObservableTrame::write(std::ostream& out) const
{
    out << "<trame type=\"new"
        << "\" date=\"" << m_time.c_str()
        << "\" name=\"" << m_name.c_str()
        << "\" parent=\"" << m_parent.c_str()
        << "\" port=\"" << m_port.c_str()
        << "\" view=\"" << m_view.c_str() << "\" />";
}

}} // namespace vle vpz
