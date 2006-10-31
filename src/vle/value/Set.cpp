/**
 * @file Set.cpp
 * @author The VLE Development Team.
 * @brief Set Value a container to Value class.
 */

/*
 * Copyright (c) 2004, 2005 The vle Development Team
 *
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <vle/value/Set.hpp>

namespace vle { namespace value {

Set::Set(xmlpp::Element* root)
{
    xmlpp::Node::NodeList lst = root->get_children();
    for (xmlpp::Node::NodeList::iterator it = lst.begin();
         it != lst.end(); ++it) {
        if (vle::utils::xml::is_element(*it)) {
            addValue(Value::getValue((xmlpp::Element*)*it));
        }
    }
}

Set::~Set()
{
    for (std::vector < Value* >::iterator it = m_value.begin();
	 it != m_value.end(); ++it)
	delete *it;
}

void Set::addValue(Value* p_value)
{
    m_value.push_back(p_value);
}

Value* Set::clone() const
{
    Set* r = new Set();
    for (std::vector<Value*>::const_iterator it = m_value.begin();
	 it != m_value.end(); ++it)
	r->addValue((*it)->clone());

    return r;
}

std::string Set::toFile() const
{
    std::string s;
    std::vector < Value* >::const_iterator it = m_value.begin();

    while (it != m_value.end()) {
	s += (*it)->toFile();
	++it;
	if (it != m_value.end())
	    s += " ";
    }
    return s;
}

std::string Set::toString() const
{
    std::string s = "(";
    std::vector < Value* >::const_iterator it = m_value.begin();

    while (it != m_value.end()) {
	s += (*it)->toString();
	++it;
	if (it != m_value.end())
	    s += ",";
    }
    s += ")";
    return s;
}

std::string Set::toXML() const
{
    std::string s="<SET>";
    std::vector < Value* >::const_iterator it = m_value.begin();

    while (it != m_value.end()) {
	s += (*it)->toXML();
	++it;
    }
    s += "</SET>";
    return s;
}

}} // namespace vle value
