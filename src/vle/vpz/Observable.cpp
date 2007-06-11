/** 
 * @file vpz/Observable.cpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 31 jan 2006 17:30:53 +0100
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

#include <vle/vpz/Observable.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {

using namespace vle::utils;

Observable::Observable()
{
}

Observable::~Observable()
{
}

void Observable::write(std::ostream& out) const
{
    out << "<observable name=\"" << m_name << "\" ";

    if (not m_group.empty()) {
        out << "group=\"" << m_group << "\" "
            << "index=\"" << m_index << "\" ";
    }

    out << "/ >";
} 

void Observable::setObservable(const std::string& name,
                               const std::string& group,
                               int index)
{
    AssertI(not name.empty());

    m_name.assign(name);
    m_group.assign(group);
    m_index = index;
}

}} // namespace vle vpz
