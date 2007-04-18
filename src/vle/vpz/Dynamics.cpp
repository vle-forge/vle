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

void Dynamics::write(std::ostream& out) const
{
    if (not m_lst.empty()) {
        out << "<dynamics>\n";
            
        std::copy(m_lst.begin(), m_lst.end(),
                  std::ostream_iterator < Dynamic >(out, "\n"));

        out << "</dynamics>\n";
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
        //xmlpp::Element* dynamic = xml::get_children(
        //(xmlpp::Element*)(*it),"DYNAMICS");
        //Dynamic d;
        //d.init(dynamic);
        /* FIXME init */

        //addDynamic(
        //xml::get_attribute((xmlpp::Element*)(*it), "NAME"), d);
    }
}

void Dynamics::addDynamics(const Dynamics& dyns)
{
    const DynamicList& lst = dyns.dynamics();
    for (DynamicList::const_iterator it = lst.begin(); it != lst.end(); ++it)
        addDynamic(*it);
}

void Dynamics::addDynamic(const Dynamic& dynamic)
{
    AssertI(not exist(dynamic.name())); 
    m_lst.insert(dynamic);
}

void Dynamics::clear()
{
    m_lst.clear();
}

void Dynamics::delDynamic(const std::string& name)
{
    DynamicList::const_iterator it = findByName(name);
    m_lst.erase(it);
}

const Dynamic& Dynamics::find(const std::string& name) const
{
    DynamicList::const_iterator it = findByName(name);

    return *it;
}

bool Dynamics::exist(const std::string& name) const
{
    Dynamic compare(name);

    return m_lst.find(compare) != m_lst.end();
}

Dynamics::DynamicList::const_iterator Dynamics::findByName(
    const std::string& name) const
{
    Dynamic compare(name);

    DynamicList::const_iterator it = m_lst.find(compare);

    Assert(utils::InternalError, it != m_lst.end(),
           boost::format("Dynamic '%1%' not found in dynamics list") %
           name);

    return it;
}

}} // namespace vle vpz
