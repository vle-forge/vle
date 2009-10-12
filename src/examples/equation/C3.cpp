/**
 * @file examples/equation/C3.cpp
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


#include <vle/extension/DifferenceEquation.hpp>

namespace vle { namespace examples { namespace equation {

    class C3 : public extension::DifferenceEquation::Simple
    {
        Var c;
        Sync b;

    public:
        C3(const devs::DynamicsInit& model,
           const devs::InitEventList& events) :
	    vle::extension::DifferenceEquation::Simple(model, events)
	    {
		c = createVar("c");
		b = createSync("b");
	    }

        virtual ~C3() { }

        virtual double compute(const devs::Time& /* time */)
	    {
		return c(-1) + b() + 1;
	    }

        virtual double initValue(const devs::Time& /* time */)
	    {
		return b() - 1;
	    }

    };

}}} // namespace vle examples equation

DECLARE_NAMED_DYNAMICS(C3, vle::examples::equation::C3)
