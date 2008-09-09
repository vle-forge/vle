/**
 * @file src/examples/dess/seir.cpp
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

#include "seir.hpp"
#include <vle/value/Double.hpp>
#include <vle/utils/Debug.hpp>
#include <cmath>

namespace vle { namespace examples { namespace dess {

seir::seir(const graph::AtomicModel& model,
	   const devs::InitEventList &evList) :
    extension::CombinedQss(model,evList)
{
    // birth and death rate
    m = value::toDouble(evList.get("m"));
    // rate at which infected individuals (I) recover (R)
    g = value::toDouble(evList.get("g"));
    // rate at which exposed individuals (E) become infected (I)
    a = value::toDouble(evList.get("a"));
    // contact rates
    b0 = value::toDouble(evList.get("b0"));
    b1 = value::toDouble(evList.get("b1"));
    // Population size
    N = value::toDouble(evList.get("N"));
}
	    
	    
double seir::compute(unsigned int i, const devs::Time& time) const
{
    double b;
    
    switch(i){
    case 0: // S
	b = b0 * (1 + b1 * cos(2 * M_PI * time.getValue()));
	return m * N - m * getValue(0) - b * getValue(0) * getValue(2);
    case 1: // E
	b = b0 * (1 + b1 * cos(2 * M_PI * time.getValue()));
	return b * getValue(0) * getValue(2) - (m + a) * getValue(1);
    case 2: // I
	return a * getValue(1) - (m + g) * getValue(2);
    case 3: // R
	return g * getValue(2) - m * getValue(3);
    default:
	Throw(utils::InternalError, boost::format(
		  "Compute problem with seir model, i == %1%") % i );
    }
}
	    
DECLARE_NAMED_DYNAMICS(seir, seir)
	
}}} // namespace vle examples dess
