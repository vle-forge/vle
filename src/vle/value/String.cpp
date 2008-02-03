/**
 * @file vle/value/String.cpp
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


#include <vle/value/String.hpp>
#include <vle/utils/Debug.hpp>



namespace vle { namespace value {

String StringFactory::create(const std::string& value)
{
    return String(new StringFactory(value));
}

Value StringFactory::clone() const
{
    return Value(new StringFactory(m_value));
}

std::string StringFactory::toXML() const
{
    std::string val("<string>");
    val += m_value;
    val += "</string>";
    return val;
}

String toStringValue(const Value& value)
{
    Assert(utils::InternalError, value->getType() == ValueBase::STRING,
           "Value is not a String");
    return boost::static_pointer_cast < StringFactory >(value);
}

const std::string& toString(const Value& value)
{
    Assert(utils::InternalError, value->getType() == ValueBase::STRING,
           "Value is not a String");
    return boost::static_pointer_cast < StringFactory >(value)->stringValue();
}

}} // namespace vle value
