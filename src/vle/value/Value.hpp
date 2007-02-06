/**
 * @file value/Value.hpp
 * @author The VLE Development Team.
 * @brief Virtual class to assign Value into Event object.
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

#ifndef UTILS_VALUE_VALUE_HPP
#define UTILS_VALUE_VALUE_HPP

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <libxml++/libxml++.h>
#include <vle/utils/Debug.hpp>

namespace vle {
    
    
    /** 
     * @brief The value transported by event. All class are manager using a
     * reference counter from Boost.org: boost::shared_ptr < type >.
     */
    namespace value {

        class ValueBase;
        class BooleanFactory;
        class IntegerFactory;
        class DoubleFactory;
        class StringFactory;
        class SetFactory;
        class MapFactory;

        template < size_t n, class Class > class CoordinateFactory ;
        class DirectionFactory;
        class SpeedFactory;


        typedef boost::shared_ptr < ValueBase > Value;
        typedef boost::shared_ptr < BooleanFactory > Boolean;
        typedef boost::shared_ptr < IntegerFactory > Integer;
        typedef boost::shared_ptr < DoubleFactory > Double;
        typedef boost::shared_ptr < StringFactory > String;
        typedef boost::shared_ptr < SetFactory > Set;
        typedef boost::shared_ptr < MapFactory > Map;
        
        typedef boost::shared_ptr < CoordinateFactory < 1, int > >
            CoordinateInt1;
        typedef boost::shared_ptr < CoordinateFactory < 1, double > >
            CoordinateDouble1;
        typedef boost::shared_ptr < CoordinateFactory < 2, int > >
            CoordinateInt2;
        typedef boost::shared_ptr < CoordinateFactory < 2, double > >
            CoordinateDouble2;
        typedef boost::shared_ptr < CoordinateFactory < 3, int > >
            CoordinateInt3;
        typedef boost::shared_ptr < CoordinateFactory < 3, double > >
            CoordinateDouble3;

        typedef boost::shared_ptr < DirectionFactory > Direction;
        typedef boost::shared_ptr < SpeedFactory > Speed;

        /**
         * @brief Virtual class to assign Value into Event object.
         */
        class ValueBase
        {
        public:
            enum type { BOOLEAN, INTEGER, DOUBLE, STRING, SET, MAP, POINTER,
                SPEED, COORDINATE, DIRECTION };

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

            inline bool isPointer() const
            { return getType() == ValueBase::POINTER; }

            inline bool isSpeed() const
            { return getType() == ValueBase::SPEED; }

            inline bool isCoordinate() const
            { return getType() == ValueBase::COORDINATE; }

            inline bool isDirection() const
            { return getType() == ValueBase::DIRECTION; }

            //
            //
            // Statical Function to help develpment 
            //
            //

            /**
             * Build values in parsing root XML node. This function is recursive
             * with the Set value. For example,
             *
             * @code
             * <MY_TAG>
             *  <DOUBLE VALUE="1.0" />
             *  <DOUBLE VALUE="4.0" />
             * </MY_TAG>
             * @endcode
             *
             * @param root the parent node of elements to parse.
             * @return 0 if error, a vector of new Value on success.
             */
            static std::vector < Value > getValues(xmlpp::Element* root);

            /**
             * Build a value in parsing root XML node. This function is
             * recursive with the Set value. For example,
             *
             * @code
             * <DOUBLE VALUE="1.0" />
             * @endcode
             *
             * @param root the node element to parse.
             * @return 0 if error, a new Value on success.
             */
            static Value getValue(xmlpp::Element* root);

            /**
             * Delete all the value contains in Vector.
             *
             * @param vals vector values to delete.
             */
            static void cleanValues(std::vector < Value >& vals);

            /**
             * @return true if the node is a complex value like Set, false if it
             * simple value like String, Boolean, Integer, Double and if an
             * error occured.
             */
            static bool isComplex(xmlpp::Element* root);
        };

        Boolean to_boolean(Value v);
        Integer to_integer(Value v);
        String to_string(Value v);
        Double to_double(Value v);
        Map to_map(Value v);
        Set to_set(Value v);
        CoordinateInt1 to_coordinateInt1(Value v);
        CoordinateInt2 to_coordinateInt2(Value v);
        CoordinateInt3 to_coordinateInt3(Value v);
        CoordinateDouble1 to_coordinateDouble1(Value v);
        CoordinateDouble2 to_coordinateDouble2(Value v);
        CoordinateDouble3 to_coordinateDouble3(Value v);

    } // namespace value

} // namespace vle
#endif
