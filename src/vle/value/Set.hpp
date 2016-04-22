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


#ifndef VLE_VALUE_SET_HPP
#define VLE_VALUE_SET_HPP 1

#include <vle/value/Value.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/XML.hpp>
#include <vle/DllDefines.hpp>
#include <vector>

namespace vle { namespace value {

/**
 * @brief Define a std::Vector of value.
 */
typedef std::vector <std::unique_ptr<Value>> VectorValue;

/**
 * @brief The Set Value is a vector of pointer of value.
 */
class VLE_API Set : public Value
{
public:
    typedef VectorValue::size_type size_type;
    typedef VectorValue::iterator iterator;
    typedef VectorValue::const_iterator const_iterator;
    typedef VectorValue::value_type value_type;

    /**
     * @brief Build an empty Set.
     */
    Set()
        : m_value()
    {}

    /**
     * @brief Build a Set with size cells initialized with NULL pointer.
     */
    Set(const size_type& size);

    /**
     * @brief Copy constructor. All the Value are cloned.
     * @param value The value to copy.
     */
    Set(const Set& value);

    /**
     * @brief Delete all Value in the set.
     */
    virtual ~Set() {}

    /**
     * @brief Build a new Set.
     * @return A new Set.
     */
    static std::unique_ptr<Value> create(const size_type& size = 0)
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
    { return std::unique_ptr<Value>(new Set(*this)); }

    /**
     * @brief Get the type of this class.
     * @return Value::SET.
     */
    virtual Value::type getType() const override
    { return Value::SET; }

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
    { return m_value; }

    /**
     * @brief Get a constant reference to the VectorValue of the Set.
     * @return A constant reference to the VectorValue.
     */
    inline const VectorValue& value() const
    { return m_value; }

    /**
     * @brief Get the first iterator from VectorValue.
     * @return the first iterator.
     */
    inline VectorValue::iterator begin()
    { return m_value.begin(); }

    /**
     * @brief Get the first const_iterator from VectorValue.
     * @return the first iterator.
     */
    inline VectorValue::const_iterator begin() const
    { return m_value.begin(); }

    /**
     * @brief Get the last iterator from VectorValue.
     * @return the last iterator.
     */
    inline VectorValue::iterator end()
    { return m_value.end(); }

    /**
     * @brief Get the last const_iterator from VectorValue.
     * @return the last iterator.
     */
    inline VectorValue::const_iterator end() const
    { return m_value.end(); }

    /**
     * @brief Assign a value to a cell of the Set. Be careful, the data is not
     * cloned,
     * Don't delete buffer after.
     * @param i The index of the value.
     * @param val The value to add.
     */
    void set(const size_type& i, std::unique_ptr<Value> val)
    {
        pp_check_index(i);
        m_value[i] = std::move(val);
    }

    /**
     * @brief Get a constant reference to the Value at specified index.
     * @param i The index of the value.
     * @return A constant reference.
     * @throw utils::ArgError if index 'i' is too big in debug mode.
     */
    const std::unique_ptr<Value>& get(const size_type& i) const
    {
        pp_check_index(i);
        return m_value[i];
    }

    /**
     * @brief Get a constant reference to the Value at specified index.
     * @param i The index of the value.
     * @return A constant reference.
     * @throw utils::ArgError if index 'i' is too big.
     */
    const std::unique_ptr<Value>& operator[](const size_type& i) const
    {
        pp_check_index(i);
        return m_value[i];
    }

    /**
     * @brief Get the pointer of the Value at specified index. The value at
     * the specified index was assign to NULL.
     * @param i The index of the value.
     * @return A reference.
     * @throw utils::ArgError if index 'i' is too big.
     */
    std::unique_ptr<Value> give(const size_type& i)
    {
        auto it = pp_get(i);

        return std::move(*it);
    }

    /**
     * @brief Get the size of the VectorValue.
     * @return the size of the VectorValue.
     */
    inline size_type size() const
    { return m_value.size(); }

