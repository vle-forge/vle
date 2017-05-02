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

#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/vpz/View.hpp>

namespace vle {
namespace vpz {

View::View(const std::string& name,
           View::Type type,
           const std::string& output,
           double timestep,
           bool enable)
  : m_timestep(timestep)
  , m_name(name)
  , m_output(output)
  , m_enabled(enable)
  , m_type(type)
{
    if (m_type == View::TIMED) {
        if (m_timestep <= 0.0) {
            throw utils::ArgError(
              (fmt(_("Cannont define the View '%1%' with a timestep '%2%'")) %
               m_name % m_timestep)
                .str());
        }
    }
}

View::View(const std::string& name)
  : m_timestep(0.0)
  , m_name(name)
  , m_enabled(true)
  , m_type(
      static_cast<Type>(View::INTERNAL | View::EXTERNAL | View::CONFLUENT))
{
}

void
View::write(std::ostream& out) const
{
    out << "<view "
        << "name=\"" << m_name.c_str() << "\" "
        << "output=\"" << m_output.c_str() << "\" "
        << "type=\"" << streamtype() << "\" ";

    if (m_type == TIMED)
        out << "timestep=\"" << m_timestep << "\" ";

    if (m_data.empty()) {
        out << "/>\n";
    } else {
        out << ">\n"
            << "<![CDATA[\n"
            << m_data.c_str() << "]]>\n"
            << "</view>\n";
    }
}

std::string
View::streamtype() const
{
    if (m_type == TIMED)
        return "timed";

    std::string ret;
    if (m_type & View::OUTPUT)
        ret = "output";

    if (m_type & View::INTERNAL) {
        if (not ret.empty())
            ret += ',';
        ret += "internal";
    }

    if (m_type & View::EXTERNAL) {
        if (not ret.empty())
            ret += ',';
        ret += "external";
    }

    if (m_type & View::CONFLUENT) {
        if (not ret.empty())
            ret += ',';
        ret += "confluent";
    }

    if (m_type & View::FINISH) {
        if (not ret.empty())
            ret += ',';
        ret += "finish";
    }

    return ret;
}

void
View::setTimestep(double time)
{
    if ((m_type == View::TIMED) && (time <= 0.0)) {
        throw utils::ArgError(
          (fmt(_("Bad time step %1% for view %2%")) % time % m_name).str());
    }

    m_timestep = time;
}

bool
View::operator==(const View& view) const
{
    return m_name == view.name() and m_type == view.type() and
           m_output == view.output() and m_timestep == view.timestep() and
           m_data == view.data();
}
}
} // namespace vle vpz
