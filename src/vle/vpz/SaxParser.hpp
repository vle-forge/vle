/** 
 * @file SaxParser.hpp
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

#ifndef VLE_VPZ_SaxParser_HPP
#define VLE_VPZ_SaxParser_HPP

#include <stack>
#include <libxml++/libxml++.h>
#include <vle/vpz/Base.hpp>
#include <vle/vpz/SaxStackValue.hpp>
#include <vle/vpz/SaxStackVpz.hpp>
#include <vle/vpz/Trame.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Map.hpp>

namespace vle { namespace vpz {

    class Vpz;
    class Trame;

    class SaxParser : public xmlpp::SaxParser
    {
    public:
        SaxParser(Vpz& vpz);

        virtual ~SaxParser();

        virtual void on_start_document();

        virtual void on_end_document();

        virtual void on_start_element(const Glib::ustring& name,
                                      const AttributeList& attributes);

        virtual void on_end_element(const Glib::ustring& name);

        virtual void on_characters(const Glib::ustring& characters);

        virtual void on_comment(const Glib::ustring& text);

        virtual void on_warning(const Glib::ustring& text);

        virtual void on_error(const Glib::ustring& text);

        virtual void on_fatal_error(const Glib::ustring& text);

        virtual void on_cdata_block(const Glib::ustring& text);

        virtual void on_validity_error(const Glib::ustring& text);

        virtual void on_validity_warning(const Glib::ustring& text);

        /** 
         * @brief Return true if the SaxParser have read a value.
         * @return true if the parser have read a value, false otherwise.
         */
        inline bool isValue() const
        { return m_isValue; }

        /** 
         * @brief Return true if the SaxParser have read a vpz file.
         * @return true if the parser have read a vpz file, false otherwise.
         */
        inline bool isVpz() const
        { return m_isVPZ; }

        /** 
         * @brief Return true if the SaxParser have read a trame.
         * @return true if the parser have read a trame, false otherwise.
         */
        inline bool isTrame() const
        { return m_isTrame; }

        /** 
         * @brief Return true if the SaxParser have read an end trame.
         * @return true if the parser have read an end trame, false otherwise.
         */
        inline bool isEndTrame() const
        { return m_isEndTrame; }

        const std::vector < value::Value >& getValues() const;
        std::vector < value::Value >& getValues();

        const value::Value& getValue(const size_t pos) const;

        inline const vpz::Vpz& vpz() const
        { return m_vpz; }

        inline const TrameList& tramelist() const
        { return m_vpzstack.trame(); }

        inline TrameList& tramelist()
        { return m_vpzstack.trame(); }

    private:
        /** 
         * @brief Delete all information from Sax parser like stack, etc. Use it
         * before restart a new parse.
         */
        void clearParserState();

        /** 
         * @brief Get the last characters from internal buffer.
         * @return A characters buffer.
         */
        const Glib::ustring& lastCharactersStored() const;

        /** 
         * @brief Clear the internal buffer for last characters.
         */
        void clearLastCharactersStored();

        void addToCharacters(const Glib::ustring& characters);

        SaxStackVpz                     m_vpzstack;
        ValueStackSax                   m_valuestack;
        Glib::ustring                   m_lastCharacters;
        Glib::ustring                   m_cdata;
        Vpz&                            m_vpz;

        bool                            m_isValue;
        bool                            m_isVPZ;
        bool                            m_isTrame;

        bool                            m_isEndTrame;
    };

    /** 
     * @brief A template function de get a value of an attribute from an
     * attribute list. The value of the attribute found is casted in template by
     * the boost::lexical_cast function.
     * @param lst the list of attribute to search an attribute
     * @param name the name of the attribute to get value
     * @return the value of the attribute.
     * @throw utils::SaxParserError if the attribute was not found or if the
     * boost::lexical_cast failed.
     */
    template < typename T > T getAttribute(const AttributeList& lst,
                                           const Glib::ustring& name)
    {
        AttributeList::const_iterator it;
        it = std::find_if(lst.begin(), lst.end(),
                          xmlpp::SaxParser::AttributeHasName(name));
        Assert(utils::SaxParserError, it != lst.end(),
               (boost::format("Unknow attribute '%1%'") % name).str());

        T result;
        try {
            result = boost::lexical_cast < T >((*it).value);
        } catch(const std::exception& e) {
            Throw(utils::SaxParserError, (boost::format(
                        "Cannot convert '%1%' into desired type: %2%") %
                    name % e.what()).str());
        }
        return result;
    }

    /** 
     * @brief Search an attribute in an attribute list.
     * @param lst the list of attribute to search an attribute.
     * @param name the attribute to search in attribute list.
     * @return true if the attribute exist in attribute list.
     */
    bool existAttribute(const AttributeList& lst, const Glib::ustring& name);

}} // namespace vle vpz

#endif
