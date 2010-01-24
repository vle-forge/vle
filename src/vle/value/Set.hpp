/**
 * @file vle/value/Set.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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


#ifndef VLE_VALUE_SET_HPP
#define VLE_VALUE_SET_HPP

#include <vle/value/Value.hpp>
#include <vle/value/DllDefines.hpp>
#include <boost/serialization/vector.hpp>
#include <vector>

namespace vle { namespace value {

    /**
     * @brief Define a std::Vector of value.
     */
    typedef std::vector < Value* > VectorValue;

    /**
     * @brief Set Value is a container of Value object and can not contains null
     * pointer.
     */
    class VLE_VALUE_EXPORT Set : public Value
    {
    public:
	typedef VectorValue::size_type size_type;
	typedef VectorValue::iterator iterator;
	typedef VectorValue::const_iterator const_iterator;

        /**
         * @brief Build a Set object with an empty VectorValue.
         */
        Set() :
            m_value()
        {}

        /**
         * @brief Copy constructor. All the Value are cloned.
         * @param value The value to copy.
         */
        Set(const Set& value);

        /**
	 * @brief Delete all Value in the set.
	 */
	virtual ~Set();

        ///
        ////
        ///

        /**
         * @brief Build a new Set using the boost::pool memory management.
         * @return A new Set allocated from the boost::pool.
         */
        static Set* create()
        { return new Set(); }

        ///
        ////
        ///

        /**
         * @brief Clone the current Set and recursively for all Value in the
         * VectorValue.
         * @return A new boost pool allocated value::Value.
         */
        virtual Value* clone() const
        { return new Set(*this); }

        /**
         * @brief Get the type of this class.
         * @return Value::SET.
         */
        virtual Value::type getType() const
        { return Value::SET; }

        /**
         * @brief Push all Value from the VectorValue, recursively and colon
         * separated.
         * @code
         * 1,2,123.312,toto,321,1e10
         * @endcode
         * @param out The output stream.
         */
        virtual void writeFile(std::ostream& out) const;

        /**
         * @brief Push all Value from the VectorValue, recursively and colon
         * separated with parentheisis.
         * @code
         * (1,2,123.312,toto,321,1e10)
         * @endcode
         * @param out The output stream.
         */
	virtual void writeString(std::ostream& out) const;

        /**
         * @brief Push all Value from the VectorValue recursively in an XML
         * representation:
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
	virtual void writeXml(std::ostream& out) const;

        ///
        ////
        ///

        /**
         * @brief Add a value into the set. Be careful, the data is not cloned,
         * Don't delete buffer after.
         * @param value the Value to add.
         * @throw std::invalid_argument if value is null.
         */
        void add(Value* value);

        /**
         * @brief Add a value into the set. Be careful, the data are not cloned,
         * Don't delete buffer.
         * @param value the Value to add.
         */
        void add(const Value& value)
        { m_value.push_back(value.clone()); }

        /**
         * @brief Add a value into the set. The value is clone and store into
         * the Set.
         * @param value the Value to clone and add.
         */
        void addCloneValue(const Value& value)
        { m_value.push_back(value.clone()); }

        /**
         * @brief Add a null value into the set.
         */
        void addNull();

        /**
         * @brief Add a BooleanValue into the set.
         * @param value
         */
        void addBoolean(bool value);

        /**
         * @brief Get a bool from the specified index.
         * @param i The index to get value.
         * @return A value
         * @throw utils::ArgError if the index 'i' is to big or if value at
         * index 'i' is not a Boolean.
         */
        bool getBoolean(const size_type i) const;

        /**
         * @brief Add a double into the set.
         * @param value
         */
        void addDouble(double value);

        /**
         * @brief Get a double from the specified index.
         * @param i The index to get value.
         * @return A value
         * @throw utils::ArgError if the index 'i' is to big or if value at
         * index 'i' is not a Double.
         */
        double getDouble(const size_type i) const;

        /**
         * @brief Add an IntegerValue into the set.
         * @param value
         */
        void addInt(int value);

        /**
         * @brief Get a int from the specified index.
         * @param i The index to get value.
         * @return A value
         * @throw utils::ArgError if the index 'i' is to big or if value at
         * index 'i' is not a Integer.
         */
        int getInt(const size_type i) const;

        /**
         * @brief Add a IntegerValue into the set.
         * @param value
         */
        void addLong(long value);

        /**
         * @brief Get a long from the specified index.
         * @param i The index to get value.
         * @return A value
         * @throw utils::ArgError if the index 'i' is to big or if value at
         * index 'i' is not a Integer.
         */
        long getLong(const size_type i) const;

        /**
         * @brief Add a StringValue into the set.
         * @param value
         */
        void addString(const std::string& value);

        /**
         * @brief Get a string from the specified index.
         * @param i The index to get value.
         * @return A value
         * @throw utils::ArgError if the index 'i' is to big or if value at
         * index 'i' is not a String.
         */
        const std::string& getString(const size_type i) const;

        /**
         * @brief Add an XMLValue into the set.
         * @param value
         */
        void addXml(const std::string& value);

        /**
         * @brief Get a string from the specified index.
         * @param i The index to get value.
         * @return A value
         * @throw utils::ArgError if the index 'i' is to big or if value at
         * index 'i' is not an XML.
         */
        const std::string& getXml(const size_type i) const;

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
         * @brief Get a constant reference to the Value at specified index.
         * @param i The index of the value.
         * @return A constant reference.
         * @throw utils::ArgError if index 'i' is too big.
         */
        const Value& get(const size_type i) const;

        /**
         * @brief Get a reference to the Value at specified index.
         * @param i The index of the value.
         * @return A reference.
         * @throw utils::ArgError if index 'i' is too big.
         */
        Value& get(const size_type i);

        /**
         * @brief Get the pointer of the Value at specified index. The value at
         * the specified index was assign to NULL.
         * @param i The index of the value.
         * @return A reference.
         * @throw utils::ArgError if index 'i' is too big.
         */
        Value* give(const size_type& i);

        /**
         * @brief Get the size of the VectorValue.
         * @return the size of the VectorValue.
         */
        inline size_type size() const
        { return m_value.size(); }

        /**
         * @brief Delete all value from the VectorValue and clean the
         * VectorValue.
         */
        void clear();

        ///
        ////
        ///

        /**
         * @brief Serialize the value::Set into a text file using the
         * text format archive of the boost::serialization library.
         * @param set The value::Set to serialize.
         * @param filename The output file.
         */
        static void serializeTxtFile(
            const Set& set, const std::string& filename);

        /**
         * @brief Serialize the value::Set into a std::string buffer using the
         * text format archive of the boost::serialization library.
         * @param set The value::Set to serialize.
         * @param filename The output file.
         */
        static void serializeTxtBuffer(
            const Set& set, std::string& buffer);

        /**
         * @brief Serialize the value::Set into a binary file using the
         * binary format archive of the boost::serialization library.
         * @param set The value::Set to serialize.
         * @param filename The output file.
         */
        static void serializeBinaryFile(
            const Set& set, const std::string& filename);

        /**
         * @brief Serialize the value::Set into a std::string buffer using the
         * binary format archive of the boost::serialization library.
         * @param set The value::Set to serialize.
         * @param filename The output file.
         */
        static void serializeBinaryBuffer(
            const Set& set, std::string& buffer);

        /**
         * @brief Deserialize the text file into a value::Set.
         * @code
         * value::Set set;
         * value::Set::deserializeTxtFile(set, "file.dat");
         * @endcode
         * @param set An output parameter to fill the Set.
         * @param filename The filename.
         */
        static void deserializeTxtFile(
            Set& set, const std::string& filename);

        /**
         * @brief Deserialize the text string buffer into a value::Set.
         * @code
         * value::Set set;
         * [...] // fill the value::Set.
         * std::string buffer;
         * value::Set::serializeTxtBuffer(set, buffer);
         * [...]
         * value::Set::deserializeTxtBuffer(set, buffer);
         * @endcode
         * @param set An output parameter to fill the Set.
         * @param filename The filename.
         */
        static void deserializeTxtBuffer(
            Set& set, const std::string& buffer);

        /**
         * @brief Deserialize the binary file into a value::Set.
         * @code
         * value::Set set;
         * value::Set::deserializeBinaryFile(set, "file.dat");
         * @endcode
         * @param set An output parameter to fill the Set.
         * @param filename The filename.
         */
        static void deserializeBinaryFile(
            Set& set, const std::string& filename);

        /**
         * @brief Deserialize the string binary buffer into a value::Set.
         * @code
         * value::Set set;
         * [...] // fill the value::Set.
         * std::string buffer;
         * value::Set::serializeBinaryBuffer(set, buffer);
         * [...]
         * value::Set::deserializeBinaryBuffer(set, buffer);
         * @endcode
         * @param set An output parameter to fill the Set.
         * @param filename The filename.
         */
        static void deserializeBinaryBuffer(
            Set& set, const std::string& buffer);

    private:
        VectorValue m_value;

        /**
         * @brief Delete the Value at the specified index. Be careful, all
         * reference and iterator are invalided after this call.
         * @param i The index of the value.
         * @throw utils::ArgError if index 'i' is too big.
         */
        void del(const size_type i);

	friend class boost::serialization::access;
	template < class Archive >
	    void serialize(Archive& ar, const unsigned int /* version */)
	    {
		ar & boost::serialization::base_object < Value >(*this);
		ar & m_value;
	    }
    };

    inline const Set& toSetValue(const Value& value)
    { return value.toSet(); }

    inline const Set* toSetValue(const Value* value)
    { return value ? &value->toSet() : 0; }

    inline Set& toSetValue(Value& value)
    { return value.toSet(); }

    inline Set* toSetValue(Value* value)
    { return value ? &value->toSet() : 0; }

    inline const VectorValue& toSet(const Value& value)
    { return value.toSet().value(); }

    inline VectorValue& toSet(Value& value)
    { return value.toSet().value(); }

    inline const VectorValue& toSet(const Value* value)
    { return value::reference(value).toSet().value(); }

    inline VectorValue& toSet(Value* value)
    { return value::reference(value).toSet().value(); }

}} // namespace vle value

#endif
