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
    out << "<dynamic formalism=\"" << m_formalism << "\" type=\""
        << (m_type == Dynamic::WRAPPING ? "wrapping" : "mapping");

    if (m_dynamic.empty()) {
        out << "\" />";
    } else {
        out << "\" >" << m_dynamic << "</dynamic>";
    }
}

void Dynamic::setWrappingDynamic(const std::string& formalism,
                                 const std::string& dynamic)
{
    AssertI(not formalism.empty());

    m_dynamic.assign(dynamic);
    m_formalism.assign(formalism);
    m_type = Dynamic::WRAPPING;
}

void Dynamic::setMappingDynamic(const std::string& formalism,
                                const std::string& dynamic)
{
    AssertI(not formalism.empty());

    m_dynamic.assign(dynamic);
    m_formalism.assign(formalism);
    m_type = Dynamic::MAPPING;
}

}} // namespace vle vpz
