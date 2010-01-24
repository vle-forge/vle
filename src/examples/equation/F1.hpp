/**
 * @file examples/equation/F1.hpp
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


#ifndef EXAMPLES_EQUATION_F1_HPP
#define EXAMPLES_EQUATION_F1_HPP

#include <vle/extension/difference-equation/Multiple.hpp>

namespace vle { namespace examples { namespace equation {

    class F1 : public extension::DifferenceEquation::Multiple
    {
        Var a;
        Var b;
        Var c;
        Nosync d;
        Sync g;

    public:
        F1(const devs::DynamicsInit& model,
           const devs::InitEventList& events) :
            extension::DifferenceEquation::Multiple(model, events)
        {
            a = createVar("a");
            b = createVar("b");
            c = createVar("c");
            d = createNosync("d");
            g = createSync("g");
        }
        virtual ~F1() { }

        virtual void compute(const devs::Time& time);
        virtual void initValue(const devs::Time& time);
    };

}}} // namespace vle examples equation

DECLARE_NAMED_DYNAMICS(F1, vle::examples::equation::F1)

#endif
