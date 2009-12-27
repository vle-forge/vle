/**
 * @file vle/value/Map.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef VLE_VALUE_MAP_HPP
#define VLE_VALUE_MAP_HPP

#include <vle/value/Value.hpp>
#include <vle/value/DllDefines.hpp>
#include <boost/serialization/map.hpp>
#include <map>

namespace vle { namespace value {

    /**
     * @brief Define a list of Value in a dictionnary.
     */
    typedef std::map < std::string, Value* > MapValue;

    /**
     * @brief Map Value a container to a pair of std::string, Value pointer. The
     * map can not contains null data. .
     */
    class VLE_VALUE_EXPORT Map : public Value
    {
    public:
        typedef MapValue::size_type size_type;
	typedef MapValue::iterator iterator;
	typedef MapValue::const_iterator const_iterator;

        /**
         * @brief Build a Map object with an empty MapValue.
         */
        Map() :
            m_value()
        {}

        /**
         * @brief Copy constructor. All the Value are cloned.
         * @param value The value to copy.
         */
        Map(const Map& value);

        /**
         * @brief Delete all Value in the Set.
         */
	virtual ~Map();

        ///
        ////
        ///

        /**
         * @brief Build a new Map using the boost::pool memory management.
         * @return A new Map allocated from the boost::pool.
         */
        static Map* create()
        { return new Map(); }

        ///
        ////
        ///

        /**
         * @brief Clone the current Map and recursively for all Value in the
         * MapValue.
         * @return A new boost pool allocated value::Value.
         */
        virtual Value* clone() const
        { return new Map(*this); }

        /**
         * @brief Get the type of this class.
         * @return Value::MAP.
         */
        virtual Value::type getType() const
        { return Value::MAP; }

        /**
         * @brief Push all Value from the MapValue, recursively and colon
         * separated.
         * @code
         * (key,value), key2,123) (key3, true)
         * @endcode
         * @param out The output stream.
         */
        virtual void writeFile(std::ostream& out) const;

        /**
         * @brief Push all VAlue from the MapValue, recursively and colon
         * separated.
         * @code
         * (key,value), key2,123) (key3, true)
         * @param out The output stream.
         */
	virtual void writeString(std::ostream& out) const;

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
	virtual void writeXml(std::ostream& out) const;

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Add a value into the map. Be carrefull, the data is not
         * cloned. Don't delete buffer after. If a value already exist with the
         * same key, it will be deleted and replaced.
         * @param name the name of Value to add.
         * @param value the Value to add.
         * @throw utils::ArgError if the value is null.
         */
        void add(const std::string& name, Value* value);

        /**
         * @brief Add a value into the map. The data is clone. If a value
         * already exist with the same name it will be replace.
         * @param name the name of Value to add.
         * @param value the Value to add.
         */
        void addClone(const std::string& name, const Value& value);

        /**
         * @brief Add a value into the map. The data is clone. If a value
         * already exist with the same name it will be replace.
         * @param name the name of Value to add.
         * @param value the Value to add.
         */
        void addClone(const std::string& name, const Value* value);

        /**
         * @brief Test if the map have already a Value with specified name.
         * @param name the name to find into value.
         * @return true if Value exist, false otherwise.
         */
        inline bool existValue(const std::string& name) const
        { return m_value.find(name) != m_value.end(); }

        /**
         * @brief Get a Value from the map specified by his name.
         * @param name The name of the value.
         * @return A reference to the specified value or a newly builded.
         * @throw utils::ArgError if the key does not exist.
         */
        Value& operator[](const std::string& name);

        /**
         * @brief Get a Value from the map specified by his name.
         * @param name The name of the value.
         * @return A constant reference to the specified value or a newly
         * builded.
         * @throw utils::ArgError if the key does not exist.
         */
        const Value& operator[](const std::string& name) const;

        /**
         * @brief Get the Value objet for specified name.
         * @param name The name of the Value in the map.
         * @return a reference to the Value.
         * @throw utils::ArgError if value don't exist.
         */
        const Value& get(const std::string& name) const;

	/**
         * @brief Get the Value objet for specified name.
         * @param name The name of the Value in the map.
         * @return a reference to the Value.
         * @throw utils::ArgError if value don't exist.
         */
        Value& get(const std::string& name);

        /**
         * @brief Get the Value object for the specified name. If the name
         * exist, the pair key value is destroyed and the value is returned.
         * @param name The name of the Value in the map.
         * @return the pointer to the Value.
         * @throw utils::ArgError if value don't exist.
         */
        Value* give(const std::string& name);

        /**
         * @brief Get an access to the std::map.
         * @return a reference to the set::map.
         */
        inline MapValue& value()
        { return m_value; }

        /**
         * @brief Get a constant access to the std::map.
         * @return a reference to the const std::map.
         */
        inline const MapValue& value() const
        { return m_value; }

        /**
         * @brief Get the String value objet from specified name.
         * @param name The name of the Value in the map.
         * @return a reference to the Value.
         * @throw utils::ArgError if type is not Value::STRING or value do not
         * exist.
         */
        const std::string& getString(const std::string& name) const;

        /**
         * @brief Set a string to the value of the specified key. If the key
         * does not exist, it will be build.
         * @param name The key of the map.
         * @param value The value of the key.
         */
        void addString(const std::string& name, const std::string& value);

        /**
         * @brief Get the String value objet from specified name.
         * @param name The name of the Value in the map.
         * @return a reference to the Value.
         * @throw utils::ArgError if type is not Value::STRING or value do not
         * exist.
         */
        bool getBoolean(const std::string& name) const;

        /**
         * @brief Set a boolean to the value of the specified key. If the key
         * does not exist, it will be build.
         * @param name The key of the map.
         * @param value The value of the key.
         */
        void addBoolean(const std::string& name, bool value);

        /**
         * @brief Get the long integer value objet from specified name.
         * @param name The name of the Value in the map.
         * @return a reference to the Value.
         * @throw utils::ArgError if type is not Value::INTEGER or value do not
         * exist.
         */
        long getLong(const std::string& name) const;

        /**
         * @brief Set a long integer to the value of the specified key. If the
         * key does not exist, it will be build.
         * @param name The key of the map.
         * @param value The value of the key.
         */
        void addLong(const std::string& name, long value);

        /**
         * @brief Get the integer value objet from specified name.
         * @param name The name of the Value in the map.
         * @return a reference to the Value.
         * @throw utils::ArgError if type is not Value::INTEGER or value do not
         * exist.
         */
        int getInt(const std::string& name) const;

        /**
         * @brief Set a integer to the value of the specified key. If the
         * key does not exist, it will be build.
         * @param name The key of the map.
         * @param value The value of the key.
         */
        void addInt(const std::string& name, int value);

        /**
         * @brief Get the Double value objet from specified name.
         * @param name The name of the Value in the map.
         * @return a reference to the Value.
         * @throw utils::ArgError if type is not Value::DOUBLE or value do not
         * exist.
         */
        double getDouble(const std::string& name) const;

        /**
         * @brief Set a double to the value of the specified key. If the
         * key does not exist, it will be build.
         * @param name The key of the map.
         * @param value The value of the key.
         */
        void addDouble(const std::string& name, double value);

        /**
         * @brief Get the XML value objet from specified name.
         * @param name The name of the Value in the map.
         * @return a reference to the Value.
         * @throw utils::ArgError if type is not Value::STRING or value do not
         * exist.
         */
        const std::string& getXml(const std::string& name) const;

        /**
         * @brief Set an XML to the value of the specified key. If the key
         * does not exist, it will be build.
         * @param name The key of the map.
         * @param value The value of the key.
         */
        void addXml(const std::string& name, const std::string& value);

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
         * @brief Delete all value from map.
         */
        void clear();

        /**
         * @brief Get the first constant iterator from Map.
         * @return the first iterator.
         */
        inline const_iterator begin() const
        { return m_value.begin(); }

        /**
         * @brief Get the last constant iterator from Map.
         * @return the last iterator.
         */
        inline const_iterator end() const
        { return m_value.end(); }

        /**
         * @brief Get the first constant iterator from Map.
         * @return the first iterator.
         */
        inline iterator begin()
        { return m_value.begin(); }

        /**
         * @brief Get the last constant iterator from Map.
         * @return the last iterator.
         */
        inline iterator end()
        { return m_value.end(); }

    private:
        MapValue m_value;

        Value* getPointer(const std::string& name);

        const Value* getPointer(const std::string& name) const;

	friend class boost::serialization::access;
	template < class Archive >
	    void serialize(Archive& ar, const unsigned int /* version */)
	    {
		ar & boost::serialization::base_object < Value >(*this);
		ar & m_value;
	    }
    };

    inline const Map& toMapValue(const Value& value)
    { return value.toMap(); }

    inline const Map* toMapValue(const Value* value)
    { return value ? &value->toMap() : 0; }

    inline Map& toMapValue(Value& value)
    { return value.toMap(); }

    inline Map* toMapValue(Value* value)
    { return value ? &value->toMap() : 0; }

    inline const MapValue& toMap(const Value& value)
    { return value.toMap().value(); }

    inline MapValue& toMap(Value& value)
    { return value.toMap().value(); }

    inline const MapValue& toMap(const Value* value)
    { return value::reference(value).toMap().value(); }

    inline MapValue& toMap(Value* value)
    { return value::reference(value).toMap().value(); }

}} // namespace vle value

#endif
