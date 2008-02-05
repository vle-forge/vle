/**
 * @file src/vle/vpz/Experiment.cpp
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




#include <vle/vpz/Experiment.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {

void Experiment::write(std::ostream& out) const
{
    out << "<experiment "
        << "name=\"" << m_name << "\" "
        << "duration=\"" << m_duration << "\" ";

    if (not m_combination.empty()) {
        out << "combination=\"" << m_combination << "\" ";
    }

    out << "seed=\"" << m_seed << "\" >\n";

    m_replicas.write(out);
    m_conditions.write(out);
    m_views.write(out);

    out << "</experiment>\n";
}

void Experiment::initFromExperiment(xmlpp::Element* /* elt */)
{
}

void Experiment::clear()
{
    m_name.clear();
    m_duration = 0;
    m_seed = 1;

    m_replicas.clear();
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
    AssertI(not name.empty());

    m_name.assign(name);
}

void Experiment::cleanNoPermanent()
{
    m_conditions.cleanNoPermanent();
    m_views.observables().cleanNoPermanent();
}

void Experiment::setDuration(double duration)
{
    AssertI(duration > 0);

    m_duration = duration;
}

void Experiment::setCombination(const std::string& name)
{
    AssertI(name == "linear" or name == "total");

    m_combination.assign(name);
}

}} // namespace vle vpz
