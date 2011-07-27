/*
 * @file examples/equation/earlyPerturbFSA.cpp
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

/**
 * Notes : This unit test is used to fix a bug related to the perturbation
 * of DifferenceEquation::Multiple extension. When a dynamics based on
 * DifferenceEquation::Multiple extension is perturbated, a compute is called
 * even if all synchrone variables are not received. In this example,
 * at time 3, the variable fsa from E2 is perturbated before the synchrone
 * variable e1 is received. An error is raised if the compute function of E2
 * is called more than once.
 */

#include <vle/extension/difference-equation/Multiple.hpp>
#include <vle/extension/fsa/Statechart.hpp>

namespace vd = vle::devs;
namespace vv = vle::value;
namespace ve = vle::extension;

namespace vle { namespace examples { namespace equation {

class PerturbFSA : public ve::fsa::Statechart
{
public:
    PerturbFSA(const vd::DynamicsInit& init, const vd::InitEventList& evts)
        : ve::fsa::Statechart(init, evts)
    {
        states(this) << 0;
        initialState(0);
        transition(this, 0, 0)
            << when(3.0)
            << send(&PerturbFSA::out);
    }

    virtual ~PerturbFSA()
    { }

    void out(const vd::Time& /*time*/, vd::ExternalEventList& output) const
    {
        vd::ExternalEvent* evt = new vd::ExternalEvent("fsa");

        evt->putAttribute("name", new vv::String("fsa"));
        evt->putAttribute("value", new vv::Double(1.0));
        output.addEvent(evt);
    }
};

}}} // namespace vle::example::equation

DECLARE_DYNAMICS(vle::examples::equation::PerturbFSA)
