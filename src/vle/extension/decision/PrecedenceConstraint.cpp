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

    if (type == SS or type == FF) {
        if (maxtimelag != mintimelag) {
            throw utils::ModellingError(
                _("Decision: Precedence constraint, SS or FF need "
                  "min = max TL"));
        }
    } else if (mintimelag > maxtimelag) {
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
                return Wait;
            case Activity::STARTED:
            case Activity::DONE:
                if (timelag() == 0.0) {
                    return Valid;
                } else {
                    return (j.startedDate() + timelag() > time) ? Wait : Valid;
                }
            case Activity::ENDED:
                return Inapplicable;
            case Activity::FAILED:
                return Failed;
            }
        case Activity::STARTED:
            switch (j.state()) {
            case Activity::WAIT:
                if (timelag() == 0.0) {
                    return Valid;
                } else {
                    return (i.startedDate() + timelag() > time) ? Wait : Valid;
                }
            case Activity::STARTED:
            case Activity::DONE:
                return Valid;
            case Activity::ENDED:
                return Inapplicable;
            case Activity::FAILED:
                return Failed;
            }
        case Activity::DONE:
            switch (j.state()) {
            case Activity::WAIT:
                if (timelag() == 0.0) {
                    return Valid;
                } else {
                    return (i.startedDate() + timelag() > time) ? Wait : Valid;
                }
            case Activity::STARTED:
            case Activity::DONE:
                return Valid;
            case Activity::ENDED:
                return Inapplicable;
            case Activity::FAILED:
                return Failed;
            }
        case Activity::ENDED:
            switch (j.state()) {
            case Activity::WAIT:
            case Activity::STARTED:
            case Activity::DONE:
            case Activity::ENDED:
                return Inapplicable;
            case Activity::FAILED:
                return Failed;
            }
        case Activity::FAILED:
            return Failed;
        }
        break;

    case FF:
        switch (i.state()) {
        case Activity::WAIT:
            switch (j.state()) {
            case Activity::WAIT:
            case Activity::STARTED:
                return Inapplicable;
            case Activity::DONE:
            case Activity::ENDED:
                return (j.doneDate() + timelag() >= time) ? Wait : Failed;
            case Activity::FAILED:
                return Failed;
            }
        case Activity::STARTED:
            switch (j.state()) {
            case Activity::WAIT:
            case Activity::STARTED:
                return Inapplicable;
            case Activity::DONE:
                return (j.doneDate() + timelag() >= time) ? Wait : Failed;
            case Activity::ENDED:
                return Wait;
            case Activity::FAILED:
                return Failed;
            }
        case Activity::DONE:
            switch (j.state()) {
            case Activity::WAIT:
                return (i.doneDate() + timelag() >= time) ? Wait : Failed;
            case Activity::STARTED:
                return (i.doneDate() + timelag() >= time) ? Wait : Failed;
            case Activity::DONE:
                return (i.doneDate() + timelag() >= time) ? Valid : Failed;
            case Activity::ENDED:
                return Valid;
            case Activity::FAILED:
                return Failed;
            }
        case Activity::ENDED:
            switch (j.state()) {
            case Activity::WAIT:
                return (i.doneDate() + timelag() >= time) ? Wait : Failed;
            case Activity::STARTED:
                return (i.doneDate() + timelag() >= time) ? Wait : Failed;
            case Activity::DONE:
                return (i.doneDate() + timelag() >= time) ? Valid: Failed;
            case Activity::ENDED:
                return (i.doneDate() + timelag() >= time) ? Valid: Failed;
            case Activity::FAILED:
                return Failed;
            }
        case Activity::FAILED:
            return Failed;
        }
        break;

    case FS:
        switch (i.state()) {
        case Activity::WAIT:
            switch (j.state()) {
            case Activity::WAIT:
                return Wait;
            case Activity::STARTED:
            case Activity::DONE:
            case Activity::ENDED:
                return Inapplicable;
            case Activity::FAILED:
                return Failed;
            }
        case Activity::STARTED:
            switch (j.state()) {
            case Activity::WAIT:
                return Wait;
            case Activity::STARTED:
            case Activity::DONE:
            case Activity::ENDED:
                return Inapplicable;
            case Activity::FAILED:
                return Failed;
            }
        case Activity::DONE:
            switch (j.state()) {
            case Activity::WAIT:
                return Wait;
            case Activity::STARTED:
            case Activity::DONE:
            case Activity::ENDED:
                return Inapplicable;
            case Activity::FAILED:
                return Failed;
            }
        case Activity::ENDED:
            switch (j.state()) {
            case Activity::WAIT:
                if (i.finishedDate() + mintimelag() > time) {
                    return Wait;
                } else {
                    if (i.finishedDate() + maxtimelag() >= time) {
                        return Valid;
                    }
                }
                return Failed;
            case Activity::STARTED:
            case Activity::DONE:
            case Activity::ENDED:
                return Inapplicable;
            case Activity::FAILED:
                return Failed;
            }
        case Activity::FAILED:
            return Failed;
        }
        break;
    }

    throw utils::InternalError(fmt(
            _("Decision: unknow precedence constraint %1%")) % (int)m_type);
}

}}} // namespace vle model decision

