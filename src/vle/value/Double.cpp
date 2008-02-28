/**
 * @file src/vle/value/Double.cpp
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




#include <vle/value/Double.hpp>
#include <vle/utils/Debug.hpp>



namespace vle { namespace value {

Double DoubleFactory::create(double value)
{
    return Double(new DoubleFactory(value));
}

Value DoubleFactory::clone() const
{
    return Value(new DoubleFactory(m_value));
}

std::string DoubleFactory::toFile() const
{
    return boost::lexical_cast < std::string >(m_value);
}

std::string DoubleFactory::toString() const
{
    return boost::lexical_cast < std::string >(m_value);
}

std::string DoubleFactory::toXML() const
{
    std::string val("<double>");
    val += boost::lexical_cast < std::string >(m_value);
    val += "</double>";
    return val;
}

Double toDoubleValue(const Value& value)
{
    Assert(utils::ArgError, value->getType() == ValueBase::DOUBLE,
           "Value is not a Double");
    return boost::static_pointer_cast < DoubleFactory >(value);
}

double toDouble(const Value& value)
{
    Assert(utils::ArgError, value->getType() == ValueBase::DOUBLE,
           "Value is not a Double");
    return boost::static_pointer_cast < DoubleFactory >(value)->doubleValue();
}

}} // namespace vle value
