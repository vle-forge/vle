/** 
 * @file vpz/Outputs.cpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 31 jan 2006 17:30:32 +0100
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

#include <vle/vpz/Outputs.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Trace.hpp>

namespace vle { namespace vpz {

using namespace vle::utils;

void Outputs::write(std::ostream& out) const
{
    if (not empty()) {
        out << "<ouputs>";

        for (const_iterator it = begin(); it != end(); ++it) {
            it->second.write(out);
        }

        out << "</outputs>";
    }

}

void Outputs::addTextStream(const std::string& name,
                            const std::string& location)
{
    Output o;
    o.setName(name);
    o.setTextStream(location);
    add(o);
}

void Outputs::addSdmlStream(const std::string& name,
                            const std::string& location)
{
    Output o;
    o.setName(name);
    o.setSdmlStream(location);
    add(o);
}

void Outputs::addEovStream(const std::string& name,
                           const std::string& plugin,
                           const std::string& location)
{
    Output o;
    o.setName(name);
    o.setEovStream(plugin, location);
    add(o);
}

void Outputs::addNetStream(const std::string& name,
                           const std::string& plugin,
                           const std::string& location)
{
    Output o;
    o.setName(name);
    o.setNetStream(plugin, location);
    add(o);
}

void Outputs::del(const std::string& name)
{
    iterator it = find(name);

    if (it != end()) {
        erase(it);
    }
}

void Outputs::add(const Outputs& o)
{
    for (const_iterator it = o.begin(); it != o.end(); ++it) {
        add(it->second);
    }
}

void Outputs::add(const Output& o)
{
    Assert(utils::InternalError, exist(o.name()) == false,
           boost::format("An output have already this name '%1%'\n") %
           o.name());

    insert(std::make_pair < std::string, Output >(o.name(), o));
}

Output& Outputs::get(const std::string& name)
{
    iterator it = find(name);
    Assert(utils::InternalError, it != end(),
           boost::format("Unknow output '%1%'\n") % name);

    return it->second;
}

const Output& Outputs::get(const std::string& name) const
{
    const_iterator it = find(name);
    Assert(utils::InternalError, it != end(),
           boost::format("Unknow output '%1%'\n") % name);

    return it->second;
}

std::list < std::string > Outputs::outputsname() const
{
    std::list < std::string > result;

    const_iterator it;
    for (it = begin(); it != end(); ++it) {
        result.push_back(it->second.name());
    }
    return result;
}

bool Outputs::exist(const std::string& name) const
{
    return find(name) != end();
}

}} // namespace vle vpz
