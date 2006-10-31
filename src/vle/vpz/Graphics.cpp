/** 
 * @file Graphics.cpp
 * @brief 
 * @author The vle Development Team
 * @date lun, 06 fév 2006 12:01:35 +0100
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

#include <vle/vpz/Graphics.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/XML.hpp>

namespace vle { namespace vpz {
    
using namespace vle::utils;

void Graphics::init(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "GRAPHICS");

    if (not xml::exist_children(elt, "MODELS"))
        return;

    xmlpp::Element* mdls = xml::get_children(elt, "MODELS");

    m_lst.clear();

    xmlpp::Node::NodeList lst = mdls->get_children("MODEL");
    xmlpp::Node::NodeList::iterator it;
    for (it = lst.begin(); it != lst.end(); ++it) {
        Graphic g;
        g.init((xmlpp::Element*)(*it));

        addGraphic(xml::get_attribute((xmlpp::Element*)(*it), "NAME"), g);
    }
}

void Graphics::initFromModels(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "MODELS");

    m_lst.clear();

    xmlpp::Node::NodeList lst = elt->get_children("MODEL");
    xmlpp::Node::NodeList::iterator it;
    for (it = lst.begin(); it != lst.end(); ++it) {
        Graphic g;
        g.init((xmlpp::Element*)(*it));

        addGraphic(xml::get_attribute((xmlpp::Element*)(*it), "NAME"), g);
    }
}

void Graphics::write(xmlpp::Element* elt) const
{
    AssertI(elt);

    if (not m_lst.empty()) {
        xmlpp::Element* grps = elt->add_child("GRAPHICS");
        xmlpp::Element* mdls = grps->add_child("MODELS");
        std::map < std::string, Graphic >::const_iterator it;
        for (it = m_lst.begin(); it != m_lst.end(); ++it) {
            xmlpp::Element* grp = mdls->add_child("MODEL");
            grp->set_attribute("NAME", (*it).first);
            (*it).second.write(grp);
        }
    }
}

void Graphics::addGraphics(const Graphics& g)
{
    const std::map < std::string, Graphic >& lst = g.graphics();
    std::map < std::string, Graphic >::const_iterator it;

    for (it = lst.begin(); it != lst.end(); ++it) {
        addGraphic((*it).first, (*it).second);
    }
}

void Graphics::addGraphic(const std::string& name, const Graphic& g)
{
    AssertI(m_lst.find(name) == m_lst.end());
    m_lst[name] = g;
}

void Graphics::clear()
{
    m_lst.clear();
}

void Graphics::delGraphic(const std::string& name)
{
    std::map < std::string, Graphic >::iterator it = m_lst.find(name);
    if (it != m_lst.end()) {
        m_lst.erase(it);
    }
}

const Graphic& Graphics::graphic(const std::string& name) const
{
    std::map < std::string, Graphic >::const_iterator it = m_lst.find(name);
    AssertI(it != m_lst.end());

    return (*it).second;
}

}} // namespace vle vpz
