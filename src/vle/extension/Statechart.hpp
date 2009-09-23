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


#ifndef VLE_EXTENSION_STATECHART_HPP
#define VLE_EXTENSION_STATECHART_HPP

#include <boost/assign.hpp>
#include <boost/bind.hpp>

#include <vle/utils/DateTime.hpp>

#include <vle/extension/FSA.hpp>

namespace vle { namespace extension {

template < typename C >
class Statechart : public Base < C >
{
public:
    Statechart(const vle::devs::DynamicsInit& init,
	       const vle::devs::InitEventList& events) :
	Base < C >(init, events),
	mTimeStep(vle::devs::Time::infinity),
	mTransitionIndex(0) { }

    virtual ~Statechart() { }

    virtual vle::devs::ExternalEventList select(
	const vle::devs::ExternalEventList& events)
	{ return events; }

    void timeStep(const vle::devs::Time& t)
	{ mTimeStep = t; }

private:
    // action
    typedef boost::function < void (const vle::devs::Time&) > Action;
    typedef boost::function < void (
	const vle::devs::Time&,
	const vle::devs::ExternalEvent*) > EventAction;

    // transition = guard + event + action + output
    typedef std::list < int > Transitions;
    typedef std::map < C, Transitions > TransitionsMap;
    typedef std::map < int, C > NextStates;

    // guard on transition
    typedef boost::function < bool (const vle::devs::Time&) > Guard;
    typedef std::map < int, Guard > Guards;

    // event on transition
    typedef std::map < int, std::string > Events;

    // after on transition
    typedef std::map < int, vle::devs::Time > Afters;

    // when on transition
    typedef std::map < int, vle::devs::Time > Whens;

    // action on transition
    typedef std::map < int, EventAction > EventTransitionActions;
    typedef std::map < int, Action > GuardTransitionActions;

    // output on transition
    typedef std::map < int, std::string > Outputs;
    typedef boost::function <
	void (const vle::devs::Time&,
	      vle::devs::ExternalEventList&) >  OutputFunc;
    typedef std::map < int, OutputFunc > OutputFuncs;

    // in or out action
    typedef std::map < C, Action > Actions;

    // action on event
    typedef std::map < std::string, EventAction > EventActions;
    typedef std::map < C, EventActions > EventActionsMap;

    // event in state
    typedef boost::function <
	void (const vle::devs::Time&,
	      const vle::devs::ExternalEvent*) > EventInStateAction;
    typedef std::map < std::string, EventInStateAction > EventInStateActions;
    typedef std::map < C, EventInStateActions > EventInStateActionsMap;

    // activity
    typedef boost::function < void (const vle::devs::Time&) > Activity;
    typedef std::map < C, Activity > Activities;

public:
    Activities& activities() { return mActivities; }

    EventInStateActions& eventInStateActions(const C& c)
	{
	    if (mEventInStateActions.find(c) == mEventInStateActions.end()) {
		mEventInStateActions[c] = EventInStateActions();
	    }
	    return mEventInStateActions.at(c);
	}

    Actions& inActions() { return mInActions; }
    Actions& outActions() { return mOutActions; }

    Guards& guards() { return mGuards; }
    Events& events() { return mEvents; }
    Afters& afters() { return mAfters; }
    Whens& whens() { return mWhens; }
    Outputs& outputs() { return mOutputs; }
    OutputFuncs& outputFuncs() { return mOutputFuncs; }
    EventTransitionActions& eventTransitionActions()
	{ return mEventTransitionActions; }
    GuardTransitionActions& guardTransitionActions()
	{ return mGuardTransitionActions; }

