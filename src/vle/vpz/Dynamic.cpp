/** 
 * @file vpz/Dynamic.cpp
 * @brief 
 * @author The vle Development Team
 * @date lun, 06 fév 2006 10:34:54 +0100
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

#include <vle/vpz/Dynamic.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/XML.hpp>

namespace vle { namespace vpz {
    
using namespace vle::utils;

void Dynamic::init(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "DYNAMICS");

    Glib::ustring type(xml::get_attribute(elt, "TYPE"));
    if (type == "wrapping") {
        if (xml::has_children(elt)) {
            setWrappingDynamic(xml::get_attribute(elt, "FORMALISM"),
                               xml::write_to_string(elt));
        } else {
            setWrappingDynamic(xml::get_attribute(elt, "FORMALISM"), "");
        }
    } else if (type == "mapping") {
        if (xml::has_children(elt)) {
            setMappingDynamic(xml::get_attribute(elt, "FORMALISM"),
                              xml::write_to_string(elt));
        } else {
            setMappingDynamic(xml::get_attribute(elt, "FORMALISM"), "");
        }
    } else {
        Throw(utils::InternalError, "Unknow type");
    }
}

void Dynamic::write(xmlpp::Element* elt) const
{
    AssertI(elt);
    AssertI(elt->get_name() == "MODEL");

    xmlpp::Element* dyn;

    if (m_dynamic.empty()) {
        dyn = elt->add_child("DYNAMICS");
    } else {
        xml::import_children_nodes(elt, m_dynamic);
        dyn = xml::get_children(elt, "DYNAMICS");
    }

    dyn->set_attribute("FORMALISM", m_formalism);
    dyn->set_attribute("TYPE", (m_type == Dynamic::WRAPPING) ?
                       "wrapping" : "mapping");
}

void Dynamic::setWrappingDynamic(const std::string& formalism,
                                 const std::string& dynamic)
{
    AssertI(not formalism.empty());

    m_dynamic.assign(dynamic);
    m_formalism.assign(formalism);
    m_type = Dynamic::WRAPPING;
}

void Dynamic::setMappingDynamic(const std::string& formalism,
                                const std::string& dynamic)
{
    AssertI(not formalism.empty());

    m_dynamic.assign(dynamic);
    m_formalism.assign(formalism);
    m_type = Dynamic::MAPPING;
}

}} // namespace vle vpz
