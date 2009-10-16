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

#include <vle/extension/DllDefines.hpp>
#include <vle/extension/FSA.hpp>
#include <boost/assign.hpp>
#include <boost/bind.hpp>

namespace vle { namespace extension {

class Moore : public Base
{
public:
    Moore(const devs::DynamicsInit& init,
          const devs::InitEventList& events)
        : Base(init, events)
    {}

    virtual ~Moore() {}

    virtual devs::ExternalEventList select(
        const devs::ExternalEventList& events)
    { return events; }

private:
    // output
    typedef std::map < int, std::string > Outputs;
    typedef boost::function < void (const devs::Time&,
                                    devs::ExternalEventList&) > OutputFunc;
    typedef std::map < int, OutputFunc > OutputFuncs;

    // transition
    typedef std::map < std::string, int > Transition;
    typedef std::map < int, Transition > Transitions;

    // action
    typedef boost::function <
        void (const devs::Time&,
              const devs::ExternalEvent* event) > Action;
    typedef std::map < int, Action > Actions;

public:
    Actions& actions() { return mActions; }
    Outputs& outputs() { return mOutputs; }
    OutputFuncs& outputFuncs() { return mOutputFuncs; }

    Transition& transitions(int s)
    {
        if (mTransitions.find(s) == mTransitions.end()) {
            mTransitions[s] = Transition();
        }
        return mTransitions.at(s);
    }

private:
    typedef Outputs::const_iterator OutputsIterator;
    typedef OutputFuncs::const_iterator OutputFuncsIterator;
    typedef Actions::const_iterator ActionsIterator;
    typedef Transitions::const_iterator TransitionsIterator;
    typedef Transition::const_iterator TransitionIterator;
    typedef std::list < devs::ExternalEventList* > EventListLILO;

    // List of actions
    Actions mActions;
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
    virtual void internalTransition(const devs::Time& time);
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

template < typename I >
struct Transition_t
{
    Transition_t(I obj, int state, int nextState) :
        obj(obj), state(state), nextState(nextState)  { }

    I obj;
    int state;
    int nextState;
};

template < typename I >
Transition_t<I> transition(I obj, int state, int nextState)
{ return Transition_t<I>(obj, state, nextState); }

template < typename I >
void operator<<(Transition_t<I> transition, Event_t event)
{
    insert(transition.obj->transitions(transition.state))(
        event.event, transition.nextState);
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

template < typename X, typename I >
InAction_t<X,I> operator>>(InAction_t<X,I> action, int state)
{
    boost::assign::insert(action.obj->actions())(
        state, boost::bind(action.func, action.obj, _1, _2));
    return action;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename I >
struct Output2_t
{
    Output2_t(I obj, int state) :
        obj(obj), state(state)  { }

    I obj;
    int state;
};

template < typename I >
Output2_t<I> output(I obj, int state)
{ return Output2_t<I>(obj, state); }

template < typename I >
void operator>>(Output2_t<I> output, const std::string& port)
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

template < typename X, typename I >
void operator>>(OutputFunc2_t<X,I> output, int state)
{
    boost::assign::insert(output.obj->outputFuncs())(
        state, boost::bind(output.func, output.obj, _1, _2));
}

}} // namespace vle extension

#endif
