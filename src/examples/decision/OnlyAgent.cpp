/**
 * @file examples/decision/OnlyAgent.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
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

#include <vle/extension/decision/Agent.hpp>
#include <vle/extension/decision/Activity.hpp>
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


}}} // namespace vle examples decision

DECLARE_NAMED_DYNAMICS_DBG(OnlyAgent, vle::examples::decision::OnlyAgent)
DECLARE_NAMED_DYNAMICS_DBG(OnlyAgentPrecedenceConstraint,
                           vle::examples::decision::OnlyAgentPrecedenceConstraint)
