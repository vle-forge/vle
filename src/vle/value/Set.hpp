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

#ifndef VLE_VALUE_SET_HPP
#define VLE_VALUE_SET_HPP 1

#include <vector>
#include <vle/DllDefines.hpp>
#include <vle/value/Value.hpp>

namespace vle {
namespace value {

/**
 * @brief Define a std::Vector of value.
 */
using VectorValue = std::vector<std::unique_ptr<Value>>;

/**
 * @brief The Set Value is a vector of pointer of value.
 */
class VLE_API Set : public Value
{
public:
    using size_type = VectorValue::size_type;
    using iterator = VectorValue::iterator;
    using const_iterator = VectorValue::const_iterator;
    using value_type = VectorValue::value_type;

    /**
     * @brief Build an empty Set.
     */
    Set() = default;

    /**
     * @brief Build a Set with size cells initialized with NULL pointer.
     */
    Set(size_type size);

    /**
     * @brief Copy constructor. All the Value are cloned.
     * @param value The value to copy.
     */
    Set(const Set& value);

    /**
     * @brief Delete all Value in the set.
     */
    virtual ~Set()
    {
    }

    /**
     * @brief Build a new Set.
     * @return A new Set.
     */
    static std::unique_ptr<Value> create(size_type size = 0)
    {
        return std::unique_ptr<Value>(new Set(size));
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Clone the Set and recursively cloned each cells.
     * @return A new allocated Set.
     */
    virtual std::unique_ptr<Value> clone() const override
    {
        return std::unique_ptr<Value>(new Set(*this));
    }

    /**
     * @brief Get the type of this class.
     * @return Value::SET.
     */
    virtual Value::type getType() const override;

    /**
     * @brief Push all Value from the Set, recursively and colon separated.
     * @code
     * 1,2,123.312,toto,321,1e10
     * @endcode
     * @param out The output stream.
     */
    virtual void writeFile(std::ostream& out) const override;

    /**
     * @brief Push all Value from the Set, recursively and colon separated with
     * parentheisis.
     * @code
     * (1,2,123.312,toto,321,1e10)
     * @endcode
     * @param out The output stream.
     */
    virtual void writeString(std::ostream& out) const override;

    /**
     * @brief Push all Value from the Set recursively in an XML representation:
     * @code
     * <set>
     *  <integer>1</integer>
     *  <integer>2</integer>
     *  <double>123.312</double>
     *  <string>toto</string>
     *  <integer>321</integer>
     *  <double>1e10</double>
     * </set>
     * @endcode
     * @param out The output stream.
     */
    virtual void writeXml(std::ostream& out) const override;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Get a reference to the VectorValue of the Set.
     * @return A reference to the VectorValue.
     */
    inline VectorValue& value()
    {
        return m_value;
    }

    /**
     * @brief Get a constant reference to the VectorValue of the Set.
     * @return A constant reference to the VectorValue.
     */
    inline const VectorValue& value() const
    {
        return m_value;
    }

    /**
     * @brief Get the first iterator from VectorValue.
     * @return the first iterator.
     */
    inline VectorValue::iterator begin()
    {
        return m_value.begin();
    }

    /**
     * @brief Get the first const_iterator from VectorValue.
     * @return the first iterator.
     */
    inline VectorValue::const_iterator begin() const
    {
        return m_value.begin();
    }

    /**
     * @brief Get the last iterator from VectorValue.
     * @return the last iterator.
     */
    inline VectorValue::iterator end()
    {
        return m_value.end();
    }

    /**
     * @brief Get the last const_iterator from VectorValue.
     * @return the last iterator.
     */
    inline VectorValue::const_iterator end() const
    {
        return m_value.end();
    }

    /**
     * @brief Assign a value to a cell of the Set. Be careful, the data is not
     * cloned,
     * Don't delete buffer after.
     * @param i The index of the value.
     * @param val The value to add.
     */
    void set(size_type i, std::unique_ptr<Value> val);

    /**
     * @brief Get a constant reference to the Value at specified index.
     * @param i The index of the value.
     * @return A constant reference.
     * @throw utils::ArgError if index 'i' is too big in debug mode.
     */
    const std::unique_ptr<Value>& get(size_type i) const;

    /**
     * @brief Get a constant reference to the Value at specified index.
     * @param i The index of the value.
     * @return A constant reference.
     * @throw utils::ArgError if index 'i' is too big.
     */
    const std::unique_ptr<Value>& operator[](size_type i) const;

    /**
     * @brief Get the pointer of the Value at specified index. The value at
     * the specified index was assign to NULL.
     * @param i The index of the value.
     * @return A reference.
     * @throw utils::ArgError if index 'i' is too big.
     */
    std::unique_ptr<Value> give(size_type i);

    /**
     * @brief Get the size of the VectorValue.
     * @return the size of the VectorValue.
     */
    inline size_type size() const
    {
        return m_value.size();
    }

    /**
     * @brief Return true if the value::Map does not contain any element.
     * @return True if empty, false otherwise.
     */
    inline bool empty() const
    {
        return m_value.empty();
    }

    /**
     * @brief Delete all value from the VectorValue and clean the
     * VectorValue.
     */
    void clear()
    {
        m_value.clear();
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Add a value into the set. Be careful, the data is not cloned,
     * Don't delete buffer after.
     * @param value the Value to add.
     * @throw std::invalid_argument if value is null.
     */
    void add(std::unique_ptr<Value> value);

    /**
     * @brief Add a null value into the set.
     */
    Null& addNull();

    /**
     * @brief Add a BooleanValue into the set.
     * @param value
     */
    Boolean& addBoolean(bool value);

    /**
     * @brief Get a bool from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not a Boolean.
     */
    bool getBoolean(size_type i) const;

    /**
     * @brief Get a bool from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not a Boolean.
     */
    bool& getBoolean(size_type i);

    /**
     * @brief Add a double into the set.
     * @param value
     */
    Double& addDouble(double value);

    /**
     * @brief Get a double from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not a Double.
     */
    double getDouble(size_type i) const;

    /**
     * @brief Get a double from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not a Double.
     */
    double& getDouble(size_type i);

    /**
     * @brief Add an IntegerValue into the set.
     * @param value
     */
    Integer& addInt(int32_t value);

    /**
     * @brief Get a int from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not a Integer.
     */
    int32_t getInt(size_type i) const;

    /**
     * @brief Get a int from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not a Integer.
     */
    int32_t& getInt(size_type i);

    /**
     * @brief Add a StringValue into the set.
     * @param value
     */
    String& addString(const std::string& value);

    /**
     * @brief Get a string from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not a String.
     */
    const std::string& getString(size_type i) const;

    /**
     * @brief Get a string from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not a String.
     */
    std::string& getString(size_type i);

    /**
     * @brief Add an XMLValue into the set.
     * @param value
     */
    Xml& addXml(const std::string& value);

    /**
     * @brief Get a string from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not an XML.
     */
    const std::string& getXml(size_type i) const;

    /**
     * @brief Get a string from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not an XML.
     */
    std::string& getXml(size_type i);

    /**
     * @brief Add an Table into the set.
     * @param value
     */
    Table& addTable(std::size_t width = 0, std::size_t height = 0);

    /**
     * @brief Get a string from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not an XML.
     */
    const Table& getTable(size_type i) const;

    /**
     * @brief Get a string from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not an XML.
     */
    Table& getTable(size_type i);

    /**
     * @brief Add an Tuple into the set.
     * @param value
     */
    Tuple& addTuple(std::size_t width = 0, double value = 0.0);

    /**
     * @brief Get a string from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not an XML.
     */
    const Tuple& getTuple(size_type i) const;

    /**
     * @brief Get a string from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not an XML.
     */
    Tuple& getTuple(size_type i);

    /**
     * @brief Add a Set at the end of the Set.
     * @return A reference to the newly allocated Set.
     */
    Set& addSet();

    /**
     * @brief Add a Map at the end of the Set.
     * @return A reference to the newly allocated Map.
     */
    Map& addMap();

    /**
     * @brief Add a Matrix at the end of the Set.
     * @return A reference to the newly allocated Set.
     */
    Matrix& addMatrix();

    /**
     * @brief Get a Set from the specified index.
     * @param i The index to get Value.
     * @return A Set.
     * @throw utils::ArgError if the index 'i' is to big (in debug mode) or if
     * the value a 'i' is null or is not a Set.
     */
    Set& getSet(size_type i);

    /**
     * @brief Get a Map from the specified index.
     * @param i The index to get Value.
     * @return A Map.
     * @throw utils::ArgError if the index 'i' is to big (in debug mode) or if
     * the value a 'i' is null or is not a Map.
     */
    Map& getMap(size_type i);

    /**
     * @brief Get a Matrix from the specified index.
     * @param i The index to get Value.
     * @return A Matrix.
     * @throw utils::ArgError if the index 'i' is to big (in debug mode) or if
     * the value a 'i' is null or is not an Matrix.
     */
    Matrix& getMatrix(size_type i);

    /**
     * @brief Get a constant Set from the specified index.
     * @param i The index to get Value.
     * @return A Set.
     * @throw utils::ArgError if the index 'i' is to big (in debug mode) or if
     * the value a 'i' is null or is not a Set.
     */
    const Set& getSet(size_type i) const;

    /**
     * @brief Get a constant Map from the specified index.
     * @param i The index to get Value.
     * @return A Map.
     * @throw utils::ArgError if the index 'i' is to big (in debug mode) or if
     * the value a 'i' is null or is not a Map.
     */
    const Map& getMap(size_type i) const;

    /**
     * @brief Get a constant Matrix from the specified index.
     * @param i The index to get Value.
     * @return A Matrix.
     * @throw utils::ArgError if the index 'i' is to big (in debug mode) or if
     * the value a 'i' is null or is not an Matrix.
     */
    const Matrix& getMatrix(size_type i) const;

    /**
     * @brief Delete the Value at the specified index. Be careful, all
     * reference and iterator are invalided after this call.
     * @brief Remove an elemet of the set
     * @param i index of value to remove
     */
    void remove(size_type i);

    /**
     * @brief Resize the set
     * @param newSize, the new size
     * @param fill, the default value for filling new elements
     */
    void resize(size_type newSize, const Value& fill);

private:
    VectorValue m_value;
};

inline const Set&
toSetValue(std::shared_ptr<Value> value)
{
    return value::reference(value).toSet();
}

inline const Set&
toSetValue(std::shared_ptr<const Value> value)
{
    return value::reference(value).toSet();
}

inline const Set&
toSetValue(const std::unique_ptr<Value>& value)
{
    return value::reference(value).toSet();
}

inline const Set&
toSetValue(const Value& value)
{
    return value.toSet();
}

inline Set&
toSetValue(Value& value)
{
    return value.toSet();
}

inline const VectorValue&
toSet(std::shared_ptr<Value> value)
{
    return value::reference(value).toSet().value();
}

inline const VectorValue&
toSet(std::shared_ptr<const Value> value)
{
    return value::reference(value).toSet().value();
}

inline const VectorValue&
toSet(const std::unique_ptr<Value>& value)
{
    return value::reference(value).toSet().value();
}

inline const VectorValue&
toSet(const Value& value)
{
    return value.toSet().value();
}

inline VectorValue&
toSet(Value& value)
{
    return value.toSet().value();
}
}
} // namespace vle value

#endif
