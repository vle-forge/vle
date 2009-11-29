/**
 * @file vle/vpz/Dynamics.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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

    if (not r.second) {
        throw utils::ArgError(fmt(
                _("The dynamics '%1%' already exists")) % dynamic.name());
    }

    return r.first->second;
}

void Dynamics::del(const std::string& name)
{
    m_list.erase(name);
}

const Dynamic& Dynamics::get(const std::string& name) const
{
    const_iterator it = m_list.find(name);

    if (it == end()) {
        throw utils::ArgError(fmt(
                _("The dynamics %1% does not exist")) % name);
    }

    return it->second;
}

Dynamic& Dynamics::get(const std::string& name)
{
    iterator it = m_list.find(name);

    if (it == end()) {
        throw utils::ArgError(fmt(
                _( "The dynamics %1% does not exist")) % name);
    }

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
