/** 
 * @file SaxVPZ.hpp
 * @brief A class to parse VPZ XML with Sax parser.
 * @author The vle Development Team
 * @date jeu, 14 déc 2006 10:33:27 +0100
 */

/*
 * Copyright (C) 2006 - The vle Development Team
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

#ifndef VLE_VPZ_SAXVPZ_HPP
#define VLE_VPZ_SAXVPZ_HPP

#include <stack>
#include <libxml++/libxml++.h>
#include <vle/vpz/Base.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Map.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {

    class VpzStackSax
    {
    public:
        VpzStackSax()
        { }

        ~VpzStackSax()
        { }

        void push_vpz();
        void push_structure();
        void push_model(const xmlpp::SaxParser::AttributeList& att);
        void push_port(const xmlpp::SaxParser::AttributeList& att);
        void push_in();
        void push_out();
        void push_init();
        void push_state();
        void push_submodels();
        void push_dynamics();
        void push_dynamic(const xmlpp::SaxParser::AttributeList& att);

    private:
        std::stack < vpz::Base* >       m_stack;
    };

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

        void pop_value();

        const value::Value& top_value();

        inline void push_on_vector_value(const value::Value& val);

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


    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    class VLESaxParser : public xmlpp::SaxParser
    {
    public:
        VLESaxParser();

        virtual ~VLESaxParser();

        /* xml */

        virtual void on_start_document();

        virtual void on_end_document();

        virtual void on_start_element(
            const Glib::ustring& name,
            const xmlpp::SaxParser::AttributeList& attributes);

        virtual void on_end_element(const Glib::ustring& name);

        virtual void on_characters(const Glib::ustring& characters);

        virtual void on_comment(const Glib::ustring& text);

        virtual void on_warning(const Glib::ustring& text);

        virtual void on_error(const Glib::ustring& text);

        virtual void on_fatal_error(const Glib::ustring& text);

        virtual void on_cdata_block(const Glib::ustring& text);

        //virtual void on_internal_subset(
        //const Glib::ustring& name,
        //const Glib::ustring& publicId,
        //const Glib::ustring& systemId);

        inline bool is_value() const
        { return m_isValue; }

        inline bool is_vpz() const
        { return m_isVPZ; }

        inline bool is_trame() const
        { return m_isTrame; }

        const std::vector < value::Value >& get_values() const;

        const value::Value& get_value(const size_t pos) const;

    private:
        /** 
         * @brief Delete all information from Sax parser like stack, etc. Use it
         * before restart a new parse.
         */
        void clear_parser_state();

        /** 
         * @brief Get the last characters from internal buffer.
         * 
         * @return A characters buffer.
         */
        const Glib::ustring& lastCharactersStored() const;

        /** 
         * @brief Clear the internal buffer for last characters.
         */
        void clearLastCharactersStored();

        void addToCharacters(const Glib::ustring& characters);

        VpzStackSax                     m_vpzstack;
        ValueStackSax                   m_valuestack;
        Glib::ustring                   m_lastCharacters;

        bool                            m_isValue;
        bool                            m_isVPZ;
        bool                            m_isTrame;
    };

}} // namespace vle vpz

#endif
