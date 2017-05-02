/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#include <algorithm>
#include <iterator>
#include <vle/utils/Algo.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/value/Double.hpp>
#include <vle/vpz/Conditions.hpp>
#include <vle/vpz/Experiment.hpp>

namespace vle {
namespace vpz {

Conditions::Conditions()
{
    Condition& def = add(Experiment::defaultSimulationEngineCondName());

    def.addValueToPort("begin", value::Double::create(0.0));
    def.addValueToPort("duration", value::Double::create(100.0));
}

std::set<std::string>
Conditions::getKeys()
{
    std::vector<std::string> conditionKeys;

    conditionKeys.resize(m_list.size());

    std::transform(
      m_list.begin(), m_list.end(), conditionKeys.begin(), conditionKey);

    std::set<std::string> conditionKeysSet(conditionKeys.begin(),
                                           conditionKeys.end());

    return conditionKeysSet;
}

void
Conditions::write(std::ostream& out) const
{
    if (not m_list.empty()) {
        out << "<conditions>\n";
        std::transform(m_list.begin(),
                       m_list.end(),
                       std::ostream_iterator<Condition>(out),
                       utils::select2nd<ConditionList::value_type>());
        out << "</conditions>\n";
    }
}

std::vector<std::string>
Conditions::conditionnames() const
{
    std::vector<std::string> lst(m_list.size());

    std::transform(m_list.begin(),
                   m_list.end(),
                   lst.begin(),
                   [](const value_type& value) { return value.first; });

    return lst;
}

std::vector<std::string>
Conditions::portnames(const std::string& condition) const
{
    const Condition& cnd(get(condition));

    return cnd.portnames();
}

void
Conditions::add(const Conditions& cdts)
{
    std::for_each(cdts.conditionlist().begin(),
                  cdts.conditionlist().end(),
                  AddCondition(*this));
}

Condition&
Conditions::add(const Condition& condition)
{
    del(condition.name());
    std::pair<iterator, bool> x;
    x = m_list.insert(value_type(condition.name(), condition));
    return x.first->second;
}

void
Conditions::del(const std::string& condition)
{
    if (condition != Experiment::defaultSimulationEngineCondName())
        m_list.erase(condition);
}

void
Conditions::rename(const std::string& oldconditionname,
                   const std::string& newconditionname)
{
    if ((oldconditionname != Experiment::defaultSimulationEngineCondName()) &&
        newconditionname != Experiment::defaultSimulationEngineCondName()) {
        Condition c = get(oldconditionname);
        c.setName(newconditionname);
        add(c);
        del(oldconditionname);
    }
}

void
Conditions::copy(const std::string& conditionname,
                 const std::string& copyconditionname)
{
    Condition c = get(conditionname);
    c.setName(copyconditionname);
    add(c);
}

void
Conditions::clear()
{
    auto it = m_list.begin();

    if (it == m_list.end()) {
        Condition& def = add(Experiment::defaultSimulationEngineCondName());
        def.setValueToPort("begin", value::Double::create(0.0));
        def.setValueToPort("duration", value::Double::create(100.0));
    } else {
        do {
            auto tmp = it;
            ++it;

            if (tmp->first != Experiment::defaultSimulationEngineCondName())
                m_list.erase(tmp);
        } while (it != m_list.end());
    }
}

const Condition&
Conditions::get(const std::string& condition) const
{
    auto it = m_list.find(condition);

    if (it == end()) {
        throw utils::ArgError(
          (fmt(_("The condition '%1%' does not exists")) % condition).str());
    }

    return it->second;
}

Condition&
Conditions::get(const std::string& condition)
{
    auto it = m_list.find(condition);

    if (it == end()) {
        throw utils::ArgError(
          (fmt(_("The condition '%1%' does not exists")) % condition).str());
    }

    return it->second;
}

void
Conditions::cleanNoPermanent()
{
    auto it = begin();

    while ((it = utils::findIf(it, end(), Condition::IsPermanent())) !=
           end()) {
        auto del = it++;
        m_list.erase(del);
    }
}

void
Conditions::deleteValueSet()
{
    utils::forEach(m_list.begin(), m_list.end(), Condition::DeleteValueSet());
}
}
} // namespace vle vpz
