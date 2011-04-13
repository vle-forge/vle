/*
 * @file examples/fsa/Statechart.cpp
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


#include <vle/extension/fsa/Statechart.hpp>
#include <vle/extension/difference-equation/Base.hpp>

namespace vle { namespace examples { namespace fsa {

namespace ve = vle::extension;
namespace vf = vle::extension::fsa;
namespace vd = vle::devs;

enum State3 { ii = 1, aa, bb, cc, dd };

class statechart6 : public vf::Statechart
{
public:
    statechart6(const vd::DynamicsInit& init,
                const vd::InitEventList& events) :
        vf::Statechart(init, events)
    {
        states(this) << ii << aa << bb << cc << dd;

        transition(this, ii, aa);
        transition(this, aa, bb) << event("in1");
        transition(this, bb, aa) << event("in2");
        transition(this, bb, cc) << event("in1");
        transition(this, cc, bb) << event("in2");
        transition(this, cc, aa) << event("in1");
        transition(this, aa, cc) << event("in2");
        transition(this, cc, dd) << when(3.);
        transition(this, cc, dd) << when(69.);
        transition(this, dd, cc) << after(2.);

        initialState(ii);
    }

    virtual ~statechart6() { }
};

DECLARE_DYNAMICS(statechart6)

}}} // namespace vle examples fsa
