/**
 * @file vle/extension/fsa/Statechart.hpp
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


#ifndef VLE_EXTENSION_FSA_STATECHART_HPP
#define VLE_EXTENSION_FSA_STATECHART_HPP

#include <vle/extension/DllDefines.hpp>
#include <vle/extension/fsa/FSA.hpp>
#include <vle/utils/DateTime.hpp>
#include <boost/assign.hpp>
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

template < typename X, typename I >
struct OutAction_t
{
    OutAction_t(I obj, X func) : obj(obj), func(func) { }

    I obj;
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
struct Guard_t
{
    Guard_t(X guard) : guard(guard)  { }

    X guard;
};

struct VLE_EXTENSION_EXPORT After_t
{
    After_t(const devs::Time& duration) : duration(duration)  { }

    devs::Time duration;
};

struct VLE_EXTENSION_EXPORT When_t
{
    When_t(const devs::Time& date) : date(date)  { }

    devs::Time date;
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
    typedef std::map < int, devs::Time > Afters;

    // when on transition
    typedef std::map < int, devs::Time > Whens;

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
    Whens& whens() { return mWhens; }
    Outputs& outputs() { return mOutputs; }
    OutputFuncs& outputFuncs() { return mOutputFuncs; }
    EventTransitionActions& eventTransitionActions()
    { return mEventTransitionActions; }
    GuardTransitionActions& guardTransitionActions()
    { return mGuardTransitionActions; }

    template < typename X, typename I >
        StatechartInAction_t<X, I> inAction(I obj, X func)
    { return StatechartInAction_t<X, I>(obj, func); }

    template < typename X, typename I >
        OutAction_t<X, I> outAction(I obj, X func)
    { return OutAction_t<X, I>(obj, func); }

    template < typename X, typename I >
        Activity_t<X, I> activity(I obj, X func)
    { return Activity_t<X, I>(obj, func); }

    template < typename X >
        Guard_t<X> guard(X guard)
    { return Guard_t<X>(guard); }

    After_t after(const devs::Time& duration)
    { return After_t(duration); }

    When_t when(const devs::Time& date)
    { return When_t(date); }

    template < typename I >
        StatechartTransition_t<I> transition(I obj, int state, int nextState)
    {
        int id = obj->addTransition(state, nextState);

        return StatechartTransition_t<I>(obj, state, nextState, id);
    }

    template < typename I, typename X >
        EventInState_t<I,X> eventInState(I obj, const std::string& event,
                                         X func)
    { return EventInState_t<I,X>(obj, event, func); }

    Output_t output(const std::string& output_)
    { return Base::output(output_); }

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
    typedef Whens::const_iterator WhensIterator;
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
    int findTransition(const devs::ExternalEvent* event) const;
    void process(const devs::Time& time,
                 const devs::ExternalEvent* event);
    void process(const devs::Time& time,
                 int transitionId);
    void processActivities(const devs::Time& time);
    void processEventInStateActions(const devs::Time& time,
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

    virtual devs::Time init(const devs::Time& time);
    virtual void output(const devs::Time& time,
                        devs::ExternalEventList& output) const;
    virtual devs::Time timeAdvance() const;
    virtual void internalTransition(const devs::Time& time);
    virtual void externalTransition(
        const devs::ExternalEventList& event,
        const devs::Time& time);
    virtual devs::Event::EventType confluentTransitions(
        const devs::Time& /* time */,
        const devs::ExternalEventList& /* extEventlist */) const
    { return devs::Event::EXTERNAL; }

    enum Phase { IDLE, PROCESSING, SEND };

    Phase mPhase;
    devs::Time mLastTime;
    devs::Time mSigma;
    EventListLILO mToProcessEvents;
    std::pair < int, int > mToProcessGuard;
    std::pair < int, int > mToProcessAfterWhen;
    bool mValidGuard;
    bool mValidAfterWhen;
};

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename C, typename X, typename I >
StatechartInAction_t<X,I> operator>>(StatechartInAction_t<X,I> action,
                                     const C& state)
{
    boost::assign::insert(action.obj->inActions())(
        state, boost::bind(action.func, action.obj, _1));
    return action;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename X, typename I >
OutAction_t<X,I> operator>>(OutAction_t<X,I> action, int state)
{
    boost::assign::insert(action.obj->outActions())(
        state, boost::bind(action.func, action.obj, _1));
    return action;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename X, typename I >
Activity_t<X,I> operator>>(Activity_t<X,I> activity, int state)
{
    boost::assign::insert(activity.obj->activities())(
        state, boost::bind(activity.func, activity.obj, _1));
    return activity;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename I, typename X >
StatechartTransition_t<I> operator<<(StatechartTransition_t<I> transition,
                                     Guard_t<X> guard)
{
    transition.obj->guards()[transition.id] =
        boost::bind(guard.guard, transition.obj, _1);
    return transition;
}

template < typename I >
StatechartTransition_t<I> operator<<(StatechartTransition_t<I> transition,
                                     Event_t event)
{
    transition.obj->events()[transition.id] = event.event;
    return transition;
}

template < typename I >
StatechartTransition_t<I> operator<<(StatechartTransition_t<I> transition,
                                     After_t after)
{
    transition.obj->afters()[transition.id] = after.duration;
    return transition;
}

template < typename I >
StatechartTransition_t<I> operator<<(StatechartTransition_t<I> transition,
                                     When_t when)
{
    transition.obj->whens()[transition.id] = when.date;
    return transition;
}

template < typename I >
StatechartTransition_t<I> operator<<(StatechartTransition_t<I> transition,
                                     Output_t output)
{
    transition.obj->outputs()[transition.id] = output.output;
    return transition;
}

template < typename I, typename X >
StatechartTransition_t<I> operator<<(StatechartTransition_t<I> transition,
                                     OutputFunc_t<X> func)
{
    transition.obj->outputFuncs()[transition.id] =
        boost::bind(func.func, transition.obj, _1, _2);
    return transition;
}

template < typename I, typename X >
StatechartTransition_t<I> operator<<(StatechartTransition_t<I> transition,
                                     Action_t<X> action)
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
EventInState_t<I,X> operator>>(EventInState_t<I,X> event, int state)
{
    insert(event.obj->eventInStateActions(state))(
        event.event, boost::bind(event.func, event.obj, _1, _2));
    return event;
}

}}} // namespace vle extension fsa

#endif
