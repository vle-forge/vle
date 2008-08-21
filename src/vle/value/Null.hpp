/**
 * @file vle/value/Null.hpp
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


#ifndef VLE_VALUE_NULL_HPP
#define VLE_VALUE_NULL_HPP

#include <vle/value/Value.hpp>

namespace vle { namespace value {

    /** 
     * @brief A null Value. This class is use to build empty value into
     * container. Can be usefull on Map, Set etc.
     */
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

    struct IsNullValue
    {
        bool operator()(const value::Value& value) const
        { return value.get() and value->getType() == ValueBase::NIL; }
    };
    
    Null toNullValue(const Value& value);

}} // namespace vle value

#endif