    /**
     * @brief Return true if the value::Map does not contain any element.
     * @return True if empty, false otherwise.
     */
    inline bool empty() const
    { return m_value.empty(); }

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
    void add(std::unique_ptr<Value> value)
    {
        m_value.push_back(std::move(value));
    }

    /**
     * @brief Add a null value into the set.
     */
    Null& addNull()
    {
        return pp_add<Null>();
    }

    /**
     * @brief Add a BooleanValue into the set.
     * @param value
     */
    Boolean& addBoolean(bool value)
    {
        return pp_add<Boolean>(value);
    }

    /**
     * @brief Get a bool from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not a Boolean.
     */
    bool getBoolean(const size_type& i) const
    {
        return pp_get_value(i).toBoolean().value();
    }

    /**
     * @brief Get a bool from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not a Boolean.
     */
    bool& getBoolean(const size_type& i)
    {
        return pp_get_value(i).toBoolean().value();
    }

    /**
     * @brief Add a double into the set.
     * @param value
     */
    Double& addDouble(const double& value)
    {
        return pp_add<Double>(value);
    }

    /**
     * @brief Get a double from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not a Double.
     */
    double getDouble(const size_type& i) const
    {
        return pp_get_value(i).toDouble().value();
    }

    /**
     * @brief Get a double from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not a Double.
     */
    double& getDouble(const size_type& i)
    {
        return pp_get_value(i).toDouble().value();
    }

    /**
     * @brief Add an IntegerValue into the set.
     * @param value
     */
    Integer& addInt(const int& value)
    {
        return pp_add<Integer>(value);
    }

    /**
     * @brief Get a int from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not a Integer.
     */
    int32_t getInt(const size_type& i) const
    {
        return pp_get_value(i).toInteger().value();
    }

    /**
     * @brief Get a int from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not a Integer.
     */
    int32_t& getInt(const size_type& i)
    {
        return pp_get_value(i).toInteger().value();
    }

    /**
     * @brief Add a StringValue into the set.
     * @param value
     */
    String& addString(const std::string& value)
    {
        return pp_add<String>(value);
    }

    /**
     * @brief Get a string from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not a String.
     */
    const std::string& getString(const size_type& i) const
    {
        return pp_get_value(i).toString().value();
    }

    /**
     * @brief Get a string from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not a String.
     */
    std::string& getString(const size_type& i)
    {
        return pp_get_value(i).toString().value();
    }

    /**
     * @brief Add an XMLValue into the set.
     * @param value
     */
    Xml& addXml(const std::string& value)
    {
        return pp_add<Xml>(value);
    }

    /**
     * @brief Get a string from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not an XML.
     */
    const std::string& getXml(const size_type& i) const
    {
        return pp_get_value(i).toXml().value();
    }

    /**
     * @brief Get a string from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not an XML.
     */
    std::string& getXml(const size_type& i)
    {
        return pp_get_value(i).toXml().value();
    }

    /**
     * @brief Add an Table into the set.
     * @param value
     */
    Table& addTable(Table::size_type width = 0,
                    Table::size_type height = 0)
    {
        return pp_add<Table>(width, height);
    }

    /**
     * @brief Get a string from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not an XML.
     */
    const Table& getTable(const size_type& i) const
    {
        return pp_get_value(i).toTable();
    }

    /**
     * @brief Get a string from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not an XML.
     */
    Table& getTable(const size_type& i)
    {
        return pp_get_value(i).toTable();
    }

    /**
     * @brief Add an Tuple into the set.
     * @param value
     */
    Tuple& addTuple(Tuple::size_type width = 0, double value = 0.0)
    {
        return pp_add<Tuple>(width, value);
    }

    /**
     * @brief Get a string from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not an XML.
     */
    const Tuple& getTuple(const size_type& i) const
    {
        return pp_get_value(i).toTuple();
    }

    /**
     * @brief Get a string from the specified index.
     * @param i The index to get value.
     * @return A value
     * @throw utils::ArgError if the index 'i' is to big or if value at
     * index 'i' is not an XML.
     */
    Tuple& getTuple(const size_type& i)
    {
        return pp_get_value(i).toTuple();
    }

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
    Set& getSet(const size_type& i);

