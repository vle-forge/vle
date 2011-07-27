/*
 * @file vle/extension/fsa/Statechart.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
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


#ifndef VLE_EXTENSION_FSA_STATECHART_HPP
#define VLE_EXTENSION_FSA_STATECHART_HPP

#include <vle/extension/DllDefines.hpp>
#include <vle/extension/fsa/FSA.hpp>
#include <vle/utils/DateTime.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace vle { namespace extension { namespace fsa {

template < typename X, typename I >
struct StatechartInAction_t
{
    StatechartInAction_t(I obj, X func) : obj(obj), func(func) { }

    I obj;
    X func;
};

template < typename X >
struct StatechartInAction2_t
{
    StatechartInAction2_t(X func) : func(func) { }

    X func;
};

template < typename X, typename I >
struct OutAction_t
{
    OutAction_t(I obj, X func) : obj(obj), func(func) { }

    I obj;
    X func;
};

template < typename X >
struct OutAction2_t
{
    OutAction2_t(X func) : func(func) { }

    X func;
};

template < typename X, typename I >
struct Activity_t
{
    Activity_t(I obj, X func) : obj(obj), func(func)  { }

    I obj;
    X func;
};

template < typename X >
struct Activity2_t
{
    Activity2_t(X func) : func(func)  { }

    X func;
};

template < typename X >
struct Guard_t
{
    Guard_t(X guard) : guard(guard)  { }

    X guard;
};

template < typename X >
struct After_t
{
    After_t(const X& duration) : duration(duration)  { }

    X duration;
};

template < typename X >
struct When_t
{
    When_t(const X& date) : date(date)  { }

    X date;
};

template < typename I >
struct StatechartTransition_t
{
    StatechartTransition_t(I obj, int state, int nextState, int id) :
        obj(obj), state(state), nextState(nextState), id(id)  { }

    I obj;
    int state;
    int nextState;
    int id;
};

template < typename I, typename X >
struct EventInState_t
{
    EventInState_t(I obj, const std::string& event, X func) :
        obj(obj), event(event), func(func)  { }

    I obj;
    std::string event;
    X func;
};

template < typename X >
struct EventInState2_t
{
    EventInState2_t(const std::string& event, X func) :
        event(event), func(func)  { }

    std::string event;
    X func;
};

template < typename X >
struct EventAction_t
{
    EventAction_t(X action) : action(action)  { }

    X action;
};

template < typename X >
struct GuardAction_t
{
    GuardAction_t(X action) : action(action)  { }

    X action;
};

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

class VLE_EXTENSION_EXPORT Statechart : public Base
{
public:
    Statechart(const devs::DynamicsInit& init,
               const devs::InitEventList& events)
        : Base(init, events), mTimeStep(devs::Time::infinity),
	  mTransitionIndex(0), mValidGuard(false), mValidAfterWhen(false)
    {}

    virtual ~Statechart() {}

    virtual devs::ExternalEventList select(
        const devs::ExternalEventList& events)
    { return events; }

    void timeStep(const devs::Time& t)
    { mTimeStep = t; }

private:
    // action
    typedef boost::function < void (const devs::Time&) > Action;
    typedef boost::function < void (
        const devs::Time&,
        const devs::ExternalEvent*) > EventAction;

    // transition = guard + event + action + output
    typedef std::list < int > Transitions;
    typedef std::map < int, Transitions > TransitionsMap;
    typedef std::map < int, int > NextStates;

    // guard on transition
    typedef boost::function < bool (const devs::Time&) > Guard;
    typedef std::map < int, Guard > Guards;

    // event on transition
    typedef std::map < int, std::string > Events;

    // after on transition
    typedef boost::function < vle::devs::Time (const devs::Time&) > AfterFunc;
    typedef std::map < int, devs::Time > Afters;
    typedef std::map < int, AfterFunc > AfterFuncs;

    // when on transition
    typedef boost::function < vle::devs::Time (const devs::Time&) > WhenFunc;
    typedef std::map < int, devs::Time > Whens;
    typedef std::map < int, WhenFunc > WhenFuncs;

    // action on transition
    typedef std::map < int, EventAction > EventTransitionActions;
    typedef std::map < int, Action > GuardTransitionActions;

    // output on transition
    typedef std::map < int, std::string > Outputs;
    typedef boost::function <
        void (const devs::Time&,
              devs::ExternalEventList&) >  OutputFunc;
    typedef std::map < int, OutputFunc > OutputFuncs;

    // in or out action
    typedef std::map < int, Action > Actions;

    // action on event
    typedef std::map < std::string, EventAction > EventActions;
    typedef std::map < int, EventActions > EventActionsMap;

    // event in state
    typedef boost::function <
        void (const devs::Time&,
              const devs::ExternalEvent*) > EventInStateAction;
    typedef std::map < std::string, EventInStateAction > EventInStateActions;
    typedef std::map < int, EventInStateActions > EventInStateActionsMap;

    // activity
    typedef boost::function < void (const devs::Time&) > Activity;
    typedef std::map < int, Activity > Activities;

public:
    template < typename X, typename I >
        StatechartInAction_t<X, I> inAction(I obj, X func)
    { return StatechartInAction_t<X, I>(obj, func); }

    template < typename X >
        StatechartInAction2_t<X> inAction(X func)
    { return StatechartInAction2_t<X>(func); }

    template < typename X, typename I >
        OutAction_t<X, I> outAction(I obj, X func)
    { return OutAction_t<X, I>(obj, func); }

    template < typename X >
        OutAction2_t<X> outAction(X func)
    { return OutAction2_t<X>(func); }

    template < typename X, typename I >
        Activity_t<X, I> activity(I obj, X func)
    { return Activity_t<X, I>(obj, func); }

    template < typename X >
        Activity2_t<X> activity(X func)
    { return Activity2_t<X>(func); }

    template < typename X >
        Guard_t<X> guard(X guard)
    { return Guard_t<X>(guard); }

    template < typename X >
        After_t <X> after(const X& duration)
    { return After_t <X>(duration); }

    template < typename X >
        When_t <X> when(const X& date)
    { return When_t <X>(date); }

    template < typename I >
        StatechartTransition_t<I> transition(I obj, int state, int nextState)
    { return StatechartTransition_t<I>(obj, state, nextState,
                                       obj->addTransition(state, nextState)); }

    template < typename I, typename X >
        EventInState_t<I,X> eventInState(I obj, const std::string& event,
                                         X func)
    { return EventInState_t<I,X>(obj, event, func); }

    template < typename X >
        EventInState2_t<X> eventInState(const std::string& event, X func)
    { return EventInState2_t<X>(event, func); }

    template < typename X >
        EventAction_t<X> eventAction(X action)
    { return EventAction_t<X>(action); }

    template < typename X >
        GuardAction_t<X> action(X action)
    { return GuardAction_t<X>(action); }

protected:
    virtual devs::Time init(const devs::Time& time);
    virtual void output(const devs::Time& time,
                        devs::ExternalEventList& output) const;
    virtual void internalTransition(const devs::Time& time);
    virtual void externalTransition(
        const devs::ExternalEventList& event,
        const devs::Time& time);

private:
    typedef Actions::const_iterator ActionsIterator;
    typedef EventInStateActions::const_iterator EventInStateActionsIterator;
    typedef EventActionsMap::const_iterator EventActionsIterator;
    typedef Activities::const_iterator ActivitiesIterator;
    typedef Transitions::const_iterator TransitionsIterator;
    typedef TransitionsMap::const_iterator TransitionsMapIterator;
    typedef NextStates::const_iterator NextStatesIterator;
    typedef Guards::const_iterator GuardsIterator;
    typedef Events::const_iterator EventsIterator;
    typedef Afters::const_iterator AftersIterator;
    typedef AfterFuncs::const_iterator AfterFuncsIterator;
    typedef Whens::const_iterator WhensIterator;
    typedef WhenFuncs::const_iterator WhenFuncsIterator;
    typedef EventTransitionActions::const_iterator
    EventTransitionActionsIterator;
    typedef GuardTransitionActions::const_iterator
        GuardTransitionActionsIterator;
    typedef Outputs::const_iterator OutputsIterator;
    typedef OutputFuncs::const_iterator OutputFuncsIterator;
    typedef std::list < devs::ExternalEventList* > EventListLILO;

    // Time step for activities
    devs::Time mTimeStep;
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
    AfterFuncs mAfterFuncs;
    // List of whens on transition
    // (id -> date)
    Whens mWhens;
    WhenFuncs mWhenFuncs;
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

    Activities& activities() { return mActivities; }

    EventInStateActions& eventInStateActions(int s)
    {
        std::pair < EventInStateActionsMap::iterator, bool > r;

        r = mEventInStateActions.insert(
            std::make_pair(s, EventInStateActions()));

        return r.first->second;
    }

    Actions& inActions() { return mInActions; }
    Actions& outActions() { return mOutActions; }

    Guards& guards() { return mGuards; }
    Events& events() { return mEvents; }
    Afters& afters() { return mAfters; }
    AfterFuncs& afterFuncs() { return mAfterFuncs; }
    Whens& whens() { return mWhens; }
    WhenFuncs& whenFuncs() { return mWhenFuncs; }
    Outputs& outputs() { return mOutputs; }
    OutputFuncs& outputFuncs() { return mOutputFuncs; }
    EventTransitionActions& eventTransitionActions()
    { return mEventTransitionActions; }
    GuardTransitionActions& guardTransitionActions()
    { return mGuardTransitionActions; }

    int addTransition(int state, int nextState)
    {
        if (mTransitionsMap.find(state) == mTransitionsMap.end()) {
            mTransitionsMap[state] = Transitions();
        }
        mTransitionsMap[state].push_back(++mTransitionIndex);
        mNextStates[mTransitionIndex] = nextState;
        return mTransitionIndex;
    }

    void buildOutputs(int transition,
		      const devs::Time& time,
		      devs::ExternalEventList& output) const;
    bool checkGuard(int transition, const devs::Time& time);
    void checkGuards(const devs::Time& time);

    bool existAction(int transition)
    { return mEventTransitionActions.find(transition) !=
            mEventTransitionActions.end() or
            mGuardTransitionActions.find(transition) !=
            mGuardTransitionActions.end();
    }
    bool existActivity(int state)
    { return mActivities.find(state) != mActivities.end(); }
    bool existAfter(int transition)
    { return mAfters.find(transition) != mAfters.end() or
            mAfterFuncs.find(transition) != mAfterFuncs.end(); }
    bool existEvent(int transition)
    { return mEvents.find(transition) != mEvents.end(); }
    bool existEventInState(int state, const std::string& event)
    {
        EventInStateActionsMap::const_iterator it =
            mEventInStateActions.find(state);

        if (it != mEventInStateActions.end()) {
            return it->second.find(event) != it->second.end();
        }
        return false;
    }
    bool existGuard(int transition)
    { return mGuards.find(transition) != mGuards.end(); }
    bool existInAction(int state)
    { return mInActions.find(state) != mInActions.end(); }
    bool existOutAction(int state)
    { return mOutActions.find(state) != mOutActions.end(); }
    bool existOuput(int transition)
    { return mOutputs.find(transition) != mOutputs.end() or
            mOutputFuncs.find(transition) != mOutputFuncs.end(); }
    bool existWhen(int transition)
    { return mWhens.find(transition) != mWhens.end() or
            mWhenFuncs.find(transition) != mWhenFuncs.end(); }

    Transitions findTransition(const devs::ExternalEvent* event) const;
    bool process(const devs::Time& time,
                 const devs::ExternalEvent* event);
    void process(const devs::Time& time,
                 int transitionId);
    void processActivities(const devs::Time& time);
    bool processEventInStateActions(const devs::Time& time,
                                    const devs::ExternalEvent* event);
    void processIn(const devs::Time& time, int nextState);
    void processInStateAction(const devs::Time& time);
    void processOutStateAction(const devs::Time& time);
    void processEventTransitionAction(int transition,
                                      const devs::Time& time,
                                      const devs::ExternalEvent* event);
    void processGuardTransitionAction(int transition,
                                      const devs::Time& time);
    void removeProcessEvent(devs::ExternalEventList* events,
			    devs::ExternalEvent* event);
    void setSigma(const devs::Time& time);
    void updateSigma(const devs::Time& time);

    virtual devs::Time timeAdvance() const;
    virtual void confluentTransitions(
        const devs::Time& time,
        const devs::ExternalEventList& extEventlist)
    {
        externalTransition(extEventlist, time);
        internalTransition(time);
    }

    enum Phase { IDLE, PROCESSING, SEND };

    Phase mPhase;
    devs::Time mLastTime;
    devs::Time mSigma;
    EventListLILO mToProcessEvents;
    std::pair < int, int > mToProcessGuard;
    std::list < std::pair < int, int > > mToProcessAfterWhen;
    bool mValidGuard;
    bool mValidAfterWhen;
    int mFiredTransition;

    template < typename I, typename X >
        friend StatechartTransition_t<I> operator<<(
            StatechartTransition_t<I> transition,
            Guard_t<X> guard);

    template < typename I >
        friend StatechartTransition_t<I> operator<<(
            StatechartTransition_t<I> transition,
            Event_t event);

    template < typename I >
        friend StatechartTransition_t<I> operator<<(
            StatechartTransition_t<I> transition,
            After_t < double > after);

    template < typename I, typename X >
        friend StatechartTransition_t<I> operator<<(
            StatechartTransition_t<I> transition,
            After_t<X> after);

    template < typename I >
        friend StatechartTransition_t<I> operator<<(
            StatechartTransition_t<I> transition,
            When_t < double > when);

    template < typename I, typename X >
        friend StatechartTransition_t<I> operator<<(
            StatechartTransition_t<I> transition,
            When_t<X> when);

    template < typename I >
        friend StatechartTransition_t<I> operator<<(
            StatechartTransition_t<I> transition,
            Output_t < std::string > output);

    template < typename I, typename X >
        friend StatechartTransition_t<I> operator<<(
            StatechartTransition_t<I> transition,
            Output_t<X> func);

    template < typename I, typename X >
        friend StatechartTransition_t<I> operator<<(
            StatechartTransition_t<I> transition,
            EventAction_t<X> action);

    template < typename I, typename X >
        friend StatechartTransition_t<I> operator<<(
            StatechartTransition_t<I> transition,
            GuardAction_t<X> action);

    template < typename I, typename X >
        friend EventInState_t<I,X> operator>>(EventInState_t<I,X> event,
                                              int state);

    template < typename X, typename I >
        friend StatechartInAction_t<X,I> operator>>(
            StatechartInAction_t<X,I> action, int state);

    template < typename X, typename I >
        friend OutAction_t<X,I> operator>>(OutAction_t<X,I> action, int state);

    template < typename X, typename I >
        friend Activity_t<X,I> operator>>(Activity_t<X,I> activity, int state);

    template < typename I, typename X >
        friend State2_t<I> operator<<(State2_t<I> state,
                                      EventInState2_t<X> event);

    template < typename I, typename X >
        friend State2_t<I> operator<<(State2_t<I> state,
                                      StatechartInAction2_t<X> action);

    template < typename I, typename X >
        friend State2_t<I> operator<<(State2_t<I> state,
                                      OutAction2_t<X> action);

    template < typename I, typename X >
        friend State2_t<I> operator<<(State2_t<I> state,
                                      Activity2_t<X> activity);
};

template <> inline
After_t < double > Statechart::after < double >(
    const double& duration)
{ return After_t < double >(duration); }

template <> inline
When_t < double > Statechart::when < double >(
    const double& date)
{ return When_t < double >(date); }

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename I, typename X >
StatechartTransition_t<I> operator<<(StatechartTransition_t<I> transition,
                                     Guard_t<X> guard)
{
    if (transition.obj->existGuard(transition.id)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Statechart: transition %2% -> %3% have " \
                       "already a guard"))
            % transition.obj->getModelName() % transition.state
            % transition.nextState);
    }

    transition.obj->guards()[transition.id] =
        boost::bind(guard.guard, transition.obj, _1);
    return transition;
}

template < typename I >
StatechartTransition_t<I> operator<<(StatechartTransition_t<I> transition,
                                     Event_t event)
{
    if (transition.obj->existEvent(transition.id)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Statechart: transition %2% -> %3% have " \
                       "already an event"))
            % transition.obj->getModelName() % transition.state
            % transition.nextState);
    }
    if (transition.obj->existAfter(transition.id)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Statechart: transition %2% -> %3% have " \
                       "an after clause but it is incompatible with event"))
            % transition.obj->getModelName() % transition.state
            % transition.nextState);
    }
    if (transition.obj->existWhen(transition.id)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Statechart: transition %2% -> %3% have " \
                       "an when clause but it is incompatible with event"))
            % transition.obj->getModelName() % transition.state
            % transition.nextState);
    }

    transition.obj->events()[transition.id] = event.event;
    return transition;
}

template < typename I >
StatechartTransition_t<I> operator<<(StatechartTransition_t<I> transition,
                                     After_t < double > after)
{
    if (transition.obj->existAfter(transition.id)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Statechart: transition %2% -> %3% have " \
                       "already an after"))
            % transition.obj->getModelName() % transition.state
            % transition.nextState);
    }
    if (transition.obj->existEvent(transition.id)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Statechart: transition %2% -> %3% have " \
                       "an event but it is incompatible with after"))
            % transition.obj->getModelName() % transition.state
            % transition.nextState);
    }

    transition.obj->afters()[transition.id] = after.duration;
    return transition;
}

template < typename I, typename X >
StatechartTransition_t<I> operator<<(StatechartTransition_t<I> transition,
                                     After_t<X> after)
{
    if (transition.obj->existAfter(transition.id)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Statechart: transition %2% -> %3% have " \
                       "already an after"))
            % transition.obj->getModelName() % transition.state
            % transition.nextState);
    }
    if (transition.obj->existEvent(transition.id)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Statechart: transition %2% -> %3% have " \
                       "an event but it is incompatible with after"))
            % transition.obj->getModelName() % transition.state
            % transition.nextState);
    }

    transition.obj->afterFuncs()[transition.id] =
        boost::bind(after.duration, transition.obj, _1);
    return transition;
}

template < typename I >
StatechartTransition_t<I> operator<<(StatechartTransition_t<I> transition,
                                     When_t < double > when)
{
     if (transition.obj->existWhen(transition.id)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Statechart: transition %2% -> %3% have " \
                       "already a when"))
            % transition.obj->getModelName() % transition.state
            % transition.nextState);
    }
    if (transition.obj->existEvent(transition.id)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Statechart: transition %2% -> %3% have " \
                       "an event but it is incompatible with when"))
            % transition.obj->getModelName() % transition.state
            % transition.nextState);
    }

   transition.obj->whens()[transition.id] = when.date;
    return transition;
}

template < typename I, typename X >
StatechartTransition_t<I> operator<<(StatechartTransition_t<I> transition,
                                     When_t<X> when)
{
     if (transition.obj->existWhen(transition.id)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Statechart: transition %2% -> %3% have " \
                       "already a when"))
            % transition.obj->getModelName() % transition.state
            % transition.nextState);
    }
    if (transition.obj->existEvent(transition.id)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Statechart: transition %2% -> %3% have " \
                       "an event but it is incompatible with when"))
            % transition.obj->getModelName() % transition.state
            % transition.nextState);
    }

    transition.obj->whenFuncs()[transition.id] =
        boost::bind(when.date, transition.obj, _1);
    return transition;
}

template < typename I >
StatechartTransition_t<I> operator<<(StatechartTransition_t<I> transition,
                                     Output_t < std::string > output)
{
     if (transition.obj->existOuput(transition.id)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Statechart: transition %2% -> %3% have " \
                       "already a send action"))
            % transition.obj->getModelName() % transition.state
            % transition.nextState);
    }

    transition.obj->outputs()[transition.id] = output.output;
    return transition;
}

template < typename I, typename X >
StatechartTransition_t<I> operator<<(StatechartTransition_t<I> transition,
                                     Output_t<X> func)
{
     if (transition.obj->existOuput(transition.id)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Statechart: transition %2% -> %3% have " \
                       "already a send action"))
            % transition.obj->getModelName() % transition.state
            % transition.nextState);
    }

    transition.obj->outputFuncs()[transition.id] =
        boost::bind(func.output, transition.obj, _1, _2);
    return transition;
}

template < typename I, typename X >
StatechartTransition_t<I> operator<<(StatechartTransition_t<I> transition,
                                     EventAction_t<X> action)
{
     if (transition.obj->existAction(transition.id)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Statechart: transition %2% -> %3% have " \
                       "already an action"))
            % transition.obj->getModelName() % transition.state
            % transition.nextState);
    }
     if (not transition.obj->existEvent(transition.id)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Statechart: transition %2% -> %3% have " \
                       "not an event but an event action is defined"))
            % transition.obj->getModelName() % transition.state
            % transition.nextState);
    }

     transition.obj->eventTransitionActions()[transition.id] =
         boost::bind(action.action, transition.obj, _1, _2);
    return transition;
}

template < typename I, typename X >
StatechartTransition_t<I> operator<<(StatechartTransition_t<I> transition,
                                     GuardAction_t<X> action)
{
    if (transition.obj->existAction(transition.id)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Statechart: transition %2% -> %3% have " \
                       "already an action"))
            % transition.obj->getModelName() % transition.state
            % transition.nextState);
    }

    transition.obj->guardTransitionActions()[transition.id] =
        boost::bind(action.action, transition.obj, _1);
    return transition;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename I, typename X >
EventInState_t<I,X> operator>>(EventInState_t<I,X> event, int state)
{
    if (event.obj->existEventInState(state, event.event)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Statechart: state %2% have " \
                       "already an event action in state"))
            % event.obj->getModelName() % state);
    }

    event.obj->eventInStateActions(state)[event.event] =
        boost::bind(event.func, event.obj, _1, _2);
    return event;
}

template < typename X, typename I >
StatechartInAction_t<X,I> operator>>(StatechartInAction_t<X,I> action,
                                     int state)
{
    if (action.obj->existInAction(state)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Statechart: state %2% have " \
                       "already an in action"))
            % action.obj->getModelName() % state);
    }

    action.obj->inActions()[state] = boost::bind(action.func, action.obj, _1);
    return action;
}

template < typename X, typename I >
OutAction_t<X,I> operator>>(OutAction_t<X,I> action, int state)
{
    if (action.obj->existOutAction(state)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Statechart: state %2% have " \
                       "already an out action"))
            % action.obj->getModelName() % state);
    }

    action.obj->outActions()[state] = boost::bind(action.func, action.obj, _1);
    return action;
}

template < typename X, typename I >
Activity_t<X,I> operator>>(Activity_t<X,I> activity, int state)
{
    if (activity.obj->existActivity(state)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Statechart: state %2% have " \
                       "already an activity"))
            % activity.obj->getModelName() % state);
    }

    activity.obj->activities()[state] =
        boost::bind(activity.func, activity.obj, _1);
    return activity;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename I, typename X >
State2_t<I> operator<<(State2_t<I> state, EventInState2_t<X> event)
{
    if (state.obj->existEventInState(state.state, event.event)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Statechart: state %2% have " \
                       "already an event action in state"))
            % state.obj->getModelName() % state.state);
    }

    state.obj->eventInStateActions(state.state)[event.event] =
        boost::bind(event.func, state.obj, _1, _2);
    return state;
}

template < typename I, typename X >
State2_t<I> operator<<(State2_t<I> state, StatechartInAction2_t<X> action)
{
    if (state.obj->existInAction(state.state)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Statechart: state %2% have " \
                       "already an in action"))
            % state.obj->getModelName() % state.state);
    }

    state.obj->inActions()[state.state] =
        boost::bind(action.func, state.obj, _1);
    return state;
}

template < typename I, typename X >
State2_t<I> operator<<(State2_t<I> state, OutAction2_t<X> action)
{
    if (state.obj->existOutAction(state.state)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Statechart: state %2% have " \
                       "already an out action"))
            % state.obj->getModelName() % state.state);
    }

    state.obj->outActions()[state.state] =
        boost::bind(action.func, state.obj, _1);
    return state;
}

template < typename I, typename X >
State2_t<I> operator<<(State2_t<I> state, Activity2_t<X> activity)
{
    if (state.obj->existActivity(state.state)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Statechart: state %2% have " \
                       "already an activity"))
            % state.obj->getModelName() % state.state);
    }

    state.obj->activities()[state.state] =
        boost::bind(activity.func, state.obj, _1);
    return state;
}

}}} // namespace vle extension fsa

#endif
