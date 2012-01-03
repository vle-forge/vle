/*
 * @file vle/extension/fsa/FSA.hpp
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


#ifndef VLE_EXTENSION_FSA_FSA_HPP
#define VLE_EXTENSION_FSA_FSA_HPP 1

#include <vle/extension/DllDefines.hpp>
#include <vle/devs/Dynamics.hpp>

namespace vle { namespace extension { namespace fsa {

template < typename I >
struct State_t
{
    State_t(I obj) : obj(obj)  { }

    I obj;
};

template < typename I >
struct State2_t
{
    State2_t(I obj, int state) : obj(obj), state(state)  { }

    I obj;
    int state;
};

struct VLE_EXTENSION_EXPORT Event_t
{
    Event_t(const std::string& event) : event(event)  { }

    std::string event;
};

template < typename X >
struct Output_t
{
    Output_t(X output) : output(output)  { }

    X output;
};

template < typename X >
struct Action_t
{
    Action_t(X action) : action(action)  { }

    X action;
};

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

class VLE_EXTENSION_EXPORT Base : public vle::devs::Dynamics
{
public:
    Base(const vle::devs::DynamicsInit& init,
	 const vle::devs::InitEventList& events) :
        vle::devs::Dynamics(init, events), mInit(false)
    {}

    virtual ~Base() {}

    /**
     * @brief Specify the initial state.
     * @param state the name of initial state.
     */
    void initialState(int state);

    virtual vle::value::Value* observation(
        const vle::devs::ObservationEvent& event) const
    {
        if (event.onPort("state")) {
            return buildInteger(currentState());
        }
        return 0;
    }

    template < typename I >
        State_t<I> states(I obj)
    { return State_t<I>(obj); }

    template < typename I >
        State2_t<I> state(I obj, int state)
    { return State2_t<I>(obj, state); }

    Event_t event(const std::string& event_)
    { return Event_t(event_); }

    template < typename X >
        Output_t<X> send(const X& func)
    { return Output_t<X>(func); }

    template < typename X >
        Action_t<X> action(X action)
    { return Action_t<X>(action); }

protected:
    typedef std::vector < int > States;

    int currentState() const
    { return mCurrentState; }

    void currentState(int newState)
    { mCurrentState = newState; }

    bool existState(int state) const
    {
        return std::find(mStates.begin(), mStates.end(), state) !=
            mStates.end();
    }

    int initialState() const
    { return mInitialState; }

    bool isInit() const
    { return mInit; }

    vle::devs::ExternalEvent* cloneExternalEvent(
        vle::devs::ExternalEvent* event) const;

    States& states() { return mStates; }

private:
    // initial state is defined
    bool mInit;
    // Initial state
    int mInitialState;
    // Current state
    int mCurrentState;

protected:
    // List of states
    States mStates;

    template < typename I >
        friend State_t<I> operator<<(State_t<I> state, int name);
};

template <>
inline Output_t < std::string > Base::send< std::string >(
    const std::string& output_)
{ return Output_t < std::string >(output_); }

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename I >
State_t<I> operator<<(State_t<I> state, int name)
{
    if (state.obj->existState(name)) {
        throw vle::utils::ModellingError(
            vle::fmt(_("[%1%] FSA::Base: state %2% is already defined"))
            % state.obj->getModelName() % name);
    }

    state.obj->states().push_back(name);
    return state;
}

}}} // namespace vle extension fsa

#endif
