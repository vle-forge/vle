/**
 * @file vle/extension/fsa/Moore.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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


#ifndef VLE_EXTENSION_FSA_MOORE_HPP
#define VLE_EXTENSION_FSA_MOORE_HPP 1

#include <vle/extension/DllDefines.hpp>
#include <vle/extension/fsa/FSA.hpp>
#include <boost/assign.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace vle { namespace extension { namespace fsa {

template < typename I >
struct MooreTransition_t
{
    MooreTransition_t(I obj, int state, int nextState) :
        obj(obj), state(state), nextState(nextState)  { }

    I obj;
    int state;
    int nextState;
};

template < typename X, typename I >
struct MooreInAction_t
{
    MooreInAction_t(I obj, X func) : obj(obj), func(func) { }

    I obj;
    X func;
};

template < typename I >
struct MooreOutput_t
{
    MooreOutput_t(I obj, int state) :
        obj(obj), state(state)  { }

    I obj;
    int state;
};

template < typename X, typename I >
struct MooreOutputFunc_t
{
    MooreOutputFunc_t(I obj, X func) : obj(obj), func(func)  { }

    I obj;
    X func;
};

class VLE_EXTENSION_EXPORT Moore : public Base
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
        std::pair < Transitions::iterator, bool > r;

        r = mTransitions.insert(std::make_pair(s, Transition()));

        return r.first->second;
    }

    template < typename I >
        MooreTransition_t<I> transition(I obj, int state, int nextState)
    { return MooreTransition_t<I>(obj, state, nextState); }

    template < typename X, typename I >
        MooreInAction_t<X, I> inAction(I obj, X func)
    { return MooreInAction_t<X, I>(obj, func); }

    template < typename I >
        MooreOutput_t<I> output(I obj, int state)
    { return MooreOutput_t<I>(obj, state); }

    template < typename X, typename I >
        MooreOutputFunc_t<X, I> outputFunc(I obj, X func)
    { return MooreOutputFunc_t<X, I>(obj, func); }

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
void operator<<(MooreTransition_t<I> transition, Event_t event)
{
    insert(transition.obj->transitions(transition.state))(
        event.event, transition.nextState);
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename X, typename I >
MooreInAction_t<X,I> operator>>(MooreInAction_t<X,I> action, int state)
{
    boost::assign::insert(action.obj->actions())(
        state, boost::bind(action.func, action.obj, _1, _2));
    return action;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename I >
void operator>>(MooreOutput_t<I> output, const std::string& port)
{
    insert(output.obj->outputs())(output.state, port);
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename X, typename I >
void operator>>(MooreOutputFunc_t<X,I> output, int state)
{
    boost::assign::insert(output.obj->outputFuncs())(
        state, boost::bind(output.func, output.obj, _1, _2));
}

}}} // namespace vle extension fsa

#endif
