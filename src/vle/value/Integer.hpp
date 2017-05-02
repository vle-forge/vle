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

#ifndef VLE_VALUE_INTEGER_HPP
#define VLE_VALUE_INTEGER_HPP 1

#include <vle/DllDefines.hpp>
#include <vle/value/Value.hpp>

namespace vle {
namespace value {

/**
 * @brief Integer Value encapsulate a C++ 'int32_t' type into a class.
 */
class VLE_API Integer : public Value
{
public:
    /**
     * @brief Build an Integer object with a default value as zero.
     */
    Integer()
      : m_value(0)
    {
    }

    /**
     * @brief Build an Integer object with a specified value.
     * @param value The value to copy.
     */
    Integer(int32_t value)
      : m_value(value)
    {
    }

    /**
     * @brief Copy constructor.
     * @param value The value to copy.
     */
    Integer(const Integer& value)
      : Value(value)
      , m_value(value.m_value)
    {
    }

    /**
     * @brief Nothing to delete.
     */
    virtual ~Integer()
    {
    }

    ///
    ////
    ///

    /**
     * @brief Build a Integer.
     * @param value the value of the Integer.
     * @return A new Integer.
     */
    static std::unique_ptr<value::Value> create(int32_t value = 0)
    {
        return std::unique_ptr<value::Value>(new Integer(value));
    }

    ///
    ////
    ///

    /**
     * @brief Clone the current Integer with the same value.
     * @return A new Integer.
     */
    virtual std::unique_ptr<Value> clone() const override
    {
        return std::unique_ptr<Value>(new Integer(m_value));
    }

    /**
     * @brief Get the type of this class.
     * @return Return Value::INTEGER.
     */
    virtual Value::type getType() const override
    {
        return Value::INTEGER;
    }

    /**
     * @brief Push the long in the stream.
     * @param out The output stream.
     */
    virtual void writeFile(std::ostream& out) const override;

    /**
     * @brief Push the long in the stream.
     * @param out The output stream.
     */
    virtual void writeString(std::ostream& out) const override;

    /**
     * @brief Push the long in the stream. The string pushed in the stream:
     * @code
     * <integer>-12345</integer>
     * <integer><987654321/integer>
     * @endcode
     * @param out The output stream.
     */
    virtual void writeXml(std::ostream& out) const override;

    ///
    ////
    ///

    /**
     * @brief Get the value of the long.
     * @return An integer.
     */
    inline const int32_t& value() const
    {
        return m_value;
    }

    /**
     * @brief Get a reference to the encapsulated long.
     * @return A reference to the encapsulated long.
     */
    inline int32_t& value()
    {
        return m_value;
    }

    /**
     * @brief Assign a value to the encapsulated long.
     * @param value The value to set.
     */
    inline void set(int32_t value)
    {
        m_value = value;
    }

private:
    int32_t m_value;
};

inline const Integer&
toIntegerValue(std::shared_ptr<Value> value)
{
    return value::reference(value).toInteger();
}

inline const Integer&
toIntegerValue(std::shared_ptr<const Value> value)
{
    return value::reference(value).toInteger();
}

inline const Integer&
toIntegerValue(const std::unique_ptr<Value>& value)
{
    return value::reference(value).toInteger();
}

inline const Integer&
toIntegerValue(const Value& value)
{
    return value.toInteger();
}

inline Integer&
toIntegerValue(Value& value)
{
    return value.toInteger();
}

inline int32_t
toInteger(std::shared_ptr<Value> value)
{
    return value::reference(value).toInteger().value();
}

inline int32_t
toInteger(std::shared_ptr<const Value> value)
{
    return value::reference(value).toInteger().value();
}

inline int32_t
toInteger(const std::unique_ptr<Value>& value)
{
    return value::reference(value).toInteger().value();
}

inline int32_t
toInteger(const Value& value)
{
    return value.toInteger().value();
}

inline int32_t&
toInteger(Value& value)
{
    return value.toInteger().value();
}
}
} // namespace vle value

#endif
