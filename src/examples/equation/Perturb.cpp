/*
 * @file examples/equation/Perturb.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
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

class Perturb : public vf::Statechart
{
public:
    Perturb(const vd::DynamicsInit& init, const vd::InitEventList& events) :
        vf::Statechart(init, events)
	{
	    states(this) << A << B;

	    transition(this, A, B) << after(5.)
                                   << send(&Perturb::out1);
	    transition(this, B, A) << after(5.)
                                   << send(&Perturb::out2);

	    initialState(A);
	}

    virtual ~Perturb() { }

    void out1(const vd::Time& /* time */, vd::ExternalEventList& output) const
	{
	    output << (ve::DifferenceEquation::Var("a") = 10);
	}

    void out2(const vd::Time& /* time */, vd::ExternalEventList& output) const
	{
	    output << (ve::DifferenceEquation::Var("a") = 0);
	}

};

class Perturb2 : public vf::Statechart
{
public:
    Perturb2(const vd::DynamicsInit& init, const vd::InitEventList& events) :
        vf::Statechart(init, events)
	{
	    states(this) << A << B;

	    transition(this, A, B) << after(5.)
                                   << send(&Perturb2::out1);
	    transition(this, B, A) << after(5.)
                                   << send(&Perturb2::out2);

	    initialState(A);
	}

    virtual ~Perturb2() { }

    void out1(const vd::Time& /* time */, vd::ExternalEventList& output) const
	{
	    output.addEvent(buildEvent("out1"));
	}

    void out2(const vd::Time& /* time */, vd::ExternalEventList& output) const
	{
	    output.addEvent(buildEvent("out2"));
	}

};

class Perturb3 : public vf::Statechart
{
public:
    Perturb3(const vd::DynamicsInit& init, const vd::InitEventList& events) :
        vf::Statechart(init, events)
	{
	    states(this) << A;

	    transition(this, A, A) << event("in1")
                                   << send(&Perturb3::out1);
	    transition(this, A, A) << event("in2")
                                   << send(&Perturb3::out2);

	    initialState(A);
	}

    virtual ~Perturb3() { }

    void out1(const vd::Time& /* time */, vd::ExternalEventList& output) const
	{
	    output << (ve::DifferenceEquation::Var("a") = 10);
	}

    void out2(const vd::Time& /* time */, vd::ExternalEventList& output) const
	{
	    output << (ve::DifferenceEquation::Var("a") = 0);
	}

};

class Perturb4 : public vf::Statechart
{
public:
    Perturb4(const vd::DynamicsInit& init, const vd::InitEventList& events) :
        vf::Statechart(init, events)
	{
	    states(this) << A;

	    transition(this, A, A) << event("in1")
                                   << send(&Perturb4::out1);
	    transition(this, A, A) << event("in2")
                                   << send(&Perturb4::out2);

	    initialState(A);
	}

    virtual ~Perturb4() { }

    void out1(const vd::Time& /* time */, vd::ExternalEventList& output) const
	{
	    output << (ve::DifferenceEquation::Var("c") = 10);
	}

    void out2(const vd::Time& /* time */, vd::ExternalEventList& output) const
	{
	    output << (ve::DifferenceEquation::Var("c") = 0);
	}

};

class Perturb5 : public vf::Statechart
{
public:
    Perturb5(const vd::DynamicsInit& init, const vd::InitEventList& events) :
        vf::Statechart(init, events)
	{
	    states(this) << A;

	    transition(this, A, A) << event("in1")
                                   << send(&Perturb5::out1);
	    transition(this, A, A) << event("in2")
                                   << send(&Perturb5::out2);

	    initialState(A);
	}

    virtual ~Perturb5() { }

    void out1(const vd::Time& /* time */, vd::ExternalEventList& output) const
	{
	    output.addEvent(buildEvent("out1"));
	}

    void out2(const vd::Time& /* time */, vd::ExternalEventList& output) const
	{
	    output.addEvent(buildEvent("out2"));
	}

};

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

class Perturb7 : public vf::Statechart
{
public:
    Perturb7(const vd::DynamicsInit& init, const vd::InitEventList& events) :
        vf::Statechart(init, events)
	{
	    states(this) << A;

	    transition(this, A, A) << after(5.)
                                   << send(&Perturb7::out);

	    initialState(A);
	}

