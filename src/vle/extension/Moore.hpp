/**
 * @file vle/extension/Moore.hpp
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


#ifndef VLE_EXTENSION_MOORE_HPP
#define VLE_EXTENSION_MOORE_HPP

#include <boost/assign.hpp>
#include <boost/bind.hpp>

#include <vle/extension/FSA.hpp>

namespace vle { namespace extension {

template < typename C >
class Moore : public Base < C >
{
public:
    Moore(const vle::devs::DynamicsInit& init,
	  const vle::devs::InitEventList& events) :
	Base < C >(init, events) { }

    virtual ~Moore() { }

    virtual vle::devs::ExternalEventList select(
	const vle::devs::ExternalEventList& events)
	{ return events; }

private:
    // output
    typedef std::map < C, std::string > Outputs;
    typedef boost::function < void (const vle::devs::Time&,
				    vle::devs::ExternalEventList&) > OutputFunc;
    typedef std::map < C, OutputFunc > OutputFuncs;

    // transition
    typedef std::map < std::string, C > Transition;
    typedef std::map < C, Transition > Transitions;

    // action
    typedef boost::function <
	void (const vle::devs::Time&,
	      const vle::devs::ExternalEvent* event) > Action;
    typedef std::map < C, Action > Actions;

public:
    Actions& actions() { return mActions; }
    Outputs& outputs() { return mOutputs; }
    OutputFuncs& outputFuncs() { return mOutputFuncs; }

    Transition& transitions(const C& c)
	{
	    if (mTransitions.find(c) == mTransitions.end()) {
		mTransitions[c] = Transition();
	    }
	    return mTransitions.at(c);
	}

private:
    typedef typename Outputs::const_iterator OutputsIterator;
    typedef typename OutputFuncs::const_iterator OutputFuncsIterator;
    typedef typename Actions::const_iterator ActionsIterator;
    typedef typename Transitions::const_iterator TransitionsIterator;
    typedef typename Transition::const_iterator TransitionIterator;
    typedef std::list < vle::devs::ExternalEventList* > EventListLILO;

    // List of actions
    Actions mActions;
    // List of ports for output function
    Outputs mOutputs;
    // List of output functions
    OutputFuncs mOutputFuncs;
    // Next states in case of external transition
    Transitions mTransitions;

    void process(const vle::devs::Time& time,
		 const vle::devs::ExternalEvent* event);

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
    EventListLILO mToProcessEvents;
};

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename C >
void Moore<C>::process(const vle::devs::Time& time,
		       const vle::devs::ExternalEvent* event)
{
    if (mTransitions.find(Base<C>::currentState()) !=
	mTransitions.end() and
	(mTransitions[Base<C>::currentState()].find(event->
						    getPortName())
	 != mTransitions[Base<C>::currentState()].end())) {
	Base<C>::currentState(mTransitions[Base<C>::currentState()]
			      [event->getPortName()]);

	ActionsIterator it = mActions.find(Base<C>::currentState());

	if (it != mActions.end()) {
	    (it->second)(time, event);
	}
    }
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename C >
void Moore<C>::output(const vle::devs::Time& time,
		      vle::devs::ExternalEventList& output) const
{
    if (mPhase == PROCESSING) {
	OutputFuncsIterator it = mOutputFuncs.find(Base<C>::currentState());

	if (it != mOutputFuncs.end()) {
	    (it->second)(time, output);
	} else {
	    OutputsIterator ito = mOutputs.find(Base<C>::currentState());

	    if (ito != mOutputs.end()) {
		output.addEvent(buildEvent(ito->second));
	    }
	}
    }
}

template < typename C >
vle::devs::Time Moore<C>::init(const vle::devs::Time& time)
{
    vle::Assert <vle::utils::InternalError>(
	Base<C>::isInit(),
	boost::format("FSA::Moore model, initial state not defined"));

    Base<C>::currentState(Base<C>::initialState());

    ActionsIterator it = mActions.find(Base<C>::initialState());

    if (it != mActions.end()) {
	vle::devs::ExternalEvent* event = new vle::devs::ExternalEvent("");

	(it->second)(time, event);
	delete event;
    }

    mPhase = IDLE;
    return 0;
}

template < typename C >
void Moore<C>::externalTransition(const vle::devs::ExternalEventList& events,
				  const vle::devs::Time& /* time */)
{
    // mNewStates.clear();
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
    mPhase = PROCESSING;
}

template < typename C >
vle::devs::Time Moore<C>::timeAdvance() const
{
    if (mPhase == IDLE) {
	return vle::devs::Time::infinity;
    } else {
	return 0;
    }
}

template < typename C >
void Moore<C>::internalTransition(const vle::devs::Time& time)
{
    if (mPhase == PROCESSING)
    {
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

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename C, typename I >
struct Transition_t
{
    Transition_t(I obj, const C& state, const C& nextState) :
	obj(obj), state(state), nextState(nextState)  { }

    I obj;
    C state;
    C nextState;
};

template < typename C, typename I >
Transition_t<C,I> transition(I obj, const C& state, const C& nextState)
{ return Transition_t<C,I>(obj, state, nextState); }

template < typename C, typename I >
void operator<<(Transition_t<C,I> transition, Event_t event)
{
    insert(transition.obj->transitions(transition.state))(event.event,
							  transition.nextState);
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
    boost::assign::insert(action.obj->actions())(
	state, boost::bind(action.func, action.obj, _1, _2));
    return action;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename C, typename I >
struct Output2_t
{
    Output2_t(I obj, const C& state) :
	obj(obj), state(state)  { }

    I obj;
    C state;
};

template < typename C, typename I >
Output2_t<C,I> output(I obj, const C& state)
{ return Output2_t<C,I>(obj, state); }

template < typename C, typename I >
void operator>>(Output2_t<C,I> output, const std::string& port)
{
    insert(output.obj->outputs())(output.state, port);
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename X, typename I >
struct OutputFunc2_t
{
    OutputFunc2_t(I obj, X func) : obj(obj), func(func)  { }

    I obj;
    X func;
};

template < typename X, typename I >
OutputFunc2_t<X, I> outputFunc(I obj, X func)
{ return OutputFunc2_t<X, I>(obj, func); }

template < typename C, typename X, typename I >
void operator>>(OutputFunc2_t<X,I> output, const C& state)
{
    boost::assign::insert(output.obj->outputFuncs())(
    	state, boost::bind(output.func, output.obj, _1, _2));
}

}} // namespace vle extension

#endif
