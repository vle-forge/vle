/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VLE_VALUE_VALUE_HPP
#define VLE_VALUE_VALUE_HPP 1

#include <memory>
#include <ostream>
#include <string>
#include <vle/DllDefines.hpp>

namespace vle {
namespace value {

class Value;
class Boolean;
class Integer;
class Double;
class String;
class Set;
class Map;
class Tuple;
class Table;
class Xml;
class Null;
class Matrix;
class User;

/**
 * @brief Virtual class to assign Value into Event object.
 */
class VLE_API Value
{
public:
    enum type
    {
        BOOLEAN,
        INTEGER,
        DOUBLE,
        STRING,
        SET,
        MAP,
        TUPLE,
        TABLE,
        XMLTYPE,
        NIL,
        MATRIX,
        USER
    };

    /**
     * @brief Default constructor.
     * @code
     * Value* val = value::Integer::create(13);
     * Value* val2 = new value::Integer(13);
     * @endcode
     */
    Value() = default;

    /**
     * @brief Copy constructor. In subclass, all datas are cloned.
     * @param value The value to clone;
     */
    Value(const Value& value) = default;

    /**
     * @brief Nothing to delete.
     */
    virtual ~Value()
    {
    }

    ///
    //// Abstract functions
    ///

    /**
     * @brief Pure virtual clone function.
     * @return Clone of instantiate object.
     */
    virtual std::unique_ptr<Value> clone() const = 0;

    /**
     * @brief Transform value into a simple std::string for text file.
     * @return std::string representation of Value.
     */
    virtual void writeFile(std::ostream& /* out */) const = 0;

    /**
     * @brief Transform value into a simple std::string.
     * @return std::string representation of Value.
     */
    virtual void writeString(std::ostream& /* out */) const = 0;

    /**
     * @brief Transform value into XML structure.
     * @return std::string representation of XML structure of Value.
     */
    virtual void writeXml(std::ostream& /* out */) const = 0;

    /**
     * @brief Return the type of value. The type is one of the 'type'
     * enumeration ie. BOOL, INTEGER, DOUBLE, STRING, SET, MAP.
     * @return the type of value object.
     */
    virtual Value::type getType() const = 0;

    ///
    ////
    ///

    /**
     * @brief Build a file representation of this class. This function
     * call the virtual function Value::writeString to build the string.
     * @return A string representation of this class.
     */
    std::string writeToFile() const;

    /**
     * @brief Build a string representation of this class. This function
     * call the virtual function Value::writeString to build the string.
     * @return A string representation of this class.
     */
    std::string writeToString() const;

    /**
     * @brief Build an xml representation of this class. This function
     * call the virtual function Value::writeString to build the string.
     * @return A string representation of this class.
     */
    std::string writeToXml() const;

    inline bool isInteger() const
    {
        return getType() == Value::INTEGER;
    }

    inline bool isBoolean() const
    {
        return getType() == Value::BOOLEAN;
    }

    inline bool isDouble() const
    {
        return getType() == Value::DOUBLE;
    }

    inline bool isString() const
    {
        return getType() == Value::STRING;
    }

    inline bool isSet() const
    {
        return getType() == Value::SET;
    }

    inline bool isMap() const
    {
        return getType() == Value::MAP;
    }

    inline bool isTuple() const
    {
        return getType() == Value::TUPLE;
    }

    inline bool isTable() const
    {
        return getType() == Value::TABLE;
    }

    inline bool isXml() const
    {
        return getType() == Value::XMLTYPE;
    }

    inline bool isNull() const
    {
        return getType() == Value::NIL;
    }

    inline bool isMatrix() const
    {
        return getType() == Value::MATRIX;
    }

    inline bool isUser() const
    {
        return getType() == Value::USER;
    }

    const Boolean& toBoolean() const;
    const Integer& toInteger() const;
    const Double& toDouble() const;
    const String& toString() const;
    const Set& toSet() const;
    const Map& toMap() const;
    const Tuple& toTuple() const;
    const Table& toTable() const;
    const Xml& toXml() const;
    const Null& toNull() const;
    const Matrix& toMatrix() const;
    const User& toUser() const;

    Boolean& toBoolean();
    Integer& toInteger();
    Double& toDouble();
    String& toString();
    Set& toSet();
    Map& toMap();
    Tuple& toTuple();
    Table& toTable();
    Xml& toXml();
    Null& toNull();
    Matrix& toMatrix();
    User& toUser();

    /**
     * @brief Stream operator for the value classes. This operator call the
     * value::Value::writeString function used, principally to debug or show
     * the values in console, gui widgets etc.
     * @param out the output stream where data are written.
     * @param obj the sub class of value::Value to write.
     * @return the std::stream.
     */
    friend std::ostream& operator<<(std::ostream& out, const Value& obj)
    {
        obj.writeString(out);
        return out;
    }
};

/**
 * @brief Clone value data.
 */
VLE_API std::shared_ptr<Value> clone(std::shared_ptr<Value> v);

/**
 * @brief Clone value data.
 */
VLE_API std::shared_ptr<Value> clone(std::shared_ptr<const Value> v);

/**
 * @brief Check if the Value is a composite value, ie., a Map, a Set or
 * a Matrix.
 * @param val The Value to check.
 * @return True if the Value is a Map, a Set or a Matrix.
 */
VLE_API bool is_composite(const std::unique_ptr<Value>& val);

/**
 * @brief Convert a constant value::Value @c std::unique_ptr to a constant
 * value::Value reference.
 * @param value The @c std::unique_ptr to convert.
 * @return A reference.
 * @throw utils::ArgError if value is NULL.
 */
VLE_API const Value& reference(const std::unique_ptr<Value>& value);

/**
 * @brief Convert a constant value::Value @c std::shared_ptr to a constant
 * value::Value reference.
 * @param value The @std::shared_ptr to convert.
 * @return A reference.
 * @throw utils::ArgError if value is NULL.
 */
VLE_API const Value& reference(std::shared_ptr<const Value> value);

/**
 * @brief Convert a constant value::Value @c std::shared_ptr to a constant
 * value::Value reference.
 * @param value The @c std::shared_ptr to convert.
 * @return A reference.
 * @throw utils::ArgError if value is NULL.
 */
VLE_API const Value& reference(std::shared_ptr<Value> value);
}
}

#endif
