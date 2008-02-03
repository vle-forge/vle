/**
 * @file vle/value/Integer.cpp
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


#include <vle/value/Integer.hpp>
#include <vle/utils/Debug.hpp>



namespace vle { namespace value {

Integer IntegerFactory::create(long value)
{
    return Integer(new IntegerFactory(value));
}

Value IntegerFactory::clone() const
{
    return Value(new IntegerFactory(m_value));
}

std::string IntegerFactory::toFile() const
{
    return boost::lexical_cast < std::string >(m_value);
}

std::string IntegerFactory::toString() const
{
    return boost::lexical_cast < std::string >(m_value); 
}

std::string IntegerFactory::toXML() const
{
    std::string val("<integer>");
    val += boost::lexical_cast < std::string >(m_value); 
    val += "</integer>";
    return val;
}

Integer toIntegerValue(const Value& value)
{
    Assert(utils::InternalError, value->getType() == ValueBase::INTEGER,
           "Value is not an Integer");
    return boost::static_pointer_cast < IntegerFactory >(value);
}

long toLong(const Value& value)
{
    Assert(utils::InternalError, value->getType() == ValueBase::INTEGER,
           "Value is not an Integer");
    return boost::static_pointer_cast < IntegerFactory >(value)->longValue();
}

int toInteger(const Value& value)
{
    Assert(utils::InternalError, value->getType() == ValueBase::INTEGER,
           "Value is not an Integer");
    return boost::static_pointer_cast < IntegerFactory >(value)->intValue();
}


}} // namespace vle value
