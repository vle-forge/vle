/*
 * @file examples/equation/E1.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
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


#include <examples/equation/E1.hpp>

namespace vle { namespace examples { namespace equation {

void E1::compute(const devs::Time& /* time */)
{
    a = a(-1) + 1;
    b = b(-1) + a(-1) + 1;
    c = c(-1) + b(-1) + 1;
}

void E1::initValue(const devs::Time& /* time */)
{
    a = 0.0;
    b = a(0) - 3;
    c = 0.0;
}

}}} // namespace vle examples equation
