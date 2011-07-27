/*
 * @file examples/fsa/Mealy1.cpp
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


#include <vle/extension/fsa/Mealy.hpp>

namespace vle { namespace examples { namespace fsa {

namespace vf = vle::extension::fsa;
namespace vd = vle::devs;

enum State { a = 1, b, c };

class mealy1 : public vf::Mealy
{
public:
    mealy1(const vd::DynamicsInit& init,
           const vd::InitEventList& events) :
        vf::Mealy(init, events)
    {
        states(this) << a << b << c;

        transition(this, a, c) << event("in1")
                               << action(&mealy1::action1)
                               << send(std::string("out1"));
        transition(this, a, c) << event("in2")
                               << send(std::string("out2"));
        transition(this, b, a) << event("in1")
                               << send(std::string("out1"));
        transition(this, c, a) << event("in2");
        transition(this, c, b) << event("in1");

        initialState(a);
    }

    void action1(const vd::Time& /*time*/,
                 const vd::ExternalEvent* /*event */) { }

    virtual vd::ExternalEventList select(
        const vd::ExternalEventList& events)
    {
        // in2 have a greater priority than in1
        if (events.front()->onPort("in1")) {
            vd::ExternalEventList list;

            list.push_back(events.back());
            list.push_back(events.front());
            return list;
        } else {
            return events;
        }
    }

    virtual ~mealy1() { }
};

DECLARE_DYNAMICS(mealy1)

}}} // namespace vle examples fsa
