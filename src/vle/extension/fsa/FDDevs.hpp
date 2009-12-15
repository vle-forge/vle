/**
 * @file vle/extension/fsa/FDDevs.hpp
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


#ifndef VLE_EXTENSION_FSA_FDDEVS_HPP
#define VLE_EXTENSION_FSA_FDDEVS_HPP 1

#include <vle/extension/DllDefines.hpp>
#include <vle/extension/fsa/FSA.hpp>
#include <vle/utils/i18n.hpp>
#include <boost/assign.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace vle { namespace extension { namespace fsa {

template < typename I >
struct Internal_t
{
    Internal_t(I obj, int state) :
        obj(obj), state(state)  { }

    I obj;
    int state;
};

template < typename I >
struct Duration_t
{
    Duration_t(I obj, int state) :
        obj(obj), state(state)  { }

    I obj;
    int state;
};

template < typename I >
struct External_t
{
    External_t(I obj, int state, const std::string& input) :
        obj(obj), state(state), input(input)  { }

    I obj;
    int state;
    std::string input;
};

template < typename I >
struct FDDevsOutput_t
{
    FDDevsOutput_t(I obj, int state) :
        obj(obj), state(state)  { }

    I obj;
    int state;
};

template < typename X, typename I >
struct FDDevsOutputFunc_t
{
    FDDevsOutputFunc_t(I obj, X func) : obj(obj), func(func)  { }

    I obj;
    X func;
};

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
class VLE_EXTENSION_EXPORT FDDevs : public Base
{
public:
    FDDevs(const devs::DynamicsInit& init,
           const devs::InitEventList& events)
        : Base(init, events)
    {}

    virtual ~FDDevs() {}

    virtual devs::ExternalEventList select(
        const devs::ExternalEventList& events)
    { return events; }

private:
    // duration
    typedef std::map < int , devs::Time > Durations;

    // output
    typedef std::map < int , std::string > Outputs;
    typedef boost::function < void (const devs::Time&,
                                    devs::ExternalEventList&) > OutputFunc;
    typedef std::map < int , OutputFunc > OutputFuncs;

    // internal
    typedef std::map < int , int > Internals;

    // external
    typedef std::map < std::string , int > External;
    typedef std::map < int , std::map < std::string , int > > Externals;

public:
    Durations& durations() { return mDurations; }
    Outputs& outputs() { return mOutputs; }
    OutputFuncs& outputFuncs() { return mOutputFuncs; }
    Internals& internals() { return mInternals; }

    External& externals(int s)
    {
        std::pair < Externals::iterator, bool > r;

        r = mExternals.insert(std::make_pair(s, External()));

        return r.first->second;
    }

    virtual devs::Event::EventType confluentTransitions(
        const devs::Time& /* time */,
        const devs::ExternalEventList& /* extEventlist */) const
    { return devs::Event::EXTERNAL; }

    template < typename I >
        Internal_t<I> internal(I obj, int state)
    { return Internal_t<I>(obj, state); }

    template < typename I >
        Duration_t<I> duration(I obj, int state)
    { return Duration_t<I>(obj, state); }

    template < typename I >
        External_t<I> external(I obj, int state, const std::string& input)
    { return External_t<I>(obj, state, input); }

    template < typename I >
        FDDevsOutput_t<I> output(I obj, int state)
    { return FDDevsOutput_t<I>(obj, state); }

    template < typename X, typename I >
        FDDevsOutputFunc_t<X, I> outputFunc(I obj, X func)
    { return FDDevsOutputFunc_t<X, I>(obj, func); }

private:
    typedef Outputs::const_iterator OutputsIterator;
    typedef OutputFuncs::const_iterator OutputFuncsIterator;
    typedef Internals::const_iterator InternalsIterator;
    typedef Externals::const_iterator ExternalsIterator;

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

template < typename I >
void operator>>(Internal_t<I> internal, int newState)
{
    insert(internal.obj->internals())(internal.state, newState);
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename I >
void operator<<(Duration_t<I> duration, const devs::Time& value)
{
    insert(duration.obj->durations())(duration.state, value);
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename I >
void operator>>(External_t<I> external, int state)
{
    insert(external.obj->externals(external.state))(external.input, state);
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename I >
void operator>>(FDDevsOutput_t<I> output, const std::string& port)
{
    insert(output.obj->outputs())(output.state, port);
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

template < typename X, typename I >
void operator>>(FDDevsOutputFunc_t<X,I> output, int state)
{
    boost::assign::insert(output.obj->outputFuncs())(
        state, boost::bind(output.func, output.obj, _1, _2));
}

}}} // namespace vle extension fsa

#endif
