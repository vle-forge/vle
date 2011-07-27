/*
 * @file examples/equation/Perturb6.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
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


#include <vle/extension/difference-equation/Base.hpp>
#include <vle/extension/fsa/Statechart.hpp>

namespace vle { namespace examples { namespace equation {

namespace ve = vle::extension;
namespace vf = vle::extension::fsa;
namespace vd = vle::devs;

enum State { A, B };

class Perturb6 : public vf::Statechart
{
public:
    Perturb6(const vd::DynamicsInit& init, const vd::InitEventList& events) :
        vf::Statechart(init, events)
	{
	    states(this) << A << B;

	    transition(this, A, B) << after(4.5)
                                   << send(&Perturb6::out1);
	    transition(this, B, A) << after(3.2)
                                   << send(&Perturb6::out2);

	    initialState(A);
	}

    virtual ~Perturb6() { }

    void out1(const vd::Time& /* time */, vd::ExternalEventList& output) const
	{
	    output << (ve::DifferenceEquation::Var("a") = 10);
	}

    void out2(const vd::Time& /* time */, vd::ExternalEventList& output) const
	{
	    output << (ve::DifferenceEquation::Var("a") = 0);
	}

};

}}} // namespace vle examples equation

DECLARE_DYNAMICS(vle::examples::equation::Perturb6)
