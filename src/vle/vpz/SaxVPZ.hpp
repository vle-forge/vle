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
#include <vle/vpz/SaxStackValue.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Map.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {

    class Vpz;

    class VpzStackSax
    {
    public:
        VpzStackSax() :
            m_vpz(0)
        { }

        ~VpzStackSax() { }

        vpz::Vpz* push_vpz(const std::string& author, float version,
                      const std::string& date);
        void push_structure();
        void push_model(const xmlpp::SaxParser::AttributeList& att);
        void push_port(const xmlpp::SaxParser::AttributeList& att);
        void push_porttype(const Glib::ustring& name);
        void push_submodels();
        void push_connections();
        void push_connection(const xmlpp::SaxParser::AttributeList& att);
        void push_origin(const xmlpp::SaxParser::AttributeList& att);
        void push_destination(const xmlpp::SaxParser::AttributeList& att);
        void build_connection();
        void push_dynamics();
        void push_dynamic(const xmlpp::SaxParser::AttributeList& att);
        vpz::Base* pop();

        vpz::Vpz& vpz();

    private:
        std::stack < vpz::Base* >       m_stack;
        vpz::Vpz*                       m_vpz;
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

        const vpz::Vpz& vpz() const
        {
            Assert(utils::SaxParserError, m_vpz, "No VPZ was read");
            return *m_vpz;
        }

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
        vpz::Vpz*                       m_vpz;

        bool                            m_isValue;
        bool                            m_isVPZ;
        bool                            m_isTrame;
    };
    
    template < typename T > T
        get_attribute(const xmlpp::SaxParser::AttributeList& lst,
                      const Glib::ustring& name)
        {
            xmlpp::SaxParser::AttributeList::const_iterator it;
            it = std::find_if(lst.begin(), lst.end(),
                              xmlpp::SaxParser::AttributeHasName(name));
            Assert(utils::SaxParserError, it != lst.end(),
                   (boost::format("Unknow attribute '%1%'") % name).str());

            T result;
            try {
                result = boost::lexical_cast < T >((*it).value);
            } catch(const std::exception& e) {
                Throw(utils::SaxParserError, (boost::format(
                            "Cannot convert '%1%' into desired type") %
                        name).str());
            }
            return result;
        }

    bool exist_attribute(const xmlpp::SaxParser::AttributeList& lst,
                         const Glib::ustring& name);

}} // namespace vle vpz

#endif