    /**
     * @brief Get a Map from the specified index.
     * @param i The index to get Value.
     * @return A Map.
     * @throw utils::ArgError if the index 'i' is to big (in debug mode) or if
     * the value a 'i' is null or is not a Map.
     */
    Map& getMap(const size_type& i);

    /**
     * @brief Get a Matrix from the specified index.
     * @param i The index to get Value.
     * @return A Matrix.
     * @throw utils::ArgError if the index 'i' is to big (in debug mode) or if
     * the value a 'i' is null or is not an Matrix.
     */
    Matrix& getMatrix(const size_type& i);

    /**
     * @brief Get a constant Set from the specified index.
     * @param i The index to get Value.
     * @return A Set.
     * @throw utils::ArgError if the index 'i' is to big (in debug mode) or if
     * the value a 'i' is null or is not a Set.
     */
    const Set& getSet(const size_type& i) const;

    /**
     * @brief Get a constant Map from the specified index.
     * @param i The index to get Value.
     * @return A Map.
     * @throw utils::ArgError if the index 'i' is to big (in debug mode) or if
     * the value a 'i' is null or is not a Map.
     */
    const Map& getMap(const size_type& i) const;

    /**
     * @brief Get a constant Matrix from the specified index.
     * @param i The index to get Value.
     * @return A Matrix.
     * @throw utils::ArgError if the index 'i' is to big (in debug mode) or if
     * the value a 'i' is null or is not an Matrix.
     */
    const Matrix& getMatrix(const size_type& i) const;

    /**
     * @brief Delete the Value at the specified index. Be careful, all
     * reference and iterator are invalided after this call.
     * @brief Remove an elemet of the set
     * @param i index of value to remove
     */
    void remove(const size_type& i)
    {
        pp_check_index(i);
        m_value.erase(begin() + i);
    }

    /**
     * @brief Resize the set
     * @param newSize, the new size
     * @param fill, the default value for filling new elements
     */
    void resize(size_type newSize, const Value& fill);

private:
    VectorValue m_value;

    inline void pp_check_index(size_type i) const
    {
#ifndef NDEBUG
        if (i >= size())
            throw utils::ArgError(
                fmt(_("Set: too big index '%1%'")) % i);
#else
        (void)i;
#endif
    }

    inline iterator pp_get(size_type i)
    {
        pp_check_index(i);
        return begin() + i;
    }

    inline const_iterator pp_get(size_type i) const
    {
        pp_check_index(i);
        return begin() + i;
    }

    inline Value& pp_get_value(size_type i)
    {
        auto it = pp_get(i);

        if (not it->get())
            throw utils::ArgError(
                fmt(_("Set: empty or null value at '%1%'")) % i);

        return *it->get();
    }

    inline const Value& pp_get_value(size_type i) const
    {
        auto it = pp_get(i);

        if (not it->get())
            throw utils::ArgError(
                fmt(_("Set: empty or null value at '%1%'")) % i);

        return *it->get();
    }

    template <typename T, typename... Args>
        T& pp_add(Args&&... args)
    {
        auto value = std::unique_ptr<Value>(new T(std::forward<Args>(args)...));
        auto *ret = static_cast<T*>(value.get());

        m_value.emplace_back(std::move(value));

        return *ret;
    }
};

inline const Set& toSetValue(const std::unique_ptr<Value>& value)
{ return value::reference(value).toSet(); }

inline const Set& toSetValue(const Value& value)
{ return value.toSet(); }

inline Set& toSetValue(Value& value)
{ return value.toSet(); }

inline const VectorValue& toSet(const std::unique_ptr<Value>& value)
{ return value::reference(value).toSet().value(); }

inline const VectorValue& toSet(const Value& value)
{ return value.toSet().value(); }

inline VectorValue& toSet(Value& value)
{ return value.toSet().value(); }

}} // namespace vle value

#endif
