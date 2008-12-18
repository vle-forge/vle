/**
 * @file src/examples/dess/sir.cpp
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

#include "sir.hpp"
#include <vle/value/Double.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace examples { namespace dess {

sir::sir(const graph::AtomicModel& model,
	 const devs::InitEventList &evList) :
    extension::QSS::Multiple(model,evList)
{
    r = value::toDouble(evList.get("r"));
    a = value::toDouble(evList.get("a"));

    S = createVar(0, "S");
    I = createVar(1, "I");
    R = createVar(2, "R");    
}

double sir::compute(unsigned int i, const devs::Time& /* time */) const
{
    switch(i){
    case 0: // S
	return -r * S() * I();
    case 1: // I
	return r * S() * I() - a * I();
    case 2: // R
	return a * I();
    default:
	Throw(utils::InternalError, boost::format(
		  "Compute problem with sir model, i == %1%") % i );
    }
}

DECLARE_NAMED_DYNAMICS(sir, sir)

}}} // namespace vle examples dess
