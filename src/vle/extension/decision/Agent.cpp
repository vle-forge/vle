/**
 * @file vle/extension/decision/Agent.cpp
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
#include <vle/utils/Debug.hpp>
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
        return 0.0;
    case Process:
        if (mNextChangeTime.second == devs::Time::negativeInfinity or
            mNextChangeTime.first == true) {
            return 0.0;
        } else {
            return mNextChangeTime.second - mCurrentTime;
        }
    case Output:
        return 0.0;
    case UpdateFact:
        return 0.0;
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

        if (port == "ack") {
            const std::string& activity((*it)->getStringAttributeValue("name"));
            const std::string& order((*it)->getStringAttributeValue("value"));

            if (order == "done") {
                setActivityDone(activity, time);
            } else if (order == "fail") {
                setActivityFailed(activity, time);
            } else {
                throw utils::ModellingError(
                    fmt(_("Decision: unknow order `%1%'")) % order);
            }
        } else {
            if (mPortMode) {
                const value::Value& value((*it)->getAttributeValue("value"));
                applyFact(port, value);
            } else {
                const std::string& fact((*it)->getStringAttributeValue("name"));
                const value::Value& value((*it)->getAttributeValue("value"));
                applyFact(fact, value);
            }
        }
    }

    mState = UpdateFact;
}

devs::Event::EventType Agent::confluentTransitions(
    const devs::Time& /*time*/,
    const devs::ExternalEventList& /*extEventlist*/) const
{
    return devs::Event::INTERNAL;
}

void Agent::request(const devs::RequestEvent& /*event*/,
                    const devs::Time& /*time*/,
                    devs::ExternalEventList& /*output*/) const
{
    throw utils::ModellingError(_("Decision: can not handle request events"));
}

value::Value* Agent::observation(
    const devs::ObservationEvent& /*event*/) const
{
    return 0;
}

void Agent::finish()
{
}

}}} // namespace vle extension decision

