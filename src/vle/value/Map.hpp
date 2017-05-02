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

#ifndef VLE_VALUE_MAP_HPP
#define VLE_VALUE_MAP_HPP 1

#include <unordered_map>
#include <vle/DllDefines.hpp>
#include <vle/value/Value.hpp>

namespace vle {
namespace value {

/**
 * @brief Define a list of Value in a dictionnary.
 */
using MapValue = std::unordered_map<std::string, std::unique_ptr<Value>>;

/**
 * @brief Map Value a container to a pair of std::string, Value pointer. The
 * map can not contains null data.
 */
class VLE_API Map : public Value
{
public:
    using size_type = MapValue::size_type;
    using iterator = MapValue::iterator;
    using const_iterator = MapValue::const_iterator;
    using value_type = MapValue::value_type;

    /**
     * @brief Build a Map object with an empty MapValue.
     */
    Map()
    {
    }

    /**
     * @brief Copy constructor. All the Value are cloned.
     * @param value The value to copy.
     */
    Map(const Map& value);

    /**
     * @brief Delete all Value in the Set.
     */
    virtual ~Map()
    {
    }

    /**
     * @brief Build a Xml.
     * @param value the default value of the Xml.
     * @return A new allocated Xml.
     */
    static std::unique_ptr<value::Value> create()
    {
        return std::unique_ptr<value::Value>(new Map());
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Clone the current Map and recursively for all Value in the
     * MapValue.
     * @return A new Map.
     */
    virtual std::unique_ptr<Value> clone() const override final;

    /**
     * @brief Get the type of this class.
     * @return Value::MAP.
     */
    virtual Value::type getType() const override final;

    /**
     * @brief Push all Value from the MapValue, recursively and colon
     * separated.
     * @code
     * (key,value), key2,123) (key3, true)
     * @endcode
     * @param out The output stream.
     */
    virtual void writeFile(std::ostream& out) const override final;

    /**
     * @brief Push all VAlue from the MapValue, recursively and colon
     * separated.
     * @code
     * (key,value), key2,123) (key3, true)
     * @param out The output stream.
     */
    virtual void writeString(std::ostream& out) const override final;

    /**
     * @brief Push all Value from the MapValue recursively in an XML
     * representation.
     * @code
     * <map>
     *  <key name="key1">
     *   <string>value</string>
     *  </key>
     *  <key name="key2">
     *   <integer>123</integer>
     *  </key>
     *  <key name="key3">
     *   <boolean>true</boolean>
     *  </key>
     * </map>
     * @endcode
     * @param out The output stream.
     */
    virtual void writeXml(std::ostream& out) const override final;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Add a value into the map. Be carrefull, the data is not
     * cloned. Don't delete buffer after. If a value already exist with the
     * same key, it will be deleted and replaced.
     * @param name the name of Value to add.
     * @param value the Value to add.
     * @throw utils::ArgError if the value is null.
     */
    void add(const std::string& name, std::unique_ptr<Value> value);

    /**
     * @brief Add a value into the map. Be carrefull, the data is not
     * cloned. Don't delete buffer after. If a value already exist with the
     * same key, it will be deleted and replaced.
     * @param name the name of Value to add.
     * @param value the Value to add.
     * @throw utils::ArgError if the value is null.
     */
    void set(const std::string& name, std::unique_ptr<Value> value);

    /**
     * @brief Test if the map have already a Value with specified name.
     * @param name the name to find into value.
     * @return true if Value exist, false otherwise.
     */
    bool exist(const std::string& name) const;

    /**
     * @brief Get a Value from the map specified by his name.
     * @param name The name of the value.
     * @return A reference to the specified value or a newly builded.
     * @throw utils::ArgError if the key does not exist.
     */
    const std::unique_ptr<Value>& operator[](const std::string& name) const;

    /**
     * @brief Get the Value objet for specified name.
     * @param name The name of the Value in the map.
     * @return a reference to the Value.
     * @throw utils::ArgError if value don't exist.
     */
    const std::unique_ptr<Value>& get(const std::string& name) const;

    /**
     * @brief Get the Value object for the specified name. If the name
     * exist, the pair key value is destroyed and the value is returned.
     * @param name The name of the Value in the map.
     * @return the pointer to the Value.
     * @throw utils::ArgError if value don't exist.
     */
    std::unique_ptr<Value> give(const std::string& name);

    /**
     * @brief Get an access to the std::map.
     * @return a reference to the set::map.
     */
    inline MapValue& value()
    {
        return m_value;
    }

    /**
     * @brief Get a constant access to the std::map.
     * @return a reference to the const std::map.
     */
    inline const MapValue& value() const
    {
        return m_value;
    }

    /**
     * @brief Delete all value from map.
     */
    void clear()
    {
        m_value.clear();
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
     * Return the number of element in the @c std::map.
     *
     * @return An integer [0..MAX_SIZE_T];
     */
    inline size_type size() const
    {
        return m_value.size();
    }

    /**
     * @brief Get the first constant iterator from Map.
     * @return the first iterator.
     */
    inline const_iterator begin() const
    {
        return m_value.begin();
    }

    /**
     * @brief Get the last constant iterator from Map.
     * @return the last iterator.
     */
    inline const_iterator end() const
    {
        return m_value.end();
    }

    /**
     * @brief Get the first constant iterator from Map.
     * @return the first iterator.
     */
    inline iterator begin()
    {
        return m_value.begin();
    }

    /**
     * @brief Get the last constant iterator from Map.
     * @return the last iterator.
     */
    inline iterator end()
    {
        return m_value.end();
    }

    /**
     * @brief Find an constant iterator into the value::Map using a key.
     * @param key The key of the std::pair < key, value > to find.
     * @return A constant iterator or end() if key is not found.
     */
    inline const_iterator find(const std::string& key) const
    {
        return m_value.find(key);
    }

    /**
     * @brief Find an iterator into the value::Map using a key.
     * @param key The key of the std::pair < key, value > to find.
     * @return An iterator or end() if key is not found.
     */
    inline iterator find(const std::string& key)
    {
        return m_value.find(key);
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Set a string to the value of the specified key. If the key
     * does not exist, it will be build.
     * @param name The key of the map.
     * @param value The value of the key.
     */
    Null& addNull(const std::string& name);

    /**
     * @brief Get the String value objet from specified name.
     * @param name The name of the Value in the map.
     * @return a reference to the Value.
     * @throw utils::ArgError if type is not Value::STRING or value do not
     * exist.
     */
    const std::string& getString(const std::string& name) const;

    /**
     * @brief Get the String value objet from specified name.
     * @param name The name of the Value in the map.
     * @return a reference to the Value.
     * @throw utils::ArgError if type is not Value::STRING or value do not
     * exist.
     */
    std::string& getString(const std::string& name);

    /**
     * @brief Set a string to the value of the specified key. If the key
     * does not exist, it will be build.
     * @param name The key of the map.
     * @param value The value of the key.
     */
    String& addString(const std::string& name, const std::string& value);

    /**
     * @brief Get the String value objet from specified name.
     * @param name The name of the Value in the map.
     * @return a reference to the Value.
     * @throw utils::ArgError if type is not Value::STRING or value do not
     * exist.
     */
    bool getBoolean(const std::string& name) const;

    /**
     * @brief Get the String value objet from specified name.
     * @param name The name of the Value in the map.
     * @return a reference to the Value.
     * @throw utils::ArgError if type is not Value::STRING or value do not
     * exist.
     */
    bool& getBoolean(const std::string& name);

    /**
     * @brief Set a boolean to the value of the specified key. If the key
     * does not exist, it will be build.
     * @param name The key of the map.
     * @param value The value of the key.
     */
    Boolean& addBoolean(const std::string& name, bool value);

    /**
     * @brief Get the Integer value objet from specified name.
     * @param name The name of the Value in the map.
     * @return a reference to the Value.
     * @throw utils::ArgError if type is not Value::STRING or value do not
     * exist.
     */
    int32_t getInt(const std::string& name) const;

    /**
     * @brief Get the integer value objet from specified name.
     * @param name The name of the Value in the map.
     * @return a reference to the Value.
     * @throw utils::ArgError if type is not Value::INTEGER or value do not
     * exist.
     */
    int32_t& getInt(const std::string& name);

    /**
     * @brief Set a integer to the value of the specified key. If the
     * key does not exist, it will be build.
     * @param name The key of the map.
     * @param value The value of the key.
     */
    void addInt(const std::string& name, const int32_t& value);

    /**
     * @brief Get the Double value objet from specified name.
     * @param name The name of the Value in the map.
     * @return a reference to the Value.
     * @throw utils::ArgError if type is not Value::DOUBLE or value do not
     * exist.
     */
    double getDouble(const std::string& name) const;

    /**
     * @brief Get the Double value objet from specified name.
     * @param name The name of the Value in the map.
     * @return a reference to the Value.
     * @throw utils::ArgError if type is not Value::DOUBLE or value do not
     * exist.
     */
    double& getDouble(const std::string& name);

    /**
     * @brief Set a double to the value of the specified key. If the
     * key does not exist, it will be build.
     * @param name The key of the map.
     * @param value The value of the key.
     */
    Double& addDouble(const std::string& name, const double& value);

    /**
     * @brief Get the Xml value objet from specified name.
     * @param name The name of the Value in the map.
     * @return a reference to the Value.
     * @throw utils::ArgError if type is not Value::STRING or value do not
     * exist.
     */
    const std::string& getXml(const std::string& name) const;

    /**
     * @brief Get the Xml value objet from specified name.
     * @param name The name of the Value in the map.
     * @return a reference to the Value.
     * @throw utils::ArgError if type is not Value::STRING or value do not
     * exist.
     */
    std::string& getXml(const std::string& name);

    /**
     * @brief Set an Xml to the value of the specified key. If the key does not
     * exist, it will be build.
     * @param name The key of the Xml.
     * @param value The value of the key.
     */
    Xml& addXml(const std::string& name, const std::string& value);

    /**
     * @brief Get the Table value objet from specified name.
     * @param name The name of the Value in the Map.
     * @return a reference to the Table.
     * @throw utils::ArgError if type is not a Table or value do not exist.
     */
    const Table& getTable(const std::string& name) const;

    /**
     * @brief Get the Table value objet from specified name.
     * @param name The name of the Value in the Map.
     * @return a reference to the Table.
     * @throw utils::ArgError if type is not a Table or value do not exist.
     */
    Table& getTable(const std::string& name);

    /**
     * @brief Add an XML to the value of the specified key. If the key does not
     * exist, it will be build.
     * @param name The key of the map.
     * @param value The value of the key.
     */
    Table& addTable(const std::string& name,
                    std::size_t width = 0,
                    std::size_t height = 0);

    /**
     * @brief Get the Tuple value objet from specified name.
     * @param name The name of the Value in the Map.
     * @return a reference to the Tuple.
     * @throw utils::ArgError if type is not a Tuple or value do not exist.
     */
    const Tuple& getTuple(const std::string& name) const;

    /**
     * @brief Get the Tuple value objet from specified name.
     * @param name The name of the Value in the Map.
     * @return a reference to the Tuple.
     * @throw utils::ArgError if type is not a Tuple or value do not exist.
     */
    Tuple& getTuple(const std::string& name);

    /**
     * @brief Add an Tuple to the value of the specified key. If the key does
     * not exist, it will be build.
     * @param name The key of the Map.
     * @param value The value of the key.
     */
    Tuple& addTuple(const std::string& name,
                    std::size_t width = 0,
                    double value = 0.0);

    /**
     * @brief Add a Map into the Map.
     * @param name The key name.
     * @return A reference to the newly allocated Map.
     */
    Map& addMap(const std::string& name);

    /**
     * @brief Add a Set into the Map.
     * @param name The key name.
     * @return A reference to the newly allocated Set.
     */
    Set& addSet(const std::string& name);

    /**
     * @brief Add a Matrix into the Map.
     * @param name The key name.
     * @return A reference to the newly allocated Matrix.
     */
    Matrix& addMatrix(const std::string& name);

    /**
     * @brief Add directly multiple values to a map
     * @param toadd, the number of values to add
     * @param fill, the default value for filling new elements
     * @note: new keys are generated for added values
     */
    void addMultilpleValues(int toadd, const vle::value::Value& fill);

    /**
     * @brief Get the Map value objet from specified name.
     * @param name The name of the Value in the map.
     * @return a reference to the Value.
     * @throw utils::ArgError if type is not Value::Map or value do not
     * exist.
     */
    Map& getMap(const std::string& name);

    /**
     * @brief Get the Set value objet from specified name.
     * @param name The name of the Value in the map.
     * @return a reference to the Value.
     * @throw utils::ArgError if type is not Value::add or value do not
     * exist.
     */
    Set& getSet(const std::string& name);

    /**
     * @brief Get the Matrix value from specified key.
     * @param name The name of the Value in the Map.
     * @return A referece to the Matrix.
     * @throw utils::ArgError if type is not Value::add or value do not
     * exist.
     */
    Matrix& getMatrix(const std::string& name);

    /**
     * @brief Get the Map value objet from specified name.
     * @param name The name of the Value in the map.
     * @return a reference to the Value.
     * @throw utils::ArgError if type is not Value::Map or value do not
     * exist.
     */
    const Map& getMap(const std::string& name) const;

    /**
     * @brief Get the Set value objet from specified name.
     * @param name The name of the Value in the map.
     * @return a reference to the Value.
     * @throw utils::ArgError if type is not Value::add or value do not
     * exist.
     */
    const Set& getSet(const std::string& name) const;

    /**
     * @brief Get the Matrix value from specified key.
     * @param name The name of the Value in the Map.
     * @return A referece to the Matrix.
     * @throw utils::ArgError if type is not Value::add or value do not
     * exist.
     */
    const Matrix& getMatrix(const std::string& name) const;

private:
    MapValue m_value;
};

inline const Map&
toMapValue(std::shared_ptr<Value> value)
{
    return value::reference(value).toMap();
}

inline const Map&
toMapValue(std::shared_ptr<const Value> value)
{
    return value::reference(value).toMap();
}

inline const Map&
toMapValue(const std::unique_ptr<Value>& value)
{
    return value::reference(value).toMap();
}

inline const Map&
toMapValue(const Value& value)
{
    return value.toMap();
}

inline Map&
toMapValue(Value& value)
{
    return value.toMap();
}

inline const MapValue&
toMap(std::shared_ptr<Value> value)
{
    return value::reference(value).toMap().value();
}

inline const MapValue&
toMap(std::shared_ptr<const Value> value)
{
    return value::reference(value).toMap().value();
}

inline const MapValue&
toMap(const std::unique_ptr<Value>& value)
{
    return value::reference(value).toMap().value();
}

inline const MapValue&
toMap(const Value& value)
{
    return value.toMap().value();
}

inline MapValue&
toMap(Value& value)
{
    return value.toMap().value();
}
}
} // namespace vle value

#endif
