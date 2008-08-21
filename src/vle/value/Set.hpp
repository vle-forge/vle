/**
 * @file vle/value/Set.hpp
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


#ifndef UTILS_VALUE_SET_HPP
#define UTILS_VALUE_SET_HPP

#include <vle/value/Value.hpp>
#include <vector>



namespace vle { namespace value {
    
    /** 
     * @brief Define a std::Vector of value.
     */
    typedef std::vector < Value > VectorValue;

    /**
     * @brief Set Value a container to Value class.
     * @author The VLE Development Team.
     */
    class SetFactory : public ValueBase
    {
    private:
        SetFactory()
        { }

        SetFactory(const SetFactory& setfactory);

    public:
        virtual ~SetFactory()
        { }

        static Set create();
            
        virtual Value clone() const;

        virtual ValueBase::type getType() const
        { return ValueBase::SET; }

        /**
         * @brief Add a value into the set. Be carrefull, the data are not
         * cloned, Don't delete buffer.
         * @param value the Value to add.
         */
        void addValue(const Value& value)
        { m_value.push_back(value); }

        /** 
         * @brief Add a null value into the set.
         */
        void addNullValue();

        /** 
         * @brief Add a BooleanValue into the set.
         * @param value
         */
        void addBooleanValue(bool value);

        /** 
         * @brief Get a bool from the specified index.
         * @param i The index to get value.
         * @return A value
         * @throw utils::ArgError if the index 'i' is to big or if value at
         * index 'i' is not a Boolean.
         */
        bool getBooleanValue(const size_t i) const;

        /** 
         * @brief Add a double into the set.
         * @param value
         */
        void addDoubleValue(double value);

        /** 
         * @brief Get a double from the specified index.
         * @param i The index to get value.
         * @return A value
         * @throw utils::ArgError if the index 'i' is to big or if value at
         * index 'i' is not a Double.
         */
        double getDoubleValue(const size_t i) const;

        /** 
         * @brief Add an IntegerValue into the set.
         * @param value 
         */
        void addIntValue(int value);

        /** 
         * @brief Get a int from the specified index.
         * @param i The index to get value.
         * @return A value
         * @throw utils::ArgError if the index 'i' is to big or if value at
         * index 'i' is not a Integer.
         */
        int getIntValue(const size_t i) const;

        /** 
         * @brief Add a IntegerValue into the set.
         * @param value 
         */
        void addLongValue(long value);

        /** 
         * @brief Get a long from the specified index.
         * @param i The index to get value.
         * @return A value
         * @throw utils::ArgError if the index 'i' is to big or if value at
         * index 'i' is not a Integer.
         */
        long getLongValue(const size_t i) const;

        /** 
         * @brief Add a StringValue into the set.
         * @param value 
         */
        void addStringValue(const std::string& value);

        /** 
         * @brief Get a string from the specified index.
         * @param i The index to get value.
         * @return A value
         * @throw utils::ArgError if the index 'i' is to big or if value at
         * index 'i' is not a String.
         */
        const std::string& getStringValue(const size_t i) const;

        /** 
         * @brief Add an XMLValue into the set.
         * @param value 
         */
        void addXMLValue(const std::string& value);

        /** 
         * @brief Get a string from the specified index.
         * @param i The index to get value.
         * @return A value
         * @throw utils::ArgError if the index 'i' is to big or if value at
         * index 'i' is not an XML.
         */
        const std::string& getXMLValue(const size_t i) const;

        /**
         * @brief Add a value into the set. The value is clone and store into
         * the Set.
         * @param value the Value to clone and add.
         */
        void addCloneValue(Value value)
        { m_value.push_back(value->clone()); }

        /** 
         * @brief Get a reference to the VectorValue of the SetFactory.
         * @return A reference to the VectorValue.
         */
        inline VectorValue& getValue()
        { return m_value; }

        /** 
         * @brief Get a constant reference to the VectorValue of the SetFactory.
         * @return A constant reference to the VectorValue.
         */
        inline const VectorValue& getValue() const
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
         * @brief Get a constant reference to the Value at specified index.
         * @param i The index of the value.
         * @return A constant reference.
         * @throw utils::ArgError if index 'i' is too big.
         */
        const Value& getValue(const size_t i) const;

        /** 
         * @brief Get a reference to the Value at specified index.
         * @param i The index of the value.
         * @return A reference.
         * @throw utils::ArgError if index 'i' is too big.
         */
        Value& getValue(const size_t i);

        /** 
         * @brief Get the size of the VectorValue.
         * @return the size of the VectorValue.
         */
        inline size_t size() const
        { return m_value.size(); }

        /** 
         * @brief Remove all value from the VectorValue.
         */
        inline void clear()
        { return m_value.clear(); }

        virtual std::string toFile() const;

        virtual std::string toString() const;

        virtual std::string toXML() const;

    private:
        VectorValue m_value;
    };
    
    Set toSetValue(const Value& value);
    
    const VectorValue& toSet(const Value& value);

}} // namespace vle value
#endif
