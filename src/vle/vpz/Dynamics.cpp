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
#include <vle/utils/Algo.hpp>

namespace vle { namespace vpz {

void Dynamics::write(std::ostream& out) const
{
    if (not m_list.empty()) {
        out << "<dynamics>\n";
        std::transform(begin(), end(),
                       std::ostream_iterator < Dynamic >(out, "\n"),
                       utils::select2nd < DynamicList::value_type >());
        out << "</dynamics>\n";
    }
}

void Dynamics::add(const Dynamics& dyns)
{
    std::for_each(dyns.begin(), dyns.end(), AddDynamic(*this));
}

Dynamic& Dynamics::add(const Dynamic& dynamic)
{
    std::pair < iterator, bool > r;

    r = m_list.insert(std::make_pair < std::string, Dynamic >(
            dynamic.name(), dynamic));

    Assert < utils::ArgError >(r.second, boost::format(
            "The dynamics '%1%' already exists") % dynamic.name());

    return r.first->second;
}

void Dynamics::del(const std::string& name)
{
    m_list.erase(name);
}

const Dynamic& Dynamics::get(const std::string& name) const
{
    const_iterator it = m_list.find(name);
    Assert < utils::ArgError >(it != end(), boost::format(
            "The dynamics %1% does not exist") % name);

    return it->second;
}

Dynamic& Dynamics::get(const std::string& name)
{
    iterator it = m_list.find(name);
    Assert < utils::ArgError >(it != end(), boost::format(
            "The dynamics %1% does not exist") % name);

    return it->second;
}

void Dynamics::cleanNoPermanent()
{
    iterator it = begin();

    while ((it = utils::find_if(it, end(), Dynamic::IsPermanent())) != end()) {
        iterator del = it++;
        m_list.erase(del);
    }
}

}} // namespace vle vpz
