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

#include <iomanip>
#include <limits>
#include <vle/value/Double.hpp>

namespace vle {
namespace value {

void
Double::writeFile(std::ostream& out) const
{
    std::streamsize old = out.precision();

    out << std::setprecision(std::numeric_limits<double>::digits10) << m_value;

    out.precision(old);
}

void
Double::writeString(std::ostream& out) const
{
    std::streamsize old = out.precision();

    out << std::setprecision(std::numeric_limits<double>::digits10) << m_value;

    out.precision(old);
}

void
Double::writeXml(std::ostream& out) const
{
    std::streamsize old = out.precision();

    out << std::setprecision(std::numeric_limits<double>::digits10)
        << "<double>" << m_value << "</double>";

    out.precision(old);
}
}
} // namespace vle value
