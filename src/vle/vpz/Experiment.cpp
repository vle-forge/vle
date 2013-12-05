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


#include <vle/vpz/Experiment.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Set.hpp>

namespace vle { namespace vpz {

Experiment::Experiment()
{
    Condition cond(defaultSimulationEngineCondName());
    cond.addValueToPort("begin", new vle::value::Double(0));
    cond.addValueToPort("duration", new vle::value::Double(1));
    conditions().add(cond);
}

void Experiment::write(std::ostream& out) const
{
    out << "<experiment "
        << "name=\"" << m_name.c_str() << "\" ";

    if (not m_combination.empty()) {
        out << "combination=\"" << m_combination.c_str()
            << "\" ";
    }

    out << " >\n";

    m_conditions.write(out);
    m_views.write(out);

    out << "</experiment>\n";
}

void Experiment::clear()
{
    m_name.clear();

    m_conditions.clear();
    m_views.clear();
}

void Experiment::addConditions(const Conditions& c)
{
    m_conditions.add(c);
}

void Experiment::addViews(const Views& m)
{
    m_views.add(m);
}

void Experiment::setName(const std::string& name)
{
    if (name.empty()) {
        throw utils::ArgError(_("Empty experiment name"));
    }

    m_name.assign(name);
}

void Experiment::setDuration(double duration)
{
    if (not conditions().exist(defaultSimulationEngineCondName())) {
        throw utils::ArgError(_("The simulation engine condition"
                "does not exist"));
    }
    vle::vpz::Condition& condSim = conditions().get(
            defaultSimulationEngineCondName());
    vle::value::Set& dur = condSim.getSetValues("duration");
    dur.clear();
    dur.add(new vle::value::Double(duration));
}

double Experiment::duration() const
{
    if (not conditions().exist(defaultSimulationEngineCondName())) {
        throw utils::ArgError(_("The simulation engine condition"
                "does not exist"));
    }
    const vle::vpz::Condition& condSim = conditions().get(
            defaultSimulationEngineCondName());
    return condSim.getSetValues("duration").getDouble(0);
}

void Experiment::setBegin(double begin)
{
    if (not conditions().exist(defaultSimulationEngineCondName())) {
        throw utils::ArgError(_("The simulation engine condition"
                "does not exist"));
    }
    vle::vpz::Condition& condSim = conditions().get(
            defaultSimulationEngineCondName());
    vle::value::Set& dur = condSim.getSetValues("begin");
    dur.clear();
    dur.add(new vle::value::Double(begin));
}

double Experiment::begin() const
{
    if (not conditions().exist(defaultSimulationEngineCondName())) {
        throw utils::ArgError(_("The simulation engine condition"
                "does not exist"));
    }
    const vle::vpz::Condition& condSim = conditions().get(
            defaultSimulationEngineCondName());
    return condSim.getSetValues("begin").getDouble(0);
}

void Experiment::cleanNoPermanent()
{
    m_conditions.cleanNoPermanent();
    m_views.observables().cleanNoPermanent();
}

void Experiment::setCombination(const std::string& name)
{
    if (name != "linear" and name != "total") {
        throw utils::ArgError(fmt(_("Unknow combination '%1%'")) % name);
    }

    m_combination.assign(name);
}

}} // namespace vle vpz
