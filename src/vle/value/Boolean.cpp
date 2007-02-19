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

namespace vle { namespace value {

Boolean BooleanFactory::create(bool value)
{
    return Boolean(new BooleanFactory(value));
}

Value BooleanFactory::clone() const
{
    return Value(new BooleanFactory(m_value));
}

std::string BooleanFactory::toFile() const
{
    if (m_value)
        return "true";
    else
        return "false";
}

std::string BooleanFactory::toString() const
{
    if (m_value)
        return "true";
    else
        return "false";
}

std::string BooleanFactory::toXML() const
{
    if (m_value)
	return "<boolean>true</boolean>";
    else
	return "<boolean>false</boolean>";
}

}} // namespace vle value
