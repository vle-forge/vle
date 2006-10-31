/** 
 * @file Class.cpp
 * @brief 
 * @author The vle Development Team
 * @date lun, 27 fév 2006 16:23:12 +0100
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

#include <vle/vpz/Class.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {

using namespace vle::utils;

void Class::init(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "CLASS");

    xmlpp::Element* str = xml::get_children(elt, "STRUCTURES");
    xmlpp::Element* dyn = xml::get_children(elt, "DYNAMICS");

    m_model.init(str);
    m_dynamics.init(dyn);

    if (xml::exist_children(elt, "GRAPHICS")) {
        xmlpp::Element* grp = xml::get_children(elt, "GRAPHICS");
        m_graphics.init(grp);
    }

    if (xml::exist_children(elt, "EXPERIMENTS")) {
        xmlpp::Element* exp = xml::get_children(elt, "EXPERIMENTS");
        m_experiement.init(exp);
    }
}

void Class::write(xmlpp::Element* elt) const
{
    AssertI(elt);
    AssertI(elt->get_name() == "CLASS");

    m_model.write(elt);
    m_dynamics.write(elt);
    m_graphics.write(elt);
    m_experiement.write(elt);
}
    
}} // namespace vle vpz
