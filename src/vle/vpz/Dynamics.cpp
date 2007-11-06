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
    if (not empty()) {
        out << "<dynamics>\n";

        for (const_iterator it = begin(); it != end(); ++it)
            out << it->second;

        out << "</dynamics>\n";
    }
}

void Dynamics::initFromModels(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "MODELS");

    clear();

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

void Dynamics::add(const Dynamics& dyns)
{
    for (const_iterator it = dyns.begin(); it != dyns.end(); ++it)
        add(it->second);
}

Dynamic& Dynamics::add(const Dynamic& dynamic)
{
    const_iterator it = find(dynamic.name());
    Assert(utils::SaxParserError, it == end(),
           (boost::format("The dynamics %1% already exist") % dynamic.name()));

    return (*insert(std::make_pair < std::string, Dynamic >(
                dynamic.name(), dynamic)).first).second;
}

void Dynamics::del(const std::string& name)
{
    erase(name);
}

const Dynamic& Dynamics::get(const std::string& name) const
{
    const_iterator it = find(name);
    Assert(utils::SaxParserError, it != end(),
           (boost::format("The dynamics %1% does not exist") % name));
    return it->second;
}

Dynamic& Dynamics::get(const std::string& name)
{
    iterator it = find(name);
    Assert(utils::SaxParserError, it != end(),
           (boost::format("The dynamics %1% does not exist") % name));

    return it->second;
}

bool Dynamics::exist(const std::string& name) const
{
    return find(name) != end();
}

void Dynamics::cleanNoPermanent()
{
    iterator previous = begin();
    iterator it = begin();
    
    while (it != end()) {
        if (not it->second.isPermanent()) {
            if (it == previous) {
                erase(it);
                previous = begin();
                it = begin();
            } else {
                erase(it);
                it = previous;
            }
        } else {
            ++previous;
            ++it;
        }
    }
}

}} // namespace vle vpz
