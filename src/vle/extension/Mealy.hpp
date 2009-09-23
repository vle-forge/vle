/**
 * @file vle/extension/Mealy.hpp
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


#ifndef VLE_EXTENSION_MEALY_HPP
#define VLE_EXTENSION_MEALY_HPP

#include <boost/assign.hpp>
#include <boost/bind.hpp>

#include <vle/extension/FSA.hpp>

namespace vle { namespace extension {

template < typename C >
class Mealy : public Base < C >
{
public:
    Mealy(const vle::devs::DynamicsInit& init,
	  const vle::devs::InitEventList& events) :
	Base < C >(init, events) { }

    virtual ~Mealy() { }

    virtual vle::devs::ExternalEventList select(
	const vle::devs::ExternalEventList& events)
	{ return events; }

private:
    // output
    typedef std::map < std::string, std::string > Output;
    typedef std::map < C, Output > Outputs;

    typedef std::map < std::string,
		       boost::function <
			   void (const vle::devs::Time&,
				 vle::devs::ExternalEventList&) >  > OutputFunc;
    typedef std::map < C, OutputFunc > OutputFuncs;

    // transition
    typedef std::map < std::string, C > Transition;
    typedef std::map < C, Transition > Transitions;

    // action
    typedef boost::function <
	void (const vle::devs::Time&,
	      const vle::devs::ExternalEvent* event) > Action;
    typedef std::map < std::string, Action > Actions;
    typedef std::map < C, Actions > ActionsMap;

public:
    Output& outputs(const C& c)
	{
	    if (mOutputs.find(c) == mOutputs.end()) {
		mOutputs[c] = Output();
	    }
	    return mOutputs.at(c);
	}

    OutputFunc& outputFuncs(const C& c)
	{
	    if (mOutputFuncs.find(c) == mOutputFuncs.end()) {
		mOutputFuncs[c] = OutputFunc();
	    }
	    return mOutputFuncs.at(c);
	}

    Transition& transitions(const C& c)
	{
	    if (mTransitions.find(c) == mTransitions.end()) {
		mTransitions[c] = Transition();
	    }
	    return mTransitions.at(c);
	}

    Actions& actions(const C& c)
	{
	    if (mActions.find(c) == mActions.end()) {
		mActions[c] = Actions();
	    }
	    return mActions.at(c);
	}

private:
    typedef typename ActionsMap::const_iterator ActionsIterator;
    typedef typename Outputs::const_iterator OutputsIterator;
    typedef typename OutputFuncs::const_iterator OutputFuncsIterator;
    typedef typename Transitions::const_iterator TransitionsIterator;
    typedef std::list < vle::devs::ExternalEventList* > EventListLILO;

    // List of actions
    ActionsMap mActions;
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
    virtual void internalTransition(const vle::devs::Time& event);
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
void Mealy<C>::process(const vle::devs::Time& time,
		       const vle::devs::ExternalEvent* event)
{
    if (mTransitions.find(Base<C>::currentState()) !=
	mTransitions.end() and
	(mTransitions[Base<C>::currentState()].find(event->
						    getPortName())
	 != mTransitions[Base<C>::currentState()].end())) {
	ActionsIterator ita = mActions.find(Base<C>::currentState());

	if (ita != mActions.end() and
	    ita->second.find(event->getPortName()) != ita->second.end()) {

	    (ita->second.find(event->getPortName())->second)(time, event);
	}

	Base<C>::currentState(mTransitions[Base<C>::currentState()]
			      [event->getPortName()]);
    }
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename C >
void Mealy<C>::output(const vle::devs::Time& time,
		      vle::devs::ExternalEventList& output) const
{
    if (mPhase == PROCESSING) {
	vle::devs::ExternalEventList* events = mToProcessEvents.front();
	vle::devs::ExternalEvent* event = events->front();
	OutputFuncsIterator itof = mOutputFuncs.find(Base<C>::currentState());

	if (itof != mOutputFuncs.end()  and
	    itof->second.find(event->getPortName()) != itof->second.end()) {
	    (itof->second.find(event->getPortName())->second)(time, output);
	} else {
	    OutputsIterator ito = mOutputs.find(Base<C>::currentState());

	    if (ito != mOutputs.end() and
		ito->second.find(event->getPortName()) != ito->second.end()) {

		output.addEvent(
		    buildEvent(ito->second.find(event->getPortName())->second));
	    }
	}
    }
}

template < typename C >
vle::devs::Time Mealy<C>::init(const vle::devs::Time& /* time */)
{
    vle::Assert < vle::utils::InternalError > (
	Base<C>::isInit(),
	boost::format("FSA::Mealy model, initial state not defined"));

    Base<C>::currentState(Base<C>::initialState());
    mPhase = IDLE;
    return 0;
}

template < typename C >
void Mealy<C>::externalTransition(const vle::devs::ExternalEventList& events,
				  const vle::devs::Time& /* time */)
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
    mPhase = PROCESSING;
}

template < typename C >
vle::devs::Time Mealy<C>::timeAdvance() const
{
    if (mPhase == IDLE) {
	return vle::devs::Time::infinity;
    } else {
	return 0;
    }
}

template < typename C >
void Mealy<C>::internalTransition(const vle::devs::Time& time)
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

template < typename I, typename C>
struct Transition_t
{
    Transition_t(I obj, const C& state, const C& nextState) :
	obj(obj), state(state), nextState(nextState)  { }

    I obj;
    C state;
    C nextState;
    std::string event;
};

template < typename I, typename C >
Transition_t<I,C> transition(I obj, const C& state, const C& nextState)
{ return Transition_t<I,C>(obj, state, nextState); }

template < typename I, typename C >
Transition_t<I,C> operator<<(Transition_t<I,C> transition, Event_t event)
{
    insert(transition.obj->transitions(transition.state))(event.event,
							  transition.nextState);
    transition.event = event.event;
    return transition;
}

template < typename I, typename C >
Transition_t<I,C> operator<<(Transition_t<I,C> transition, Output_t output)
{
    insert(transition.obj->outputs(transition.state))(transition.event,
						      output.output);
    return transition;
}

template < typename I, typename C, typename X >
Transition_t<I,C> operator<<(Transition_t<I,C> transition, OutputFunc_t<X> func)
{
    boost::assign::insert(transition.obj->outputFuncs(transition.state))(
    	transition.event, boost::bind(func.func, transition.obj, _1, _2));
    return transition;
}

template < typename I, typename C, typename X >
Transition_t<I,C> operator<<(Transition_t<I,C> transition, Action_t<X> action)
{
    boost::assign::insert(transition.obj->actions(transition.state))(
	transition.event, boost::bind(action.action, transition.obj, _1, _2));
    return transition;
}

}} // namespace vle extension

#endif
