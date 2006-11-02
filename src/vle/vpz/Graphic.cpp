/** 
 * @file vpz/Graphic.cpp
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

#include <vle/vpz/Graphic.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/XML.hpp>

namespace vle { namespace vpz {
    
using namespace vle::utils;

void Graphic::init(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "MODEL");

    if (xml::exist_children(elt, "POSITION")) {
        xmlpp::Element* pos = xml::get_children(elt, "POSITION");
        setGraphic(xml::get_int_attribute(pos, "X"),
                   xml::get_int_attribute(pos, "Y"));
    }

    if (xml::exist_children(elt, "SIZE")) {
        xmlpp::Element* siz = xml::get_children(elt, "SIZE");
        setSize(xml::get_int_attribute(siz, "WIDTH"),
                xml::get_int_attribute(siz, "HEIGHT"));
    }
}

void Graphic::write(xmlpp::Element* elt) const
{
    AssertI(elt);
    AssertI(elt->get_name() == "MODEL");

    xmlpp::Element* pos = elt->add_child("POSITION");
    pos->set_attribute("X", utils::to_string(m_x));
    pos->set_attribute("Y", utils::to_string(m_y));

    if (m_width > 0 or m_height > 0) {
        xmlpp::Element* siz = elt->add_child("SIZE");
        siz->set_attribute("WIDTH", utils::to_string(m_width));
        siz->set_attribute("HEIGHT", utils::to_string(m_height));
    }
}

void Graphic::setGraphic(int x, int y)
{
    m_x = x;
    m_y = y;
}

void Graphic::setSize(int width, int height)
{
    m_width = width;
    m_height = height;
}

}} // namespace vle vpz
