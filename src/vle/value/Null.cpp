/**
 * @file src/vle/value/Null.cpp
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




#include <vle/value/Null.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace value {

Null NullFactory::create()
{
    return Null(new NullFactory());
}

Value NullFactory::clone() const
{
    return Value(new NullFactory());
}

std::string NullFactory::toFile() const
{
    return "NA";
}

std::string NullFactory::toString() const
{
    return "NA";
}

std::string NullFactory::toXML() const
{
    return "<null />";
}

Null toNullValue(const Value& value)
{
    Assert(utils::ArgError, value->getType() == ValueBase::NIL,
           "Value is not a Null");
    return boost::static_pointer_cast < NullFactory >(value);
}

}} // namespace vle value

