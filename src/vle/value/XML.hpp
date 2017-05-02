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

#ifndef VLE_VALUE_XML_HPP
#define VLE_VALUE_XML_HPP 1

#include <vle/DllDefines.hpp>
#include <vle/value/Value.hpp>

namespace vle {
namespace value {

/**
 * @brief Xml Value encapsulates a C++ std::string type into a class.
 */
class VLE_API Xml : public Value
{
public:
    /**
     * @brief Build a Xml object with a default value as empty string.
     */
    Xml()
      : m_value()
    {
    }

    /**
     * @brief Build an Xml object with a specified value.
     * @param value The value to copy.
     */
    Xml(std::string value)
      : m_value(std::move(value))
    {
    }

    /**
     * @brief Copy constructor.
     * @param value The value to copy.
     */
    Xml(const Xml& value)
      : Value(value)
      , m_value(value.m_value)
    {
    }

    /**
     * @brief Nothing to delete.
     */
    virtual ~Xml()
    {
    }

    ///
    ////
    ///

    /**
     * @brief Build a Xml.
     * @param value the default value of the Xml.
     * @return A new allocated Xml.
     */
    static std::unique_ptr<value::Value> create(
      const std::string& value = std::string())
    {
        return std::unique_ptr<value::Value>(new Xml(value));
    }

    ///
    ////
    ///

    /**
     * @brief Clone the current Xml with the same value.
     * @return A new Xml.
     */
    virtual std::unique_ptr<Value> clone() const override
    {
        return std::unique_ptr<Value>(new Xml(m_value));
    }

    /**
     * @brief Get the type of this class.
     * @return Return Value::XMLTYPE.
     */
    virtual Value::type getType() const override
    {
        return Value::XMLTYPE;
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
     * @brief Push the std::string into the stream. The string pushed in
     * the stream can contains XML characters like '&', '<', etc. The string
     * is encapsulated into a CDATA XML special tag.
     * @code
     * <xml>
     * <![CDATA[the string, the string, etc.
     * ]]>
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

inline const Xml&
toXmlValue(std::shared_ptr<Value> value)
{
    return value::reference(value).toXml();
}

inline const Xml&
toXmlValue(std::shared_ptr<const Value> value)
{
    return value::reference(value).toXml();
}

inline const Xml&
toXmlValue(const std::unique_ptr<Value>& value)
{
    return value::reference(value).toXml();
}

inline const Xml&
toXmlValue(const Value& value)
{
    return value.toXml();
}

inline Xml&
toXmlValue(Value& value)
{
    return value.toXml();
}

inline const std::string&
toXml(std::shared_ptr<Value> value)
{
    return value::reference(value).toXml().value();
}

inline const std::string&
toXml(std::shared_ptr<const Value> value)
{
    return value::reference(value).toXml().value();
}

inline const std::string&
toXml(const std::unique_ptr<Value>& value)
{
    return value::reference(value).toXml().value();
}

inline const std::string&
toXml(const Value& value)
{
    return value.toXml().value();
}

inline std::string&
toXml(Value& value)
{
    return value.toXml().value();
}
}
} // namespace vle value

#endif
