/**
 * @file vle/extension/FDDevs.hpp
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


#ifndef VLE_EXTENSION_FDDEVS_HPP
#define VLE_EXTENSION_FDDEVS_HPP

#include <vle/extension/DllDefines.hpp>
#include <vle/extension/FSA.hpp>
#include <vle/utils/i18n.hpp>
#include <boost/assign.hpp>
#include <boost/bind.hpp>

namespace vle { namespace extension {

/**
 * @brief Devs The class Devs is a DEVS mapping of structure and
 * dynamics of the DEVS formalism with a finite number of
 * discrete states. Two kind of transitions are possible :
 * internal on fixed duration and external on event.
 * At each internal transition, external event without data
 * can be generated.
 * The structure and dynamics of Finite State Automaton are
 * specified with C++ code.
 */
template < typename C >
class FDDevs : public Base < C >
{
public:
    FDDevs(const devs::DynamicsInit& init,
           const devs::InitEventList& events)
        : Base < C >(init, events)
    {}

    virtual ~FDDevs() {}

    virtual devs::ExternalEventList select(
        const devs::ExternalEventList& events)
    { return events; }

private:
    // duration
    typedef std::map < C , devs::Time > Durations;

    // output
    typedef std::map < C , std::string > Outputs;
    typedef boost::function < void (const devs::Time&,
                                    devs::ExternalEventList&) > OutputFunc;
    typedef std::map < C , OutputFunc > OutputFuncs;

    // internal
    typedef std::map < C , C > Internals;

    // external
    typedef std::map < std::string , C > External;
    typedef std::map < C , std::map < std::string , C > > Externals;

public:
    Durations& durations() { return mDurations; }
    Outputs& outputs() { return mOutputs; }
    OutputFuncs& outputFuncs() { return mOutputFuncs; }
    Internals& internals() { return mInternals; }

    External& externals(const C& c)
    {
        if (mExternals.find(c) == mExternals.end()) {
            mExternals[c] = External();
        }
        return mExternals.at(c);
    }

    virtual devs::Event::EventType confluentTransitions(
        const devs::Time& /* time */,
        const devs::ExternalEventList& /* extEventlist */) const
    { return devs::Event::EXTERNAL; }

private:
    typedef typename Outputs::const_iterator OutputsIterator;
    typedef typename OutputFuncs::const_iterator OutputFuncsIterator;
    typedef typename Internals::const_iterator InternalsIterator;
    typedef typename Externals::const_iterator ExternalsIterator;

    // Next states in case of external transition
    Externals mExternals;
    // List of durations of states
    Durations mDurations;
    // List of ports for output function
    Outputs mOutputs;
    // List of output functions
    OutputFuncs mOutputFuncs;
    // Next states in case of internal transition
    Internals mInternals;

    void process(const devs::ExternalEvent* event,
                 const devs::Time& time);

    virtual devs::Time init(const devs::Time& time);
    virtual void output(const devs::Time& time,
                        devs::ExternalEventList& output) const;
    virtual devs::Time timeAdvance() const
    { return mSigma; }
    virtual void internalTransition(const devs::Time& event);
    virtual void externalTransition(
        const devs::ExternalEventList& events,
        const devs::Time& time);