    int addTransition(const C& state, const C& nextState)
	{
	    if (mTransitionsMap.find(state) == mTransitionsMap.end()) {
		mTransitionsMap[state] = Transitions();
	    }
	    mTransitionsMap[state].push_back(++mTransitionIndex);
	    mNextStates[mTransitionIndex] = nextState;
	    return mTransitionIndex;
	}

private:
    typedef typename Actions::const_iterator ActionsIterator;
    typedef typename EventInStateActions::const_iterator
    EventInStateActionsIterator;
    typedef typename EventActionsMap::const_iterator EventActionsIterator;
    typedef typename Activities::const_iterator ActivitiesIterator;
    typedef typename Transitions::const_iterator TransitionsIterator;
    typedef typename TransitionsMap::const_iterator TransitionsMapIterator;
    typedef typename NextStates::const_iterator NextStatesIterator;
    typedef typename Guards::const_iterator GuardsIterator;
    typedef typename Events::const_iterator EventsIterator;
    typedef typename Afters::const_iterator AftersIterator;
    typedef typename Whens::const_iterator WhensIterator;
    typedef typename EventTransitionActions::const_iterator
    EventTransitionActionsIterator;
    typedef typename GuardTransitionActions::const_iterator
    GuardTransitionActionsIterator;
    typedef typename Outputs::const_iterator OutputsIterator;
    typedef typename OutputFuncs::const_iterator OutputFuncsIterator;
    typedef std::list < vle::devs::ExternalEventList* > EventListLILO;

    // Time step for activities
    vle::devs::Time mTimeStep;
    // List of activities
    Activities mActivities;
    // List of actions
    // (state -> action )
    Actions mInActions;
    // (state -> action )
    Actions mOutActions;
    // (state -> (event -> action ))
    EventInStateActionsMap mEventInStateActions;
    // Index of transition
    int mTransitionIndex;
    // List of transitions
    // (state -> { id })
    TransitionsMap mTransitionsMap;
    // (id -> state)
    NextStates mNextStates;
    // List of guards on transition
    // (id -> guard)
    Guards mGuards;
    // List of events on transition
    // (id -> event)
    Events mEvents;
    // List of afters on transition
    // (id -> duration)
    Afters mAfters;
    // List of whens on transition
    // (id -> date)
    Whens mWhens;
    // List of actions on transition
    // (id -> action)
    EventTransitionActions mEventTransitionActions;
    GuardTransitionActions mGuardTransitionActions;
    // List of ports for output function
    // (id -> port)
    Outputs mOutputs;
    // List of output functions
    // (id -> output function)
    OutputFuncs mOutputFuncs;

    bool checkGuard(int transition, const vle::devs::Time& time);
    void checkGuards(const vle::devs::Time& time);
    int findTransition(const vle::devs::ExternalEvent* event) const;
    void process(const vle::devs::Time& time,
		 const vle::devs::ExternalEvent* event);
    void process(const vle::devs::Time& time,
		 int transitionId, const C& nextState);
    void processActivities(const vle::devs::Time& time);
    void processEventInStateActions(const vle::devs::Time& time,
				    const vle::devs::ExternalEvent* event);
    void processInStateAction(const vle::devs::Time& time);
    void processOutStateAction(const vle::devs::Time& time);
    void processEventTransitionAction(int transition,
				      const vle::devs::Time& time,
				      const vle::devs::ExternalEvent* event);
    void processGuardTransitionAction(int transition,
				      const vle::devs::Time& time);
    void setSigma(const vle::devs::Time& time);
    void updateSigma(const vle::devs::Time& time);

    virtual vle::devs::Time init(const vle::devs::Time& time);
    virtual void output(const vle::devs::Time& time,
			vle::devs::ExternalEventList& output) const;
    virtual vle::devs::Time timeAdvance() const;
    virtual void internalTransition(const vle::devs::Time& time);
    virtual void externalTransition(
	const vle::devs::ExternalEventList& event,
	const vle::devs::Time& time);
    virtual vle::devs::Event::EventType confluentTransitions(
	const vle::devs::Time& /* time */,
	const vle::devs::ExternalEventList& /* extEventlist */) const
        { return vle::devs::Event::EXTERNAL; }

    enum Phase { IDLE, PROCESSING };

