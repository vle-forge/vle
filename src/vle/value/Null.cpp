/** 
 * @file Null.cpp
 * @author The vle Development Team
 */

/*
 * Copyright (C) 2008 - The vle Development Team
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
    Assert(utils::InternalError, value->getType() == ValueBase::NIL,
           "Value is not a Null");
    return boost::static_pointer_cast < NullFactory >(value);
}

}} // namespace vle value

