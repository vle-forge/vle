/*
 * @file vle/extension/fsa/Statechart.cpp
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


#include <vle/extension/fsa/Statechart.hpp>

namespace vle { namespace extension { namespace fsa {

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
	    // transition without event/after/when
	    if (mEvents.find(*itt) == mEvents.end() and
		mAfters.find(*itt) == mAfters.end() and
		mWhens.find(*itt) == mWhens.end() and
		mAfterFuncs.find(*itt) == mAfterFuncs.end() and
		mWhenFuncs.find(*itt) == mWhenFuncs.end()) {
		GuardsIterator itg = mGuards.find(*itt);

		if (itg != mGuards.end()) { // guarded transition
		    if ((itg->second)(time)) {
			mToProcessGuard = std::make_pair(*itt,
							 mNextStates[*itt]);
			mValidGuard = true;
			mPhase = PROCESSING;
		    }
		} else { // automatic transition
                    mToProcessGuard = std::make_pair(*itt, mNextStates[*itt]);
                    mValidGuard = true;
                    mPhase = PROCESSING;
                }
            }
	    ++itt;
        }
    }
}

Statechart::Transitions Statechart::findTransition(
    const devs::ExternalEvent* event) const
{
    TransitionsMapIterator it = mTransitionsMap.find(currentState());
    Transitions transitions;

    if (it != mTransitionsMap.end()) {
        TransitionsIterator itt = it->second.begin();

        while (itt != it->second.end()) {
            EventsIterator ite = mEvents.find(*itt);

            if (ite != mEvents.end() and ite->second == event->getPortName()) {
                transitions.push_back(ite->first);
            }
            ++itt;
        }
    }
    return transitions;
}

bool Statechart::process(const devs::Time& time,
			 const devs::ExternalEvent* event)
{
    bool proceed = true;
    Transitions transitions = findTransition(event);

    mFiredTransition = -1;
    if (not transitions.empty()) {
        bool found = false;
        TransitionsIterator it = transitions.begin();

        while (not found and it != transitions.end()) {
            if (checkGuard(*it, time)) {
                processOutStateAction(time);
                processEventTransitionAction(*it, time, event);
                processGuardTransitionAction(*it, time);
                mFiredTransition = *it;
                found = true;
            }
            ++it;
        }
        if (not found) {
            proceed = false;
        }
    } else {
        proceed = processEventInStateActions(time, event);
    }
    return proceed;
}

void Statechart::process(const devs::Time& time,
			 int transitionId)
{
    processOutStateAction(time);
    processGuardTransitionAction(transitionId, time);
}

void Statechart::processActivities(const devs::Time& time)
{
    ActivitiesIterator it = mActivities.find(currentState());

    if (it != mActivities.end()) {
        (it->second)(time);
    }
}

void Statechart::processIn(const devs::Time& time, int nextState)
{
    currentState(nextState);
    processInStateAction(time);
    checkGuards(time);
    if (mValidGuard) {
        mSigma = 0;
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

bool Statechart::processEventInStateActions(
    const devs::Time& time,
    const devs::ExternalEvent* event)
{
    bool proceed = false;

    if (mEventInStateActions.find(currentState()) !=
        mEventInStateActions.end()) {
        EventInStateActions& actions = eventInStateActions(
            currentState());
        EventInStateActionsIterator it = actions.find(event->getPortName());

        if (it != actions.end()) {
            (it->second)(time, event);
            checkGuards(time);
            proceed = true;
        }
    }
    return proceed;
}

void Statechart::removeProcessEvent(devs::ExternalEventList* events,
				    devs::ExternalEvent* event)
{
    events->erase(events->begin());
    event->deleter();
    delete event;
    if (events->empty()) {
	mToProcessEvents.pop_front();
	delete events;
    }
}

void Statechart::setSigma(const devs::Time& time)
{
    devs::Time sigma = devs::Time::infinity;
    std::vector < int > id;
    TransitionsMapIterator it = mTransitionsMap.find(currentState());

    if (it != mTransitionsMap.end()) {
        TransitionsIterator itt = it->second.begin();

        while (itt != it->second.end()) {
            AftersIterator ita = mAfters.find(*itt);

            if (ita != mAfters.end()) {
                if (ita->second < sigma) {
                    sigma = ita->second;
                    id.clear();
                    id.push_back(*itt);
                } else if (ita->second == sigma) {
                    id.push_back(*itt);
                }
            } else {
                AfterFuncsIterator itaf = mAfterFuncs.find(*itt);

                if (itaf != mAfterFuncs.end()) {
                    double duration = (itaf->second)(time);

                    if (duration < sigma) {
                        sigma = duration;
                        id.clear();
                        id.push_back(*itt);
                    } else if (duration == sigma) {
                        id.push_back(*itt);
                    }
                } else {
                    WhensIterator itw = mWhens.find(*itt);

                    if (itw != mWhens.end()) {
                        devs::Time duration = itw->second - time;

                        if (duration > 0 and duration < sigma) {
                            sigma = duration;
                            id.clear();
                            id.push_back(*itt);
                        } else if (duration > 0 and duration == sigma) {
                            id.push_back(*itt);
                        }
                    } else {
                        WhenFuncsIterator itwf = mWhenFuncs.find(*itt);

                        if (itwf != mWhenFuncs.end()) {
                            devs::Time duration = (itwf->second)(time) - time;

                            if (duration > 0 and duration < sigma) {
                                sigma = duration;
                                id.clear();
                                id.push_back(*itt);
                            } else if (duration > 0 and duration == sigma) {
                                id.push_back(*itt);
                            }
                        } else {
                            if (mEvents.find(*itt) == mEvents.end() and
                                mGuards.find(*itt) == mGuards.end()) {
                                sigma = 0;
                                id.clear();
                                id.push_back(*itt);
                            }
                        }
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
        if (not id.empty()) {
            std::vector < int >::const_iterator it = id.begin();

            while (it != id.end()) {
                NextStatesIterator itn = mNextStates.find(*it);

                if (itn != mNextStates.end()) {
                    mToProcessAfterWhen.push_back(
                        std::make_pair(*it, itn->second));
                }
                ++it;
            }
            mValidAfterWhen = true;
        }
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
    if (mPhase == SEND) {
        if (not mToProcessEvents.empty()) {
	    buildOutputs(mFiredTransition, time, output);
	}
	if (mValidGuard) {
	    buildOutputs(mToProcessGuard.first, time, output);
	} else if (mValidAfterWhen) {
	    buildOutputs(mToProcessAfterWhen.front().first, time, output);
	}
    }
}

devs::Time Statechart::init(const devs::Time& time)
{
    if (not isInit()) {
        throw utils::InternalError(
            _("FSA::Statechart model, initial state not defined"));
    }

    currentState(initialState());

    processInStateAction(time);

    mPhase = IDLE;
    mValidGuard = false;
    checkGuards(time);

    if (mValidGuard) {
        mSigma = 0;
    } else {
        setSigma(time);
    }
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
    if (mPhase != SEND) {
        mPhase = PROCESSING;
    }
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
    bool update = true;

    switch (mPhase) {
    case PROCESSING:
    {
        if (mValidGuard) {
            process(time, mToProcessGuard.first);
	    mPhase = SEND;
        } else {
            if (not mToProcessEvents.empty()) {
                devs::ExternalEventList* events = mToProcessEvents.front();
                devs::ExternalEvent* event = events->front();

                if (process(time, event)) {
                    if (findTransition(event).empty()) { // eventInState
                        removeProcessEvent(events, event);
                        if (mToProcessEvents.empty()) {
                            if (mValidGuard) {
                                mPhase = PROCESSING;
                            } else {
                                mPhase = IDLE;
                                update = false;
                            }
                        } else {
                            mPhase = PROCESSING;
                        }
                    } else {
                        mPhase = SEND;
                    }
                } else {
                    removeProcessEvent(events, event);
                    if (mToProcessEvents.empty()) {
                        mPhase = IDLE;
                    } else {
                        mPhase = PROCESSING;
                    }
                    update = false;
                }
            }
        }
	break;
    }
    case IDLE:
    {
        if (mValidAfterWhen) {
            bool found = false;

            while (not mToProcessAfterWhen.empty() and not found) {
                if (checkGuard(mToProcessAfterWhen.front().first, time)) {
                    found = true;
                    process(time, mToProcessAfterWhen.front().first);
                    mPhase = SEND;
                } else {
                    mToProcessAfterWhen.pop_front();
                }
            }
            if (not found) {
                mValidAfterWhen = false;
                mToProcessAfterWhen.clear();
            }
        } else {
            processActivities(time);
            checkGuards(time);
        }
	break;
    }
    case SEND:
    {
        if (mValidGuard) {
            mValidGuard = false;
            if (mToProcessEvents.empty()) {
		processIn(time, mToProcessGuard.second);
                if (mValidGuard) {
                    mPhase = PROCESSING;
                } else {
                    mPhase = IDLE;
                }
            } else {
		processIn(time, mToProcessGuard.second);
		mPhase = PROCESSING;
	    }
        } else {
            if (not mToProcessEvents.empty()) {
                devs::ExternalEventList* events = mToProcessEvents.front();
                devs::ExternalEvent* event = events->front();

		processIn(time, mNextStates.at(mFiredTransition));

                if (findTransition(event).empty()) { // eventInState
                    process(time, event);
                }

		removeProcessEvent(events, event);
                if (mToProcessEvents.empty()) {
		    if (not mValidGuard) {
			mPhase = IDLE;
		    } else {
			mPhase = PROCESSING;
		    }
                } else {
		    mPhase = PROCESSING;
		}
            } else {
		if (mValidAfterWhen) {
		    processIn(time, mToProcessAfterWhen.front().second);
		    mValidAfterWhen = false;
                    mToProcessAfterWhen.clear();
                    if (not mValidGuard) {
                        mPhase = IDLE;
                    }
		}
	    }
        }
    }
    }
    if (mPhase == IDLE and update) {
	setSigma(time);
    }
    mLastTime = time;
}

}}} // namespace vle extension fsa
