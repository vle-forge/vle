/*
 * @file examples/decision/DecisionCounter.cpp
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


#include <vle/devs/Dynamics.hpp>
#include <vle/devs/DynamicsDbg.hpp>
#include <vle/value/Boolean.hpp>

namespace vd = vle::devs;
namespace vv = vle::value;

namespace vle { namespace examples { namespace decision {

class Counter : public vd::Dynamics
{
public:
    Counter(const vd::DynamicsInit& model,
            const vd::InitEventList& events)
        : vd::Dynamics(model, events), mCounter(0)
    {
    }

    virtual ~Counter()
    {
    }

    virtual vd::Time init(const vd::Time /*time*/)
    {
        mCounter = 0;
        return vd::Time::infinity;
    }

    virtual void externalTransition(const vd::ExternalEventList& events,
                                    const devs::Time& /*time*/)
    {
        mCounter += events.size();
    }

    virtual vv::Value* observation(const vd::ObservationEvent& event) const
    {
        if (event.onPort("counter")) {
            return new vv::Integer(mCounter);
        }

        return vd::Dynamics::observation(event);
    }

private:
    int mCounter;
};

}}} // namespace vle examples decision

DECLARE_DYNAMICS_DBG(vle::examples::decision::Counter)
