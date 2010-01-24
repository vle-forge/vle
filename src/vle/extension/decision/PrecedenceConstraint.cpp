/**
 * @file vle/extension/decision/PrecedenceConstraint.cpp
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


#include <vle/extension/decision/PrecedenceConstraint.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>

namespace vle { namespace extension { namespace decision {

PrecedenceConstraint::PrecedenceConstraint(iterator first,
                                           iterator second,
                                           Type type,
                                           const devs::Time& mintimelag,
                                           const devs::Time& maxtimelag)
    : m_type(type), m_mintimelag(mintimelag), m_maxtimelag(maxtimelag),
    m_first(first), m_second(second)
{
    if (mintimelag < 0.0) {
        throw utils::ModellingError(
            _("Decision: Precedence constraint, bad min TL"));
    }

    if (mintimelag > maxtimelag) {
        throw utils::ModellingError(
            _("Decision: Precedence constraint, FS min >= max TL"));
    }
}

PrecedenceConstraint::Result
PrecedenceConstraint::isValid(const devs::Time& time) const
{
    const Activity& i(first()->second);
    const Activity& j(second()->second);

    switch (m_type) {
    case SS:
        switch (i.state()) {
        case Activity::Activity::WAIT:
            switch (j.state()) {
            case Activity::WAIT:
                return std::make_pair(Wait, devs::Time::infinity);
            case Activity::STARTED:
            case Activity::DONE:
            case Activity::ENDED:
                throw utils::InternalError(_("Decision: impossible state (SS)"));
            case Activity::FAILED:
                return std::make_pair(Failed, devs::Time::infinity);
            }
        case Activity::STARTED:
            switch (j.state()) {
            case Activity::WAIT:
                {
                    devs::Time dmin = i.startedDate() + mintimelag();
                    devs::Time dmax = i.startedDate() + maxtimelag();

                    if (i.startedDate() <= time and time == dmin and
                        time == dmax) {
                        return std::make_pair(Valid, dmin);
                    } else if (i.startedDate() <= time and time < dmin) {
                        return std::make_pair(Wait, dmin);
                    } else if (dmin <= time and time <= dmax) {
                        return std::make_pair(Valid, dmax);
                    } else if (dmax < time) {
                        return std::make_pair(Failed, devs::Time::infinity);
                    }
                }
            case Activity::STARTED:
            case Activity::DONE:
                return std::make_pair(Valid, devs::Time::infinity);
            case Activity::ENDED:
                return std::make_pair(Inapplicable, devs::Time::infinity);
            case Activity::FAILED:
                return std::make_pair(Failed, devs::Time::infinity);
            }
        case Activity::DONE:
            switch (j.state()) {
            case Activity::WAIT:
                {
                    devs::Time dmin = i.startedDate() + mintimelag();
                    devs::Time dmax = i.startedDate() + maxtimelag();

                    if (i.startedDate() <= time and time == dmin and
                        time == dmax) {
                        return std::make_pair(Valid, dmin);
                    } else if (i.startedDate() <= time and time < dmin) {
                        return std::make_pair(Wait, dmin);
                    } else if (dmin <= time and time <= dmax) {
                        return std::make_pair(Valid, dmax);
                    } else if (dmax < time) {
                        return std::make_pair(Failed, devs::Time::infinity);
                    }
                }
            case Activity::STARTED:
            case Activity::DONE:
                return std::make_pair(Valid, devs::Time::infinity);
            case Activity::ENDED:
                return std::make_pair(Inapplicable, devs::Time::infinity);
            case Activity::FAILED:
                return std::make_pair(Failed, devs::Time::infinity);
            }
        case Activity::ENDED:
            switch (j.state()) {
            case Activity::WAIT:
            case Activity::STARTED:
            case Activity::DONE:
            case Activity::ENDED:
                return std::make_pair(Inapplicable, devs::Time::infinity);
            case Activity::FAILED:
                return std::make_pair(Failed, devs::Time::infinity);
            }
        case Activity::FAILED:
            return std::make_pair(Failed, devs::Time::infinity);
        }
        break;

    case FF:
        switch (i.state()) {
        case Activity::WAIT:
            switch (j.state()) {
            case Activity::WAIT:
            case Activity::STARTED:
            case Activity::DONE:
            case Activity::ENDED:
                return std::make_pair(Inapplicable, devs::Time::infinity);
            case Activity::FAILED:
                return std::make_pair(Failed, devs::Time::infinity);
            }
        case Activity::STARTED:
            switch (j.state()) {
            case Activity::WAIT:
            case Activity::STARTED:
            case Activity::DONE:
            case Activity::ENDED:
                return std::make_pair(Inapplicable, devs::Time::infinity);
            case Activity::FAILED:
                return std::make_pair(Failed, devs::Time::infinity);
            }
        case Activity::DONE:
            switch (j.state()) {
            case Activity::WAIT:
            case Activity::STARTED:
            case Activity::DONE:
                {
                    devs::Time dmin = i.doneDate() + mintimelag();
                    devs::Time dmax = i.doneDate() + maxtimelag();

                    if (i.doneDate() <= time and time == dmin and
                        time == dmax) {
                        return std::make_pair(Valid, dmin);
                    } else if (i.doneDate() <= time and time < dmin) {
                        return std::make_pair(Wait, dmin);
                    } else if (dmin <= time and time <= dmax) {
                        return std::make_pair(Valid, dmax);
                    } else if (dmax < time) {
                        return std::make_pair(Failed, devs::Time::infinity);
                    }
                }
            case Activity::ENDED:
                return std::make_pair(Valid, devs::Time::infinity);
            case Activity::FAILED:
                return std::make_pair(Failed, devs::Time::infinity);
            }
        case Activity::ENDED:
            switch (j.state()) {
            case Activity::WAIT:
            case Activity::STARTED:
            case Activity::DONE:
            case Activity::ENDED:
                {
                    devs::Time dmin = i.doneDate() + mintimelag();
                    devs::Time dmax = i.doneDate() + maxtimelag();

                    if (i.doneDate() <= time and time == dmin and
                        time == dmax) {
                        return std::make_pair(Valid, dmin);
                    } else if (i.doneDate() <= time and time < dmin) {
                        return std::make_pair(Wait, dmin);
                    } else if (dmin <= time and time <= dmax) {
                        return std::make_pair(Valid, dmax);
                    } else if (dmax < time) {
                        return std::make_pair(Failed, devs::Time::infinity);
                    }
                }
            case Activity::FAILED:
                return std::make_pair(Failed, devs::Time::infinity);
            }
        case Activity::FAILED:
            return std::make_pair(Failed, devs::Time::infinity);
        }
        break;

    case FS:
        switch (i.state()) {
        case Activity::WAIT:
            switch (j.state()) {
            case Activity::WAIT:
                return std::make_pair(Wait, devs::Time::infinity);
            case Activity::STARTED:
            case Activity::DONE:
            case Activity::ENDED:
                return std::make_pair(Inapplicable, devs::Time::infinity);
            case Activity::FAILED:
                return std::make_pair(Failed, devs::Time::infinity);
            }
        case Activity::STARTED:
            switch (j.state()) {
            case Activity::WAIT:
                return std::make_pair(Wait, devs::Time::infinity);
            case Activity::STARTED:
            case Activity::DONE:
            case Activity::ENDED:
                return std::make_pair(Inapplicable, devs::Time::infinity);
            case Activity::FAILED:
                return std::make_pair(Failed, devs::Time::infinity);
            }
        case Activity::DONE:
            switch (j.state()) {
            case Activity::WAIT:
                return std::make_pair(Wait, devs::Time::infinity);
            case Activity::STARTED:
            case Activity::DONE:
            case Activity::ENDED:
                return std::make_pair(Inapplicable, devs::Time::infinity);
            case Activity::FAILED:
                return std::make_pair(Failed, devs::Time::infinity);
            }
        case Activity::ENDED:
            switch (j.state()) {
            case Activity::WAIT:
                {
                    devs::Time dmin = i.finishedDate() + mintimelag();
                    devs::Time dmax = i.finishedDate() + maxtimelag();

                    if (i.finishedDate() <= time and time == dmin and
                        time == dmax) {
                        return std::make_pair(Valid, devs::Time::infinity);
                    } else if (i.finishedDate() <= time and time < dmin) {
                        return std::make_pair(Wait, dmin);
                    } else if (dmin <= time and time <= dmax) {
                        return std::make_pair(Valid, devs::Time::infinity);
                    } else if (dmax < time) {
                        return std::make_pair(Failed, devs::Time::infinity);
                    }
                }
            case Activity::STARTED:
            case Activity::DONE:
            case Activity::ENDED:
                return std::make_pair(Inapplicable, devs::Time::infinity);
            case Activity::FAILED:
                return std::make_pair(Failed, devs::Time::infinity);
            }
        case Activity::FAILED:
            return std::make_pair(Failed, devs::Time::infinity);
        }
        break;
    }

    throw utils::InternalError(fmt(
            _("Decision: unknow precedence constraint %1%")) % (int)m_type);
}

}}} // namespace vle model decision

