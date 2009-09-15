/**
 * @file vle/extension/decision/Activity.cpp
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


#include <vle/extension/decision/Activity.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <boost/format.hpp>

namespace vle { namespace extension { namespace decision {

bool Activity::validRules() const
{
    if (not m_rules.empty()) {
	Rules::result_t result = m_rules.apply();
	return not result.empty();
    }
    return true;
}

void Activity::initStartTimeFinishTime(const devs::Time& start,
                                       const devs::Time& finish)
{
    if (start > finish) {
        throw utils::ModellingError(
            _("Decision: temporal constraint start < finish"));
    }

    m_date = (DateType)(Activity::START | Activity::FINISH);
    m_start = start;
    m_finish = finish;
}

void Activity::initStartTimeFinishRange(const devs::Time& start,
                                        const devs::Time& minfinish,
                                        const devs::Time& maxfinish)
{
    if (not (start < minfinish and minfinish < maxfinish)) {
        throw utils::ModellingError(
            _("Decision: temporal constraint start < minfinish < maxfinish"));
    }

    m_date = (DateType)(Activity::START | Activity::MINF | Activity::MAXF);
    m_start = start;
    m_minfinish = minfinish;
    m_maxfinish = maxfinish;
}

void Activity::initStartRangeFinishTime(const devs::Time& minstart,
                                        const devs::Time& maxstart,
                                        const devs::Time& finish)
{
    if (not (minstart < maxstart and maxstart < finish)) {
        throw utils::ModellingError(
            _("Decision: temporal constraint start < minfinish < maxfinish"));
    }

    m_date = (DateType)(Activity::MINS | Activity::MAXS | Activity::FINISH);
    m_minstart = minstart;
    m_maxstart = maxstart;
    m_finish = finish;
}

void Activity::initStartRangeFinishRange(const devs::Time& minstart,
                                         const devs::Time& maxstart,
                                         const devs::Time& minfinish,
                                         const devs::Time& maxfinish)
{
    if (not (minstart < maxstart and maxstart < minfinish and minfinish <
             maxfinish)) {
        throw utils::ModellingError(
            _("Decision: temporal constraint minstart < maxstart < minfinish"
              " < maxfinish"));
    }

    m_date = (DateType)(Activity::MINF | Activity::MAXF | Activity::MINS |
                        Activity::MAXS);
    m_minstart = minstart;
    m_maxstart = maxstart;
    m_minfinish = minfinish;
    m_maxfinish = maxfinish;
}

bool Activity::isValidTimeConstraint(const devs::Time& time) const
{
    switch (m_date & (START | FINISH | MINS | MAXS | MINF | MAXF)) {
        // nothing declared
    case 0:
        return true;
        // only start / finish or start & finish
    case START:
        return m_start <= time;
    case FINISH:
        return time < m_finish;
    case START | FINISH:
        return m_start <= time and time < m_finish;

        // only start and min/max finish
    case START | MAXF:
        return m_start <= time and time < m_maxfinish;
    case START | MINF:
        return m_start <= time and time < m_minfinish;
    case START | MINF | MAXF:
        return m_start <= time and time <= m_minfinish;

        // only finish and min/max start
    case FINISH | MAXS:
        return time < m_maxstart ;
    case FINISH | MINS:
        return m_minstart <= time;
    case FINISH | MINS | MAXS:
        return m_minstart <= time and time < m_maxstart;

    case MAXF:
        return time < m_maxfinish;
    case MINF:
        return m_minstart <= time;
    case MINF | MAXF:
        return time < m_maxfinish;
    case MAXS:
    case MAXS | MAXF:
    case MAXS | MINF:
    case MAXS | MINF | MAXF:
        return time < m_maxstart;
    case MINS:
        return m_minstart <= time;
    case MINS | MAXF:
        return m_minstart <= time and time < m_maxfinish;
    case MINS | MINF:
        return m_minstart <= time and time < m_minfinish;
    case MINS | MINF | MAXF:
        return m_minstart <= time and time < m_maxfinish;
    case MINS | MAXS:
    case MINS | MAXS | MAXF:
    case MINS | MAXS | MINF:
    case MINS | MAXS | MINF | MAXF:
        return m_minstart <= time and time < m_maxstart;
    }

    throw utils::InternalError(fmt(
            _("Decision: activity time type invalid: %1%")) % (int)m_date);
}

bool Activity::isBeforeTimeConstraint(const devs::Time& time) const
{
    switch (m_date & (START | FINISH | MINS | MAXS | MINF | MAXF)) {
        // nothing declared
    case 0:
        return false;
        // only start / finish or start & finish
    case START:
        return time < m_start;
    case FINISH:
        return time < m_finish;
    case START | FINISH:
        return time < m_start;

        // only start and min/max finish
    case START | MAXF:
    case START | MINF:
    case START | MINF | MAXF:
        return time < m_start;

        // only finish and min/max start
    case FINISH | MAXS:
        return time < m_maxstart ;
    case FINISH | MINS:
    case FINISH | MINS | MAXS:
        return time < m_minstart;

    case MAXF:
        return time < m_maxfinish;
    case MINF:
    case MINF | MAXF:
        return time < m_minfinish;
    case MAXS:
    case MAXS | MAXF:
    case MAXS | MINF:
    case MAXS | MINF | MAXF:
        return time < m_maxstart;
    case MINS:
    case MINS | MAXF:
    case MINS | MINF:
    case MINS | MINF | MAXF:
    case MINS | MAXS:
    case MINS | MAXS | MAXF:
    case MINS | MAXS | MINF:
    case MINS | MAXS | MINF | MAXF:
        return time < m_minstart;
    }

    throw utils::InternalError(fmt(
            _("Decision: activity time type invalid: %1%")) % (int)m_date);
}

bool Activity::isAfterTimeConstraint(const devs::Time& time) const
{
    switch (m_date & (START | FINISH | MINS | MAXS | MINF | MAXF)) {
        // nothing declared
    case 0:
        return false;
        // only start / finish or start & finish
    case START:
        return time > m_start;
    case FINISH:
    case START | FINISH:
        return time > m_finish;

        // only start and min/max finish
    case START | MAXF:
        return time > m_maxfinish;
    case START | MINF:
        return time > m_minfinish;
    case START | MINF | MAXF:
        return time > m_maxfinish;;

        // only finish and min/max start
    case FINISH | MAXS:
    case FINISH | MINS:
    case FINISH | MINS | MAXS:
        return time > m_finish;

    case MAXF:
        return time > m_maxfinish;
    case MINF:
        return time > m_minfinish;
    case MINF | MAXF:
        return time > m_maxfinish;
    case MAXS:
    case MAXS | MAXF:
    case MAXS | MINF:
        return time > m_minfinish;
    case MAXS | MINF | MAXF:
        return time > m_maxfinish;
    case MINS:
        return time > m_minstart;
    case MINS | MAXF:
        return time > m_maxfinish;
    case MINS | MINF:
        return time > m_minfinish;
    case MINS | MINF | MAXF:
        return time > m_maxfinish;
    case MINS | MAXS:
        return time > m_maxstart;
    case MINS | MAXS | MAXF:
        return time > m_maxfinish;
    case MINS | MAXS | MINF:
        return time > m_minfinish;
    case MINS | MAXS | MINF | MAXF:
        return time > m_maxfinish;
    }

    throw utils::InternalError(fmt(
            _("Decision: activity time type invalid: %1%")) % (int)m_date);
}

}}} // namespace vle model decision

