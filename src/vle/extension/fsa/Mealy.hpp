/*
 * @file vle/extension/fsa/Mealy.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
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


#ifndef VLE_EXTENSION_FSA_MEALY_HPP
#define VLE_EXTENSION_FSA_MEALY_HPP 1

#include <vle/extension/DllDefines.hpp>
#include <vle/extension/fsa/FSA.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace vle { namespace extension { namespace fsa {

template < typename I>
struct MealyTransition_t
{
    MealyTransition_t(I obj, int state, int nextState) :
        obj(obj), state(state), nextState(nextState)  { }

    I obj;
    int state;
    int nextState;
    std::string event;
};

class VLE_EXTENSION_EXPORT Mealy : public Base
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
        std::pair < Outputs::iterator, bool > r;

        r = mOutputs.insert(std::make_pair(s, Output()));

        return r.first->second;
    }

    OutputFunc& outputFuncs(int s)
    {
        std::pair < OutputFuncs::iterator, bool > r;

        r = mOutputFuncs.insert(std::make_pair(s, OutputFunc()));

        return r.first->second;
    }

    Transition& transitions(int s)
    {
        std::pair < Transitions::iterator, bool > r;

        r = mTransitions.insert(std::make_pair(s, Transition()));

        return r.first->second;
    }

    Actions& actions(int s)
    {
        std::pair < ActionsMap::iterator, bool > r;

        r = mActions.insert(std::make_pair(s, Actions()));

        return r.first->second;
    }

    // Output_t < std::string > output(const std::string& output)
    // { return Base::outputF(output); }

    template < typename I >
        MealyTransition_t<I> transition(I obj, int state, int nextState)
    { return MealyTransition_t<I>(obj, state, nextState); }

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
    virtual void confluentTransitions(
        const devs::Time& time,
        const devs::ExternalEventList& extEventlist)
    {
        externalTransition(extEventlist, time);
        internalTransition(time);
    }

    enum Phase { IDLE, PROCESSING };

    Phase mPhase;
    EventListLILO mToProcessEvents;
};

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename I >
MealyTransition_t<I> operator<<(MealyTransition_t<I> transition,
                                Event_t event)
{
    transition.obj->transitions(transition.state)[event.event] =
        transition.nextState;
    transition.event = event.event;
    return transition;
}

template < typename I >
MealyTransition_t<I> operator<<(MealyTransition_t<I> transition,
                                Output_t < std::string > output)
{
    transition.obj->outputs(transition.state)[transition.event] =
        output.output;
    return transition;
}

template < typename I, typename X >
MealyTransition_t<I> operator<<(MealyTransition_t<I> transition,
                                Output_t<X> func)
{
    transition.obj->outputFuncs(transition.state)[transition.event] =
        boost::bind(func.output, transition.obj, _1, _2);
    return transition;
}

template < typename I, typename X >
MealyTransition_t<I> operator<<(MealyTransition_t<I> transition,
                                Action_t<X> action)
{
    transition.obj->actions(transition.state)[transition.event] =
        boost::bind(action.action, transition.obj, _1, _2);
    return transition;
}

}}} // namespace vle extension fsa

#endif
