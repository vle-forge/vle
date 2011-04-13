/*
 * @file examples/fsa/Moore.cpp
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


#include <vle/extension/fsa/Moore.hpp>

using namespace boost::assign;

namespace vle { namespace examples { namespace fsa {

namespace vf = vle::extension::fsa;
namespace vd = vle::devs;

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

DECLARE_DYNAMICS(counter2)

}}} // namespace vle examples fsa
