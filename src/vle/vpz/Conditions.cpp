/**
 * @file vle/vpz/Conditions.cpp
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


#include <vle/vpz/Conditions.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Algo.hpp>
#include <vle/value/Value.hpp>
#include <algorithm>

namespace vle { namespace vpz {

void Conditions::write(std::ostream& out) const
{
    if (not m_list.empty()) {
        out << "<conditions>\n";
        std::transform(m_list.begin(), m_list.end(),
                       std::ostream_iterator < Condition >(out),
                       utils::select2nd < ConditionList::value_type >());
        out << "</conditions>\n";
    }
}

void Conditions::conditionnames(std::list < std::string >& lst) const
{
    lst.resize(m_list.size());
    std::transform(m_list.begin(), m_list.end(), lst.begin(),
                   utils::select1st < ConditionList::value_type >());
}

void Conditions::portnames(const std::string& condition,
                           std::list < std::string >& lst) const
{
    const Condition& cnd(get(condition));
    cnd.portnames(lst);
}

void Conditions::add(const Conditions& cdts)
{
    std::for_each(cdts.conditionlist().begin(), cdts.conditionlist().end(),
                  AddCondition(*this));
}

Condition& Conditions::add(const Condition& condition)
{
    std::pair < iterator, bool > x;
    x = m_list.insert(std::make_pair < std::string, Condition >(
            condition.name(), condition));

    if (not x.second) {
        throw utils::ArgError(fmt(
                _("The condition '%1%' already exists")) % condition.name());
    }

    return x.first->second;
}

void Conditions::del(const std::string& condition)
{
    m_list.erase(condition);
}

void Conditions::rename(const std::string& oldconditionname,
                        const std::string& newconditionname)
{
    Condition c = get(oldconditionname);
    c.setName(newconditionname);
    add(c);
    del(oldconditionname);
}

void Conditions::copy(const std::string& conditionname,
		      const std::string& copyconditionname)
{
    Condition c = get(conditionname);
    c.setName(copyconditionname);
    add(c);
}

const Condition& Conditions::get(const std::string& condition) const
{
    const_iterator it = m_list.find(condition);

    if (it == end()) {
        throw utils::ArgError(fmt(
                _("The condition '%1%' does not exists")) % condition);
    }

    return it->second;
}

Condition& Conditions::get(const std::string& condition)
{
    iterator it = m_list.find(condition);

    if (it == end()) {
        throw utils::ArgError(fmt(
                _("The condition '%1%' does not exists")) % condition);
    }

    return it->second;
}

void Conditions::cleanNoPermanent()
{
    iterator it = begin();

    while ((it = utils::find_if(it, end(), Condition::IsPermanent())) != end()) {
        iterator del = it++;
        m_list.erase(del);
    }
}

void Conditions::rebuildValueSet()
{
    utils::for_each(m_list.begin(), m_list.end(), Condition::RebuildValueSet());
}

}} // namespace vle vpz
