/*
 * @file examples/equation/C1.hpp
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


#ifndef EXAMPLES_EQUATION_C1_HPP
#define EXAMPLES_EQUATION_C1_HPP

#include <vle/extension/difference-equation/Simple.hpp>

namespace vle { namespace examples { namespace equation {

    class C1 : public extension::DifferenceEquation::Simple
    {
        Var c;
        Nosync b;

    public:
        C1(const devs::DynamicsInit& model,
           const devs::InitEventList& events);
        virtual ~C1() { }

        virtual double compute(const devs::Time& time);
    };

}}} // namespace vle examples equation

DECLARE_DYNAMICS(vle::examples::equation::C1)

#endif
