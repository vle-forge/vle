/**
 * @file vle/extension/decision/Activities.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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


#include <vle/extension/decision/Activities.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <numeric>

namespace vle { namespace extension { namespace decision {

Activity& Activities::add(const std::string& name, const Activity& act)
{
    iterator it(m_lst.find(name));

    if (it == m_lst.end()) {
        throw utils::ArgError(_("Decision: activity already exist"));
    }

    return (*m_lst.insert(std::make_pair < std::string, Activity >(
                name, act)).first).second;
}

Activity& Activities::add(const std::string& name)
{
    iterator it(m_lst.find(name));

    if (it != m_lst.end()) {
        throw utils::ArgError(_("Decision: activity already exist"));
    }

    return (*m_lst.insert(std::make_pair < std::string, Activity >(
                name, Activity())).first).second;
}

Activity& Activities::add(const std::string& name,
                          const devs::Time& start,
                          const devs::Time& end)
{
    Activity& result = add(name);
    result.initStartTimeFinishTime(start, end);

    return result;
}

void Activities::addStartToStartConstraint(const std::string& acti,
                                           const std::string& actj,
                                           const devs::Time& timelag)
{
    addPrecedenceConstraint(
        PrecedenceConstraint(get(acti), get(actj), PrecedenceConstraint::SS,
                             timelag, timelag));
}

void Activities::addFinishToStartConstraint(const std::string& acti,
                                            const std::string& actj,
                                            const devs::Time& mintimelag,
                                            const devs::Time& maxtimelag)
{
    addPrecedenceConstraint(
        PrecedenceConstraint(get(acti), get(actj), PrecedenceConstraint::FS,
                             mintimelag, maxtimelag));
}

void Activities::addFinishToStartConstraint(const std::string& acti,
                                            const std::string& actj,
                                            const devs::Time& mintimelag)
{
    addPrecedenceConstraint(
        PrecedenceConstraint(get(acti), get(actj), PrecedenceConstraint::FS,
                             mintimelag, devs::Time::infinity));
}

void Activities::addFinishToFinishConstraint(const std::string& acti,
                                             const std::string& actj,
                                             const devs::Time& timelag)
{
    addPrecedenceConstraint(
        PrecedenceConstraint(get(acti), get(actj), PrecedenceConstraint::FF,
                             timelag, timelag));
}

void Activities::process(const devs::Time& time, result_t& waitedact,
                         result_t& startedact, result_t& doneact,
                         result_t& failedact, result_t& endedact)
{
    bool update = false;

    do {
        waitedact.clear();
        startedact.clear();
        doneact.clear();
        failedact.clear();
        endedact.clear();

        for (iterator activity = begin(); activity != end(); ++activity) {
            switch (activity->second.state()) {
            case Activity::WAIT:
                update = processWaitState(activity, time, waitedact,
                                          startedact, doneact, failedact,
                                          endedact);
                break;

            case Activity::STARTED:
                update = processStartedState(activity, time, waitedact,
                                             startedact, doneact, failedact,
                                             endedact);
                break;

            case Activity::DONE:
                update = processDoneState(activity, time, waitedact, startedact,
                                          doneact, failedact, endedact);
                break;

            case Activity::ENDED:
                update = processEndedState(activity, time, waitedact,
                                           startedact, doneact, failedact,
                                           endedact);
                break;

            case Activity::FAILED:
                update = processFailedState(activity, time, waitedact,
                                            startedact, doneact, failedact,
                                            endedact);
                break;

            default:
                throw utils::InternalError(_("Decision: unknow state"));
            }
        }
    } while (update);
}


bool Activities::processWaitState(iterator activity,
                                  const devs::Time& time, result_t& waitedact,
                                  result_t& startedact,
                                  result_t& /* doneact */,
                                  result_t& failedact,
                                  result_t& /* endedact */)
{
    PrecedenceConstraint::Result newstate = updateState(activity, time);
    bool update = false;

    switch (newstate) {
    case PrecedenceConstraint::Valid:
    case PrecedenceConstraint::Inapplicable:
        activity->second.start(time);
        startedact.push_back(activity);
        update = true;
        break;
    case PrecedenceConstraint::Wait:
        waitedact.push_back(activity);
        update = false;
        break;
    case PrecedenceConstraint::Failed:
        activity->second.fail(time);
        failedact.push_back(activity);
        update = true;
        break;
    }

    return update;
}

