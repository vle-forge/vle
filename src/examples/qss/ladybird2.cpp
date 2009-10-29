/**
 * @file examples/qss/ladybird2.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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

Ladybird2::Ladybird2(const devs::DynamicsInit& model,
                     const devs::InitEventList& events) :
    extension::DifferenceEquation::Simple(model, events)
{
    b = value::toDouble(events.get("b"));
    d = value::toDouble(events.get("d"));
    e = value::toDouble(events.get("e"));
    y = createVar("y");
    x = createSync("x");
}

Ladybird2::~Ladybird2()
{
}

double Ladybird2::compute(const vle::devs::Time& time)
{
    return y(-1) + timeStep(time) * (b * d * x(0) * y(-1) - e * y(-1));
}

}}} // namespace vle examples qss

DECLARE_NAMED_DYNAMICS(ladybird2, vle::examples::qss::Ladybird2)
