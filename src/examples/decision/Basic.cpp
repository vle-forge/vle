/*
 * @file examples/decision/Basic.cpp
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


#include <vle/devs/Dynamics.hpp>
#include <vle/devs/DynamicsDbg.hpp>
#include <vle/value/Boolean.hpp>

namespace vd = vle::devs;
namespace vv = vle::value;

namespace vle { namespace examples { namespace decision {

class Start : public vd::Dynamics
{
public:
    Start(const vd::DynamicsInit& mdl, const vd::InitEventList& evts)
        : vd::Dynamics(mdl, evts)
    {
        mStart = evts.getDouble("start");
    }

    virtual ~Start()
    {
    }

    virtual vd::Time init(const vd::Time& /*time*/)
    {
        return mStart;
    }

    virtual vd::Time timeAdvance() const
    {
        return 1.0;
    }

    virtual void output(const vd::Time& /*time*/,
                        vd::ExternalEventList& output) const
    {
	vd::ExternalEvent* ev = new vd::ExternalEvent("out");
	ev->putAttribute("value", new vv::Boolean(true));

        output.addEvent(ev);
    }

private:
    vd::Time mStart;
};

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

class Retarder : public vd::Dynamics
{
public:
    Retarder(const vd::DynamicsInit& model,
             const vd::InitEventList& events)
        : vd::Dynamics(model, events), mVal(0)
    {
    }

    virtual ~Retarder()
    {
    }

    virtual vd::Time timeAdvance() const
    {
        if (mVal == 0) {
            return vd::Time::infinity;
        } else {
            return 0.0;
        }
    }

    virtual void output(const vd::Time& /*time*/,
                        vd::ExternalEventList& output) const
    {
        if (mVal) {
            vd::ExternalEvent* evt = new vd::ExternalEvent("out");
            evt->putAttribute("value", mVal);
            output.addEvent(evt);
        }
    }

    virtual void internalTransition(const devs::Time& /*time*/)
    {
        mVal = 0;
    }

    virtual void externalTransition(const vd::ExternalEventList& events,
                                    const devs::Time& /*time*/)
    {
        for (vd::ExternalEventList::const_iterator it = events.begin();
             it != events.end(); ++it) {
            mVal = (*it)->getAttributeValue("value").clone();
        }
    }

    vv::Value*  mVal;
};

}}} // namespace vle examples decision

DECLARE_NAMED_DYNAMICS_DBG(Start, vle::examples::decision::Start)
DECLARE_NAMED_DYNAMICS_DBG(Counter, vle::examples::decision::Counter)
DECLARE_NAMED_DYNAMICS(Retarder, vle::examples::decision::Retarder)
