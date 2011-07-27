/*
 * @file examples/fsa/StatechartListener.cpp
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

/**
 * Notes : these dynamics are developped to test, in the model
 * statechartMultipleSend.vpz,  if, during a transition, the send function
 * is called more than once ; that should not be the case in this example.
 */

#include <vle/extension/fsa/Statechart.hpp>

namespace vle { namespace examples { namespace fsa {

namespace ve = vle::extension;
namespace vf = vle::extension::fsa;
namespace vd = vle::devs;
namespace vv = vle::value;

class StatechartListener: public vf::Statechart
{
public:
    StatechartListener(const vd::DynamicsInit& init,
                       const vd::InitEventList& events) :
        vf::Statechart(init, events), mReceived(false)
    {
        //states
        states(this) << 0;
        //listened ports
        eventInState(this, "in", &StatechartListener::listen) >> 0;
        //init state
        initialState(0);
    }

    virtual ~StatechartListener()
    { }

private:
    void listen(const vd::Time& /*time*/, const vd::ExternalEvent* /*evt*/)
    {
        if (mReceived) {
            throw "error";
        }
        mReceived = true;
    }

    bool mReceived;
};

DECLARE_DYNAMICS(StatechartListener)

}}} // namespace vle examples fsa