    Phase mPhase;
    vle::devs::Time mLastTime;
    vle::devs::Time mSigma;
    EventListLILO mToProcessEvents;
    std::pair < int, C > mToProcessGuard;
    std::pair < int, C > mToProcessAfterWhen;
    bool mValidGuard;
    bool mValidAfterWhen;
};

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename C >
bool Statechart<C>::checkGuard(int transition, const vle::devs::Time& time)
{
    GuardsIterator itg = mGuards.find(transition);
    bool valid = true;

    if (itg != mGuards.end()) {
	valid = (itg->second)(time);
    }
    return valid;
}

template < typename C >
void Statechart<C>::checkGuards(const vle::devs::Time& time)
{
    TransitionsMapIterator it = mTransitionsMap.find(Base<C>::currentState());

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

template < typename C >
int Statechart<C>::findTransition(const vle::devs::ExternalEvent* event) const
{
    TransitionsMapIterator it = mTransitionsMap.find(Base<C>::currentState());
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

template < typename C >
void Statechart<C>::process(const vle::devs::Time& time,
			    const vle::devs::ExternalEvent* event)
{
    int transition = findTransition(event);

    if (transition != -1) {
	C nextState = mNextStates[transition];

	if (checkGuard(transition, time)) {
	    processOutStateAction(time);
	    processEventTransitionAction(transition, time, event);
	    Base<C>::currentState(nextState);
	    processInStateAction(time);
	    checkGuards(time);
	}
    } else {
	processEventInStateActions(time, event);
    }
}

template < typename C >
void Statechart<C>::process(const vle::devs::Time& time,
			    int transitionId, const C& nextState)
{
    processOutStateAction(time);
    processGuardTransitionAction(transitionId, time);
    Base<C>::currentState(nextState);
    processInStateAction(time);
    checkGuards(time);
}

template < typename C >
void Statechart<C>::processActivities(const vle::devs::Time& time)
{
    ActivitiesIterator it = mActivities.find(Base<C>::currentState());

    if (it != mActivities.end()) {
	(it->second)(time);
    }
}

template < typename C >
void Statechart<C>::processInStateAction(const vle::devs::Time& time)
{
    ActionsIterator it = mInActions.find(Base<C>::currentState());

    if (it != mInActions.end()) {
	(it->second)(time);
    }
}

template < typename C >
void Statechart<C>::processOutStateAction(const vle::devs::Time& time)
{
    ActionsIterator it = mOutActions.find(Base<C>::currentState());

    if (it != mOutActions.end()) {
	(it->second)(time);
    }
}

template < typename C >
void Statechart<C>::processEventTransitionAction(
    int transition,
    const vle::devs::Time& time,
    const vle::devs::ExternalEvent* event)
{
    EventTransitionActionsIterator it =
	mEventTransitionActions.find(transition);

    if (it != mEventTransitionActions.end()) {
	(it->second)(time, event);
    }
}

template < typename C >
void Statechart<C>::processGuardTransitionAction(
    int transition,
    const vle::devs::Time& time)
{
    GuardTransitionActionsIterator it =
	mGuardTransitionActions.find(transition);

    if (it != mGuardTransitionActions.end()) {
	(it->second)(time);
    }
}

template < typename C >
void Statechart<C>::processEventInStateActions(
    const vle::devs::Time& time,
    const vle::devs::ExternalEvent* event)
{
    if (mEventInStateActions.find(Base<C>::currentState()) !=
	mEventInStateActions.end()) {
	EventInStateActions& actions = eventInStateActions(
	    Base<C>::currentState());
	EventInStateActionsIterator it = actions.find(event->getPortName());

	if (it != actions.end()) {
	    (it->second)(time, event);
	}
	checkGuards(time);
    }
}

template < typename C >
void Statechart<C>::setSigma(const vle::devs::Time& time)
{
    vle::devs::Time sigma = vle::devs::Time::infinity;
    int id = -1;
    TransitionsMapIterator it = mTransitionsMap.find(Base<C>::currentState());

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
		    vle::devs::Time duration = itw->second - time;

		    if (duration > 0 and duration < sigma) {
			sigma = duration;
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

template < typename C >
void Statechart<C>::updateSigma(const vle::devs::Time& time)
{
    mSigma -= time - mLastTime;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename C >
void Statechart<C>::output(const vle::devs::Time& time,
			   vle::devs::ExternalEventList& output) const
{
    if (mPhase == PROCESSING) {
    	if (not mToProcessEvents.empty()) {
    	    const vle::devs::ExternalEventList* events =
		mToProcessEvents.front();
    	    const vle::devs::ExternalEvent* event = events->front();
     	    int transition = findTransition(event);

	    if (transition != -1) {
		OutputFuncsIterator itof = mOutputFuncs.find(transition);

		if (itof != mOutputFuncs.end()) {
		    (itof->second)(time, output);
		} else {
		    OutputsIterator ito = mOutputs.find(transition);

		    if (ito != mOutputs.end()) {
			output.addEvent(Base<C>::buildEvent(ito->second));
		    }
		}
	    }
    	}
    }
}

template < typename C >
vle::devs::Time Statechart<C>::init(const vle::devs::Time& time)
{
    vle::Assert <vle::utils::InternalError>(
	Base<C>::isInit(),
	boost::format("FSA::Statechart model, initial state not defined"));

    Base<C>::currentState(Base<C>::initialState());

    processInStateAction(time);

    mPhase = IDLE;
    mValidGuard = false;
    checkGuards(time);

    setSigma(time);
    mLastTime = time;
    return 0;
}

template < typename C >
void Statechart<C>::externalTransition(
    const vle::devs::ExternalEventList& events,
    const vle::devs::Time& time)
{
    if (events.size() > 1) {
	vle::devs::ExternalEventList sortedEvents = select(events);
	vle::devs::ExternalEventList* clonedEvents =
	    new vle::devs::ExternalEventList;
	vle::devs::ExternalEventList::const_iterator it = sortedEvents.begin();

	while (it != sortedEvents.end()) {
	    clonedEvents->addEvent(Base<C>::cloneExternalEvent(*it));
	    ++it;
	}
	mToProcessEvents.push_back(clonedEvents);
    } else {
	vle::devs::ExternalEventList::const_iterator it = events.begin();
	vle::devs::ExternalEventList* clonedEvents =
	    new vle::devs::ExternalEventList;

	clonedEvents->addEvent(Base<C>::cloneExternalEvent(*it));
	mToProcessEvents.push_back(clonedEvents);
    }
    updateSigma(time);
    mLastTime = time;
    mPhase = PROCESSING;
}

template < typename C >
vle::devs::Time Statechart<C>::timeAdvance() const
{
    if (mPhase == IDLE) {
	return mSigma;
    } else {
	return 0;
    }
}

template < typename C >
void Statechart<C>::internalTransition(const vle::devs::Time& time)
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
		vle::devs::ExternalEventList* events = mToProcessEvents.front();
		vle::devs::ExternalEvent* event = events->front();

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

template < typename X, typename I >
struct InAction_t
{
    InAction_t(I obj, X func) : obj(obj), func(func) { }

    I obj;
    X func;
};

template < typename X, typename I >
InAction_t<X, I> inAction(I obj, X func)
{ return InAction_t<X, I>(obj, func); }

template < typename C, typename X, typename I >
InAction_t<X,I> operator>>(InAction_t<X,I> action, const C& state)
{
    boost::assign::insert(action.obj->inActions())(
	state, boost::bind(action.func, action.obj, _1));
    return action;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename X, typename I >
struct OutAction_t
{
    OutAction_t(I obj, X func) : obj(obj), func(func) { }

    I obj;
    X func;
};

template < typename X, typename I >
OutAction_t<X, I> outAction(I obj, X func)
{ return OutAction_t<X, I>(obj, func); }

template < typename C, typename X, typename I >
OutAction_t<X,I> operator>>(OutAction_t<X,I> action, const C& state)
{
    boost::assign::insert(action.obj->outActions())(
	state, boost::bind(action.func, action.obj, _1));
    return action;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename X, typename I >
struct Activity_t
{
    Activity_t(I obj, X func) : obj(obj), func(func)  { }

    I obj;
    X func;
};

template < typename X, typename I >
Activity_t<X, I> activity(I obj, X func)
{ return Activity_t<X, I>(obj, func); }

template < typename C, typename X, typename I >
Activity_t<X,I> operator>>(Activity_t<X,I> activity, const C& state)
{
    boost::assign::insert(activity.obj->activities())(
	state, boost::bind(activity.func, activity.obj, _1));
    return activity;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename I, typename C >
struct Transition_t
{
    Transition_t(I obj, const C& state, const C& nextState, int id) :
	obj(obj), state(state), nextState(nextState), id(id)  { }

    I obj;
    C state;
    C nextState;
    int id;
};

template < typename I, typename C >
Transition_t<I,C> transition(I obj, const C& state, const C& nextState)
{
    int id = obj->addTransition(state, nextState);

    return Transition_t<I,C>(obj, state, nextState, id);
}

template < typename X >
struct Guard_t
{
    Guard_t(X guard) : guard(guard)  { }

    X guard;
};

template < typename X >
Guard_t<X> guard(X guard)
{ return Guard_t<X>(guard); }

struct After_t
{
    After_t(const vle::devs::Time& duration) : duration(duration)  { }

    vle::devs::Time duration;
};

After_t after(const vle::devs::Time& duration);

struct When_t
{
    When_t(const vle::devs::Time& date) : date(date)  { }

    vle::devs::Time date;
};

When_t when(const vle::devs::Time& date);

template < typename I, typename C, typename X >
Transition_t<I,C> operator<<(Transition_t<I,C> transition, Guard_t<X> guard)
{
    transition.obj->guards()[transition.id] =
	boost::bind(guard.guard, transition.obj, _1);
    return transition;
}

template < typename I, typename C >
Transition_t<I,C> operator<<(Transition_t<I,C> transition, Event_t event)
{
    transition.obj->events()[transition.id] = event.event;
    return transition;
}

template < typename I, typename C >
Transition_t<I,C> operator<<(Transition_t<I,C> transition, After_t after)
{
    transition.obj->afters()[transition.id] = after.duration;
    return transition;
}

template < typename I, typename C >
Transition_t<I,C> operator<<(Transition_t<I,C> transition, When_t when)
{
    transition.obj->whens()[transition.id] = when.date;
    return transition;
}

template < typename I, typename C >
Transition_t<I,C> operator<<(Transition_t<I,C> transition, Output_t output)
{
    transition.obj->outputs()[transition.id] = output.output;
    return transition;
}

template < typename I, typename C, typename X >
Transition_t<I,C> operator<<(Transition_t<I,C> transition, OutputFunc_t<X> func)
{
    transition.obj->outputFuncs()[transition.id] =
	boost::bind(func.func, transition.obj, _1);
    return transition;
}

template < typename I, typename C, typename X >
Transition_t<I,C> operator<<(Transition_t<I,C> transition, Action_t<X> action)
{
    if (transition.obj->events().find(transition.id) !=
	transition.obj->events().end()) {
	transition.obj->eventTransitionActions()[transition.id] =
	    boost::bind(action.action, transition.obj, _1);
    } else {
	transition.obj->guardTransitionActions()[transition.id] =
	    boost::bind(action.action, transition.obj, _1);
    }
    return transition;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename I, typename X >
struct EventInState_t
{
    EventInState_t(I obj, const std::string& event, X func) :
	obj(obj), event(event), func(func)  { }

    I obj;
    std::string event;
    X func;
};

template < typename I, typename X >
EventInState_t<I,X> eventInState(I obj, const std::string& event, X func)
{ return EventInState_t<I,X>(obj, event, func); }

template < typename I, typename C, typename X >
EventInState_t<I,X> operator>>(EventInState_t<I,X> event, const C& state)
{
    insert(event.obj->eventInStateActions(state))(event.event,
						  boost::bind(event.func,
							      event.obj,
							      _1, _2));
    return event;
}

}} // namespace vle extension

#endif
