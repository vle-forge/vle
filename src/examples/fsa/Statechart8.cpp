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

enum State { a = 1, b, c };

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
    int x;

    void add(const vd::Time& /* time */)
    { ++x; }
    void in(const vd::Time& /* time */, const vd::ExternalEvent* /* event */)
    { ++x; }
    void start(const vd::Time& /* time */)
    { x = 0; }
    bool c1(const vd::Time& /* time */)
    { return x % 2 == 0; }
};

DECLARE_DYNAMICS(statechart8)

}}} // namespace vle examples fsa
