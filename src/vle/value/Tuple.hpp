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


#ifndef VLE_VALUE_TUPLE_HPP
#define VLE_VALUE_TUPLE_HPP 1

#include <vle/value/Value.hpp>
#include <vle/DllDefines.hpp>
#include <vector>

namespace vle { namespace value {

/**
 * @brief Define a array of double.
 */
typedef std::vector < double > TupleValue;

/**
 * @brief A Tuple Value is a container to store a list of double value into
 * an std::vector standard container.
 */
class VLE_API Tuple : public Value
{
public:
    typedef TupleValue::size_type size_type;
    typedef TupleValue::iterator iterator;
    typedef TupleValue::const_iterator const_iterator;

    /**
     * @brief Build a Tuple object with a default value to empty TupleValue.
     */
    Tuple()
        : m_value()
    {}

    /**
     * @brief Build a Tuple object with a default size of `n' elements
     * initialized to `value'.
     * @param n The number of elements to initially create.
     * @param value The default value.
     */
    Tuple(const size_type& n, const double& value = 0.0)
        : m_value(n, value)
    {}

    /**
     * @brief Copy constructor.
     * @param value The value to copy.
     */
    Tuple(const Tuple& value)
        : Value(value), m_value(value.m_value)
    {}

    /**
     * @brief Nothing to delete.
     */
    virtual ~Tuple()
    {}

    ///
    ////
    ///

    /**
     * @brief Build a Tuple.
     * @return A new Tuple.
     */
    static Tuple* create()
    { return new Tuple(); }

    /**
     * @brief Build a Tuple object with a default size of `n' elements
     * initialized to `value'.
     * @param n The number of elements to initially create.
     * @param value The default value.
     * @return A new Tuple.
     */
    static Tuple* create(const size_type& n, const double& value = 0.0)
    { return new Tuple(n, value); }

    ///
    ////
    ///

    /**
     * @brief Clone the current Tuple with the same TupleValue datas.
     * @return A new Tuple.
     */
    virtual Value* clone() const
    { return new Tuple(*this); }

    /**
     * @brief Get the type of this class.
     * @return Value::TUPLE.
     */
    virtual Value::type getType() const
    { return Value::TUPLE; }

    /**
     * @brief Push all real from the TupleValue separated by space.
     * @param out The output stream.
     */
    virtual void writeFile(std::ostream& out) const;

    /**
     * @brief Push all real from the TupleValue separated by colon.
     * @param out The output stream.
     */
    virtual void writeString(std::ostream& out) const;

    /**
     * @brief Push all real from the TupleValue. The XML representation of this
     * class is:
     * @code
     * <tuple>0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0</tuple>
     * <tuple>-0.534e-5 1234.e34</tuple>
     * @endcode
     * @param out The output stream.
     */
    virtual void writeXml(std::ostream& out) const;

    ///
    ////
    ///

    /**
     * @brief Get a reference to the TupleValue.
     * @return A reference to the TupleValue.
     */
    inline TupleValue& value()
    { return m_value; }

    /**
     * @brief Get a constant reference to the TupleValue.
     * @return A constant reference to the TupleValue.
     */
    inline const TupleValue& value() const
    { return m_value; }

    /**
     * @brief Push a real at the end of the TupleValue.
     * @param value the value to push.
     */
    inline void add(const double& value)
    { m_value.push_back(value); }

    /**
     * @brief Check if the TupleValue is empty.
     * @return True if the TupleValue is empty, false otherwise.
     */
    inline bool empty() const
    { return m_value.empty(); }

    /**
     * @brief Return the number of element in the TupleValue.
     * @return An number
     */
    inline size_type size() const
    { return m_value.size(); }

    /**
     * @brief Get a constant reference to the real at the specified index. Be
     * careful, the index is not tested.
     * @param i The index of the value to get.
     * @return The real at the specified index.
     */
    inline const double& operator[](const size_type& i) const
    { return m_value[i]; }

    /**
     * @brief Get a reference to the real at the specified index. Be
     * careful, the index is not tested.
     * @param i The index of the value to get.
     * @return The real at the specified index.
     */
    inline double& operator[](const size_type& i)
    { return m_value[i]; }

    /**
     * @brief Get a constant reference to the real at the specified index.
     * @param i The index of the value to get.
     * @return The real at the specified index.
     * @throw std::out_of_range if the index is too big.
     */
    inline const double& at(const size_type& i) const
    { return m_value.at(i); }

    /**
     * @brief Get a reference to the real at the specified index.
     * @param i The index of the value to get.
     * @return The real at the specified index.
     * @throw std::out_of_range if the index is too big.
     */
    inline double& at(const size_type& i)
    { return m_value.at(i); }

    /**
     * @brief Fill the current tuple with multiple reals read from a string.
     * @param str A string with [0..n] reals.
     * @throw utils::ArgError if string have problem.
     */
    void fill(const std::string& str);

private:
    TupleValue              m_value;
};

inline const Tuple& toTupleValue(const Value& value)
{ return value.toTuple(); }

inline const Tuple* toTupleValue(const Value* value)
{ return value ? &value->toTuple() : 0; }

inline Tuple& toTupleValue(Value& value)
{ return value.toTuple(); }

inline Tuple* toTupleValue(Value* value)
{ return value ? &value->toTuple() : 0; }

inline const TupleValue& toTuple(const Value& value)
{ return value.toTuple().value(); }

inline TupleValue& toTuple(Value& value)
{ return value.toTuple().value(); }

inline const TupleValue& toTuple(const Value* value)
{ return value::reference(value).toTuple().value(); }

inline TupleValue& toTuple(Value* value)
{ return value::reference(value).toTuple().value(); }

}} // namespace vle value

#endif
