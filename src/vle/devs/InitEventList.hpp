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

#ifndef VLE_DEVS_INITEVENTLIST_HPP
#define VLE_DEVS_INITEVENTLIST_HPP

#include <unordered_map>
#include <vle/DllDefines.hpp>
#include <vle/value/Value.hpp>

namespace vle {
namespace devs {

/**
 * @brief Map Value a container to a pair of std::string, Value pointer. The
 * map can not contains null data.
 */
class VLE_API InitEventList
{
public:
    using container_type =
      std::unordered_map<std::string, std::shared_ptr<const value::Value>>;
    using size_type = container_type::size_type;
    using iterator = container_type::iterator;
    using const_iterator = container_type::const_iterator;
    using value_type = container_type::value_type;

    InitEventList() = default;
    InitEventList(const InitEventList&) = default;
    InitEventList(InitEventList&&) = default;
    InitEventList& operator=(const InitEventList&) = default;
    InitEventList& operator=(InitEventList&&) = default;
    ~InitEventList() = default;

    /**
     * @brief Add a value::Value into the list.
     *
     * @throw vle::utils::ArgError if @c name already exists.
     * @param name Name of the value.
     * @param value Value.
     */
    void add(const std::string& name,
             std::shared_ptr<const value::Value> value);

    /**
     * @brief Test if the map have already a Value with specified name.
     * @param name the name to find into value.
     * @return true if Value exist, false otherwise.
     */
    bool exist(const std::string& name) const
    {
        return find(name) != end();
    }

    /**
     * @brief Get a Value from the map specified by his name.
     * @param name The name of the value.
     * @return A reference to the specified value or a newly builded.
     * @throw utils::ArgError if the key does not exist.
     */
    const std::shared_ptr<const value::Value>& operator[](
      const std::string& name) const;

    /**
     * @brief Get the Value objet for specified name.
     * @param name The name of the Value in the map.
     * @return a reference to the Value.
     * @throw utils::ArgError if value don't exist.
     */
    const std::shared_ptr<const value::Value>& get(
      const std::string& name) const;

    /**
     * @brief Get a constant access to the std::map.
     * @return a reference to the const std::map.
     */
    inline const container_type& value() const
    {
        return m_value;
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
    bool getBoolean(const std::string& name) const;

    /**
     * @brief Get the Integer value objet from specified name.
     * @param name The name of the Value in the map.
     * @return a reference to the Value.
     * @throw utils::ArgError if type is not Value::STRING or value do not
     * exist.
     */
    int32_t getInt(const std::string& name) const;

    /**
     * @brief Get the Double value objet from specified name.
     * @param name The name of the Value in the map.
     * @return a reference to the Value.
     * @throw utils::ArgError if type is not Value::DOUBLE or value do not
     * exist.
     */
    double getDouble(const std::string& name) const;

    /**
     * @brief Get the Xml value objet from specified name.
     * @param name The name of the Value in the map.
     * @return a reference to the Value.
     * @throw utils::ArgError if type is not Value::STRING or value do not
     * exist.
     */
    const std::string& getXml(const std::string& name) const;

    /**
     * @brief Get the Table value objet from specified name.
     * @param name The name of the Value in the Map.
     * @return a reference to the Table.
     * @throw utils::ArgError if type is not a Table or value do not exist.
     */
    const value::Table& getTable(const std::string& name) const;

    /**
     * @brief Get the Tuple value objet from specified name.
     * @param name The name of the Value in the Map.
     * @return a reference to the Tuple.
     * @throw utils::ArgError if type is not a Tuple or value do not exist.
     */
    const value::Tuple& getTuple(const std::string& name) const;

    /**
     * @brief Get the Map value objet from specified name.
     * @param name The name of the Value in the map.
     * @return a reference to the Value.
     * @throw utils::ArgError if type is not Value::Map or value do not
     * exist.
     */
    const value::Map& getMap(const std::string& name) const;

    /**
     * @brief Get the Set value objet from specified name.
     * @param name The name of the Value in the map.
     * @return a reference to the Value.
     * @throw utils::ArgError if type is not Value::add or value do not
     * exist.
     */
    const value::Set& getSet(const std::string& name) const;

    /**
     * @brief Get the Matrix value from specified key.
     * @param name The name of the Value in the Map.
     * @return A referece to the Matrix.
     * @throw utils::ArgError if type is not Value::add or value do not
     * exist.
     */
    const value::Matrix& getMatrix(const std::string& name) const;

private:
    container_type m_value;
};
}
} // namespace vle devs

#endif
