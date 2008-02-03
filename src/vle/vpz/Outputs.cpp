/**
 * @file vle/vpz/Outputs.cpp
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


#include <vle/vpz/Outputs.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {

void Outputs::write(std::ostream& out) const
{
    if (not m_list.empty()) {
        out << "<outputs>\n";

        for (OutputList::const_iterator it = m_list.begin();
             it != m_list.end(); ++it) {
            it->second.write(out);
        }

        out << "</outputs>\n";
    }
}

Output& Outputs::addLocalStream(const std::string& name,
                                const std::string& location,
                                const std::string& plugin)
{
    Output o;
    o.setName(name);
    o.setLocalStream(location, plugin);
    return add(o);
}

Output& Outputs::addDistantStream(const std::string& name,
                                  const std::string& location,
                                  const std::string& plugin)
{
    Output o;
    o.setName(name);
    o.setDistantStream(location, plugin);
    return add(o);
}

void Outputs::del(const std::string& name)
{
    OutputList::iterator it = m_list.find(name);

    if (it != m_list.end()) {
        m_list.erase(it);
    }
}

void Outputs::add(const Outputs& o)
{
    std::for_each(o.outputlist().begin(),
                  o.outputlist().end(),
                  AddOutput(*this));
}

Output& Outputs::add(const Output& o)
{
    Assert(utils::InternalError, not exist(o.name()),
           boost::format("An output have already this name '%1%'\n") %
           o.name());

    return (*m_list.insert(std::make_pair < std::string, Output >(
                o.name(), o)).first).second;
}

Output& Outputs::get(const std::string& name)
{
    OutputList::iterator it = m_list.find(name);
    Assert(utils::InternalError, it != m_list.end(),
           boost::format("Unknow output '%1%'\n") % name);

    return it->second;
}

const Output& Outputs::get(const std::string& name) const
{
    OutputList::const_iterator it = m_list.find(name);
    Assert(utils::InternalError, it != m_list.end(),
           boost::format("Unknow output '%1%'\n") % name);

    return it->second;
}

std::list < std::string > Outputs::outputsname() const
{
    std::list < std::string > result;

    std::for_each(m_list.begin(), m_list.end(), AddOutputName(result));

    return result;
}

}} // namespace vle vpz
