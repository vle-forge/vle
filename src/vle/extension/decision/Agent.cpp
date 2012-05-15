/*
 * @file vle/extension/decision/Agent.cpp
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


#include <vle/extension/decision/Agent.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/value/String.hpp>
#include <cassert>

namespace vle { namespace extension { namespace decision {

devs::Time Agent::init(const devs::Time& time)
{
    mState = Output;
    mCurrentTime = time;
    mNextChangeTime = processChanges(time);

    return 0.0;
}

void Agent::output(const devs::Time& time,
                   devs::ExternalEventList& output) const
{
    if (mState == Output) {
        mCurrentTime = time;

        {
            const Agent::ActivityList& lst = latestStartedActivities();
            Agent::ActivityList::const_iterator it = lst.begin();
            for (; it != lst.end(); ++it) {
                (*it)->second.output((*it)->first, output);
            }
        }
        {
            const Agent::ActivityList& lst = latestFailedActivities();
            Agent::ActivityList::const_iterator it = lst.begin();
            for (; it != lst.end(); ++it) {
                (*it)->second.output((*it)->first, output);
            }
        }
        {
            const Agent::ActivityList& lst = latestDoneActivities();
            Agent::ActivityList::const_iterator it = lst.begin();
            for (; it != lst.end(); ++it) {
                (*it)->second.output((*it)->first, output);
            }
        }
        {
            const Agent::ActivityList& lst = latestEndedActivities();
            Agent::ActivityList::const_iterator it = lst.begin();
            for (; it != lst.end(); ++it) {
                (*it)->second.output((*it)->first, output);
            }
        }
    }
}

devs::Time Agent::timeAdvance() const
{
    switch (mState) {
    case Init:
    case Output:
    case UpdateFact:
        return 0.0;
    case Process:
        if (mNextChangeTime.second == devs::Time::negativeInfinity or
            mNextChangeTime.first == true or
            haveActivityInLatestActivitiesLists()) {
            return 0.0;
        } else {
            return mNextChangeTime.second - mCurrentTime;
        }
    }

    throw utils::InternalError();
}

void Agent::internalTransition(const devs::Time& time)
{
    mCurrentTime = time;

    switch (mState) {
    case Output:
        clearLatestActivitiesLists();
    case Init:
    case UpdateFact:
        mNextChangeTime = processChanges(time);
        mState = Process;
        break;
    case Process:
        mState = Output;
        break;
    }
}

void Agent::externalTransition(
    const devs::ExternalEventList& events,
    const devs::Time& time)
{
    mCurrentTime = time;

    for (devs::ExternalEventList::const_iterator it = events.begin();
         it != events.end(); ++it) {
        const std::string& port((*it)->getPortName());
        const value::Map& atts = (*it)->getAttributes();

        if (port == "ack") {
            const std::string& activity(atts.getString("name"));
            const std::string& order(atts.getString("value"));

            if (order == "done") {
                setActivityDone(activity, time);
            } else if (order == "fail") {
                setActivityFailed(activity, time);
            } else {
                throw utils::ModellingError(
                    fmt(_("Decision: unknown order `%1%'")) % order);
            }
        } else {
            value::Map::const_iterator jt = atts.value().find("value");
            if (jt == atts.end()) {
                jt = atts.value().find("init");
            }

            if (jt == atts.end() or not jt->second) {
                throw utils::ModellingError(
                    fmt(_("Decision: no value in this message: `%1%'")) %
                    (*it));
            }

            if (mPortMode) {
                applyFact(port, *jt->second);
            } else {
                const std::string& fact((*it)->getStringAttributeValue("name"));
                applyFact(fact, *jt->second);
            }
        }
    }

    mState = UpdateFact;
}

void Agent::confluentTransitions(
    const devs::Time& time,
    const devs::ExternalEventList& extEventlist)
{
    internalTransition(time);
    externalTransition(extEventlist, time);
}

void Agent::request(const devs::RequestEvent& /*event*/,
                    const devs::Time& /*time*/,
                    devs::ExternalEventList& /*output*/) const
{
    throw utils::ModellingError(_("Decision: can not handle request events"));
}

value::Value* Agent::observation(
    const devs::ObservationEvent& event) const
{
    const std::string port = event.getPortName();

    if (port == "KnowledgeBase") {
        std::stringstream out;
        out << *this;
        return new value::String(out.str());
    } else if (port == "Activities") {
        std::stringstream out;
        out << activities();
        return new value::String(out.str());
    } else if ((port.compare(0, 9, "Activity_") == 0) and port.size() > 9) {
        std::string activity(port, 9, std::string::npos);
        const Activity& act(activities().get(activity)->second);
        std::stringstream out;
        out << act.state();
        return new value::String(out.str());
    } else if ((port.compare(0, 6, "Rules_") == 0) and port.size() > 6) {
        std::string rule(port, 6, std::string::npos);
        const Rule& ru(rules().get(rule));
        return new value::Boolean(ru.isAvailable());
    }
    return 0;
}

void Agent::finish()
{
}

}}} // namespace vle extension decision

