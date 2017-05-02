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

#ifndef VLE_VALUE_STRING_HPP
#define VLE_VALUE_STRING_HPP 1

#include <vle/DllDefines.hpp>
#include <vle/value/Value.hpp>

namespace vle {
namespace value {

/**
 * @brief String Value encapsulates a C++ std::string type into a class.
 */
class VLE_API String : public Value
{
public:
    /**
     * @brief Build a String object with a default value as empty string.
     */
    String()
      : m_value()
    {
    }

    /**
     * @brief Build a String object with a specified value.
     * @param value The value to copy.
     */
    String(std::string value)
      : m_value(std::move(value))
    {
    }

    /**
     * @brief Copy constructor.
     * @param value The value to copy.
     */
    String(const String& value)
      : Value(value)
      , m_value(value.m_value)
    {
    }

    /**
     * @brief Nothing to delete.
     */
    virtual ~String()
    {
    }

    ///
    ////
    ///

    /**
     * @brief Build a String.
     * @param value the value of the string.
     * @return A new String.
     */
    static std::unique_ptr<value::Value> create(
      const std::string value = std::string())
    {
        return std::unique_ptr<value::Value>(new String(value));
    }

    ///
    ////
    ///

    /**
     * @brief Clone the current String with the same value.
     * @return A new String.
     */
    virtual std::unique_ptr<Value> clone() const override
    {
        return std::unique_ptr<Value>(new String(m_value));
    }

    /**
     * @brief Get the type of this class.
     * @return Return Value::STRING.
     */
    virtual Value::type getType() const override
    {
        return Value::STRING;
    }

    /**
     * @brief Push the std::string into the stream.
     * @param out The output stream.
     */
    virtual void writeFile(std::ostream& out) const override;

    /**
     * @brief Push the std::string into the stream.
     * @param out The output stream.
     */
    virtual void writeString(std::ostream& out) const override;

    /**
     * @brief Push the std::string into the stream. Be careful, the
     * std::string does not contains XML characters like '&', '<', '>'
     * etc. The std::string XML representation is:
     * @code
     * <xml>
     * the string, the string, etc.
     * </xml>
     * @endcode
     * @param out
     */
    virtual void writeXml(std::ostream& out) const override;

    /**
     * @brief Get a constant reference to the encapsulated std::string.
     * @return A constant reference to the encapsulated std::string.
     */
    inline const std::string& value() const
    {
        return m_value;
    }

    /**
     * @brief Get a reference to the encapsulated std::string.
     * @return A reference to the encapsulated std::string.
     */
    inline std::string& value()
    {
        return m_value;
    }

    /**
     * @brief Assign a value to the encapsulated std::string.
     * @param value The Value to set.
     */
    inline void set(const std::string& value)
    {
        m_value.assign(value);
    }

private:
    std::string m_value;
};

inline const String&
toStringValue(std::shared_ptr<Value> value)
{
    return value::reference(value).toString();
}

inline const String&
toStringValue(std::shared_ptr<const Value> value)
{
    return value::reference(value).toString();
}

inline const String&
toStringValue(const std::unique_ptr<Value>& value)
{
    return value::reference(value).toString();
}

inline const String&
toStringValue(const Value& value)
{
    return value.toString();
}

inline String&
toStringValue(Value& value)
{
    return value.toString();
}

inline const std::string&
toString(std::shared_ptr<Value> value)
{
    return value::reference(value).toString().value();
}

inline const std::string&
toString(std::shared_ptr<const Value> value)
{
    return value::reference(value).toString().value();
}

inline const std::string&
toString(const std::unique_ptr<Value>& value)
{
    return value::reference(value).toString().value();
}

inline const std::string&
toString(const Value& value)
{
    return value.toString().value();
}

inline std::string&
toString(Value& value)
{
    return value.toString().value();
}
}
} // namespace vle value

#endif
