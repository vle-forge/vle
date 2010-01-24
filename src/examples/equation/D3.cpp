/**
 * @file examples/equation/D3.cpp
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


#include <vle/extension/difference-equation/Simple.hpp>

namespace vle { namespace examples { namespace equation {

    class D3 : public extension::DifferenceEquation::Simple
    {
        Var d;
        Sync c;

    public:
        D3(const devs::DynamicsInit& model,
           const devs::InitEventList& events) :
	    vle::extension::DifferenceEquation::Simple(model, events)
	    {
		d = createVar("d");
		c = createSync("c");
	    }

        virtual ~D3() { }

        virtual double compute(const devs::Time& /* time */)
	    {
		return d(-1) + c() + 1;
	    }

        virtual double initValue(const devs::Time& /* time */)
	    {
		return c() + 1;
	    }

    };

}}} // namespace vle examples equation

DECLARE_NAMED_DYNAMICS(D3, vle::examples::equation::D3)
