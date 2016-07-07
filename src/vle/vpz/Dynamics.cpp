/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2016 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2016 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2016 INRA http://www.inra.fr
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


#include <vle/vpz/Dynamics.hpp>
#include <vle/utils/Algo.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <iterator>
#include <vector>
#include <set>

namespace vle { namespace vpz {

std::set < std::string > Dynamics::getKeys()
{
    std::vector< std::string> dynamicKeys;

    dynamicKeys.resize(m_list.size());

    std::transform (m_list.begin(),
                    m_list.end(),
                    dynamicKeys.begin(),
                    dynamicKey);


    std::set <std::string> dynamicKeysSet (dynamicKeys.begin(),
                                           dynamicKeys.end());

    return dynamicKeysSet;
}

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
    auto r = m_list.insert(std::make_pair(dynamic.name(), dynamic));

    if (not r.second)
        throw utils::ArgError(
            (fmt(_("The dynamics '%1%' already exists")) % dynamic.name()).str());

    return r.first->second;
}

void Dynamics::del(const std::string& name)
{
    m_list.erase(name);
}

const Dynamic& Dynamics::get(const std::string& name) const
{
    auto it = m_list.find(name);

    if (it == end()) {
        throw utils::ArgError((fmt(_("The dynamics %1% does not exist"))
                               % name).str());
    }

    return it->second;
}

Dynamic& Dynamics::get(const std::string& name)
{
    auto it = m_list.find(name);

    if (it == end()) {
        throw utils::ArgError(
            (fmt(_( "The dynamics %1% does not exist")) % name).str());
    }

    return it->second;
}

void Dynamics::cleanNoPermanent()
{
    auto it = begin();

    while ((it = utils::findIf(it, end(), Dynamic::IsPermanent())) != end()) {
        auto del = it++;
        m_list.erase(del);
    }
}

void Dynamics::copy(const std::string& sourcename,
                    const std::string& targetname)
{
    vpz::Dynamic copy = get(sourcename);
    copy.setName(targetname);

    add(copy);
}

void Dynamics::rename(const std::string& oldname,
                      const std::string& newname)
{
    copy(oldname, newname);
    del(oldname);
}

std::set < std::string > Dynamics::depends() const
{
    std::set < std::string > result;

    for (const auto & elem : *this) {
        if (not elem.second.package().empty()) {
            result.insert(elem.second.package());
        }
    }

    return result;
}

}} // namespace vle vpz
