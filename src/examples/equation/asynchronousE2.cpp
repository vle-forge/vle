/*
 * @file examples/equation/asynchronousE2.cpp
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
 * Notes : these dynamics are developped to test, in the model
 * asynchronous.vpz,  if the DifferenceEquation extension handles correctly
 * multiple receipts of the same asynchronous variable at the same time.
 * In this unit test, the dynamic tested is E3, it receives sequentially
 * (at time 3.0 but in different bags) :
 * - the synchronous variable e0 and an asynchronous variable ae2
 * - (again) the asynchronous variable ae2
 * - the synchonous variable e1
 */

#include <vle/extension/difference-equation/Multiple.hpp>

namespace vd = vle::devs;
namespace vv = vle::value;
namespace ve = vle::extension;

namespace vle { namespace examples { namespace equation {

class E2 : public ve::DifferenceEquation::Multiple
{
public:
    E2(const vd::DynamicsInit& init, const vd::InitEventList& evts)
        :  ve::DifferenceEquation::Multiple(init,evts)
    {
        fsa = createVar("fsa");
        e2 = createVar("e2");
    }
    virtual ~E2()
    {
    }
    void compute(const vd::Time& /*time*/)
    {
        //can be perturbated
        fsa = 0;
        //other var
        e2 = fsa() + 1;
    }
    void initValue(const vd::Time& /*time*/)
    {
        fsa = 0;
        e2 = 0;
    }
    Var fsa;
    Var e2;
};

DECLARE_DYNAMICS(E2)

}}} // namespace vle examples equation

