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
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
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
#include <cassert>

namespace vle { namespace extension { namespace decision {

void KnowledgeBase::setActivityDone(const std::string& name,
                                    const devs::Time& date)
{
    Activities::iterator it(m_activities.get(name));

    if (not it->second.isInStartedState()) {
        throw utils::ArgError(fmt(
                _("Decision: activity '%1%' is not started")) % name);
    }

    m_activities.setDoneAct(it);
    it->second.done(date);
    it->second.acknowledge(name);
}

void KnowledgeBase::setActivityFailed(const std::string& name,
                                      const devs::Time& date)
{
    Activities::iterator it(m_activities.get(name));

    if (it->second.isInEndedState()) {
        throw utils::ArgError(fmt(
                _("Decision: activity '%1%' is already finish")) % name);
    }

    if (not it->second.isInFailedState()) {
        m_activities.setFailedAct(it);
        it->second.fail(date);
        it->second.acknowledge(name);
    }
}

devs::Time KnowledgeBase::duration(const devs::Time& time)
{
    devs::Time next = nextDate(time);

    if (next == devs::Time::negativeInfinity) {
        return 0.0;
    } else {
        assert(next >= time);
        return next - time;
    }
}

KnowledgeBase::Result KnowledgeBase::processChanges(const devs::Time& time)
{
    Activities::Result r = m_activities.process(time);
    return std::make_pair(r.first, r.second);
}

}}} // namespace vle model decision
