/** 
 * @file vpz/Replicas.cpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 31 jan 2006 17:27:02 +0100
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

#include <vle/vpz/Replicas.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Rand.hpp>

namespace vle { namespace vpz {

using namespace vle::utils;

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
