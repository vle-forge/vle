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

namespace vle { namespace vpz {

    class VLEStack
    {
    public:
        VLEStack()
        { }

        ~VLEStack()
        { }

        void push(vpz::Base* element);

        void pop();

        vpz::Base* top() const;

        bool empty() const;

    private:
        std::stack < vpz::Base* > m_stack;
    };
    
    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    class ValueStack
    {
    public:
        ValueStack()
        { }

        ~ValueStack()
        { }

        void push(const value::Value& val);

        void pop();

        value::Value& top();

        bool empty() const;

        value::Value clear();

    private:
        std::stack < value::Value > m_stack;
    };

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    class VLESaxParser : public xmlpp::SaxParser
    {
    public:
        VLESaxParser();

        virtual ~VLESaxParser();

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

        inline value::Value get_value_stack()
        { return m_value.clear(); }

        inline bool is_value() const
        { return m_isValue; }

        inline bool is_vpz() const
        { return m_isVPZ; }

    private:
        void startValue(value::Value val);

        /** 
         * @brief Add a value into the VLEStack and check if no error is found.
         * The boolean, integer, double and string values parent are map, set,
         * experimental factor or empty VLEStack. Map and Set parent are only
         * VLEStack or experimental factor.
         * 
         * @param val the value to push
         *
         * @throw SaxParserError if Value inserted with a bad parent.
         */
        void addValue(value::Value val);

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
        
        /** 
         * @brief Store the characters into internal buffer.
         * @param characters buffer to store.
         * @throw SaxParserError if buffer is not empty.
         */
        void storeCharacters(const Glib::ustring& characters);

        VLEStack        m_stack;
        ValueStack      m_value;
        Glib::ustring   m_lastCharacters;

        bool            m_isValue;
        bool            m_isVPZ;
    };

}} // namespace vle vpz

#endif
