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


#include <vle/extension/fsa/Statechart.hpp>
#include <vle/extension/difference-equation/Base.hpp>

namespace vle { namespace examples { namespace fsa {

namespace ve = vle::extension;
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
            << send(std::string("out"));
        transition(this, B, A) << event("in2");
        transition(this, B, C) << event("in1")
            << send(std::string("out"));
        transition(this, C, B) << event("in2");
        transition(this, C, A) << event("in1")
            << send(std::string("out"));
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
        transition(this, cc, dd) << after(2.);
        transition(this, dd, cc) << after(2.);

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
        transition(this, cc, dd) << when(3.);
        transition(this, cc, dd) << when(69.);
        transition(this, dd, cc) << after(2.);

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
        transition(this, b, c) << after(&statechart7::after1);

        initialState(a);
    }

    virtual ~statechart7() { }

private:
    vd::Time after1(const vd::Time& /* time */)
    { return 4; }
};

class statechart8 : public vf::Statechart
{
public:
    statechart8(const vd::DynamicsInit& init,
                const vd::InitEventList& events) :
        vf::Statechart(init, events)
    {
        states(this) << a;
        states(this) << b;
        states(this) << c;

        transition(this, a, b) << action(&statechart8::start);
        transition(this, b, c) << guard(&statechart8::c1);
        transition(this, c, b) << action(&statechart8::add)
            << send(std::string("out"));

        eventInState(this, "in", &statechart8::in) >> b;

        initialState(a);
    }

    virtual ~statechart8() { }

private:
    int a;

    void add(const vd::Time& /* time */)
    { ++a; }
    void in(const vd::Time& /* time */, const vd::ExternalEvent* /* event */)
    { ++a; }
    void start(const vd::Time& /* time */)
    { a = 0; }
    bool c1(const vd::Time& /* time */)
    { return a % 2 == 0; }
};

class statechart9 : public vf::Statechart
{
public:
    statechart9(const vd::DynamicsInit& init,
                const vd::InitEventList& events) :
        vf::Statechart(init, events)
    {
        states(this) << a << b;

        transition(this, a, b) << action(&statechart9::start);
        transition(this, b, b) << after(&statechart9::d)
            << guard(&statechart9::c1)
            << send(std::string("out"));

        state(this, b) << eventInState("in", &statechart9::in);

        initialState(a);
    }

    virtual ~statechart9() { }

    virtual vle::value::Value* observation(
        const vd::ObservationEvent& event) const
    {
        if (event.onPort("a")) {
            return vle::value::Integer::create(a);
        } else {
            return vf::Statechart::observation(event);
        }
    }

private:
    int a;

    void add(const vd::Time& /* time */)
    { ++a; }
    vle::devs::Time d(const vd::Time& /* time */)
    { return 5.; }
    void in(const vd::Time& /* time */, const vd::ExternalEvent* /* event */)
    { ++a; }
    void start(const vd::Time& /* time */)
    { a = 0; }
    bool c1(const vd::Time& /* time */)
    { return a % 2 == 0; }
};

typedef std::pair < double, double > param_pair;
typedef std::map < double, param_pair > param_pair_map;

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

DECLARE_NAMED_DYNAMICS(statechart1, statechart1)
DECLARE_NAMED_DYNAMICS(statechart2, statechart2)
DECLARE_NAMED_DYNAMICS(statechart3, statechart3)
DECLARE_NAMED_DYNAMICS(statechart4, statechart4)
DECLARE_NAMED_DYNAMICS(statechart5, statechart5)
DECLARE_NAMED_DYNAMICS(statechart6, statechart6)
DECLARE_NAMED_DYNAMICS(statechart7, statechart7)
DECLARE_NAMED_DYNAMICS(statechart8, statechart8)
DECLARE_NAMED_DYNAMICS(statechart9, statechart9)
DECLARE_NAMED_DYNAMICS(StagesThreshold, StagesThreshold)

}}} // namespace vle examples fsa
