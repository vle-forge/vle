/**
 * @file vle/vpz/Dynamics.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/vpz/Dynamics.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {

void Dynamics::write(std::ostream& out) const
{
    if (not m_list.empty()) {
        out << "<dynamics>\n";

        std::transform(m_list.begin(), m_list.end(), 
                       std::ostream_iterator < Dynamic >(out, "\n"),
                       DynamicValue());

        out << "</dynamics>\n";
    }
}

void Dynamics::add(const Dynamics& dyns)
{
    std::for_each(dyns.dynamiclist().begin(), dyns.dynamiclist().end(),
                  AddDynamic(*this));
}

Dynamic& Dynamics::add(const Dynamic& dynamic)
{
    DynamicList::const_iterator it = m_list.find(dynamic.name());
    Assert(utils::SaxParserError, it == m_list.end(),
           (boost::format("The dynamics %1% already exist") % dynamic.name()));

    return (*m_list.insert(std::make_pair < std::string, Dynamic >(
                dynamic.name(), dynamic)).first).second;
}

void Dynamics::del(const std::string& name)
{
    m_list.erase(name);
}

const Dynamic& Dynamics::get(const std::string& name) const
{
    DynamicList::const_iterator it = m_list.find(name);
    Assert(utils::SaxParserError, it != m_list.end(),
           (boost::format("The dynamics %1% does not exist") % name));
    return it->second;
}

Dynamic& Dynamics::get(const std::string& name)
{
    DynamicList::iterator it = m_list.find(name);
    Assert(utils::SaxParserError, it != m_list.end(),
           (boost::format("The dynamics %1% does not exist") % name));

    return it->second;
}

bool Dynamics::exist(const std::string& name) const
{
    return m_list.find(name) != m_list.end();
}

void Dynamics::cleanNoPermanent()
{
    DynamicList::iterator previous = m_list.begin();
    DynamicList::iterator it = m_list.begin();
    
    while (it != m_list.end()) {
        if (not it->second.isPermanent()) {
            if (it == previous) {
                m_list.erase(it);
                previous = m_list.begin();
                it = m_list.begin();
            } else {
                m_list.erase(it);
                it = previous;
            }
        } else {
            ++previous;
            ++it;
        }
    }
}

}} // namespace vle vpz
