/*
 * @file examples/decision/OnlyAgent.cpp
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


#include <vle/extension/Decision.hpp>
#include <vle/devs/DynamicsDbg.hpp>
#include <vle/utils/Trace.hpp>
#include <boost/cast.hpp>
#include <sstream>

namespace vd = vle::devs;
namespace vv = vle::value;
namespace vmd = vle::extension::decision;

namespace vle { namespace examples { namespace decision {

class OnlyAgent : public vmd::Agent
{
public:
    OnlyAgent(const vd::DynamicsInit& mdl, const vd::InitEventList& evts)
        : vmd::Agent(mdl, evts), mStart(false)
    {
        addActivity("A", 0.0, 100.0).addOutputFunction(boost::bind(
            &OnlyAgent::aout, this, _1, _2, _3));
        addActivity("B", 0.5, 100.0).addOutputFunction(boost::bind(
            &OnlyAgent::aout, this, _1, _2, _3));
        addActivity("C", 0.6, 100.0).addOutputFunction(boost::bind(
            &OnlyAgent::aout, this, _1, _2, _3));
        addActivity("D", 0.9, 100.0).addOutputFunction(boost::bind(
            &OnlyAgent::aout, this, _1, _2, _3));
        addActivity("E", 1.1, 100.0).addOutputFunction(boost::bind(
            &OnlyAgent::aout, this, _1, _2, _3));
        addActivity("F", 1.9, 100.0).addOutputFunction(boost::bind(
            &OnlyAgent::aout, this, _1, _2, _3));
        addActivity("G", 1.95, 100.0).addOutputFunction(boost::bind(
            &OnlyAgent::aout, this, _1, _2, _3));
        addActivity("H", 2.1, 100.0).addOutputFunction(boost::bind(
            &OnlyAgent::aout, this, _1, _2, _3));
        addActivity("I", 9.9, 100.0).addOutputFunction(boost::bind(
            &OnlyAgent::aout, this, _1, _2, _3));
    }

    virtual ~OnlyAgent()
    {
    }

    virtual vv::Value* observation(const vd::ObservationEvent& evt) const
    {
        if (evt.onPort("text")) {
            std::ostringstream out;
            out << *this;

            return new vv::String(out.str());
        }

        return 0;
    }

    void aout(const std::string& /*name*/, const vmd::Activity& activity,
              vd::ExternalEventList& out)
    {
        if (activity.isInStartedState()) {
            vd::ExternalEvent* evt = new vd::ExternalEvent("out");
            out.addEvent(evt);
        }
    }

private:
    bool mStart;
};

class OnlyAgentPrecedenceConstraint : public vmd::Agent
{
public:
    OnlyAgentPrecedenceConstraint(const vd::DynamicsInit& mdl,
                                  const vd::InitEventList& evts)
        : vmd::Agent(mdl, evts), mStart(false)
    {
        vmd::Activity& a = addActivity("A", 0, devs::Time::infinity);
        a.addOutputFunction(
            boost::bind(&OnlyAgentPrecedenceConstraint::aout, this, _1, _2,
                        _3));

        vmd::Activity& b = addActivity("B", devs::Time::negativeInfinity,
                                       devs::Time::infinity);
        b.addOutputFunction(
            boost::bind(&OnlyAgentPrecedenceConstraint::aout, this, _1, _2,
                        _3));

        vmd::Activity& c = addActivity("C", devs::Time::negativeInfinity,
                                       devs::Time::infinity);
        c.addOutputFunction(
            boost::bind(&OnlyAgentPrecedenceConstraint::aout, this, _1, _2,
                        _3));

        vmd::Activity& d = addActivity("D", devs::Time::negativeInfinity,
                                       devs::Time::infinity);
        d.addOutputFunction(
            boost::bind(&OnlyAgentPrecedenceConstraint::aout, this, _1, _2,
                        _3));

        addStartToStartConstraint("A", "B", 1.0, devs::Time::infinity);
        addStartToStartConstraint("B", "C", 1.0, devs::Time::infinity);
        addStartToStartConstraint("C", "D", 8.0, devs::Time::infinity);
    }

    virtual ~OnlyAgentPrecedenceConstraint()
    {
    }

    virtual vv::Value* observation(const vd::ObservationEvent& evt) const
    {
        if (evt.onPort("text")) {
            std::ostringstream out;
            out << *this;

            return new vv::String(out.str());
        }

        return 0;
    }

    void aout(const std::string& /*name*/, const vmd::Activity& activity,
              vd::ExternalEventList& out)
    {
        if (activity.isInStartedState()) {
            vd::ExternalEvent* evt = new vd::ExternalEvent("out");
            out.addEvent(evt);
        }
    }

private:
    bool mStart;
};

class OnlyAgentWakeUp : public vmd::Agent
{
public:
    OnlyAgentWakeUp(const vd::DynamicsInit& mdl, const vd::InitEventList& evts)
        : vmd::Agent(mdl, evts), mWakeup(false)
    {
        addFact("wakeup", boost::bind(&OnlyAgentWakeUp::wakeup, this, _1));

        vmd::Activity& a = addActivity("A", 1.0, 100.0);

        a.addOutputFunction(boost::bind(
                &OnlyAgentWakeUp::aout, this, _1, _2, _3));

        vmd::Rule& r = addRule("r1");
        r.add(boost::bind(&OnlyAgentWakeUp::isWakeup, this));

        a.addRule("r1", r);

        vmd::Activity& b = addActivity("B", 6.0, 100.0);
        b.addOutputFunction(boost::bind(
                &OnlyAgentWakeUp::aout, this, _1, _2, _3));
    }

    virtual ~OnlyAgentWakeUp()
    {
    }

    void wakeup(const vv::Value& val)
    {
        if (not mWakeup) {
            mWakeup = val.toBoolean().value();
        }
    }

    bool isWakeup() const
    {
        return mWakeup;
    }

    void aout(const std::string& /*name*/, const vmd::Activity& activity,
              vd::ExternalEventList& out)
    {
        if (activity.isInStartedState()) {
            vd::ExternalEvent* evt = new vd::ExternalEvent("out");
            out.addEvent(evt);
        }
    }

private:
    bool mWakeup;
};

}}} // namespace vle examples decision

DECLARE_NAMED_DYNAMICS_DBG(
    OnlyAgent,
    vle::examples::decision::OnlyAgent)
DECLARE_NAMED_DYNAMICS_DBG(
    OnlyAgentPrecedenceConstraint,
    vle::examples::decision::OnlyAgentPrecedenceConstraint)
DECLARE_NAMED_DYNAMICS_DBG(
    OnlyAgentWakeUp,
    vle::examples::decision::OnlyAgentWakeUp)
