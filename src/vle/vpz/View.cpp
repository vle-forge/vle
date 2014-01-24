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


#include <vle/vpz/View.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>

namespace vle { namespace vpz {

View::View(const std::string& name,
           View::Type type,
           const std::string& output,
           double timestep) :
    m_name(name),
    m_type(type),
    m_output(output),
    m_timestep(timestep)
{
    if (m_type == View::TIMED) {
        if (m_timestep <= 0.0) {
            throw utils::ArgError(fmt(
                    _("Cannont define the View '%1%' with a timestep '%2%'")) %
                m_name % m_timestep);
        }
    }
}

void View::write(std::ostream& out) const
{
    out << "<view "
        << "name=\"" << m_name.c_str() << "\" "
        << "output=\"" << m_output.c_str() << "\" ";

    switch (m_type) {
    case View::EVENT:
        out << "type=\"event\"";
        break;
    case View::TIMED:
        out << "type=\"timed\" "
            << "timestep=\"" << m_timestep << "\"";
        break;
    case View::FINISH:
        out << "type=\"finish\"";
        break;
    }

    if (m_data.empty()) {
        out << " />\n";
    } else {
        out << ">\n"
            << "<![CDATA[\n"
            << m_data.c_str()
            << "]]>\n"
            << "</view>\n";
    }
}

void View::setTimestep(double time)
{
    if ((m_type == View::TIMED) && (time <= 0.0)) {
        throw utils::ArgError(fmt(
                _("Bad time step %1% for view %2%")) % time % m_name);
    }

    m_timestep = time;
}

bool View::operator==(const View& view) const
{
    return m_name == view.name() and m_type == view.type()
	and m_output == view.output()
	and m_timestep == view.timestep() and m_data == view.data();
}


}} // namespace vle vpz
