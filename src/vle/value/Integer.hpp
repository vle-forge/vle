/**
 * @file vle/value/Integer.hpp
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


#ifndef VLE_VALUE_INTEGER_HPP
#define VLE_VALUE_INTEGER_HPP

#include <vle/value/Value.hpp>
#include <boost/cast.hpp>
#include <boost/serialization/export.hpp>

namespace vle { namespace value {

    /**
     * @brief Integer Value encapsulate a C++ 'long' type into a class to
     * perform serialization, pool of memory etc.
     */
    class Integer : public Value
    {
    public:
        /**
         * @brief Build an Integer object with a default value as zero.
         */
        Integer() :
            m_value(0)
        {}

        /**
         * @brief Build an Integer object with a specified value.
         * @param value The value to copy.
         */
        Integer(long value) :
            m_value(value)
        {}

        /**
         * @brief Copy constructor.
         * @param value The value to copy.
         */
        Integer(const Integer& value) :
            Value(value),
            m_value(value.m_value)
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~Integer() {}

        ///
        ////
        ///

        /**
         * @brief Build a Integer using the boost::pool memory management.
         * @param value the value of the Integer.
         * @return A new Integer allocated from the boost::pool.
         */
        static Integer* create(long value = 0)
        { return new Integer(value); }

        ///
        ////
        ///

        /**
         * @brief Clone the current Integer with the same value.
         * @return A new boost::pool allocated value::Value.
         */
        virtual Value* clone() const
        { return new Integer(m_value); }

        /**
         * @brief Get the type of this class.
         * @return Return Value::INTEGER.
         */
        virtual Value::type getType() const
        { return Value::INTEGER; }

        /**
         * @brief Push the long in the stream.
         * @param out The output stream.
         */
        virtual void writeFile(std::ostream& out) const;

        /**
         * @brief Push the long in the stream.
         * @param out The output stream.
         */
	virtual void writeString(std::ostream& out) const;

        /**
         * @brief Push the long in the stream. The string pushed in the stream:
         * @code
         * <integer>-12345</integer>
         * <integer><987654321/integer>
         * @endcode
         * @param out The output stream.
         */
	virtual void writeXml(std::ostream& out) const;

        ///
        ////
        ///

        /**
         * @brief Get the numeric int value from the long. Be carrefull, the int
         * can represent lesser integer than long.
         * @return The int representation of the long.
         * @throw boost::numeric_cast_error if the long is greather than the
         * int.
         */
        inline int intValue() const
        { return boost::numeric_cast < int >(m_value); }

        /**
         * @brief Get the value of the long.
         * @return An integer.
         */
        inline long value() const
        { return m_value; }

        /**
         * @brief Get a reference to the encapsulated long.
         * @return A reference to the encapsulated long.
         */
        inline long& value()
        { return m_value; }

        /**
         * @brief Assign a value to the encapsulated long.
         * @param value The value to set.
         */
        inline void set(long value)
        { m_value = value; }

    private:
        long m_value;

	friend class boost::serialization::access;
	template < class Archive >
	    void serialize(Archive& ar, const unsigned int /* version */)
	    {
		ar & boost::serialization::base_object < Value >(*this);
		ar & m_value;
	    }
    };

    inline const Integer& toIntegerValue(const Value& value)
    { return value.toInteger(); }

    inline const Integer* toIntegerValue(const Value* value)
    { return value ? &value->toInteger() : 0; }

    inline Integer& toIntegerValue(Value& value)
    { return value.toInteger(); }

    inline Integer* toIntegerValue(Value* value)
    { return value ? &value->toInteger() : 0; }

    inline int toInteger(const Value& value)
    { return value.toInteger().intValue(); }

    inline long toLong(const Value& value)
    { return value.toInteger().value(); }

    inline long toLong(Value& value)
    { return value.toInteger().value(); }

    inline int toInteger(const Value* value)
    { return value::reference(value).toInteger().intValue(); }

    inline long toLong(const Value* value)
    { return value::reference(value).toInteger().value(); }

    inline long toLong(Value* value)
    { return value::reference(value).toInteger().value(); }

}} // namespace vle value

#endif
