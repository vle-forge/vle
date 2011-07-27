/*
 * @file examples/fsa/Statechart9.cpp
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

enum State { a = 1, b, c };

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
            return vle::value::Integer::create(x);
        } else {
            return vf::Statechart::observation(event);
        }
    }

private:
    int x;

    void add(const vd::Time& /* time */)
    { ++x; }
    vle::devs::Time d(const vd::Time& /* time */)
    { return 5.; }
    void in(const vd::Time& /* time */, const vd::ExternalEvent* /* event */)
    { ++x; }
    void start(const vd::Time& /* time */)
    { x = 0; }
    bool c1(const vd::Time& /* time */)
    { return x % 2 == 0; }
};

DECLARE_DYNAMICS(statechart9)

}}} // namespace vle examples fsa
