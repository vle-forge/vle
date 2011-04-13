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

enum State2 { I = 1, A, B, C };

class statechart4 : public vf::Statechart
{
public:
    statechart4(const vd::DynamicsInit& init,
                const vd::InitEventList& events) :
        vf::Statechart(init, events)
    {
        states(this) << I << A << B << C;

        transition(this, I, A);
        transition(this, A, B) << event("in1")
            << send(std::string("out"));
        transition(this, B, A) << event("in2")
            << action(&statechart4::add);
        transition(this, B, C) << event("in1")
            << send(std::string("out"));
        transition(this, C, B) << event("in2")
            << action(&statechart4::add);
        transition(this, C, A) << event("in1")
            << send(std::string("out"));
        transition(this, A, C) << event("in2")
            << action(&statechart4::add);

        state(this, I) << inAction(&statechart4::start);
        // other syntax: inAction(this, &statechart4::start) >> I;
        state(this, B) << outAction(&statechart4::add);
        // other syntax: outAction(this, &statechart4::add) >> B;
        state(this, C) << eventInState("in3", &statechart4::in);
        // other syntax: eventInState(this, "in3", &statechart4::in) >> C;

        initialState(I);
    }

    virtual ~statechart4() { }

    virtual vle::value::Value* observation(
        const vd::ObservationEvent& event) const
    {
        if (event.onPort("a")) {
            return vle::value::Double::create(x);
        } else {
            return vf::Statechart::observation(event);
        }
    }

private:
    void add(const vd::Time& /* time */)
    { ++x; }

    void in(const vd::Time& /* time */,
            const vd::ExternalEvent* /* event */)
    { ++x; }

    void start(const vd::Time& /* time */)
    { x = 0; }

    int x;
};

DECLARE_DYNAMICS(statechart4)

}}} // namespace vle examples fsa
