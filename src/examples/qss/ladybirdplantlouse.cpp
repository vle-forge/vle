/*
 * @file examples/qss/ladybirdplantlouse.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
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


#include "ladybirdplantlouse.hpp"
#include <cmath>

namespace vle { namespace examples { namespace qss {

Ladybirdplantlouse::Ladybirdplantlouse(const devs::DynamicsInit& model,
                                       const devs::InitEventList& events) :
    extension::QSS::Multiple(model,events)
{
    a = value::toDouble(events.get("a"));
    b = value::toDouble(events.get("b"));
    d = value::toDouble(events.get("d"));
    e = value::toDouble(events.get("e"));

    x = createVar(0, "x");
    y = createVar(1, "y");
}

Ladybirdplantlouse::~Ladybirdplantlouse()
{
}

double Ladybirdplantlouse::compute(unsigned int i,
				   const devs::Time& /* time */) const
{
    switch (i) {
    case 0: // x
        return a * x() - b * x() * y();
    case 1: // y
        return b * d * x() * y() - e * y();
    default:
        throw utils::InternalError(fmt(
                _("Compute problem with Ladybirdplantlouse, i == %1%")) % i );
    }
}

}}} // namespace vle examples qss

DECLARE_NAMED_DYNAMICS(ladybirdplantlouse,
		       vle::examples::qss::Ladybirdplantlouse)