bool Activities::processStartedState(iterator activity,
                                     const devs::Time& time,
                                     result_t& /* waitedact */,
                                     result_t& startedact,
                                     result_t& /*doneact*/,
                                     result_t& failedact,
                                     result_t& /* endedact */)
{
    PrecedenceConstraint::Result newstate = updateState(activity, time);
    bool update = false;

    switch (newstate) {
    case PrecedenceConstraint::Valid:
    case PrecedenceConstraint::Inapplicable:
    case PrecedenceConstraint::Wait:
        startedact.push_back(activity);
        update = false;
        break;
    case PrecedenceConstraint::Failed:
        activity->second.fail(time);
        failedact.push_back(activity);
        update = true;
        break;
    }

    return update;
}

bool Activities::processDoneState(iterator activity,
                                  const devs::Time& time,
                                  result_t& /*waitedact*/,
                                  result_t& /*startedact*/,
                                  result_t& doneact,
                                  result_t& failedact,
                                  result_t& endedact)
{
    bool update = false;
    PrecedenceConstraint::Result newstate = updateState(activity, time);

    switch (newstate) {
    case PrecedenceConstraint::Valid:
    case PrecedenceConstraint::Inapplicable:
        activity->second.end(time);
        endedact.push_back(activity);
        update = true;
        break;
    case PrecedenceConstraint::Wait:
        doneact.push_back(activity);
        update = false;
        break;
    case PrecedenceConstraint::Failed:
        activity->second.fail(time);
        failedact.push_back(activity);
        update = true;
        break;
    }

    return update;
}

bool Activities::processFailedState(iterator activity,
                                    const devs::Time& /* time */,
                                    result_t& /* waitedact */,
                                    result_t& /* startedact */,
                                    result_t& /* doneact */,
                                    result_t& failedact,
                                    result_t& /* endedact */)
{
    failedact.push_back(activity);
    return false;
}

bool Activities::processEndedState(iterator activity,
                                   const devs::Time& /* time */,
                                   result_t& /* waitedact */,
                                   result_t& /* startedact */,
                                   result_t& /* doneact */,
                                   result_t& /* failedact */,
                                   result_t& endedact)
{
    endedact.push_back(activity);
    return false;
}

PrecedenceConstraint::Result Activities::updateState(iterator activity,
                                                     const devs::Time& time)
{
    PrecedenceConstraint::Result newstate = PrecedenceConstraint::Valid;

    if (activity->second.isBeforeTimeConstraint(time)) {
        newstate = PrecedenceConstraint::Wait;
    } else if (activity->second.isAfterTimeConstraint(time)) {
        newstate = PrecedenceConstraint::Failed;
    } else {
        if (activity->second.validRules()) {
            PrecedencesGraph::findIn in = m_graph.findPrecedenceIn(activity);
            PrecedencesGraph::iteratorIn it;

            if (activity->second.waitAllFsBeforeStart()) {
                it = in.first;
                while (newstate == PrecedenceConstraint::Valid and
                       it != in.second) {
                    PrecedenceConstraint::Result r = it->isValid(time);

                    switch (r) {
                    case PrecedenceConstraint::Wait:
                        newstate = r;
                        break;
                    case PrecedenceConstraint::Failed:
                        newstate = r;
                        break;
                    case PrecedenceConstraint::Valid:
                    case PrecedenceConstraint::Inapplicable:
                        newstate = PrecedenceConstraint::Valid;
                        break;
                    }
                    ++it;
                }
            } else {
                it = in.first;
                while (it != in.second and
                       newstate != PrecedenceConstraint::Valid) {
                    PrecedenceConstraint::Result r = it->isValid(time);

                    switch (r) {
                    case PrecedenceConstraint::Wait:
                        newstate = r;
                        break;
                    case PrecedenceConstraint::Failed:
                        newstate = r;
                        break;
                    case PrecedenceConstraint::Valid:
                        newstate = r;
                        break;
                    case PrecedenceConstraint::Inapplicable:
                        newstate = r;
                        break;
                    }
                    ++it;
                }
            }
        } else {
            newstate = PrecedenceConstraint::Wait;
        }
    }
    return newstate;
}

}}} // namespace vle model decision

