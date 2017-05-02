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

#ifndef VLE_VALUE_NULL_HPP
#define VLE_VALUE_NULL_HPP 1

#include <vle/DllDefines.hpp>
#include <vle/value/Value.hpp>

namespace vle {
namespace value {

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
    {
    }

    /**
     * @brief Copy constructor.
     * @param value The value to copy.
     */
    Null(const Null& value)
      : Value(value)
    {
    }

    /**
     * @brief Nothing to delete.
     */
    virtual ~Null()
    {
    }

    ///
    ////
    ///

    /**
     * @brief Build a new Null value.
     * @return A new allocated Null.
     */
    static std::unique_ptr<value::Value> create()
    {
        return std::unique_ptr<value::Value>(new Null());
    }

    ///
    ////
    ///

    /**
     * @brief Clone the current Null.
     * @return A new Null.
     */
    virtual std::unique_ptr<Value> clone() const override
    {
        return std::unique_ptr<Value>(new Null());
    }

    /**
     * @brief Get the type of this class.
     * @return Return Value::NIL.
     */
    inline virtual Value::type getType() const override
    {
        return Value::NIL;
    }

    /**
     * @brief Push the string 'NA' into the stream.
     * @param out The output stream.
     */
    virtual void writeFile(std::ostream& out) const override;

    /**
     * @brief Push the string 'NA' into the stream.
     * @param out The output stream.
     */
    virtual void writeString(std::ostream& out) const override;

    /**
     * @brief Push a specific string into the stream. The XML representation of
     * the Null value is:
     * @code
     * <null />
     * @endcode
     * @param out The output stream.
     */
    virtual void writeXml(std::ostream& out) const override;
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
    {
        return value.getType() == Value::NIL;
    }

    /**
     * @brief Check if the pointer value is 0 or a Null value.
     * @param value The pointer to check.
     * @return True if the value is 0 or a Null value, false otherwise.
     */
    bool operator()(const std::unique_ptr<value::Value>& value) const
    {
        return value and value->getType() == Value::NIL;
    }
};

inline const Null&
toNullValue(std::shared_ptr<Value> value)
{
    return value::reference(value).toNull();
}

inline const Null&
toNullValue(std::shared_ptr<const Value> value)
{
    return value::reference(value).toNull();
}

inline const Null&
toNullValue(const std::unique_ptr<Value>& value)
{
    return value::reference(value).toNull();
}

inline const Null&
toNullValue(const Value& value)
{
    return value.toNull();
}

inline Null&
toNullValue(Value& value)
{
    return value.toNull();
}
}
} // namespace vle value

#endif
