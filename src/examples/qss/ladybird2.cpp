/**
 * @file examples/qss/ladybird2.cpp
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


#include "ladybird2.hpp"
#include <vle/utils/Debug.hpp>

namespace vle { namespace examples { namespace qss {

Ladybird2::Ladybird2(const graph::AtomicModel& model,
                     const devs::InitEventList& events) :
    extension::DifferenceEquation(model, events)
{
    b = value::toDouble(events.get("b"));
    d = value::toDouble(events.get("d"));
    e = value::toDouble(events.get("e"));
}

Ladybird2::~Ladybird2()
{
}

double Ladybird2::compute(const vle::devs::Time& /*time*/)
{
    return getValue() + getTimeStep() * (b * d * getValue("x") * getValue() - e *
					 getValue());
}

}}} // namespace vle examples qss

DECLARE_NAMED_DYNAMICS(ladybird2, vle::examples::qss::Ladybird2)
