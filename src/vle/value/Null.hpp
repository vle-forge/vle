/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
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


#ifndef VLE_VALUE_NULL_HPP
#define VLE_VALUE_NULL_HPP 1

#include <vle/value/Value.hpp>
#include <vle/DllDefines.hpp>

namespace vle { namespace value {

/**
 * @brief A null Value. This class is uses to build empty value into
 * container. Can be useful on Map, Set etc.
 */
class VLE_API Null : public Value
{
public:
    /**
     * @brief Build a Null object.
     */
    Null()
    {}

    /**
     * @brief Copy constructor.
     * @param value The value to copy.
     */
    Null(const Null& value)
        : Value(value)
    {}

    /**
     * @brief Nothing to delete.
     */
    virtual ~Null()
    {}

    ///
    ////
    ///

    /**
     * @brief Build a new Null value.
     * @return A new allocated Null.
     */
    static Null* create()
    { return new Null(); }

    ///
    ////
    ///

    /**
     * @brief Clone the current Null.
     * @return A new Null.
     */
    virtual Value* clone() const
    { return new Null(); }

    /**
     * @brief Get the type of this class.
     * @return Return Value::NIL.
     */
    inline virtual Value::type getType() const
    { return Value::NIL; }

    /**
     * @brief Push the string 'NA' into the stream.
     * @param out The output stream.
     */
    virtual void writeFile(std::ostream& out) const;

    /**
     * @brief Push the string 'NA' into the stream.
     * @param out The output stream.
     */
    virtual void writeString(std::ostream& out) const;

    /**
     * @brief Push a specific string into the stream. The XML representation of
     * the Null value is:
     * @code
     * <null />
     * @endcode
     * @param out The output stream.
     */
    virtual void writeXml(std::ostream& out) const;
};

/**
 * @brief A functor useful to test if a value is NULL or Null. To use with
 * the std::find_if for instance.
 * @code
 * std::vector < value::Value* > vals;
 * std::vector < value::Value* >::iterator it;
 * it = std::find_if(v.begin(), v.end(), value::IsNullValue());
 * @endcode
 */
struct VLE_API IsNullValue
{
    /**
     * @brief Check if the value is a Null value.
     * @param value The value to check.
     * @return Ture if the value is a Null, false otherwise.
     */
    bool operator()(const value::Value& value) const
    { return value.getType() == Value::NIL; }

    /**
     * @brief Check if the pointer value is 0 or a Null value.
     * @param value The pointer to check.
     * @return True if the value is 0 or a Null value, false otherwise.
     */
    bool operator()(const value::Value* value) const
    { return value and value->getType() == Value::NIL; }
};

inline const Null& toNullValue(const Value& value)
{ return value.toNull(); }

inline const Null* toNullValue(const Value* value)
{ return value ? &value->toNull() : 0; }

inline Null& toNullValue(Value& value)
{ return value.toNull(); }

inline Null* toNullValue(Value* value)
{ return value ? &value->toNull() : 0; }

}} // namespace vle value

#endif
