/*
 * @file vle/devs/Time.cpp
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


#include <vle/devs/Time.hpp>
#include <sstream>
#include <iomanip>

namespace vle { namespace devs {

const Time Time::infinity(std::numeric_limits < double >::max());
const Time Time::negativeInfinity(-std::numeric_limits < double >::max());

std::string Time::toString() const
{
    if (m_value == devs::Time::infinity) {
        return std::string("+infinity");
    } else if (m_value == devs::Time::negativeInfinity) {
        return std::string("-infinity");
    } else {
        std::ostringstream out;

        out << std::showpoint
            << std::fixed
            << std::setprecision(std::numeric_limits < double >::digits10)
            << m_value;

        return out.str();
    }
}

}} // namespace vle devs
