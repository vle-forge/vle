/** 
 * @file vpz/Classes.cpp
 * @brief 
 * @author The vle Development Team
 * @date lun, 27 fév 2006 17:27:56 +0100
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

#include <vle/vpz/Classes.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {
    
using namespace vle::utils;

void Classes::init(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "CLASSES");

    m_classes.clear();

    xmlpp::Node::NodeList lst = elt->get_children("CLASS");
    xmlpp::Node::NodeList::iterator it;
    for (it = lst.begin(); it != lst.end(); ++it) {
        Class c;
        c.init((xmlpp::Element*)(*it));
        addClass(xml::get_attribute((xmlpp::Element*)(*it), "NAME"), c);
    }
}

void Classes::write(xmlpp::Element* elt) const
{
    AssertI(elt);

    if (not m_classes.empty()) {
        xmlpp::Element* classes = elt->add_child("CLASSES");
        std::map < std::string, Class >::const_iterator it;
        for (it = m_classes.begin(); it != m_classes.end(); ++it) {
            xmlpp::Element* clas = classes->add_child("CLASS");
            clas->set_attribute("NAME", (*it).first);
            (*it).second.write(clas);
        }
    }
}

void Classes::addClass(const std::string& name, const Class& c)
{
    AssertI(m_classes.find(name) == m_classes.end());
    m_classes[name] = c;
}

void Classes::setClass(const std::string& name, const Class& c)
{
    AssertI(m_classes.find(name) != m_classes.end());
    m_classes[name] = c;
}

void Classes::clear()
{
    m_classes.clear();
}

void Classes::delClass(const std::string& name)
{
    std::map < std::string, Class >::iterator it = m_classes.find(name);
    if (it != m_classes.end()) {
        m_classes.erase(it);
    }
}

const Class& Classes::getClass(const std::string& name) const
{
    std::map < std::string, Class >::const_iterator it = m_classes.find(name);

    Assert(utils::InternalError, it != m_classes.end(),
           boost::format("Unknow class '%1%'\n") % name);

    return (*it).second;
}

Class& Classes::getClass(const std::string& name)
{
    std::map < std::string, Class >::iterator it = m_classes.find(name);

    Assert(utils::InternalError, it != m_classes.end(),
           boost::format("Unknow class '%1%'\n") % name);

    return (*it).second;
}

}} // namespace vle vpz
