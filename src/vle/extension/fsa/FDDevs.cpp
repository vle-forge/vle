/*
 * @file vle/extension/fsa/FDDevs.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
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


#include <vle/extension/fsa/FDDevs.hpp>

namespace vle { namespace extension { namespace fsa {

void FDDevs::process(const devs::ExternalEvent* event,
                        const devs::Time& time)
{
    if (mExternals.find(currentState()) != mExternals.end() and
        (mExternals[currentState()].find(event->getPortName()) !=
         mExternals[currentState()].end())) {
        currentState(mExternals[currentState()]
                              [event->getPortName()]);

        if (mDurations.find(currentState()) == mDurations.end()) {
            throw utils::InternalError(
                fmt(_("FSA::FDDevs model, unknow duration of state %1%")) %
                currentState());
        }

        mSigma = mDurations[currentState()];
    }
    else mSigma -= time - mLastTime;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

void FDDevs::output(const devs::Time& time,
                       devs::ExternalEventList& output) const
{
    OutputFuncsIterator it = mOutputFuncs.find(currentState());

    if (it != mOutputFuncs.end()) {
        (it->second)(time, output);
    } else {
        OutputsIterator ito = mOutputs.find(currentState());

        if (ito != mOutputs.end()) {
            output.addEvent(buildEvent(ito->second));
        }
    }
}

devs::Time FDDevs::init(const devs::Time& /* time */)
{
    if (not isInit()) {
        throw utils::InternalError(
            _("FSA::FDDevs model, initial state not defined"));
    }

    currentState(initialState());

    if (mDurations.find(currentState()) == mDurations.end()) {
        throw utils::InternalError(fmt(
                _("FSA::FDDevs model, unknow duration of state %1%")) %
        currentState());
    }

    mSigma = mDurations[currentState()];
    return mSigma;
}

void FDDevs::externalTransition(const devs::ExternalEventList& events,
                                   const devs::Time& time)
{
    if (events.size() > 1) {
        devs::ExternalEventList sortedEvents = select(events);
        devs::ExternalEventList::const_iterator it = sortedEvents.begin();

        while (it != sortedEvents.end()) {
            process(*it, time);
            ++it;
        }
    } else {
        devs::ExternalEventList::const_iterator it = events.begin();

        process(*it, time);
    }
}

void FDDevs::internalTransition(const devs::Time& time)
{
    if (mInternals.find(currentState()) == mInternals.end()) {
        throw utils::InternalError(fmt(
                _("FSA::FDDevs model, unknow internal transition on state %1%"))
                % currentState());
    }

    currentState(mInternals[currentState()]);

    if (mDurations.find(currentState()) == mDurations.end()) {
        throw utils::InternalError(fmt(
                _("FSA::FDDevs model, unknow duration of state %1%")) %
            currentState());
    }

    mSigma = mDurations[currentState()];
    mLastTime = time;
}

}}} // namespace vle extension fsa
