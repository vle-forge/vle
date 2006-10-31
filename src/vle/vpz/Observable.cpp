/** 
 * @file Observable.cpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 31 jan 2006 17:30:53 +0100
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

#include <vle/vpz/Observable.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {

using namespace vle::utils;

Observable::Observable()
{
}

Observable::~Observable()
{
}

void Observable::init(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "OBSERVABLE");

    if (xml::exist_attribute(elt, "GROUP")) {
        setObservable(xml::get_attribute(elt, "MODEL_NAME"),
                      xml::get_attribute(elt, "PORT_NAME"),
                      xml::get_attribute(elt, "GROUP"),
                      xml::get_int_attribute(elt, "INDEX"));
    } else {
        setObservable(xml::get_attribute(elt, "MODEL_NAME"),
                      xml::get_attribute(elt, "PORT_NAME"));
    }
}

void Observable::write(xmlpp::Element* elt) const
{
    xmlpp::Element* observable = elt->add_child("OBSERVABLE");
    observable->set_attribute("MODEL_NAME", m_modelname);
    observable->set_attribute("PORT_NAME", m_portname);

    if (not m_group.empty()) {
        observable->set_attribute("GROUP", m_group);
        observable->set_attribute("INDEX", utils::to_string(m_index));
    }
} 

bool Observable::operator==(const Observable& o) const
{
    return m_modelname == o.modelname() and
        m_portname == o.portname() and
        m_group == o.group() and
        m_index == o.index();
}

void Observable::setObservable(const std::string& modelname,
                               const std::string& portname,
                               const std::string& group,
                               int index)
{
    AssertI(not modelname.empty() and not portname.empty());

    m_modelname.assign(modelname);
    m_portname.assign(portname);

    m_group.assign(group);
    m_index = index;
}

}} // namespace vle vpz
