/**
 * @file value/String.cpp
 * @author The VLE Development Team.
 * @brief A string Value.
 */

/*
 * Copyright (c) 2004, 2005 The vle Development Team
 *
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <vle/value/String.hpp>
#include <vle/utils/Tools.hpp>

namespace vle { namespace value {

String::String(xmlpp::Element* root)
{
    if (root->get_name() == "STRING") {
        m_value = vle::utils::xml::get_attribute(root, "VALUE");
    }
}

std::string String::toXML() const
{
    std::string val("<STRING VALUE=\"");
    val += m_value;
    val += "\" />";
    return val;
}

}} // namespace vle value
