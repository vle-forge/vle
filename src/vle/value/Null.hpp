/** 
 * @file Null.hpp
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

#ifndef VLE_VALUE_NULL_HPP
#define VLE_VALUE_NULL_HPP

#include <vle/value/Value.hpp>

namespace vle { namespace value {

    class NullFactory : public ValueBase
    {
    private:
        NullFactory()
        { }

    public:
        virtual ~NullFactory()
        { }

        static Null create();

        virtual Value clone() const;

        inline virtual ValueBase::type getType() const
        { return ValueBase::NIL; }

        virtual std::string toFile() const;

        virtual std::string toString() const;

        virtual std::string toXML() const;
    };
    
    Null toNullValue(const Value& value);

}} // namespace vle value

#endif
