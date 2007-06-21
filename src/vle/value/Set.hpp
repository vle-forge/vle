/**
 * @file value/Set.hpp
 * @author The VLE Development Team.
 * @brief Set Value a container to Value class.
 */

/*
 * Copyright (c) 2004, 2005 The vle Development Team
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

#ifndef UTILS_VALUE_SET_HPP
#define UTILS_VALUE_SET_HPP

#include <vle/value/Value.hpp>
#include <vector>

namespace vle { namespace value {

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
        typedef std::vector < Value > VectorValue;
        typedef std::vector < Value >::iterator VectorValueIt;
        typedef std::vector < Value >::const_iterator VectorValueConstIt;

        virtual ~SetFactory()
        { }

        static Set create();
            
        virtual Value clone() const;

        virtual ValueBase::type getType() const
        { return ValueBase::SET; }

        /**
         * Add a value into the set. Be carrefull, the data are not clone, the
         * use the pointer. Don't delete buffer.
         *
         * @param value the Value to add.
         */
        void addValue(Value value);

        inline VectorValue& getValue()
        { return m_value; }

        /** 
         * @brief Get the first constant iterator from Vector.
         * 
         * @return the first iterator.
         */
        inline VectorValueIt begin()
        { return m_value.begin(); }

        /** 
         * @brief Get the first iterator from Vector.
         * 
         * @return the first iterator.
         */
        inline VectorValueConstIt begin() const
        { return m_value.begin(); }

        /** 
         * @brief Get the last iterator from Vector.
         * 
         * @return the last iterator.
         */
        inline VectorValueIt end()
        { return m_value.end(); }

        /** 
         * @brief Get the last constant iterator from Vector.
         * 
         * @return the last iterator.
         */
        inline VectorValueConstIt end() const
        { return m_value.end(); }

        inline const VectorValue& getValue() const
        { return m_value; }

        inline const Value& getValue(const size_t i) const
        { return m_value.at(i); }

        inline size_t size() const
        { return m_value.size(); }

        inline void clear()
        { return m_value.clear(); }

        virtual std::string toFile() const;

        virtual std::string toString() const;

        virtual std::string toXML() const;

    private:
        VectorValue m_value;
    };

}} // namespace vle value
#endif
