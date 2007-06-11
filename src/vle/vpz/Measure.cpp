/** 
 * @file vpz/Measure.cpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 31 jan 2006 17:31:58 +0100
 */

/*
 * Copyright (C) 2006 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <vle/vpz/Measure.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {

using namespace vle::utils;

Measure::Measure()
{
}

void Measure::write(std::ostream& out) const
{
    out << "<measure "
        << "output=\"" << m_output << "\" ";

    switch (m_type) {
    case Measure::EVENT:
        out << "type=\"event\"";
        break;

    case Measure::TIMED:
        out << "type=\"timed\" time_step=\"" << m_timestep << "\"";
        break;
    }

    out << ">";


    for (const_iterator it = m_observables.begin(); it != m_observables.end();
         ++it) {
        it->second.write(out);
    }

    out << "</measure>";
}

void Measure::setEventMeasure(const std::string& output)
{
    AssertI(not output.empty());

    m_output.assign(output);
    m_type = Measure::EVENT;
}

void Measure::setTimedMeasure(double timestep,
                              const std::string& output)
{
    AssertI(not output.empty());
    AssertI(timestep > 0);

    m_output.assign(output);
    m_type = Measure::TIMED;
    m_timestep = timestep;
}

Observable& Measure::addObservable(const Observable& o)
{
    const_iterator it = m_observables.find(o.name());
    Assert(utils::SaxParserError, it == m_observables.end(),
           (boost::format("Add an already existing observable %1% in %2%") %
            o.name(), m_name));

    return (m_observables[o.name()] = o);
}

Observable& Measure::addObservable(const std::string& name,
                                   const std::string& group,
                                   int index)
{
    vpz::Observable o;
    o.setObservable(name, group, index);
    return addObservable(o);
}

void Measure::delObservable(const std::string& name)
{
    iterator it = m_observables.find(name);
    if (it != m_observables.end()) {
        m_observables.erase(it);
    }
}

const Observable& Measure::getObservable(const std::string& name) const
{
    const_iterator it = m_observables.find(name);
    Assert(utils::InternalError, it != m_observables.end(),
           (boost::format("Observable %1% not found") % name));

    return it->second;
}

bool Measure::existObservable(const std::string& name) const
{
    return m_observables.find(name) != m_observables.end();
}

}} // namespace vle vpz