    virtual ~Perturb7() { }

    void out(const vd::Time& /* time */, vd::ExternalEventList& output) const
	{
	    vd::ExternalEvent* ee = new vd::ExternalEvent("out");

	    ee << vd::attribute("name", std::string("a"));
            ee << vd::attribute("value", 2.);
            ee << vd::attribute("type", ve::DifferenceEquation::ADD);
	    output.addEvent(ee);
	}

};

class Perturb8 : public vf::Statechart
{
public:
    Perturb8(const vd::DynamicsInit& init, const vd::InitEventList& events) :
        vf::Statechart(init, events)
	{
	    states(this) << A << B;

	    transition(this, A, B) << after(5.)
                                   << send(&Perturb8::out1);
	    transition(this, B, A) << after(5.)
                                   << send(&Perturb8::out2);

	    initialState(A);
	}

    virtual ~Perturb8() { }

    void out1(const vd::Time& /* time */, vd::ExternalEventList& output) const
	{
	    output << (ve::DifferenceEquation::Var("g") = 10);
	}

    void out2(const vd::Time& /* time */, vd::ExternalEventList& output) const
	{
	    output << (ve::DifferenceEquation::Var("g") = 0);
	}

};

class Perturb9 : public vf::Statechart
{
public:
    Perturb9(const vd::DynamicsInit& init, const vd::InitEventList& events) :
        vf::Statechart(init, events)
	{
	    states(this) << A;

	    transition(this, A, A) << event("in1")
                                   << send(&Perturb9::out1);
	    transition(this, A, A) << event("in2")
                                   << send(&Perturb9::out2);

	    initialState(A);
	}

    virtual ~Perturb9() { }

    void out1(const vd::Time& /* time */, vd::ExternalEventList& output) const
	{
	    output << (ve::DifferenceEquation::Var("g") = 10);
	}

    void out2(const vd::Time& /* time */, vd::ExternalEventList& output) const
	{
	    output << (ve::DifferenceEquation::Var("g") = 0);
	}

};

class Perturb10 : public vf::Statechart
{
public:
    Perturb10(const vd::DynamicsInit& init, const vd::InitEventList& events) :
        vf::Statechart(init, events)
	{
	    states(this) << A << B;

	    transition(this, A, B) << after(5.)
                                   << send(&Perturb10::out1);
	    transition(this, B, A) << after(5.)
                                   << send(&Perturb10::out2);

	    initialState(A);
	}

    virtual ~Perturb10() { }

    void out1(const vd::Time& /* time */, vd::ExternalEventList& output) const
	{
	    output << (ve::DifferenceEquation::Var("g") = 10);
	    output << (ve::DifferenceEquation::Var("b") = 10);
	}

    void out2(const vd::Time& /* time */, vd::ExternalEventList& output) const
	{
	    output << (ve::DifferenceEquation::Var("g") = 0);
	    output << (ve::DifferenceEquation::Var("b") = 0);
	}
};

}}} // namespace vle examples equation

DECLARE_NAMED_DYNAMICS(Perturb, vle::examples::equation::Perturb)
DECLARE_NAMED_DYNAMICS(Perturb2, vle::examples::equation::Perturb2)
DECLARE_NAMED_DYNAMICS(Perturb3, vle::examples::equation::Perturb3)
DECLARE_NAMED_DYNAMICS(Perturb4, vle::examples::equation::Perturb4)
DECLARE_NAMED_DYNAMICS(Perturb5, vle::examples::equation::Perturb5)
DECLARE_NAMED_DYNAMICS(Perturb6, vle::examples::equation::Perturb6)
DECLARE_NAMED_DYNAMICS(Perturb7, vle::examples::equation::Perturb7)
DECLARE_NAMED_DYNAMICS(Perturb8, vle::examples::equation::Perturb8)
DECLARE_NAMED_DYNAMICS(Perturb9, vle::examples::equation::Perturb9)
DECLARE_NAMED_DYNAMICS(Perturb10, vle::examples::equation::Perturb10)
