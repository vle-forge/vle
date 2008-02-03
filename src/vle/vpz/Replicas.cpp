/**
 * @file vle/vpz/Replicas.cpp
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


#include <vle/vpz/Replicas.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Rand.hpp>

namespace vle { namespace vpz {

Replicas::Replicas() :
    m_number(1),
    m_seed(0)
{ }

void Replicas::write(std::ostream& out) const
{
    out << "<replicas"
        << " seed=\"" << m_seed << "\""
        << " number=\"" << m_number << "\""
        << " />\n";
}

void Replicas::buildList()
{
    utils::Rand::rand().set_seed(m_seed);
    m_list.resize(m_number);

    for (std::vector < guint32 >::iterator it = m_list.begin(); 
         it != m_list.end(); ++it) {
        *it = utils::Rand::rand().get_int();
    }
}


void Replicas::clear()
{
    m_list.clear();
}

}} // namespace vle vpz
