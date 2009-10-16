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

#include <vle/extension/DllDefines.hpp>
#include <vle/extension/FSA.hpp>
#include <boost/assign.hpp>
#include <boost/bind.hpp>

namespace vle { namespace extension {

class Mealy : public Base
{
public:
    Mealy(const devs::DynamicsInit& init,
          const devs::InitEventList& events) :
        Base(init, events)
    {}

    virtual ~Mealy()
    {}

    virtual devs::ExternalEventList select(
        const devs::ExternalEventList& events)
    { return events; }

private:
    // output
    typedef std::map < std::string, std::string > Output;
    typedef std::map < int, Output > Outputs;

    typedef std::map < std::string,
            boost::function <
                void (const devs::Time&,
                      devs::ExternalEventList&) >  > OutputFunc;
    typedef std::map < int, OutputFunc > OutputFuncs;

    // transition
    typedef std::map < std::string, int > Transition;
    typedef std::map < int, Transition > Transitions;

    // action
    typedef boost::function <
        void (const devs::Time&,
              const devs::ExternalEvent* event) > Action;
    typedef std::map < std::string, Action > Actions;
    typedef std::map < int, Actions > ActionsMap;

public:
    Output& outputs(int s)
    {
        if (mOutputs.find(s) == mOutputs.end()) {
            mOutputs[s] = Output();
        }
        return mOutputs.at(s);
    }

    OutputFunc& outputFuncs(int s)
    {
        if (mOutputFuncs.find(s) == mOutputFuncs.end()) {
            mOutputFuncs[s] = OutputFunc();
        }
        return mOutputFuncs.at(s);
    }

    Transition& transitions(int s)
    {
        if (mTransitions.find(s) == mTransitions.end()) {
            mTransitions[s] = Transition();
        }
        return mTransitions.at(s);
    }

    Actions& actions(int s)
    {
        if (mActions.find(s) == mActions.end()) {
            mActions[s] = Actions();
        }
        return mActions.at(s);
    }

private:
    typedef ActionsMap::const_iterator ActionsIterator;
    typedef Outputs::const_iterator OutputsIterator;
    typedef OutputFuncs::const_iterator OutputFuncsIterator;
    typedef Transitions::const_iterator TransitionsIterator;
    typedef std::list < devs::ExternalEventList* > EventListLILO;

    // List of actions
    ActionsMap mActions;
    // List of ports for output function
    Outputs mOutputs;
    // List of output functions
    OutputFuncs mOutputFuncs;
    // Next states in case of external transition
    Transitions mTransitions;

    void process(const devs::Time& time,
                 const devs::ExternalEvent* event);

    virtual devs::Time init(const devs::Time& time);
    virtual void output(const devs::Time& time,
                        devs::ExternalEventList& output) const;
    virtual devs::Time timeAdvance() const;
    virtual void internalTransition(const devs::Time& event);
    virtual void externalTransition(
        const devs::ExternalEventList& event,
        const devs::Time& time);
    virtual devs::Event::EventType confluentTransitions(
        const devs::Time& /* time */,
        const devs::ExternalEventList& /* extEventlist */) const
    { return devs::Event::EXTERNAL; }

    enum Phase { IDLE, PROCESSING };

    Phase mPhase;
    EventListLILO mToProcessEvents;
};

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename I>
struct Transition_t
{
    Transition_t(I obj, int state, int nextState) :
        obj(obj), state(state), nextState(nextState)  { }

    I obj;
    int state;
    int nextState;
    std::string event;
};

template < typename I >
Transition_t<I> transition(I obj, int state, int nextState)
{ return Transition_t<I>(obj, state, nextState); }

template < typename I >
Transition_t<I> operator<<(Transition_t<I> transition, Event_t event)
{
    insert(transition.obj->transitions(transition.state))(event.event,
                                                          transition.nextState);
    transition.event = event.event;
    return transition;
}

template < typename I >
Transition_t<I> operator<<(Transition_t<I> transition, Output_t output)
{
    insert(transition.obj->outputs(transition.state))(transition.event,
                                                      output.output);
    return transition;
}

template < typename I, typename X >
Transition_t<I> operator<<(Transition_t<I> transition, OutputFunc_t<X> func)
{
    boost::assign::insert(transition.obj->outputFuncs(transition.state))(
        transition.event, boost::bind(func.func, transition.obj, _1, _2));
    return transition;
}

template < typename I, typename X >
Transition_t<I> operator<<(Transition_t<I> transition, Action_t<X> action)
{
    boost::assign::insert(transition.obj->actions(transition.state))(
        transition.event, boost::bind(action.action, transition.obj, _1, _2));
    return transition;
}

}} // namespace vle extension

#endif
