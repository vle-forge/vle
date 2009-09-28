/**
 * @file examples/fsa/Statechart.cpp
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

#include <vle/extension/Statechart.hpp>

using namespace boost::assign;

namespace vle { namespace examples { namespace fsa {

namespace ve = vle::extension;
namespace vd = vle::devs;

enum State { a = 1 };

class statechart1 : public ve::Statechart < State >
{
public:
    statechart1(const vd::DynamicsInit& init,
                const vd::InitEventList& events) :
        ve::Statechart < State >(init, events)
    {
        ve::states(this) << a;

        initialState(a);
    }

    virtual ~statechart1() { }
};

enum State2 { I = 1, A, B, C };

class statechart2 : public ve::Statechart < State2 >
{
public:
    statechart2(const vd::DynamicsInit& init,
                const vd::InitEventList& events) :
        ve::Statechart < State2 >(init, events)
    {
        ve::states(this) << I << A << B << C;

        ve::transition(this, I, A);
        ve::transition(this, A, B) << ve::event("in1");
        ve::transition(this, B, A) << ve::event("in2");
        ve::transition(this, B, C) << ve::event("in1");
        ve::transition(this, C, B) << ve::event("in2");
        ve::transition(this, C, A) << ve::event("in1");
        ve::transition(this, A, C) << ve::event("in2");

        initialState(I);
    }

    virtual ~statechart2() { }
};

class statechart3 : public ve::Statechart < State2 >
{
public:
    statechart3(const vd::DynamicsInit& init,
                const vd::InitEventList& events) :
        ve::Statechart < State2 >(init, events)
    {
        ve::states(this) << I << A << B << C;

        ve::transition(this, I, A);
        ve::transition(this, A, B) << ve::event("in1")
            << ve::output("out");
        ve::transition(this, B, A) << ve::event("in2");
        ve::transition(this, B, C) << ve::event("in1")
            << ve::output("out");
        ve::transition(this, C, B) << ve::event("in2");
        ve::transition(this, C, A) << ve::event("in1")
            << ve::output("out");
        ve::transition(this, A, C) << ve::event("in2");

        initialState(I);
    }

    virtual ~statechart3() { }
};

class statechart4 : public ve::Statechart < State2 >
{
public:
    statechart4(const vd::DynamicsInit& init,
                const vd::InitEventList& events) :
        ve::Statechart < State2 >(init, events)
    {
        ve::states(this) << I << A << B << C;

        ve::transition(this, I, A);
        ve::transition(this, A, B) << ve::event("in1")
            << ve::output("out");
        ve::transition(this, B, A) << ve::event("in2")
            << ve::action(&statechart4::add);
        ve::transition(this, B, C) << ve::event("in1")
            << ve::output("out");
        ve::transition(this, C, B) << ve::event("in2")
            << ve::action(&statechart4::add);
        ve::transition(this, C, A) << ve::event("in1")
            << ve::output("out");
        ve::transition(this, A, C) << ve::event("in2")
            << ve::action(&statechart4::add);

        ve::inAction(this, &statechart4::start) >> I;
        ve::outAction(this, &statechart4::add) >> B;
        ve::eventInState(this, "in3", &statechart4::in) >> C;

        initialState(I);
    }

    virtual ~statechart4() { }

    virtual vle::value::Value* observation(
        const vd::ObservationEvent& event) const
    {
        if (event.onPort("a")) {
            return vle::value::Double::create(a);
        } else {
            return ve::Statechart < State2 >::observation(event);
        }
    }

private:
    void add(const vd::Time& /* time */)
    { ++a; }
    void in(const vd::Time& /* time */,
            const vd::ExternalEvent* /* event */)
    { ++a; }
    void start(const vd::Time& /* time */)
    { a = 0; }

    int a;
};

enum State3 { ii = 1, aa, bb, cc, dd };

class statechart5 : public ve::Statechart < State3 >
{
public:
    statechart5(const vd::DynamicsInit& init,
                const vd::InitEventList& events) :
        ve::Statechart < State3 >(init, events)
    {
        ve::states(this) << ii << aa << bb << cc << dd;

        ve::transition(this, ii, aa);
        ve::transition(this, aa, bb) << ve::event("in1");
        ve::transition(this, bb, aa) << ve::event("in2");
        ve::transition(this, bb, cc) << ve::event("in1");
        ve::transition(this, cc, bb) << ve::event("in2");
        ve::transition(this, cc, aa) << ve::event("in1");
        ve::transition(this, aa, cc) << ve::event("in2");
        ve::transition(this, cc, dd) << ve::after(2);
        ve::transition(this, dd, cc) << ve::after(2);

        initialState(ii);
    }

    virtual ~statechart5() { }
};

class statechart6 : public ve::Statechart < State3 >
{
public:
    statechart6(const vd::DynamicsInit& init,
                const vd::InitEventList& events) :
        ve::Statechart < State3 >(init, events)
    {
        ve::states(this) << ii << aa << bb << cc << dd;

        ve::transition(this, ii, aa);
        ve::transition(this, aa, bb) << ve::event("in1");
        ve::transition(this, bb, aa) << ve::event("in2");
        ve::transition(this, bb, cc) << ve::event("in1");
        ve::transition(this, cc, bb) << ve::event("in2");
        ve::transition(this, cc, aa) << ve::event("in1");
        ve::transition(this, aa, cc) << ve::event("in2");
        ve::transition(this, cc, dd) << ve::when(3);
        ve::transition(this, cc, dd) << ve::when(69);
        ve::transition(this, dd, cc) << ve::after(2);

        initialState(ii);
    }

    virtual ~statechart6() { }
};

DECLARE_NAMED_DYNAMICS(statechart1, statechart1)
DECLARE_NAMED_DYNAMICS(statechart2, statechart2)
DECLARE_NAMED_DYNAMICS(statechart3, statechart3)
DECLARE_NAMED_DYNAMICS(statechart4, statechart4)
DECLARE_NAMED_DYNAMICS(statechart5, statechart5)
DECLARE_NAMED_DYNAMICS(statechart6, statechart6)

}}} // namespace vle examples fsa
