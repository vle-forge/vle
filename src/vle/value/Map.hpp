/**
 * @file src/vle/value/Map.hpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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




#ifndef UTILS_VALUE_MAP_HPP
#define UTILS_VALUE_MAP_HPP

#include <vle/value/Value.hpp>
#include <map>



namespace vle { namespace value {
    
    /** 
     * @brief Define a list of Value in a dictionnary.
     */
    typedef std::map < std::string, Value > MapValue;

    /**
     * @brief Map Value a container to a pair of (std::string,  Value class).
     */
    class MapFactory : public ValueBase
    {
    private:
        MapFactory()
        { }

        MapFactory(const MapFactory& mapfactory);

    public:
        virtual ~MapFactory()
        { }

        static Map create();

        virtual Value clone() const;

        virtual ValueBase::type getType() const
        { return ValueBase::MAP; }

        virtual std::string toFile() const;

        virtual std::string toString() const;

        /**
         * @code
         * <map>
         *  <value name="x">
         *   <integer value="5" />
         *  </value>
         *  <value name="y">
         *   <integer value="0" />
         *  </value>
         * </map>
         * @endcode
         */
        virtual std::string toXML() const;

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Add a value into the map. Be carrefull, the data is not
         * cloned. Don't delete buffer after. If a value already exist with the
         * same name it will be replace.
         * @param name the name of Value to add.
         * @param value the Value to add.
         */
        void addValue(const std::string& name, Value value)
        { m_value[name] = value; }

        /**
         * @brief Add a value into the map. The data is clone. If a value
         * already exist with the same name it will be replace.
         * @param name the name of Value to add.
         * @param value the Value to add.
         */
        void addCloneValue(const std::string& name, Value value)
        { m_value[name] = CloneValue()(value); }

        /** 
         * @brief Test if the map have already a Value with specified name.
         * @param name the name to find into value.
         * @return true if Value exist, false otherwise.
         */
        inline bool existValue(const std::string& name) const
        { return m_value.find(name) != m_value.end(); }

        /** 
         * @brief Get a Value from the map specified by his name. If name does
         * not exist, a new value::Value() is build.
         * @param name The name of the value.
         * @return A reference to the specified value or a newly builded.
         */
        inline Value& operator[](const std::string& name)
        { return m_value[name]; }

        /** 
         * @brief Get an access to the std::map.
         * @return a reference to the set::map.
         */
        inline MapValue& getValue()
        { return m_value; }

        /** 
         * @brief Get a constant access to the std::map.
         * @return a reference to the const std::map.
         */
        inline const MapValue& getValue() const
        { return m_value; }

        /**
         * @brief Get the Value objet for specified name.
         * @param name The name of the Value in the map.
         * @return a reference to the Value.
         * @throw utils::ArgError if value don't exist.
         */
        Value getValue(const std::string& name) const;

        /** 
         * @brief Get the String value objet from specified name.
         * @param name The name of the Value in the map.
         * @return a reference to the Value.
         * @throw utils::ArgError if type is not Value::STRING or value do not
         * exist.
         */
        const std::string& getStringValue(const std::string& name) const;

        /** 
         * @brief Set a string to the value of the specified key. If the key
         * does not exist, it will be build.
         * @param name The key of the map.
         * @param value The value of the key.
         */
        void setStringValue(const std::string& name, const std::string& value);

        /** 
         * @brief Get the String value objet from specified name.
         * @param name The name of the Value in the map.
         * @return a reference to the Value.
         * @throw utils::ArgError if type is not Value::STRING or value do not
         * exist.
         */
        bool getBooleanValue(const std::string& name) const;

        /** 
         * @brief Set a boolean to the value of the specified key. If the key
         * does not exist, it will be build.
         * @param name The key of the map.
         * @param value The value of the key.
         */
        void setBooleanValue(const std::string& name, bool value);

        /** 
         * @brief Get the long integer value objet from specified name.
         * @param name The name of the Value in the map.
         * @return a reference to the Value.
         * @throw utils::ArgError if type is not Value::INTEGER or value do not
         * exist.
         */
        long getLongValue(const std::string& name) const;

        /** 
         * @brief Set a long integer to the value of the specified key. If the
         * key does not exist, it will be build.
         * @param name The key of the map.
         * @param value The value of the key.
         */
        void setLongValue(const std::string& name, long value);

        /** 
         * @brief Get the integer value objet from specified name.
         * @param name The name of the Value in the map.
         * @return a reference to the Value.
         * @throw utils::ArgError if type is not Value::INTEGER or value do not
         * exist.
         */
        int getIntValue(const std::string& name) const;

        /** 
         * @brief Set a integer to the value of the specified key. If the
         * key does not exist, it will be build.
         * @param name The key of the map.
         * @param value The value of the key.
         */
        void setIntValue(const std::string& name, int value);

        /** 
         * @brief Get the Double value objet from specified name.
         * @param name The name of the Value in the map.
         * @return a reference to the Value.
         * @throw utils::ArgError if type is not Value::DOUBLE or value do not
         * exist.
         */
        double getDoubleValue(const std::string& name) const;

        /** 
         * @brief Set a double to the value of the specified key. If the
         * key does not exist, it will be build.
         * @param name The key of the map.
         * @param value The value of the key.
         */
        void setDoubleValue(const std::string& name, double value);

        /** 
         * @brief Get the XML value objet from specified name.
         * @param name The name of the Value in the map.
         * @return a reference to the Value.
         * @throw utils::ArgError if type is not Value::STRING or value do not
         * exist.
         */
        const std::string& getXMLValue(const std::string& name) const;

        /** 
         * @brief Set an XML to the value of the specified key. If the key
         * does not exist, it will be build.
         * @param name The key of the map.
         * @param value The value of the key.
         */
        void setXMLValue(const std::string& name, const std::string& value);

        /** 
         * @brief Get the Map value objet from specified name.
         * @param name The name of the Value in the map.
         * @return a reference to the Value.
         * @throw utils::ArgError if type is not Value::Map or value do not
         * exist.
         */
        Map getMapValue(const std::string& name) const;

        /** 
         * @brief Get the Set value objet from specified name.
         * @param name The name of the Value in the map.
         * @return a reference to the Value.
         * @throw utils::ArgError if type is not Value::SET or value do not
         * exist.
         */
        Set getSetValue(const std::string& name) const;

        /** 
         * @brief Delete all value from map.
         */
        void clear();

        /** 
         * @brief Get the first constant iterator from Map.
         * @return the first iterator.
         */
        inline MapValue::const_iterator begin() const
        { return m_value.begin(); }

        /** 
         * @brief Get the last constant iterator from Map.
         * @return the last iterator.
         */
        inline MapValue::const_iterator end() const
        { return m_value.end(); }

    private:
        MapValue m_value;
    };

    Map toMapValue(const Value& value);

    const MapValue& toMap(const Value& value);

}} // namespace vle value
#endif
