/**
 * @file src/vle/vpz/Outputs.cpp
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
#include <vle/utils/Algo.hpp>

namespace vle { namespace vpz {

void Outputs::write(std::ostream& out) const
{
    if (not m_list.empty()) {
        out << "<outputs>\n";
        std::transform(begin(), end(),
                       std::ostream_iterator < Output >(out, "\n"),
                       utils::select2nd < OutputList::value_type >());
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
    iterator it = m_list.find(name);

    if (it != end()) {
        m_list.erase(it);
    }
}

void Outputs::add(const Outputs& o)
{
    std::for_each(o.begin(), o.end(), AddOutput(*this));
}

Output& Outputs::add(const Output& o)
{
    std::pair < iterator, bool > x;

    x = m_list.insert(std::make_pair < std::string, Output >( o.name(), o));

    Assert(utils::ArgError, x.second, boost::format(
            "An output have already this name '%1%'\n") %
        o.name());

    return x.first->second;
}

Output& Outputs::get(const std::string& name)
{
    iterator it = m_list.find(name);
    Assert(utils::ArgError, it != end(),
           boost::format("Unknow output '%1%'\n") % name);

    return it->second;
}

const Output& Outputs::get(const std::string& name) const
{
    const_iterator it = m_list.find(name);
    Assert(utils::ArgError, it != end(),
           boost::format("Unknow output '%1%'\n") % name);

    return it->second;
}

std::vector < std::string > Outputs::outputsname() const
{
    std::vector < std::string > result(m_list.size());

    std::transform(begin(), end(), result.begin(),
                   utils::select1st < OutputList::value_type >());

    return result;
}

}} // namespace vle vpz
