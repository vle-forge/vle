/**
 * @file vle/extension/decision/KnowledgeBase.cpp
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


#include <vle/extension/decision/KnowledgeBase.hpp>
#include <vle/utils/i18n.hpp>
#include <algorithm>

namespace vle { namespace extension { namespace decision {

void KnowledgeBase::setActivityDone(const std::string& name,
                                    const devs::Time& date)
{
    Activities::iterator it(m_activities.get(name));

    if (not it->second.isInStartedState()) {
        throw utils::ArgError(fmt(
                _("Decision: activity '%1%' is not started")) % name);
    }

    Activities::result_t::iterator toDel = std::find(m_startedAct.begin(),
                                                     m_startedAct.end(), it);
    m_startedAct.erase(toDel);

    m_doneAct.push_back(it);
    it->second.done(date);
}

void KnowledgeBase::setActivityFailed(const std::string& name,
                                      const devs::Time& date)
{
    Activities::iterator it(m_activities.get(name));

    if (it->second.isInEndedState()) {
        throw utils::ArgError(fmt(
                _("Decision: activity '%1%' is already finish")) % name);
    }

    if (it->second.isInWaitState()) {
        Activities::result_t::iterator toDel = std::find(m_waitedAct.begin(),
                                                         m_waitedAct.end(),
                                                         it);
        m_waitedAct.erase(toDel);
    } else if (it->second.isInStartedState()) {
        Activities::result_t::iterator toDel = std::find(m_startedAct.begin(),
                                                         m_startedAct.end(),
                                                         it);
        m_startedAct.erase(toDel);
    } else if (it->second.isInDoneState()) {
        Activities::result_t::iterator toDel = std::find(m_doneAct.begin(),
                                                         m_doneAct.end(),
                                                         it);
        m_doneAct.erase(toDel);
    }

    m_failedAct.push_back(it);
    it->second.fail(date);
}

void KnowledgeBase::processChanges(const devs::Time& time)
{
    m_latestWaitedAct = m_waitedAct;
    m_latestStartedAct = m_startedAct;
    m_latestDoneAct = m_doneAct;
    m_latestFailedAct = m_failedAct;
    m_latestEndedAct = m_endedAct;

    m_waitedAct.clear();
    m_startedAct.clear();
    m_doneAct.clear();
    m_failedAct.clear();
    m_endedAct.clear();

    m_activities.process(time, m_waitedAct, m_startedAct, m_doneAct,
                         m_failedAct, m_endedAct);

    buildLatestList(m_waitedAct, m_latestWaitedAct);
    buildLatestList(m_startedAct, m_latestStartedAct);
    buildLatestList(m_doneAct, m_latestDoneAct);
    buildLatestList(m_failedAct, m_latestFailedAct);
    buildLatestList(m_endedAct, m_latestEndedAct);
}

void KnowledgeBase::buildLatestList(const Activities::result_t& in,
                                    Activities::result_t& out)
{

    if (out.empty()) {
        out = in;
    } else {
        Activities::result_t::iterator it;
        Activities::result_t::const_iterator jt;
        Activities::result_t result;

        for (it = out.begin(); it != out.end(); ++it) {
            jt = in.begin();
            bool found = false;

            while (not found and jt != in.end()) {
                if (*it == *jt) {
                    found = true;
                }
                ++jt;
            }

            if (not found) {
                result.push_back(*it);
            }
        }
        out = result;
    }
}

}}} // namespace vle model decision
