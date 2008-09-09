/**
 * @file src/examples/dess/R2.cpp
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

#include "R2.hpp"
#include <vle/utils/Debug.hpp>
#include <cmath>

namespace vle { namespace examples { namespace dess {

R2::R2(const graph::AtomicModel& model,
       const devs::InitEventList& evList):
    extension::DESS(model, evList)
{
    // birth and death rate
    m = value::toDouble(evList.get("m"));
    // rate at which infected individuals (I) recover (R)
    g = value::toDouble(evList.get("g"));
}

double R2::compute(const vle::devs::Time& /* time */) const
{    
    return g * getValue("I") - m * getValue();
} 

DECLARE_NAMED_DYNAMICS(R2, R2)

}}} // namespace vle examples dess
