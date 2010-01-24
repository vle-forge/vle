/**
 * @file examples/dess/seir.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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

seir::seir(const devs::DynamicsInit& model,
	   const devs::InitEventList &evList) :
    extension::QSS::Multiple(model,evList)
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

    S = createVar(0, "S");
    E = createVar(1, "E");
    I = createVar(2, "I");
    R = createVar(3, "R");    
}

double seir::compute(unsigned int i, const devs::Time& time) const
{
    double b;

    switch(i){
    case 0: // S
	b = b0 * (1 + b1 * cos(2 * M_PI * time.getValue()));
	return m * N - m * S() - b * S() * I();
    case 1: // E
	b = b0 * (1 + b1 * cos(2 * M_PI * time.getValue()));
	return b * S() * I() - (m + a) * E();
    case 2: // I
	return a * E() - (m + g) * I();
    case 3: // R
	return g * I() - m * R();
    default:
	throw utils::InternalError(fmt(
		  "Compute problem with seir model, i == %1%") % i );
    }
}

DECLARE_NAMED_DYNAMICS(seir, seir)

}}} // namespace vle examples dess
