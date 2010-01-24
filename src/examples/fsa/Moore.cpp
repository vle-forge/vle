/**
 * @file examples/fsa/Moore.cpp
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


#include <vle/extension/fsa/Moore.hpp>

using namespace boost::assign;

namespace vle { namespace examples { namespace fsa {

namespace vf = vle::extension::fsa;
namespace vd = vle::devs;

enum State { a = 1, b, c };

class moore1 : public vf::Moore
{
public:
    moore1(const vd::DynamicsInit& init,
           const vd::InitEventList& events) :
        vf::Moore(init, events)
    {
        states(this) << a << b << c;

        transition(this, a, c) << event("in1");
        transition(this, a, b) << event("in2");
        transition(this, b, a) << event("in1");
        transition(this, c, a) << event("in2");
        transition(this, c, b) << event("in1");

        inAction(this, &moore1::action1) >> a;

        output(this, a) >> "out";
        output(this, c) >> "out";

        initialState(a);
    }

    virtual ~moore1() { }

    void action1(const vd::Time& /*time*/,
                 const vd::ExternalEvent* /*event */) {  }

    virtual vd::ExternalEventList select(
        const vd::ExternalEventList& events)
    {
        // in1 have a greater priority than in2
        if (events.front()->onPort("in2")) {
            vd::ExternalEventList list;

            list.push_back(events.back());
            list.push_back(events.front());
            return list;
        } else {
            return events;
        }
    }
};

enum State2 { START = 1, RUN };

class counter2 : public vf::Moore
{
public:
    counter2(const vd::DynamicsInit& init,
             const vd::InitEventList& events) :
        vf::Moore(init, events)
    {
        states(this) << START << RUN;

        transition(this, START, RUN) << event("in");
        transition(this, RUN, RUN) << event("in");

        inAction(this, &counter2::start) >> START;
        inAction(this, &counter2::run) >> RUN;

        outputFunc(this, &counter2::out) >> RUN;

        initialState(START);
    }

    void start(const vd::Time& /*time*/,
               const vd::ExternalEvent* /*event */)
    { value = 0; }
    void run(const vd::Time& /*time*/,
             const vd::ExternalEvent* /*event */)
    { ++value; }
    void out(const vd::Time& /*time*/,
             vd::ExternalEventList& output) const
    { output.addEvent(buildEventWithAInteger("out", "counter", value)); }

    virtual ~counter2() { }

    virtual vle::value::Value* observation(
        const vd::ObservationEvent& event) const
    {
        if (event.onPort("counter")) {
            return vle::value::Integer::create(value);
        }
        return vf::Moore::observation(event);
    }

private:
    int value;
};

DECLARE_NAMED_DYNAMICS(moore1, moore1)
DECLARE_NAMED_DYNAMICS(counter2, counter2)

}}} // namespace vle examples fsa
