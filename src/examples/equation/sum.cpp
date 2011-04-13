/*
 * @file examples/equation/sum.cpp
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


#ifndef EXAMPLES_EQUATION_SUM_HPP
#define EXAMPLES_EQUATION_SUM_HPP 1

#include <vle/extension/difference-equation/Generic.hpp>

namespace vle { namespace examples { namespace equation {

class sum : public extension::DifferenceEquation::Generic
{
public:
    sum(const devs::DynamicsInit& model,
        const devs::InitEventList& events) :
	extension::DifferenceEquation::Generic(model, events)
    { allSync(); }

    virtual ~sum() { }

    virtual double compute(const devs::Time& /* time */)
    {
        double s = 0;

        beginExt();
        while (not endExt()) {
            s += valueExt(0);
            nextExt();
        }
        return s;
    }

    virtual double initValue(const devs::Time& time)
    { return compute(time); }
};

}}} // namespace vle examples equation

DECLARE_DYNAMICS(vle::examples::equation::sum)

#endif
