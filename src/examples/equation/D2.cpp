/**
 * @file examples/equation/D2.cpp
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


#include <examples/equation/D2.hpp>

namespace vle { namespace examples { namespace equation {

// D(t)=B(t)+1
D2::D2(const devs::DynamicsInit& model,
       const devs::InitEventList& events) :
    extension::DifferenceEquation::Simple(model, events)
{
    d = createVar("d");
    b = createSync("b");
}

double D2::compute(const devs::Time& /* time */)
{
    return b(0) + 1;
}

double D2::initValue(const devs::Time& time)
{
    return compute(time);
}

}}} // namespace vle examples equation
