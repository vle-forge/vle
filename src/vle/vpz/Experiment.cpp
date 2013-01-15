/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2013 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2013 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2013 INRA http://www.inra.fr
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

namespace vle { namespace vpz {

void Experiment::write(std::ostream& out) const
{
    out << "<experiment "
        << "name=\"" << m_name.c_str() << "\" "
        << "duration=\"" << m_duration << "\" "
        << "begin=\"" << m_begin << "\" ";

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
    m_duration = 1.0;
    m_begin = 0;

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

void Experiment::cleanNoPermanent()
{
    m_conditions.cleanNoPermanent();
    m_views.observables().cleanNoPermanent();
}

void Experiment::setDuration(double duration)
{
    if (duration <= 0.0) {
        throw utils::ArgError(fmt(
                _("Experiment duration error: %1% (must be > 0)")) % duration);
    }

    m_duration = duration;
}

void Experiment::setCombination(const std::string& name)
{
    if (name != "linear" and name != "total") {
        throw utils::ArgError(fmt(_("Unknow combination '%1%'")) % name);
    }

    m_combination.assign(name);
}

}} // namespace vle vpz
