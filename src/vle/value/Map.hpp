/**
 * @file value/Map.hpp
 * @author The VLE Development Team.
 * @brief Map Value a container to a pair of (std::string,  Value class).
 */

/*
 * Copyright (c) 2005 The vle Development Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef UTILS_VALUE_MAP_HPP
#define UTILS_VALUE_MAP_HPP

#include <vle/value/Value.hpp>
#include <map>

namespace vle { namespace value {

    /**
     * @brief Map Value a container to a pair of (std::string,  Value class).
     *
     */
    class MapFactory : public ValueBase
    {
    private:

        MapFactory()
        { }

        MapFactory(const MapFactory& mapfactory);

    public:
        typedef std::map < std::string, Value > MapValue;
        typedef std::map < std::string, Value >::iterator MapValueIt;
        typedef std::map < std::string, Value >::const_iterator MapValueConstIt;

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
         * <MAP>
         *  <VALUE NAME="x">
         *   <INTEGER VALUE="5" />
         *  </VALUE>
         *  <VALUE NAME="y">
         *   <INTEGER VALUE="0" />
         *  </VALUE>
         * </MAP>
         * @endcode
         */
        virtual std::string toXML() const;

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * Add a value into the map. Be carrefull, the data are not clone, the
         * use the pointer. Don't delete buffer. If a value already exist with
         * the same name it will be replace.
         *
         * @param name the name of Value to add.
         * @param value the Value to add.
         */
        void addValue(const std::string& name, Value value);

        /** 
         * @brief Test if the map have a Value with specified name.
         * 
         * @param name the name to find into value.
         * 
         * @return true if Value exist, false otherwise.
         */
        inline bool existValue(const std::string& name) const
        { return m_value.find(name) != m_value.end(); }

        /** 
         * @brief Get an access to the std::map.
         * 
         * @return a reference to the set::map.
         */
        inline MapValue& getValue()
        { return m_value; }

        /** 
         * @brief Get a constant access to the std::map.
         * 
         * @return a reference to the const std::map.
         */
        inline const MapValue& getValue() const
        { return m_value; }

        /**
         * Get the Value objet for specified name.
         *
         * @param name The name of the Value in the map.
         *
         * @return a reference to the Value.
         *
         * @throw Exception::Internal if value don't exist.
         */
        Value getValue(const std::string& name) const;

        /** 
         * @brief Get the String value objet from specified name.
         * 
         * @param name The name of the Value in the map.
         * 
         * @return a reference to the Value.
         *
         * @throw Exception::Internal if type is not Value::STRING or value do not
         * exist.
         */
        const std::string& getStringValue(const std::string& name) const;

        /** 
         * @brief Get the String value objet from specified name.
         * 
         * @param name The name of the Value in the map.
         * 
         * @return a reference to the Value.
         *
         * @throw Exception::Internal if type is not Value::STRING or value do not
         * exist.
         */
        bool getBooleanValue(const std::string& name) const;

        /** 
         * @brief Get the integer value objet from specified name.
         * 
         * @param name The name of the Value in the map.
         * 
         * @return a reference to the Value.
         *
         * @throw Exception::Internal if type is not Value::INTEGER or value do not
         * exist.
         */
        long getLongValue(const std::string& name) const;

        /** 
         * @brief Get the integer value objet from specified name.
         * 
         * @param name The name of the Value in the map.
         * 
         * @return a reference to the Value.
         *
         * @throw Exception::Internal if type is not Value::INTEGER or value do not
         * exist.
         */
        int getIntValue(const std::string& name) const;

        /** 
         * @brief Get the Double value objet from specified name.
         * 
         * @param name The name of the Value in the map.
         * 
         * @return a reference to the Value.
         *
         * @throw Exception::Internal if type is not Value::DOUBLE or value do not
         * exist.
         */
        double getDoubleValue(const std::string& name) const;

        /** 
         * @brief Get the Map value objet from specified name.
         * 
         * @param name The name of the Value in the map.
         * 
         * @return a reference to the Value.
         *
         * @throw Exception::Internal if type is not Value::Map or value do not
         * exist.
         */
        Map getMapValue(const std::string& name) const;

        /** 
         * @brief Get the Set value objet from specified name.
         * 
         * @param name The name of the Value in the map.
         * 
         * @return a reference to the Value.
         *
         * @throw Exception::Internal if type is not Value::SET or value do not
         * exist.
         */
        Set getSetValue(const std::string& name) const;

        /** 
         * @brief Delete all value from map.
         */
        void clear();

        /** 
         * @brief Get the first constant iterator from Map.
         * 
         * @return the first iterator.
         */
        inline MapValueConstIt begin() const
        { return m_value.begin(); }

        /** 
         * @brief Get the last constant iterator from Map.
         * 
         * @return the last iterator.
         */
        inline MapValueConstIt end() const
        { return m_value.end(); }

    private:
        MapValue m_value;
    };

}} // namespace vle value
#endif
