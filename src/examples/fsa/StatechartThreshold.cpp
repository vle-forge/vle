/*
 * @file examples/fsa/StatechartThreshold.cpp
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


#include <vle/extension/fsa/Statechart.hpp>
#include <vle/extension/difference-equation/Base.hpp>

namespace vle { namespace examples { namespace fsa {

namespace ve = vle::extension;
namespace vf = vle::extension::fsa;
namespace vd = vle::devs;

typedef std::pair < double, double > param_pair;
typedef std::map < double, param_pair > param_pair_map;

enum State2 { I = 1, A, B, C };

class StagesThreshold : public vf::Statechart
{
public:
    StagesThreshold(const vd::DynamicsInit& init,
                    const vd::InitEventList& events) :
        vf::Statechart(init, events)
    {
        mStages[25.0] = std::make_pair(1.0, -25.0);
        mStages[55.0] = std::make_pair(0.0, 30.0);
        mStages[64.0] = std::make_pair(-1.5, 126);

        states(this) << I << A << B << C;

        transition(this, I, A) << when(25.0)
            << send(&StagesThreshold::output0);

        state(this, A) << eventInState("Y", &StagesThreshold::in);

        transition(this, A, B) << guard(&StagesThreshold::c1)
            << send(&StagesThreshold::output0);

        transition(this, B, C) << when(64.0)
            << send(&StagesThreshold::output0);

        initialState(I);

        Y = 0.0;
    }

    virtual ~StagesThreshold() { }

    void output0(const vd::Time& time,
                 vd::ExternalEventList& output) const
    {
        param_pair_map::const_iterator it = mStages.find(time);

        output << (ve::DifferenceEquation::Var("A") = it->second.first);
        output << (ve::DifferenceEquation::Var("B") = it->second.second);
    }

    void in(const vd::Time& /* time */, const vd::ExternalEvent* event)
    { Y << ve::DifferenceEquation::Var("Y", event); }

    bool c1(const vd::Time& /* time */)
    { return Y >= 30; }

private:
    double Y;
    param_pair_map mStages;
};

DECLARE_DYNAMICS(StagesThreshold)

}}} // namespace vle examples fsa
