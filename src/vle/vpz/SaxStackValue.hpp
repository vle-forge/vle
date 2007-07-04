/** 
 * @file SaxStackValue.hpp
 * @brief A class to store Value into a stack to be use by SaxVPZ.
 * @author The vle Development Team
 * @date dim, 25 fév 2007 20:18:19 +0100
 */

/*
 * Copyright (C) 2007 - The vle Development Team
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
        void push_integer();

        void push_boolean();

        void push_string();

        void push_double();

        void push_map();

        void push_map_key(const Glib::ustring& key);

        void push_set();

        void push_tuple();

        void push_table(const size_t width, const size_t height);

        void push_xml();

        void pop_value();

        const value::Value& top_value();

        void push_on_vector_value(const value::Value& val);

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

        inline void push_result(const value::Value& val)
        { m_result.push_back(val); }

        inline const value::Value& get_result(size_t i) const
        {
            Assert(utils::SaxParserError, m_result.size() >= i,
                   (boost::format("Get result value with to big index %1%.") %
                    i));

            return m_result[i];
        }

        inline const value::Value& get_last_result() const
        {
            Assert(utils::SaxParserError, not m_result.empty(),
                   "Get last result value with empty result vector");

            return m_result[m_result.size() - 1];
        }

        const std::vector < value::Value >& get_results() const
        { return m_result; }

        std::vector < value::Value >& get_results()
        { return m_result; }

    private:
        /** 
         * @brief Test if top of values stack are a composite like map, set,
         * coordinate or direction.
         * 
         * @return 
         */
        bool is_composite_parent() const;

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
