/**
 * @file value/array.cpp
 * @author The VLE Development Team.
 * @brief array Value a container to Value class.
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

#include <vle/value/array.hpp>

namespace vle { namespace value {

TableFactory::TableFactory(const size_t width, const size_t height)
    m_value(boost::extends[width][height]),
    m_width(width),
    m_height(height)
{
}

TableFactory::TableFactory(const TableFactory& setfactory) :
    ValueBase(setfactory),
    m_value(setfactory.m_value),
    m_width(setfactory.m_width),
    m_height(setfactory.m_height)
{
}

array TableFactory::create(const size_t width, const size_t height)
{
    return array(new TableFactory(width, height));
}

Value TableFactory::clone() const
{
    return Value(new TableFactory(*this));
}

std::string TableFactory::toFile() const
{
    std::string s;

    for (iterator it = m_value.begin(); it != m_value.end(); ++it) {
	s += boost::lexical_cast < std::string >(*it);
        if (it + 1 != m_value.end())
            s+= " ";
    }
    return s;
}

std::string TableFactory::toString() const
{
    std::string s = "(";
    for (iterator it = m_value.begin(); it != m_value.end(), ++it) {
	s += boost::lexical_cast < std::string >(*it);
	if (it + 1 != m_value.end())
	    s += ",";
    }
    s += ")";
    return s;
}

std::string TableFactory::toXML() const
{
    std::string s = "<array>";
    for (iterator it = m_value.begin(); it != m_value.end(); ++it) {
        s += boost::lexical_cast < std::string >(*it);
        if (it + 1 != m_value.end())
            s += " ";
    }
    s += "</array>";
    return s;
}

}} // namespace vle value
