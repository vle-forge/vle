/**
 * @file vle/value/Null.hpp
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


#ifndef VLE_VALUE_NULL_HPP
#define VLE_VALUE_NULL_HPP

#include <vle/value/Value.hpp>
#include <vle/value/DllDefines.hpp>

namespace vle { namespace value {

    /**
     * @brief A null Value. This class is uses to build empty value into
     * container. Can be useful on Map, Set etc.
     */
    class VLE_VALUE_EXPORT Null : public Value
    {
    public:
        /**
         * @brief Build a Null object.
         */
        Null() {}

        /**
         * @brief Copy constructor.
         * @param value The value to copy.
         */
        Null(const Null& value) :
            Value(value)
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~Null() {}

        ///
        ////
        ///

        /**
         * @brief Build a new Null value.
         * @return A new allocated Null.
         */
        static Null* create()
        { return new Null(); }

        ///
        ////
        ///

        /**
         * @brief Clone the current Null.
         * @return A new Null.
         */
        virtual Value* clone() const
        { return new Null(); }

        /**
         * @brief Get the type of this class.
         * @return Return Value::NIL.
         */
        inline virtual Value::type getType() const
        { return Value::NIL; }

        /**
         * @brief Push the string 'NA' into the stream.
         * @param out The output stream.
         */
        virtual void writeFile(std::ostream& out) const;

        /**
         * @brief Push the string 'NA' into the stream.
         * @param out The output stream.
         */
	virtual void writeString(std::ostream& out) const;

        /**
         * @brief Push a specific string into the stream. The XML representation
         * of the Null value is:
         * @code
         * <null />
         * @endcode
         * @param out The output stream.
         */
	virtual void writeXml(std::ostream& out) const;

    private:
	friend class boost::serialization::access;
	template < class Archive >
	    void serialize(Archive& ar, const unsigned int /* version */)
	    {
		ar & boost::serialization::base_object < Value >(*this);
	    }
    };

    /**
     * @brief A functor useful to test if a value is NULL or Null. To use with
     * the std::find_if for instance.
     * @code
     * std::vector < value::Value* > vals;
     * std::vector < value::Value* >::iterator it;
     * it = std::find_if(v.begin(), v.end(), value::IsNullValue());
     * @endcode
     */
    struct VLE_VALUE_EXPORT IsNullValue
    {
        /**
         * @brief Check if the value is a Null value.
         * @param value The value to check.
         * @return Ture if the value is a Null, false otherwise.
         */
        bool operator()(const value::Value& value) const
        { return value.getType() == Value::NIL; }

        /**
         * @brief Check if the pointer value is 0 or a Null value.
         * @param value The pointer to check.
         * @return True if the value is 0 or a Null value, false otherwise.
         */
        bool operator()(const value::Value* value) const
        { return value and value->getType() == Value::NIL; }
    };

    inline const Null& toNullValue(const Value& value)
    { return value.toNull(); }

    inline const Null* toNullValue(const Value* value)
    { return value ? &value->toNull() : 0; }

    inline Null& toNullValue(Value& value)
    { return value.toNull(); }

    inline Null* toNullValue(Value* value)
    { return value ? &value->toNull() : 0; }

}} // namespace vle value

#endif
