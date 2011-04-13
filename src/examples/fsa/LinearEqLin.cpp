/*
 * @file examples/fsa/Linear.cpp
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


#include <vle/extension/difference-equation/Simple.hpp>
#include <vle/extension/difference-equation/Multiple.hpp>

namespace vd = vle::devs;
namespace ve = vle::extension;
namespace vv = vle::value;

namespace vle { namespace examples { namespace fsa {

class EqLin : public ve::DifferenceEquation::Multiple
{
public:
    EqLin(const vd::DynamicsInit& init, const vd::InitEventList& evts)
        : ve::DifferenceEquation::Multiple(init, evts)
    {
        a = vv::toDouble(evts.get("a"));
        b = vv::toDouble(evts.get("b"));
        Y = createVar("Y");
        A = createVar("A");
        B = createVar("B");
        X = createSync("X");
    }

    virtual ~EqLin()
    {}

    virtual void compute(const vd::Time& /*time*/)
    {
        A = A(-1);
        B = B(-1);
        Y = A() * X() + B();
    }

    virtual void initValue(const vd::Time& /*time*/)
    {
        A = a;
        B = b;
        Y = A() * X() + B();
    }

private:
    double a;
    double b;
    Var Y;
    Var A;
    Var B;
    Sync X;
};

DECLARE_DYNAMICS(EqLin)

}}} // namespace vle examples fsa
