/**
 * @file value/Double.cpp
 * @author The VLE Development Team.
 * @brief Double Value.
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

#include <vle/value/Double.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/XML.hpp>

namespace vle { namespace value {

Double::Double(xmlpp::Element* root)
{
    if (root->get_name() == "DOUBLE") {
        m_value = vle::utils::to_double(
            vle::utils::xml::get_attribute(root, "VALUE"));
    }
}

std::string Double::toFile() const
{
    return vle::utils::to_string(m_value);
}

std::string Double::toString() const
{
    return vle::utils::to_string(m_value);
}

std::string Double::toXML() const
{
    std::string val("<DOUBLE VALUE=\"");
    val += vle::utils::to_string(m_value);
    val += "\" />";
    return val;
}

}} // namespace vle value
