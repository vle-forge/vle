/**
 * @file vle/extension/FSA.hpp
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


#ifndef VLE_EXTENSION_FSA_HPP
#define VLE_EXTENSION_FSA_HPP

#include <vle/extension/DllDefines.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Debug.hpp>
#include <boost/assign.hpp>
#include <boost/bind.hpp>

namespace vle { namespace extension {

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

    typedef std::vector < int > States;

    States& states() { return mStates; }

    virtual vle::value::Value* observation(
        const vle::devs::ObservationEvent& event) const
    {
        if (event.onPort("state")) {
            return buildInteger(currentState());
        }
        return 0;
    }

protected:
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
};

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename I >
struct State_t
{
    State_t(I obj) : obj(obj)  { }

    I obj;
};

template < typename I >
State_t<I> states(I obj)
{ return State_t<I>(obj); }

template < typename C, typename I >
State_t<I> operator<<(State_t<I> state, const C& name)
{
    state.obj->states() += name;
    return state;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

struct VLE_EXTENSION_EXPORT Event_t
{
    Event_t(const std::string& event) : event(event)  { }

    std::string event;
};

Event_t VLE_EXTENSION_EXPORT event(const std::string& event);

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

struct VLE_EXTENSION_EXPORT Output_t
{
    Output_t(const std::string& output) : output(output)  { }

    std::string output;
};

Output_t VLE_EXTENSION_EXPORT output(const std::string& output);

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename X >
struct OutputFunc_t
{
    OutputFunc_t(X func) : func(func)  { }

    X func;
};

template < typename X >
OutputFunc_t<X> outputFunc(X func)
{ return OutputFunc_t<X>(func); }

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename X >
struct Action_t
{
    Action_t(X action) : action(action)  { }

    X action;
};

template < typename X >
Action_t<X> action(X action)
{ return Action_t<X>(action); }

}} // namespace vle extension

#endif
