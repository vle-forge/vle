/**
 * @file examples/fsa/Statechart.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
 * Copyright (C) 2003-2009 ULCO http://www.univ-littoral.fr
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

using namespace boost::assign;

namespace vle { namespace examples { namespace fsa {

namespace vf = vle::extension::fsa;
namespace vd = vle::devs;

enum State { a = 1, b, c };

class statechart1 : public vf::Statechart
{
public:
    statechart1(const vd::DynamicsInit& init,
                const vd::InitEventList& events) :
        vf::Statechart(init, events)
    {
        states(this) << a;

        initialState(a);
    }

    virtual ~statechart1() { }
};

enum State2 { I = 1, A, B, C };

class statechart2 : public vf::Statechart
{
public:
    statechart2(const vd::DynamicsInit& init,
                const vd::InitEventList& events) :
        vf::Statechart(init, events)
    {
        states(this) << I << A << B << C;

        transition(this, I, A);
        transition(this, A, B) << event("in1");
        transition(this, B, A) << event("in2");
        transition(this, B, C) << event("in1");
        transition(this, C, B) << event("in2");
        transition(this, C, A) << event("in1");
        transition(this, A, C) << event("in2");

        initialState(I);
    }

    virtual ~statechart2() { }
};

class statechart3 : public vf::Statechart
{
public:
    statechart3(const vd::DynamicsInit& init,
                const vd::InitEventList& events) :
        vf::Statechart(init, events)
    {
        states(this) << I << A << B << C;

        transition(this, I, A);
        transition(this, A, B) << event("in1")
                               << output("out");
        transition(this, B, A) << event("in2");
        transition(this, B, C) << event("in1")
                               << output("out");
        transition(this, C, B) << event("in2");
        transition(this, C, A) << event("in1")
                               << output("out");
        transition(this, A, C) << event("in2");

        initialState(I);
    }

    virtual ~statechart3() { }
};

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
                               << output("out");
        transition(this, B, A) << event("in2")
                               << action(&statechart4::add);
        transition(this, B, C) << event("in1")
                               << output("out");
        transition(this, C, B) << event("in2")
                               << action(&statechart4::add);
        transition(this, C, A) << event("in1")
                               << output("out");
        transition(this, A, C) << event("in2")
                               << action(&statechart4::add);

        inAction(this, &statechart4::start) >> I;
        outAction(this, &statechart4::add) >> B;
        eventInState(this, "in3", &statechart4::in) >> C;

        initialState(I);
    }

    virtual ~statechart4() { }

    virtual vle::value::Value* observation(
        const vd::ObservationEvent& event) const
    {
        if (event.onPort("a")) {
            return vle::value::Double::create(a);
        } else {
            return vf::Statechart::observation(event);
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

class statechart5 : public vf::Statechart
{
public:
    statechart5(const vd::DynamicsInit& init,
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
        transition(this, cc, dd) << after(2);
        transition(this, dd, cc) << after(2);

        initialState(ii);
    }

    virtual ~statechart5() { }
};

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
        transition(this, cc, dd) << when(3);
        transition(this, cc, dd) << when(69);
        transition(this, dd, cc) << after(2);

        initialState(ii);
    }

    virtual ~statechart6() { }
};

class statechart7 : public vf::Statechart
{
public:
    statechart7(const vd::DynamicsInit& init,
                const vd::InitEventList& events) :
        vf::Statechart(init, events)
    {
        states(this) << a << b << c;

	transition(this, a, b);
	transition(this, b, c) << after(4);

        initialState(a);
    }

    virtual ~statechart7() { }
};

DECLARE_NAMED_DYNAMICS(statechart1, statechart1)
DECLARE_NAMED_DYNAMICS(statechart2, statechart2)
DECLARE_NAMED_DYNAMICS(statechart3, statechart3)
DECLARE_NAMED_DYNAMICS(statechart4, statechart4)
DECLARE_NAMED_DYNAMICS(statechart5, statechart5)
DECLARE_NAMED_DYNAMICS(statechart6, statechart6)
DECLARE_NAMED_DYNAMICS(statechart7, statechart7)

}}} // namespace vle examples fsa