    devs::Time mLastTime;
    devs::Time mSigma;
};

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename C >
void FDDevs<C>::process(const devs::ExternalEvent* event,
                        const devs::Time& time)
{
    if (mExternals.find(Base<C>::currentState()) != mExternals.end() and
        (mExternals[Base<C>::currentState()].find(event->getPortName()) !=
         mExternals[Base<C>::currentState()].end())) {
        Base<C>::currentState(mExternals[Base<C>::currentState()]
                              [event->getPortName()]);

        if (mDurations.find(Base<C>::currentState()) == mDurations.end()) {
            throw utils::InternalError(
                fmt(_("FSA::FDDevs model, unknow duration of state %1%")) %
                Base<C>::currentState());
        }

        mSigma = mDurations[Base<C>::currentState()];
    }
    else mSigma -= time - mLastTime;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename C >
void FDDevs<C>::output(const devs::Time& time,
                       devs::ExternalEventList& output) const
{
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

template < typename C >
devs::Time FDDevs<C>::init(const devs::Time& /* time */)
{
    if (not Base<C>::isInit()) {
        throw utils::InternalError(
            _("FSA::FDDevs model, initial state not defined"));
    }

    Base<C>::currentState(Base<C>::initialState());

    if (mDurations.find(Base<C>::currentState()) == mDurations.end()) {
        throw utils::InternalError(fmt(
                _("FSA::FDDevs model, unknow duration of state %1%")) %
        Base<C>::currentState());
    }

    mSigma = mDurations[Base<C>::currentState()];
    return mSigma;
}

template < typename C >
void FDDevs<C>::externalTransition(const devs::ExternalEventList& events,
                                   const devs::Time& time)
{
    if (events.size() > 1) {
        devs::ExternalEventList sortedEvents = select(events);
        devs::ExternalEventList::const_iterator it = sortedEvents.begin();

        while (it != sortedEvents.end()) {
            process(*it, time);
            ++it;
        }
    } else {
        devs::ExternalEventList::const_iterator it = events.begin();

        process(*it, time);
    }
}

template < typename C >
void FDDevs<C>::internalTransition(const devs::Time& time)
{
    if (mInternals.find(Base<C>::currentState()) == mInternals.end()) {
        throw utils::InternalError(fmt(
                _("FSA::FDDevs model, unknow internal transition on state %1%"))
                % Base<C>::currentState());
    }

    Base<C>::currentState(mInternals[Base<C>::currentState()]);

    if (mDurations.find(Base<C>::currentState()) == mDurations.end()) {
        throw utils::InternalError(fmt(
                _("FSA::FDDevs model, unknow duration of state %1%")) %
            Base<C>::currentState());
    }

    mSigma = mDurations[Base<C>::currentState()];
    mLastTime = time;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename C, typename I >
struct Internal_t
{
    Internal_t(I obj, const C& state) :
        obj(obj), state(state)  { }

    I obj;
    C state;
};

template < typename C, typename I >
Internal_t<C,I> internal(I obj, const C& state)
{ return Internal_t<C,I>(obj, state); }

template < typename C, typename I >
void operator>>(Internal_t<C,I> internal, const C& newState)
{
    insert(internal.obj->internals())(internal.state, newState);
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename C, typename I >
struct Duration_t
{
    Duration_t(I obj, const C& state) :
        obj(obj), state(state)  { }

    I obj;
    C state;
};

template < typename C, typename I >
Duration_t<C,I> duration(I obj, const C& state)
{ return Duration_t<C,I>(obj, state); }

template < typename C, typename I >
void operator<<(Duration_t<C,I> duration, const devs::Time& value)
{
    insert(duration.obj->durations())(duration.state, value);
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename C, typename I >
struct External_t
{
    External_t(I obj, C state, const std::string& input) :
        obj(obj), state(state), input(input)  { }

    I obj;
    C state;
    std::string input;
};

template < typename C, typename I >
External_t<C,I> external(I obj, C state, const std::string& input)
{ return External_t<C,I>(obj, state, input); }

template < typename C, typename I >
void operator>>(External_t<C,I> external, const C& state)
{
    insert(external.obj->externals(external.state))(external.input, state);
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename C, typename I >
struct Output3_t
{
    Output3_t(I obj, const C& state) :
        obj(obj), state(state)  { }

    I obj;
    C state;
};

template < typename C, typename I >
Output3_t<C,I> output(I obj, const C& state)
{ return Output3_t<C,I>(obj, state); }

template < typename C, typename I >
void operator>>(Output3_t<C,I> output, const std::string& port)
{
    insert(output.obj->outputs())(output.state, port);
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename X, typename I >
struct OutputFunc3_t
{
    OutputFunc3_t(I obj, X func) : obj(obj), func(func)  { }

    I obj;
    X func;
};

template < typename X, typename I >
OutputFunc3_t<X, I> outputFunc(I obj, X func)
{ return OutputFunc3_t<X, I>(obj, func); }

template < typename C, typename X, typename I >
void operator>>(OutputFunc3_t<X,I> output, const C& state)
{
    boost::assign::insert(output.obj->outputFuncs())(
        state, boost::bind(output.func, output.obj, _1, _2));
}

}} // namespace vle extension

#endif
