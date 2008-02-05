/**
 * @file src/vle/vpz/SaxStackValue.hpp
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




#ifndef VLE_SAX_STACK_VALUE_HPP
#define VLE_SAX_STACK_VALUE_HPP

#include <libxml++/libxml++.h>
#include <stack>
#include <vle/value/Value.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/Table.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {

class ValueStackSax
{
    public:
        void pushInteger();

        void pushBoolean();

        void pushString();

        void pushDouble();

        void pushMap();

        void pushMapKey(const Glib::ustring& key);

        void pushSet();

        void pushTuple();

        void pushTable(const size_t width, const size_t height);

        void pushXml();

        void pushNull();

        void popValue();

        const value::Value& topValue();

        void pushOnVectorValue(const value::Value& val);

        /** 
         * @brief Pop the current head. If stack is empty, do nothing.
         */
        inline void pop()
        { m_valuestack.pop(); }

        /** 
         * @brief Return true if this sax parser stack is empty.
         * 
         * @return true if empty, false otherwise.
         */
        inline bool empty() const
        { return m_valuestack.empty(); }

        /** 
         * @brief Clear stack value and result stack value.
         */
        void clear();

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        inline void pushResult(const value::Value& val)
        { m_result.push_back(val); }

        inline const value::Value& getResult(size_t i) const
        {
            Assert(utils::SaxParserError, m_result.size() >= i,
                   (boost::format("Get result value with to big index %1%.") %
                    i));

            return m_result[i];
        }

        inline const value::Value& getLastResult() const
        {
            Assert(utils::SaxParserError, not m_result.empty(),
                   "Get last result value with empty result vector");

            return m_result[m_result.size() - 1];
        }

        const std::vector < value::Value >& getResults() const
        { return m_result; }

        std::vector < value::Value >& getResults()
        { return m_result; }

    private:
        /** 
         * @brief Test if top of values stack are a composite like map, set,
         * coordinate or direction.
         * 
         * @return 
         */
        bool isCompositeParent() const;

        /** 
         * @brief Store the value stack, usefull for composite value, set, map,
         * etc.
         */
        std::stack < value::Value >  m_valuestack;

        /** 
         * @brief Store result of Values parsing from trame, simple value,
         * factor.
         */
        std::vector < value::Value > m_result; 

        /** 
         * @brief Last map key read.
         */
        Glib::ustring                m_lastkey;
    };


}} // namespace vle vpz

#endif
