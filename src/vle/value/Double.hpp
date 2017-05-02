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

#ifndef VLE_VALUE_DOUBLE_HPP
#define VLE_VALUE_DOUBLE_HPP 1

#include <vle/DllDefines.hpp>
#include <vle/value/Value.hpp>

namespace vle {
namespace value {

/**
 * @brief Double Value encapsulate a C++ 'double' type into a class.
 */
class VLE_API Double : public Value
{
public:
    /**
     * @brief Build a Double object with a default value as 0.0.
     */
    Double()
      : m_value(0.0)
    {
    }

    /**
     * @brief Build a Double object with a specifiied value.
     * @param value The value to copy.
     */
    Double(double value)
      : m_value(value)
    {
    }

    /**
     * @brief Copy constuctor.
     * @param value The value to copy.
     */
    Double(const Double& value)
      : Value(value)
      , m_value(value.m_value)
    {
    }

    /**
     * @brief Nothing to delete.
     */
    virtual ~Double()
    {
    }

    ///
    ////
    ///

    /**
     * @brief Build a Double.
     * @param value the value of the Double.
     * @return A new Double.
     */
    static std::unique_ptr<value::Value> create(double value = 0.0)
    {
        return std::unique_ptr<value::Value>(new Double(value));
    }

    ///
    ////
    ///

    /**
     * @brief Clone the current Double with the same value.
     * @return A new Double.
     */
    virtual std::unique_ptr<Value> clone() const override
    {
        return std::unique_ptr<Value>(new Double(m_value));
    }

    /**
     * @brief Get the type of this class.
     * @return Return Value::DOUBLE.
     */
    inline virtual Value::type getType() const override
    {
        return Value::DOUBLE;
    }

    /**
     * @brief Push the double in the stream. Use the stream operator
     * setprecision, fill etc. to manage the representation of this double.
     * @param out The output stream.
     */
    virtual void writeFile(std::ostream& out) const override;

    /**
     * @brief Push the double in the stream. Use the stream operator
     * setprecision, fill etc. to manage the representation of this double.
     * @param out The output stream.
     */
    virtual void writeString(std::ostream& out) const override;

    /**
     * @brief Push the double in the stream. The string pushed in the stream:
     * @code
     * <double>12.134</double>
     * <double>-0.7854e-123</double>
     * @endcode
     * @param out The output stream.
     */
    virtual void writeXml(std::ostream& out) const override;

    ///
    ////
    ///

    /**
     * @brief Get the value of the double.
     * @return A double.
     */
    inline const double& value() const
    {
        return m_value;
    }

    /**
     * @brief Get a reference to the encapsulated double.
     * @return A reference to the encapsulated double.
     */
    inline double& value()
    {
        return m_value;
    }

    /**
     * @brief Assign a value to the encapsulated double.
     * @param value The value to set.
     */
    inline void set(double value)
    {
        m_value = value;
    }

private:
    double m_value;
};

inline const Double&
toDoubleValue(std::shared_ptr<Value> value)
{
    return value::reference(value).toDouble();
}

inline const Double&
toDoubleValue(std::shared_ptr<const Value> value)
{
    return value::reference(value).toDouble();
}

inline const Double&
toDoubleValue(const std::unique_ptr<Value>& value)
{
    return value::reference(value).toDouble();
}

inline const Double&
toDoubleValue(const Value& value)
{
    return value.toDouble();
}

inline Double&
toDoubleValue(Value& value)
{
    return value.toDouble();
}

inline double
toDouble(std::shared_ptr<Value> value)
{
    return value::reference(value).toDouble().value();
}

inline double
toDouble(std::shared_ptr<const Value> value)
{
    return value::reference(value).toDouble().value();
}

inline double
toDouble(const std::unique_ptr<Value>& value)
{
    return value::reference(value).toDouble().value();
}

inline double
toDouble(const Value& value)
{
    return value.toDouble().value();
}

inline double&
toDouble(Value& value)
{
    return value.toDouble().value();
}
}
} // namespace vle value

#endif
