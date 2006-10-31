/**
 * @file Value.hpp
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
#include <vle/utils/XML.hpp>

namespace vle { namespace value {

    /**
     * @brief Virtual class to assign Value into Event object.
     */
    class Value
    {
    public:
        enum type { BOOLEAN, INTEGER, DOUBLE, STRING, SET, MAP, POINTER,
            SPEED, COORDINATE, DIRECTION };

        /**
         * Defaut constructor.  All value class must develop a constructor with
         * xmlpp::Element value to parse the current node set the value. If the
         * parsing failed, initial value are used, no exception are throw.
         *
         * @code
         * Value* i = new Integer(xmlroot);
         * @endcode
         */
        Value() { }

        virtual ~Value() { }

        /**
         * Pure virtual clone function.
         *
         * @return Clone of instantiate object.
         */
        virtual Value* clone() const =0;

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
         * Return the type of value. The type is one of the 'type' enumeration ie.
         * BOOL, INTEGER, DOUBLE, STRING, SET, MAP.
         *
         * @return the type of value objet.
         */
        virtual Value::type getType() const =0;

        inline bool isInteger() const
        { return getType() == Value::INTEGER; }

        inline bool isBoolean() const
        { return getType() == Value::BOOLEAN; }

        inline bool isDouble() const
        { return getType() == Value::DOUBLE; }

        inline bool isString() const
        { return getType() == Value::STRING; }

        inline bool isSet() const
        { return getType() == Value::SET; }

        inline bool isMap() const
        { return getType() == Value::MAP; }

        inline bool isPointer() const
        { return getType() == Value::POINTER; }

        inline bool isSpeed() const
        { return getType() == Value::SPEED; }

        inline bool isCoordinate() const
        { return getType() == Value::COORDINATE; }

        inline bool isDirection() const
        { return getType() == Value::DIRECTION; }

        //
        //
        // Statical Function to help develpment 
        //
        //

        /**
         * Build values in parsing root XML node. This function is recursive with
         * the Set value. For example,
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
        static std::vector < Value* > getValues(xmlpp::Element* root);

        /**
         * Build a value in parsing root XML node. This function is recursive with
         * the Set value. For example,
         *
         * @code
         * <DOUBLE VALUE="1.0" />
         * @endcode
         *
         * @param root the node element to parse.
         * @return 0 if error, a new Value on success.
         */
        static Value* getValue(xmlpp::Element* root);

        /**
         * Delete all the value contains in Vector.
         *
         * @param vals vector values to delete.
         */
        static void cleanValues(std::vector < Value* >& vals);

        /**
         * @return true if the node is a complex value like Set, false if it simple
         * value like String, Boolean, Integer, Double and if an error occured.
         */
        static bool isComplex(xmlpp::Element* root);
    };

}} // namespace vle value
#endif
