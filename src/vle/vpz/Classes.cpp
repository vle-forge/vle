/**
 * @file vle/vpz/Classes.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
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


#include <vle/vpz/Classes.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Algo.hpp>

namespace vle { namespace vpz {

void Classes::write(std::ostream& out) const
{
    if (not m_lst.empty()) {
        out << "<classes>\n";
        std::transform(begin(), end(),
                       std::ostream_iterator < Class >(out),
                       utils::select2nd < ClassList::value_type >());
        out << "</classes>\n";
    }
}

Class& Classes::add(const std::string& name)
{
    std::pair < iterator, bool > x;
    x = m_lst.insert(std::make_pair < std::string, Class >(
            name, Class(name)));

    if (not x.second) {
        throw utils::ArgError(fmt(_("Class '%1%' already exist")) % name);
    }

    return x.first->second;
}

void Classes::del(const std::string& name)
{
    m_lst.erase(name);
}

const Class& Classes::get(const std::string& name) const
{
    const_iterator it = m_lst.find(name);

    if (it == end()) {
        throw utils::ArgError(fmt(_("Unknow class '%1%'")) % name);
    }

    return it->second;
}

Class& Classes::get(const std::string& name)
{
    iterator it = m_lst.find(name);

    if (it == end()) {
        throw utils::ArgError(fmt(_("Unknow class '%1%'")) % name);
    }

    return it->second;
}

}} // namespace vle vpz

