/** 
 * @file vpz/Dynamics.cpp
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

#include <vle/vpz/Dynamics.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/XML.hpp>

namespace vle { namespace vpz {

using namespace vle::utils;

void Dynamics::init(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "DYNAMICS");

    if (not xml::exist_children(elt, "MODELS"))
        return;

    xmlpp::Element* models = xml::get_children(elt, "MODELS");
    m_lst.clear();

    xmlpp::Node::NodeList lst = models->get_children("MODEL");
    xmlpp::Node::NodeList::iterator it;
    for (it = lst.begin(); it != lst.end(); ++it) {
        xmlpp::Element* dynamic = xml::get_children(
            (xmlpp::Element*)(*it),"DYNAMICS");
        Dynamic d;
        d.init(dynamic);

        addDynamic(
            xml::get_attribute((xmlpp::Element*)(*it), "NAME"), d);
    }
}

void Dynamics::write(xmlpp::Element* elt) const
{
    AssertI(elt);

    if (not m_lst.empty()) {
        xmlpp::Element* dyn = elt->add_child("DYNAMICS");
        xmlpp::Element* models = dyn->add_child("MODELS");
        std::map < std::string, Dynamic >::const_iterator it;
        for (it = m_lst.begin(); it != m_lst.end(); ++it) {
            xmlpp::Element* model = models->add_child("MODEL");
            model->set_attribute("NAME", (*it).first);
            (*it).second.write(model);
        }
    }
}

void Dynamics::initFromModels(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "MODELS");

    m_lst.clear();

    xmlpp::Node::NodeList lst = elt->get_children("MODEL");
    xmlpp::Node::NodeList::iterator it;
    for (it = lst.begin(); it != lst.end(); ++it) {
        xmlpp::Element* dynamic = xml::get_children(
            (xmlpp::Element*)(*it),"DYNAMICS");
        Dynamic d;
        d.init(dynamic);

        addDynamic(
            xml::get_attribute((xmlpp::Element*)(*it), "NAME"), d);
    }
}

void Dynamics::addDynamics(const Dynamics& dyns)
{
    const std::map < std::string, Dynamic >& lst = dyns.dynamics();
    std::map < std::string, Dynamic >::const_iterator it;

    for (it = lst.begin(); it != lst.end(); ++it) {
        addDynamic((*it).first, (*it).second);
    }
}

void Dynamics::addDynamic(const std::string& name,
                          const Dynamic& d)
{
    AssertI(not name.empty());
    AssertI(m_lst.find(name) == m_lst.end());
    m_lst[name] = d;
}

void Dynamics::clear()
{
    m_lst.clear();
}

void Dynamics::delDynamic(const std::string& name)
{
    std::map < std::string, Dynamic >::iterator it = m_lst.find(name);
    if (it != m_lst.end()) {
        m_lst.erase(it);
    }
}

const Dynamic& Dynamics::find(const std::string& name) const
{
    std::map < std::string, Dynamic >::const_iterator it =
        m_lst.find(name);

    Assert(utils::InternalError, it != m_lst.end(),
           boost::format("Dynamic '%1%' not found in dynamics list") %
           name);

    return (*it).second;
}

Dynamic& Dynamics::find(const std::string& name)
{
    std::map < std::string, Dynamic >::iterator it = m_lst.find(name);

    Assert(utils::InternalError, it != m_lst.end(),
           boost::format("Dynamic '%1%' not found in dynamics list") %
           name);

    return (*it).second;
}

bool Dynamics::exist(const std::string& name) const
{
    return m_lst.find(name) != m_lst.end();
}

}} // namespace vle vpz
