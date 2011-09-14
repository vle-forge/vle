/*
 * @file vle/extension/decision/Rules.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#include <vle/extension/decision/Rules.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>

namespace vle { namespace extension { namespace decision {

Rule& Rules::add(const std::string& name, const Rule& rule)
{
    const_iterator it(m_lst.find(name));

    if (it != m_lst.end()) {
        throw utils::ArgError(vle::fmt(_("Decision: rule '%1%' already exists"))
            % name);
    }

    return (*m_lst.insert(value_type(name, rule)).first).second;
}

Rule& Rules::add(const std::string& name, const Predicate& pred)
{
    iterator it(m_lst.find(name));

    if (it == m_lst.end()) {
        Rule& r(add(name));
        r.add(pred);
        return r;
    } else {
        it->second.add(pred);
        return it->second;
    }
}

Rules::result_t Rules::apply() const
{
    result_t result;
    const_iterator it(m_lst.begin());

    while (it != m_lst.end()) {
        if (it->second.isAvailable()) {
            result.push_back(it);
        }
        ++it;
    }
    return result;
}

const Rule& Rules::get(const std::string& name) const
{
    const_iterator it = m_lst.find(name);

    if (it == m_lst.end()) {
        throw utils::ArgError(vle::fmt(_("Decision: rule '%1%' does not exist"))
            % name);
    }

    return it->second;
}

}}} // namespace vle model decision

