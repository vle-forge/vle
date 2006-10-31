/** 
 * @file Conditions.cpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 31 jan 2006 17:29:08 +0100
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

#include <vle/vpz/Conditions.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/value/Value.hpp>

namespace vle { namespace vpz {

using namespace vle::utils;

Conditions::Conditions()
{
}

Conditions::~Conditions()
{
}

void Conditions::init(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "EXPERIMENTAL_CONDITIONS");

    m_conditions.clear();

    xmlpp::Node::NodeList lst = elt->get_children("CONDITION");
    for (xmlpp::Node::NodeList::iterator it = lst.begin();
         it != lst.end(); ++it) {

        Condition c;
        c.init((xmlpp::Element*)(*it));
        addCondition(c);
    }
}

void Conditions::write(xmlpp::Element* elt) const
{
    AssertI(elt);

    if (not m_conditions.empty()) {
        xmlpp::Element* cond = elt->add_child("EXPERIMENTAL_CONDITIONS");
        std::list < Condition >::const_iterator it = m_conditions.begin();
        while (it != m_conditions.end()) {
            (*it).write(cond);
            ++it;
        }
    }
}

void Conditions::addConditions(const Conditions& c)
{
    std::list < Condition >::const_iterator it = c.conditions().begin();
    for (it = c.conditions().begin(); it != c.conditions().end(); ++it) {
        addCondition(*it);
    }
}

void Conditions::addCondition(const Condition& c)
{
    std::list < Condition >::iterator it = m_conditions.begin();
    while (it != m_conditions.end()) {
        if ((*it).modelname() == c.modelname() and
            (*it).portname() == c.portname()) {

            Throw(utils::InternalError,
                  boost::format(
                      "Already defined condition model '%1%' on port '%2%'\n") %
                  c.modelname() % c.portname());
        }
        ++it;
    }
    m_conditions.push_back(c);
}

void Conditions::clear()
{
    m_conditions.clear();
}

void Conditions::delCondition(const std::string& modelname,
                              const std::string& portname)
{
    std::list < Condition >::iterator it = m_conditions.begin();
    std::list < Condition >::iterator prec = m_conditions.end();

    while (it != m_conditions.end()) {
        if ((*it).modelname() == modelname and
            (*it).portname() == portname) {
            m_conditions.erase(it);
            it = prec;
        }
        prec = it++;
    }
}

}} // namespace vle vpz
