/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2003-2018 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2018 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2018 INRA http://www.inra.fr
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

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <utility>
#include <vle/vpz/AtomicModel.hpp>

namespace vle {
namespace vpz {

AtomicModel::AtomicModel(const std::string& name, CoupledModel* parent)
  : BaseModel(name, parent)
  , m_simulator(nullptr)
  , m_debug(false)
{}

AtomicModel::AtomicModel(const std::string& name,
                         CoupledModel* parent,
                         const std::string& condition,
                         std::string dynamic,
                         std::string observable)
  : BaseModel(name, parent)
  , m_simulator(nullptr)
  , m_dynamics(std::move(dynamic))
  , m_observables(std::move(observable))
  , m_debug(false)
{
    std::string conditionList(condition);
    boost::trim(conditionList);

    if (not conditionList.empty()) {
        boost::split(m_conditions,
                     conditionList,
                     boost::is_any_of(","),
                     boost::algorithm::token_compress_on);
        if (m_conditions.front().empty()) {
            m_conditions.pop_back();
        }
    }
}

AtomicModel::AtomicModel(const AtomicModel& mdl)
  : BaseModel(mdl)
  , m_simulator(nullptr)
  , m_dynamics(mdl.dynamics())
  , m_observables(mdl.observables())
  , m_debug(mdl.m_debug)
{
    m_conditions = mdl.m_conditions;
}

AtomicModel&
AtomicModel::operator=(const AtomicModel& mdl)
{
    AtomicModel m(mdl);
    swap(m);
    return *this;
}

void
AtomicModel::delCondition(const std::string& str)
{
    auto itfind = std::find(m_conditions.begin(), m_conditions.end(), str);

    m_conditions.erase(itfind);
}

BaseModel*
AtomicModel::findModel(const std::string& name) const
{
    return (getName() == name)
             ? const_cast<BaseModel*>(reinterpret_cast<const BaseModel*>(this))
             : nullptr;
}

void
AtomicModel::writeXML(std::ostream& out) const
{
    out << "<model name=\"" << getName().c_str() << R"(" type="atomic")"
        << ">\n";
    writePortListXML(out);
    out << "</model>\n";
}

void
AtomicModel::write(std::ostream& out) const
{
    out << "<model name=\"" << getName().c_str() << "\" "
        << "type=\"atomic\" ";

    if (not conditions().empty()) {
        out << "conditions=\"";

        auto it = conditions().begin();
        while (it != conditions().end()) {
            out << it->c_str();
            ++it;
            if (it != conditions().end()) {
                out << ",";
            }
        }

        out << "\" ";
    }

    out << "dynamics=\"" << dynamics().c_str() << "\" ";

    if (not observables().empty()) {
        out << "observables=\"" << observables().c_str() << "\" ";
    }

    writeGraphics(out);

    out << ">\n";

    writePort(out);

    out << "</model>\n";
}

void
AtomicModel::updateDynamics(const std::string& oldname,
                            const std::string& newname)
{
    if (dynamics() == oldname) {
        setDynamics(newname);
    }
}

void
AtomicModel::purgeDynamics(const std::set<std::string>& dynamicslist)
{
    if (dynamicslist.find(dynamics()) == dynamicslist.end()) {
        setDynamics("");
    }
}

void
AtomicModel::updateObservable(const std::string& oldname,
                              const std::string& newname)
{
    if (observables() == oldname) {
        setObservables(newname);
    }
}

void
AtomicModel::purgeObservable(const std::set<std::string>& observablelist)
{
    if (observablelist.find(observables()) == observablelist.end()) {
        setObservables("");
    }
}

void
AtomicModel::updateConditions(const std::string& oldname,
                              const std::string& newname)
{
    std::replace(m_conditions.begin(), m_conditions.end(), oldname, newname);
}

void
AtomicModel::purgeConditions(const std::set<std::string>& conditionlist)
{
    if (m_conditions.empty())
        return;

    for (auto i = static_cast<long>(m_conditions.size()) - 1; i >= 0; --i) {

        auto itfind = conditionlist.find(m_conditions[i]);

        if (itfind == conditionlist.end()) {
            m_conditions.erase(m_conditions.begin() + i);
        }
    }
}
}
} // namespace vpz graph
