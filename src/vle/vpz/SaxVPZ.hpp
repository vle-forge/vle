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

namespace vle { namespace vpz {

    class VLEStack
    {
    public:
        VLEStack();

        ~VLEStack();

	void push(vpz::Base* element);

	void pop();

	vpz::Base* top() const;

	bool empty() const;

    private:
        std::stack < vpz::Base* > m_stack;
    };

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
      * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    class VLESaxParser : public xmlpp::SaxParser
    {
    public:
	VLESaxParser();

	virtual ~VLESaxParser();

	virtual void on_start_document();

	virtual void on_end_document();

        virtual void on_start_element(const Glib::ustring& name,
            const xmlpp::SaxParser::AttributeList& attributes);

        virtual void on_end_element(const Glib::ustring& name);

        virtual void on_characters(const Glib::ustring& characters);

        virtual void on_comment(const Glib::ustring& text);

        virtual void on_warning(const Glib::ustring& text);

        virtual void on_error(const Glib::ustring& text);

        virtual void on_fatal_error(const Glib::ustring& text);

        virtual void on_cdata_block(const Glib::ustring& text);

        virtual void on_internal_subset(const Glib::ustring& name,
                                        const Glib::ustring& publicId,
                                        const Glib::ustring& systemId);

    private:
        VLEStack    m_stack;
    };

}} // namespace vle vpz

#endif
