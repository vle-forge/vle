/**
 * @file examples/qss/ladybirdplantlouse.cpp
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


#include "ladybirdplantlouse.hpp"
#include <vle/utils/Debug.hpp>
#include <cmath>

namespace vle { namespace examples { namespace qss {

Ladybirdplantlouse::Ladybirdplantlouse(const graph::AtomicModel& model,
                                       const devs::InitEventList& events) :
    extension::CombinedQss(model,events)
{
    a = value::toDouble(events.get("a"));   
    b = value::toDouble(events.get("b"));
    d = value::toDouble(events.get("d"));
    e = value::toDouble(events.get("e"));
}

Ladybirdplantlouse::~Ladybirdplantlouse()
{
}

double Ladybirdplantlouse::compute(unsigned int i) const
{        
    switch (i) {
    case 0:
        return a * getValue(0) - b * getValue(0) * getValue(1);
    case 1:
        return b * d * getValue(0) * getValue(1) - e * getValue(1);
    default:
        Throw(utils::InternalError, boost::format(
                "Compute problem with Ladybirdplantlouse, i == %1%") % i );
    } 
}

}}} // namespace vle examples qss

DECLARE_NAMED_DYNAMICS(ladybirdplantlouse, vle::examples::qss::Ladybirdplantlouse)

