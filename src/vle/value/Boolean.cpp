/**
 * @file value/Boolean.cpp
 * @author The VLE Development Team.
 * @brief Boolean Value.
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

#include <vle/value/Boolean.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/XML.hpp>

namespace vle { namespace value {

Boolean::Boolean(xmlpp::Element* root)
{
    if (root->get_name() == "BOOLEAN") {
        m_value = vle::utils::xml::get_attribute(root, "VALUE") == "true";
    }
}

std::string Boolean::toFile() const
{
    if (m_value)
        return "true";
    else
        return "false";
}

std::string Boolean::toString() const
{
    if (m_value)
        return "true";
    else
        return "false";
}

std::string Boolean::toXML() const
{
    if (m_value)
	return "<BOOLEAN VALUE=\"true\" />";
    else
	return "<BOOLEAN VALUE=\"false\" />";
}

}} // namespace vle value
