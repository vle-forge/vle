/**
 * @file vle/extension/Statechart.hpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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

#include <vle/extension/Statechart.hpp>

namespace vle { namespace extension {

void Statechart::buildOutputs(int transition,
			      const devs::Time& time,
			      devs::ExternalEventList& output) const
{
    if (transition != -1) {
	OutputFuncsIterator itof = mOutputFuncs.find(transition);

	if (itof != mOutputFuncs.end()) {
	    (itof->second)(time, output);
	} else {
	    OutputsIterator ito = mOutputs.find(transition);

	    if (ito != mOutputs.end()) {
		output.addEvent(buildEvent(ito->second));
	    }
	}
    }
}

bool Statechart::checkGuard(int transition, const devs::Time& time)
{
    GuardsIterator itg = mGuards.find(transition);
    bool valid = true;

    if (itg != mGuards.end()) {
        valid = (itg->second)(time);
    }
    return valid;
}

void Statechart::checkGuards(const devs::Time& time)
{
    TransitionsMapIterator it = mTransitionsMap.find(currentState());

    if (it != mTransitionsMap.end()) {
        TransitionsIterator itt = it->second.begin();

        while (not mValidGuard and itt != it->second.end()) {
            GuardsIterator itg = mGuards.find(*itt);

            if (itg != mGuards.end()) {
                if ((itg->second)(time)) {
                    mToProcessGuard = std::make_pair(*itt, mNextStates[*itt]);
                    mValidGuard = true;
                    mPhase = PROCESSING;
                }
            } else {
                if (mEvents.find(*itt) == mEvents.end() and
                    mAfters.find(*itt) == mAfters.end() and
                    mWhens.find(*itt) == mWhens.end()) {
                    mToProcessGuard = std::make_pair(*itt, mNextStates[*itt]);
                    mValidGuard = true;
                    mPhase = PROCESSING;
                }
            }
            ++itt;
        }
    }
}

int Statechart::findTransition(const devs::ExternalEvent* event) const
{
    TransitionsMapIterator it = mTransitionsMap.find(currentState());
    int transition = -1;

    if (it != mTransitionsMap.end()) {
        TransitionsIterator itt = it->second.begin();

        while (transition == -1 and itt != it->second.end()) {
            EventsIterator ite = mEvents.find(*itt);

            if (ite != mEvents.end() and ite->second == event->getPortName()) {
                transition = ite->first;
            }
            ++itt;
        }
    }
    return transition;
}

void Statechart::process(const devs::Time& time,
			 const devs::ExternalEvent* event)
{
    int transition = findTransition(event);

    if (transition != -1) {
        int nextState = mNextStates[transition];

        if (checkGuard(transition, time)) {
            processOutStateAction(time);
            processEventTransitionAction(transition, time, event);
            currentState(nextState);
            processInStateAction(time);
            checkGuards(time);
        }
    } else {
        processEventInStateActions(time, event);
    }
}

void Statechart::process(const devs::Time& time,
			 int transitionId, int nextState)
{
    processOutStateAction(time);
    processGuardTransitionAction(transitionId, time);
    currentState(nextState);
    processInStateAction(time);
    checkGuards(time);
}

void Statechart::processActivities(const devs::Time& time)
{
    ActivitiesIterator it = mActivities.find(currentState());

    if (it != mActivities.end()) {
        (it->second)(time);
    }
}

void Statechart::processInStateAction(const devs::Time& time)
{
    ActionsIterator it = mInActions.find(currentState());

    if (it != mInActions.end()) {
        (it->second)(time);
    }
}

void Statechart::processOutStateAction(const devs::Time& time)
{
    ActionsIterator it = mOutActions.find(currentState());

    if (it != mOutActions.end()) {
        (it->second)(time);
    }
}

void Statechart::processEventTransitionAction(
    int transition,
    const devs::Time& time,
    const devs::ExternalEvent* event)
{
    EventTransitionActionsIterator it =
        mEventTransitionActions.find(transition);

    if (it != mEventTransitionActions.end()) {
        (it->second)(time, event);
    }
}

void Statechart::processGuardTransitionAction(
    int transition,
    const devs::Time& time)
{
    GuardTransitionActionsIterator it =
        mGuardTransitionActions.find(transition);

    if (it != mGuardTransitionActions.end()) {
        (it->second)(time);
    }
}

void Statechart::processEventInStateActions(
    const devs::Time& time,
    const devs::ExternalEvent* event)
{
    if (mEventInStateActions.find(currentState()) !=
        mEventInStateActions.end()) {
        EventInStateActions& actions = eventInStateActions(
            currentState());
        EventInStateActionsIterator it = actions.find(event->getPortName());

        if (it != actions.end()) {
            (it->second)(time, event);
        }
        checkGuards(time);
    }
}

void Statechart::setSigma(const devs::Time& time)
{
    devs::Time sigma = devs::Time::infinity;
    int id = -1;
    TransitionsMapIterator it = mTransitionsMap.find(currentState());

    if (it != mTransitionsMap.end()) {
        TransitionsIterator itt = it->second.begin();

        while (itt != it->second.end()) {
            AftersIterator ita = mAfters.find(*itt);

            if (ita != mAfters.end()) {
                if (ita->second < sigma) {
                    sigma = ita->second;
                    id = *itt;
                }
            } else {
                WhensIterator itw = mWhens.find(*itt);

                if (itw != mWhens.end()) {
                    devs::Time duration = itw->second - time;

                    if (duration > 0 and duration < sigma) {
                        sigma = duration;
                        id = *itt;
                    }
                } else {
		    if (mEvents.find(*itt) == mEvents.end() and
			mGuards.find(*itt) == mGuards.end()) {
			sigma = 0;
			id = *itt;
		    }
		}
            }
            ++itt;
        }
    }
    if (mTimeStep < sigma) {
        mSigma = mTimeStep;
        mValidAfterWhen = false;
    } else {
        mSigma = sigma;
        mToProcessAfterWhen = std::make_pair(id, mNextStates[id]);
        mValidAfterWhen = true;
    }
}

void Statechart::updateSigma(const devs::Time& time)
{
    mSigma -= time - mLastTime;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

void Statechart::output(const devs::Time& time,
			devs::ExternalEventList& output) const
{
    if (mPhase == PROCESSING) {
        if (not mToProcessEvents.empty()) {
            const devs::ExternalEventList* events =
                mToProcessEvents.front();
            const devs::ExternalEvent* event = events->front();

	    buildOutputs(findTransition(event), time, output);
        }
    } else {
	if (mValidAfterWhen) {
	    buildOutputs(mToProcessAfterWhen.first, time, output);
	}
    }
}

devs::Time Statechart::init(const devs::Time& time)
{
    Assert <utils::InternalError>(
        isInit(),
        _("FSA::Statechart model, initial state not defined"));

    currentState(initialState());

    processInStateAction(time);

    mPhase = IDLE;
    mValidGuard = false;
    checkGuards(time);

    setSigma(time);
    mLastTime = time;
    return mSigma;
}

void Statechart::externalTransition(
    const devs::ExternalEventList& events,
    const devs::Time& time)
{
    if (events.size() > 1) {
        devs::ExternalEventList sortedEvents = select(events);
        devs::ExternalEventList* clonedEvents =
            new devs::ExternalEventList;
        devs::ExternalEventList::const_iterator it = sortedEvents.begin();

        while (it != sortedEvents.end()) {
            clonedEvents->addEvent(cloneExternalEvent(*it));
            ++it;
        }
        mToProcessEvents.push_back(clonedEvents);
    } else {
        devs::ExternalEventList::const_iterator it = events.begin();
        devs::ExternalEventList* clonedEvents =
            new devs::ExternalEventList;

        clonedEvents->addEvent(cloneExternalEvent(*it));
        mToProcessEvents.push_back(clonedEvents);
    }
    updateSigma(time);
    mLastTime = time;
    mPhase = PROCESSING;
}

devs::Time Statechart::timeAdvance() const
{
    if (mPhase == IDLE) {
        return mSigma;
    } else {
        return 0;
    }
}

void Statechart::internalTransition(const devs::Time& time)
{
    if (mPhase == PROCESSING) {
        if (mValidGuard) {
            process(time, mToProcessGuard.first, mToProcessGuard.second);
            mValidGuard = false;
            if (mToProcessEvents.empty()) {
                mPhase = IDLE;
            }
        } else {
            if (not mToProcessEvents.empty()) {
                devs::ExternalEventList* events = mToProcessEvents.front();
                devs::ExternalEvent* event = events->front();

                process(time, event);

                events->erase(events->begin());
                delete event;

                if (events->empty()) {
                    mToProcessEvents.pop_front();
                    delete events;
                }
                if (mToProcessEvents.empty()) {
                    mPhase = IDLE;
                }
            }
        }
    } else if (mPhase == IDLE) {
        if (mValidAfterWhen) {
            process(time, mToProcessAfterWhen.first,
                    mToProcessAfterWhen.second);
            mValidAfterWhen = false;
        } else {
            processActivities(time);
            checkGuards(time);
        }
    }
    setSigma(time);
    mLastTime = time;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

After_t after(const devs::Time& duration)
{
    return After_t(duration);
}

When_t when(const devs::Time& date)
{
    return When_t(date);
}

}} // namespace vle extension
