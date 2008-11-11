/**
 * @file vle/value/Value.hpp
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


#ifndef VLE_VALUE_VALUE_HPP
#define VLE_VALUE_VALUE_HPP

#include <vle/utils/Exception.hpp>
#include <vle/utils/Pool.hpp>
#include <ostream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>

namespace vle { namespace value {

    class Value;
    class Boolean;
    class Integer;
    class Double;
    class String;
    class Set;
    class Map;
    class Tuple;
    class Table;
    class Xml;
    class Null;
    class Matrix;

    /**
     * @brief A thread-safe singleton to manage a set of boost::pools defined in
     * the utils::Pools class.
     */
    class Pools
    {
    public:
        /**
         * @brief Thread-Safe access to the Pools singleton.
         * @return A reference to the boost::pools.
         */
        static utils::Pools < Value >& pools();

        /**
         * @brief Initialize the singleton.
         */
        static void init()
        { pools(); }

        /**
         * @brief Delete the singleton.
         */
        static void kill()
        { delete m_pool; m_pool = 0; }

    private:
        /**
         * @brief The singleton object.
         */
        static Pools* m_pool;

        /**
         * @brief The set of boost::pool for Value class.
         */
        utils::Pools < Value > m_pools;

        ///
        ////
        ///

        /**
         * @brief Build a new utils::Pools with a default size defined to the
         * max size of the Value (Boolean, Integer, Map, Matrix, Null, Set,
         * String, Table, Tuple or Xml).
         */
        Pools();

        /**
         * @brief Private copy constructor.
         * @param other
         */
        Pools(const Pools& other);

        /**
         * @brief Private assign operator.
         * @param other
         * @return
         */
        Pools& operator=(const Pools& /* other */) { return *this; }
    };

    /**
     * @brief Virtual class to assign Value into Event object.
     */
    class Value
    {
    public:
        enum type { BOOLEAN, INTEGER, DOUBLE, STRING, SET, MAP, TUPLE, TABLE,
            XMLTYPE, NIL, MATRIX };

        /**
         * @brief Override the new operator to use the boost::pool allocator.
         * See the class Pools.
         * @param size The size of the object to allocate.
         * @return A pointer to the newly area.
         */
        inline static void* operator new(size_t size)
        { return Pools::pools().allocate(size); }

        /**
         * @brief Override the delete operator to use the boost::pool allocator.
         * See the class Pools.
         * @param deletable The pointer to delete.
         * @param size The size of the memory.
         */
        inline static void operator delete(void* deletable, size_t size)
        { Pools::pools().deallocate(deletable, size); }

        ///
        ////
        ///

	/**
	 * @brief Default constructor.
	 * @code
         * Value* val = value::Integer::create(13);
         * Value* val2 = new value::Integer(13);
	 * @endcode
	 */
	Value() {}

        /**
         * @brief Copy constructor. In subclass, all datas are cloned.
         * @param value The value to clone;
         */
        Value(const Value& /* value */) {}

        /**
	 * @brief Nothing to delete.
	 */
	virtual ~Value() {}

	/**
	 * @brief Pure virtual clone function.
	 * @return Clone of instantiate object.
	 */
	virtual Value* clone() const = 0;

	/**
	 * @brief Transform value into a simple std::string for text file.
	 * @return std::string representation of Value.
	 */
	virtual void writeFile(std::ostream& out) const = 0;

	/**
	 * @brief Transform value into a simple std::string.
	 * @return std::string representation of Value.
	 */
	virtual void writeString(std::ostream& out) const = 0;

	/**
	 * @brief Transform value into XML structure.
	 * @return std::string representation of XML structure of Value.
	 */
	virtual void writeXml(std::ostream& out) const = 0;

	/**
	 * @brief Return the type of value. The type is one of the 'type'
	 * enumeration ie. BOOL, INTEGER, DOUBLE, STRING, SET, MAP.
	 * @return the type of value object.
	 */
	virtual Value::type getType() const = 0;

        /**
         * @brief Build a file representation of this class. This function
         * call the virtual function Value::writeString to build the string.
         * @return A string representation of this class.
         */
        std::string writeToFile() const;

        /**
         * @brief Build a string representation of this class. This function
         * call the virtual function Value::writeString to build the string.
         * @return A string representation of this class.
         */
        std::string writeToString() const;

        /**
         * @brief Build an xml representation of this class. This function
         * call the virtual function Value::writeString to build the string.
         * @return A string representation of this class.
         */
        std::string writeToXml() const;

	inline bool isInteger() const
	{ return getType() == Value::INTEGER; }

	inline bool isBoolean() const
	{ return getType() == Value::BOOLEAN; }

	inline bool isDouble() const
	{ return getType() == Value::DOUBLE; }

	inline bool isString() const
	{ return getType() == Value::STRING; }

	inline bool isSet() const
	{ return getType() == Value::SET; }

	inline bool isMap() const
	{ return getType() == Value::MAP; }

	inline bool isTuple() const
	{ return getType() == Value::TUPLE; }

	inline bool isTable() const
	{ return getType() == Value::TABLE; }

	inline bool isXml() const
	{ return getType() == Value::XMLTYPE; }

	inline bool isNull() const
	{ return getType() == Value::NIL; }

	inline bool isMatrix() const
	{ return getType() == Value::MATRIX; }

        const Boolean& toBoolean() const;
        const Integer& toInteger() const;
        const Double& toDouble() const;
        const String& toString() const;
        const Set& toSet() const;
        const Map& toMap() const;
        const Tuple& toTuple() const;
        const Table& toTable() const;
        const Xml& toXml() const;
        const Null& toNull() const;
        const Matrix& toMatrix() const;

        /**
         * @brief Check if the Value is a composite value, ie., a Map, a Set or
         * a Matrix.
         * @param val The Value to check.
         * @return True if the Value is a Map, a Set or a Matrix.
         */
        static bool isComposite(const Value* val);

        Boolean& toBoolean();
        Integer& toInteger();
        Double& toDouble();
        String& toString();
        Set& toSet();
        Map& toMap();
        Tuple& toTuple();
        Table& toTable();
        Xml& toXml();
        Null& toNull();
        Matrix& toMatrix();

        /**
         * @brief Stream operator for the value classes. This operator call the
         * value::Value::writeString function used, principally to debug or show
         * the values in console, gui widgets etc.
         * @param out the output stream where data are written.
         * @param obj the sub class of value::Value to write.
         * @return the std::stream.
         */
        friend std::ostream& operator<<(std::ostream& out, const Value& obj)
        { obj.writeString(out); return out; }

	/**
	 * @brief Boost serialization operator to permit a text, xml or binary
	 * stream of the value value.
	 * @param ar The archive where writting the values.
	 * @param version The version of the serialization.
	 */
	friend class boost::serialization::access;
	template < class Archive >
	    void serialize(Archive& /* ar */, const unsigned int /* version */)
	    {}

    private:
        Value& operator=(const Value& /* value */) { return *this; }
    };

    inline utils::Pools < Value >& Pools::pools()
    {
        if (not m_pool) {
            m_pool = new Pools();
        }
        return m_pool->m_pools;
    }

    inline const Value& reference(const Value* value)
    {
        if (not value) throw utils::ArgError("Null value");
        return *value;
    }

    inline Value& reference(Value* value)
    {
        if (not value) throw utils::ArgError("Null value");
        return *value;
    }

    inline bool Value::isComposite(const Value* val)
    {
        switch (val->getType()) {
        case Value::BOOLEAN:
        case Value::INTEGER:
        case Value::DOUBLE:
        case Value::NIL:
        case Value::STRING:
        case Value::TUPLE:
        case Value::TABLE:
        case Value::XMLTYPE:
            return false;
        default:
            return true;
        }
    }

    ///
    ////
    ///

    /**
     * @brief A functor to find composite Value, ie., values of type Map, Set or
     * Matrix. To use with std::find_if for instance:
     * @code
     * iterator it = std::find_if(begin(), end(), IsComposite());
     * @endcode
     */
    struct IsComposite
    {
        bool operator()(const Value& val) const
        { return Value::isComposite(&val); }

        bool operator()(const Value* val) const
        { return Value::isComposite(val); }
    };

    /**
     * @brief A functor to help to the clone of value to use with
     * std::transform for instance
     * @code
     * std::transform(vec.begin(), vec.end(), out.begin(), CloneValue());
     * @endcode
     */
    struct CloneValue
    {
	Value* operator()(const Value& val) const
	{ return val.clone(); }

	Value* operator()(const Value* val) const
	{ return (not val) ? 0 : val->clone(); }
    };

    /**
     * @brief Initialize the Pools singleton.
     */
    void init();

    /**
     * @brief Kill the Pools singleton.
     */
    void finalize();

}} // namespace vle

/*
 * Macro used to define the Value class as an abstract class for the
 * boost::serialization library.
 */
BOOST_IS_ABSTRACT(vle::value::Value)

#endif
