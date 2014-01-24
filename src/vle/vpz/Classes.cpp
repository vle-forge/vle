/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/vpz/Classes.hpp>
#include <vle/utils/Algo.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <iterator>

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
    x = m_lst.insert(value_type(name, Class(name)));

    if (not x.second) {
        throw utils::ArgError(fmt(_("Class '%1%' already exists")) % name);
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

void Classes::rename(const std::string& oldname, const std::string& newname)
{
    if (exist(newname)) {
        throw utils::ArgError(fmt(_("Class '%1%' already exists")) % newname);
    }

    iterator it = m_lst.find(oldname);
    if (it == end()) {
        throw utils::ArgError(fmt(_("Unknow class '%1%'")) % oldname);
    }

    std::pair < iterator, bool > x;
    x = m_lst.insert(value_type(newname, it->second));
    x.first->second.setName(newname);

    m_lst.erase(it);
}

void Classes::updateDynamics(const std::string& oldname,
                             const std::string& newname)
{
    vpz::ClassList::iterator it = m_lst.begin();
    while (it != m_lst.end()) {
        it->second.updateDynamics(oldname, newname);
        ++it;
    }
}

void Classes::purgeDynamics(const std::set < std::string >& dynamicslist)
{
    vpz::ClassList::iterator it = m_lst.begin();
    while (it != m_lst.end()) {
        it->second.purgeDynamics(dynamicslist);
        ++it;
    }
}

void Classes::updateObservable(const std::string& oldname,
                               const std::string& newname)
{
    vpz::ClassList::iterator it = m_lst.begin();
    while (it != m_lst.end()) {
        it->second.updateObservable(oldname, newname);
         ++it;
    }
}

void Classes::purgeObservable(const std::set < std::string >& observablelist)
{
    vpz::ClassList::iterator it = m_lst.begin();
    while (it != m_lst.end()) {
        it->second.purgeObservable(observablelist);
         ++it;
    }
}

void Classes::updateConditions(const std::string& oldname,
                              const std::string& newname)
{
    vpz::ClassList::iterator it = m_lst.begin();
    while (it != m_lst.end()) {
        it->second.updateConditions(oldname, newname);
         ++it;
    }
}

void Classes::purgeConditions(const std::set < std::string >& conditionlist)
{
    vpz::ClassList::iterator it = m_lst.begin();
    while (it != m_lst.end()) {
        it->second.purgeConditions(conditionlist);
        ++it;
    }
}

}} // namespace vle vpz

