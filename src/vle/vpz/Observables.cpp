/** 
 * @file Observables.cpp
 * @brief 
 * @author The vle Development Team
 * @date ven, 15 jun 2007 17:48:56 +0200
 */

/*
 * Copyright (C) 2007 - The vle Development Team
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

#include <vle/vpz/Observables.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {

void Observables::write(std::ostream& out) const
{
    if (not empty()) {
        out << "<observables>\n";
        for (const_iterator it = begin(); it != end(); ++it) {
            it->second.write(out);
        }
        out << "</observables>\n";
    }
}

void Observables::add(const Observables& obs)
{
    for (const_iterator it = obs.begin(); it != obs.end(); ++it) {
        add(it->second);
    }
}

Observable& Observables::add(const Observable& obs)
{
    Assert(utils::SaxParserError, not exist(obs.name()),
           (boost::format("Observable %1% already exist") % obs.name()));

    return (*insert(std::make_pair < std::string, Observable >(
                obs.name(), obs)).first).second;
}

Observable& Observables::get(const std::string& name)
{
    iterator it = find(name);
    Assert(utils::SaxParserError, it != end(),
           (boost::format("Observable %1% does not exist") % name));

    return it->second;
}

const Observable& Observables::get(const std::string& name) const
{
    const_iterator it = find(name);
    Assert(utils::SaxParserError, it != end(),
           (boost::format("Observable %1% doest not exist") % name));

    return it->second;
}

void Observables::clean_no_permanent()
{
    iterator previous = begin();
    iterator it = begin();
    
    while (it != end()) {
        if (not it->second.is_permanent()) {
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
