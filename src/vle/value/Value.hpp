/**
 * @file src/vle/value/Value.hpp
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




#ifndef UTILS_VALUE_VALUE_HPP
#define UTILS_VALUE_VALUE_HPP

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <libxml++/libxml++.h>

namespace vle { namespace value {

        class ValueBase;
        class BooleanFactory;
        class IntegerFactory;
        class DoubleFactory;
        class StringFactory;
        class SetFactory;
        class MapFactory;
        class TupleFactory;
        class TableFactory;
        class XMLFactory;
        class NullFactory;

        typedef boost::shared_ptr < ValueBase > Value;
        typedef boost::shared_ptr < BooleanFactory > Boolean;
        typedef boost::shared_ptr < IntegerFactory > Integer;
        typedef boost::shared_ptr < DoubleFactory > Double;
        typedef boost::shared_ptr < StringFactory > String;
        typedef boost::shared_ptr < SetFactory > Set;
        typedef boost::shared_ptr < MapFactory > Map;
        typedef boost::shared_ptr < TupleFactory > Tuple;
        typedef boost::shared_ptr < TableFactory > Table;
        typedef boost::shared_ptr < XMLFactory > XML;
        typedef boost::shared_ptr < NullFactory > Null;
        

        /**
         * @brief Virtual class to assign Value into Event object.
         */
        class ValueBase
        {
        public:
            enum type { BOOLEAN, INTEGER, DOUBLE, STRING, SET, MAP,
                TUPLE, TABLE, XMLTYPE, NIL };

            /**
             * Defaut constructor. To use Values class, you must use the static
             * function create from all subclass.
             *
             * @code
             * Value val = value::IntegerFactory::create(13);
             * @endcode
             */
            ValueBase() { }

            virtual ~ValueBase() { }

            static const Value empty;

            /**
             * Pure virtual clone function.
             *
             * @return Clone of instantiate object.
             */
            virtual Value clone() const =0;

            //
            //
            // Pure virtual function
            //
            // 

            /**
             * Transform value into a simple std::string for text file.
             *
             * @return std::string representation of Value.
             */
            virtual std::string toFile() const =0;

            /**
             * Transform value into a simple std::string.
             *
             * @return std::string representation of Value.
             */
            virtual std::string toString() const =0;

            /**
             * Transform value into XML structure.
             *
             * @return std::string representation of XML structure of Value.
             */
            virtual std::string toXML() const =0;

            //
            //
            // Function to check type at runtime.
            //
            //

            /**
             * Return the type of value. The type is one of the 'type'
             * enumeration ie. BOOL, INTEGER, DOUBLE, STRING, SET, MAP.
             *
             * @return the type of value objet.
             */
            virtual ValueBase::type getType() const =0;

            inline bool isInteger() const
            { return getType() == ValueBase::INTEGER; }

            inline bool isBoolean() const
            { return getType() == ValueBase::BOOLEAN; }

            inline bool isDouble() const
            { return getType() == ValueBase::DOUBLE; }

            inline bool isString() const
            { return getType() == ValueBase::STRING; }

            inline bool isSet() const
            { return getType() == ValueBase::SET; }

            inline bool isMap() const
            { return getType() == ValueBase::MAP; }

            inline bool isTuple() const
            { return getType() == ValueBase::TUPLE; }

            inline bool isTable() const
            { return getType() == ValueBase::TABLE; }

            inline bool isXML() const
            { return getType() == ValueBase::XMLTYPE; }

            inline bool isNull() const
            { return getType() == ValueBase::NIL; }

        };

        /** 
         * @brief A functor to help to the clone of value to use with
         * std::transform for instance
         * @code
         * std::transform(vec.begin(), vec.end(), out.begin(), CloneValue());
         * @endcode
         */
        struct CloneValue
        {
            Value operator()(const Value& val) const 
            { return val->clone(); }
        };

    } // namespace value

} // namespace vle
#endif
