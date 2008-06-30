/**
 * @file src/vle/extension/PetriNet.cpp
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


#include <vle/extension/PetriNet.hpp>
#include <boost/checked_delete.hpp>
#include <algorithm>

namespace vle { namespace extension {

PetriNet::Marking::~Marking()
{
    std::for_each(mTokens.begin(), mTokens.end(),
                  boost::checked_deleter < Token >());
}

bool PetriNet::Marking::removeTokens(unsigned int number)
{
    for (unsigned int i = 0; i < number; i++) {
        Token* token = mTokens.back();

        mTokens.pop_back();
        delete token;
    }
    return mTokens.empty();
}

void PetriNet::addEnabledTransition(Transition* transition)
{
    if (std::find(mEnabledTransitions.begin(),
                  mEnabledTransitions.end(),
                  transition) == mEnabledTransitions.end()) {
        mEnabledTransitions.push_back(transition);
    }
}

void PetriNet::computeEnabledTransition(const devs::Time& /* time */)
{
    TransitionList::const_iterator it = mTransitions.begin();

    mEnabledTransitions.clear();
    while (it != mTransitions.end()) {
        if (isEnabled(it->second)) {
            addEnabledTransition(it->second);
        }
        ++it;
    }
}

void PetriNet::fire(const devs::Time& time)
{
    int index = rand().get_int_range(0, mEnabledTransitions.size());
    Transition* transition = mEnabledTransitions[index];

    if (goInTransition(transition)) {
        goOutTransition(transition, time);
    }
    mEnabledTransitions.erase(std::find(mEnabledTransitions.begin(),
                                        mEnabledTransitions.end(),
                                        transition));
}

bool PetriNet::goInTransition(Transition* transition)
{
    if (isEnabled(transition)) {
        const InputList& inputs = transition->inputs();
        InputList::const_iterator it = inputs.begin();

        while (it != inputs.end()) {
            Place* place = (*it)->getPlace();
            MarkingList::const_iterator itm = mMarkings.find(place->getName());

            if (removeTokens(itm->second, (*it)->getConsumedTokenNumber())) {
                mMarkings.erase(place->getName());
                delete itm->second;
            }
            if (transition->outputs().size() == 0 and
                mOutTransitionMarkings.find(transition->getName()) !=
                mOutTransitionMarkings.end()) {
                mOutTransitionMarkings[transition->getName()].second = true;
            }
            ++it;
        }
        return true;
    } else {
        return false;
    }
}

void PetriNet::putTokens(Transition* transition,
                         const devs::Time& time)
{
    const OutputList& outputs = transition->outputs();

    for (OutputList::const_iterator it = outputs.begin() ;
         it != outputs.end(); it++) {
        Place* place = (*it)->getPlace();
        unsigned int producedTokenNumber = (*it)->getProducedTokenNumber();

        if (producedTokenNumber > 0) {
            MarkingList::const_iterator itm = mMarkings.find(place->getName());
            Marking* marking = 0;

            if (itm == mMarkings.end()) {
                marking = new Marking(place);
                mMarkings[place->getName()] = marking;
            } else {
                marking = itm->second;
            }
            for (unsigned int i = 0; i < producedTokenNumber; i++) {
                putToken(marking, time);
            }
        }
    }
}

void PetriNet::goOutTransition(Transition* transition,
                               const devs::Time& time)
{
    const InputList& inputs = transition->inputs();
    const OutputList& outputs = transition->outputs();
    unsigned int tokenNumber = 0;

    for (OutputList::const_iterator it = outputs.begin() ;
         it != outputs.end(); it++) {
        tokenNumber += (*it)->getProducedTokenNumber();
    }

    for (InputList::const_iterator it = inputs.begin();
         it != inputs.end();it++) {
        tokenNumber -= (*it)->getConsumedTokenNumber();
    }

    mTokenNumber += tokenNumber;
    putTokens(transition, time);
}

bool PetriNet::isEnabled(Transition* transition)
{
    const InputList& inputs = transition->inputs();
    bool active = false;
    if (not inputs.empty()) {
        active = true;

        InputList::const_iterator it = inputs.begin();
        while (it != inputs.end() and active) {
            Place* place = (*it)->getPlace();

            if (mMarkings.find(place->getName()) == mMarkings.end() or
                mMarkings[place->getName()]->getTokenNumber() <
                (*it)->getConsumedTokenNumber()) {
                active = false;
            }
            ++it;
        }
    }
    return active;
}

bool PetriNet::isAlive() const
{
    bool alive = not mEnabledTransitions.empty();

    if (not alive) {
        alive = mTokenNumber > 0;
        if (alive) {
            MarkingList::const_iterator it = mMarkings.begin();
            bool found = false;

            while(it != mMarkings.end() && !found) {
                const std::string& placeName(it->first);
                Marking* marking = it->second;

                found = marking->getTokenNumber() >=
                    mOutPlaceMarkings.find(placeName)->second.second;
                ++it;
            }
            alive = found;
        }
    }
    return alive;
}

bool PetriNet::removeTokens(Marking* marking,
                            unsigned int tokenNumber)
{
    return marking->removeTokens(tokenNumber);
}

void PetriNet::updateSigma(const devs::Time& time)
{
    mSigma -= time - mLastTime;
}

PetriNet::PetriNet(const graph::AtomicModel& model,
                   const devs::InitEventList& events) :
    devs::Dynamics(model, events),
    mInitEvents(events),
    mTokenNumber(0)
{
}

PetriNet::~PetriNet()
{
    for (PlaceList::const_iterator it = mPlaces.begin();
         it != mPlaces.end(); it++)
        delete it->second;
    for (TransitionList::const_iterator it = mTransitions.begin();
         it != mTransitions.end(); it++)
        delete it->second;

    std::for_each(mInputs.begin(), mInputs.end(),
                  boost::checked_deleter < Input >());

    std::for_each(mOutputs.begin(), mOutputs.end(),
                  boost::checked_deleter < Output >());

    for (MarkingList::const_iterator it = mMarkings.begin();
         it != mMarkings.end(); it++)
        delete it->second;
}

void PetriNet::initArcs(const value::VectorValue& arcs)
{
    for(value::VectorValue::const_iterator it = arcs.begin();
        it != arcs.end(); it++) {
        value::VectorValue arc = value::toSet(*it);
        std::string first = value::toString(arc[0]);
        std::string second = value::toString(arc[1]);

        unsigned int tokenNumber = 1;
        if (arc.size() > 2) {
            tokenNumber = value::toInteger(arc[2]);
        }

        if (existPlace(first)) {
            Assert(utils::ModellingError, existTransition(second), 
                   boost::format("Petri Net: Unknow transition: %1%") % second);
            Input* input = new Input(mPlaces[first],
                                     mTransitions[second],
                                     tokenNumber);

            mTransitions[second]->addInput(input);
            mPlaces[first]->addInput(input);
            mInputs.push_back(input);
        } else if (existPlace(second)) {
            Assert(utils::ModellingError, existTransition(first),
                   boost::format( "Petri Net: Unknow transition: %1%") % first);
            Output* output = new Output(mTransitions[first],
                                        mPlaces[second],
                                        tokenNumber);

            mTransitions[first]->addOutput(output);
            mPlaces[second]->addOutput(output);
            mOutputs.push_back(output);
        } else {
            Throw(utils::ModellingError, boost::format(
                    "Petri Net: unknow place: %1% or %2%") % first % second);
        }
    }
}

void PetriNet::initInitialMarking(const value::VectorValue& initialMarkings,
                                  const devs::Time& time)
{

    for(value::VectorValue::const_iterator it = initialMarkings.begin();
        it != initialMarkings.end(); it++) {
        value::VectorValue initialMarking = value::toSet(*it);
        std::string name = value::toString(initialMarking[0]);
        unsigned int tokenNumber = value::toInteger(initialMarking[1]);	

        mInitialMarking[name] = tokenNumber;
    }

    // Build of initial marking
    std::map < std::string, unsigned int >::const_iterator
        it(mInitialMarking.begin());

    mTokenNumber = 0;
    while (it != mInitialMarking.end()) {
        Marking* marking = new Marking(mPlaces[it->first]);
        for (unsigned int i = 0; i < it->second; i++) {
            putToken(marking, time);
        }
        mMarkings[it->first] = marking;
        mTokenNumber += it->second;
        ++it;
    }
}

void PetriNet::initParameters()
{
}

void PetriNet::initPlaces(const value::VectorValue& places)
{
    for(value::VectorValue::const_iterator it = places.begin();
        it != places.end(); it++) {
        value::VectorValue place = value::toSet(*it);
        const std::string& name(value::toString(place[0]));

        Assert(utils::ModellingError, not existPlace(name), boost::format(
                "Petri Net: place '%1%' already exist") % name);

        Assert(utils::ModellingError, not existTransition(name), boost::format(
                "Petri Net: A transition have already the name '%1%'") % name);

        mPlaces[name] = new Place(name);

        if (place.size() > 1) {
            const std::string& portname(value::toString(place[1]));
            mOutPlaceMarkings[portname] = std::make_pair(name, 1);
        }
    }
}

void PetriNet::initTransitions(const value::VectorValue& transitions)
{
    for(value::VectorValue::const_iterator it = transitions.begin();
        it != transitions.end(); it++) {
        value::VectorValue transition = value::toSet(*it);
        const std::string& name(value::toString(transition[0]));

        mTransitions[name] = new Transition(name);

        if (transition.size() > 1) {
            const std::string& type(value::toString(transition[1]));
            std::string portName(value::toString(transition[2]));

            if (type == "input") {
                mInTransitionMarkings[portName] = name;
            } else if (type == "output") {
                mOutTransitionMarkings[name] = std::make_pair(portName, false);
            }
        }
    }
}

devs::Time PetriNet::start(const devs::Time& time)
{
    computeEnabledTransition(time);
    mLastTime = time;
    mPhase = RUN;
    if (isAlive()) {
        mSigma = devs::Time(0);
        return devs::Time(0);
    } else {
        mSigma = devs::Time::infinity;
        return devs::Time::infinity;
    }
}

devs::Time PetriNet::init(const devs::Time& time)
{
    std::string dynamics = value::toString(mInitEvents.get("dynamics"));

    if (dynamics == "StepByStep") {
        mDynamics = STEP_BY_STEP;
    } else if (dynamics == "PhaseByPhase") {
        mDynamics = PHASE_BY_PHASE;
    } else if (dynamics == "WhileAlive") {
        mDynamics = WHILE_ALIVE;
    } else if (dynamics == "TransitionTimed") {
        mDynamics = TRANSITION_TIMED;
    } else {
        Throw(utils::ModellingError, boost::format(
              "Petri Net dynamics: %1% is not defined") % dynamics);
    }

    initParameters();
    initPlaces(value::toSet(mInitEvents.get("places")));
    initTransitions(value::toSet(mInitEvents.get("transitions")));
    initArcs(value::toSet(mInitEvents.get("arcs")));
    initInitialMarking(value::toSet(mInitEvents.get("initialMarkings")), time);

    return start(time);
}

void PetriNet::output(const devs::Time& /* time */,
                      devs::ExternalEventList& output) const
{
    if (mPhase == OUT) {
        {
            devsPlaceMarkingList::const_iterator it = mOutPlaceMarkings.begin();

            while (it != mOutPlaceMarkings.end()) {
                const std::string& portName = it->first;
                const std::string& placeName = it->second.first;
                unsigned int tokenNumber = it->second.second;
                MarkingList::const_iterator itm = mMarkings.find(placeName);

                if (itm != mMarkings.end() and
                    itm->second->getTokenNumber() >= tokenNumber) {
                    output.addEvent(new devs::ExternalEvent(portName));
                }
                ++it;
            }
        }

        {
            devsOutTransitionMarkingList::const_iterator it =
                mOutTransitionMarkings.begin();

            while (it != mOutTransitionMarkings.end()) {
                if (it->second.second) {
                    const std::string& portName = it->second.first;
                    output.addEvent(new devs::ExternalEvent(portName));
                }
                ++it;
            }
        }

    }
}

devs::Time PetriNet::timeAdvance() const
{
    switch (mPhase) {
    case OUT:
        return devs::Time(0);
    case RUN:
        return mSigma;
    }
    return devs::Time::infinity;
}

void PetriNet::run(const devs::Time& time)
{
    if (mDynamics == STEP_BY_STEP) {
        if (not mEnabledTransitions.empty()) {
            fire(time);
        }
        computeEnabledTransition(time);
    } else if (mDynamics == PHASE_BY_PHASE) {
        while (not mEnabledTransitions.empty()) {
            fire(time);
        }
        computeEnabledTransition(time);
    } else if (mDynamics == WHILE_ALIVE) {
        while (not mEnabledTransitions.empty()) {
            while (not mEnabledTransitions.empty()) {
                fire(time);
            }
            computeEnabledTransition(time);
        }
    }

    if (isAlive()) {
        mSigma = devs::Time(0);
    } else {
        mSigma = devs::Time::infinity;
    }
}

void PetriNet::internalTransition(const devs::Time& time)
{
    switch (mPhase) {
    case OUT:
        {
            {
                devsOutTransitionMarkingList::iterator it =
                    mOutTransitionMarkings.begin();

                while (it != mOutTransitionMarkings.end()) {
                    it->second.second = false;
                    ++it;
                }
            }

            mPhase = RUN;
            mSigma = devs::Time::infinity;
            break;
        }
    case RUN:
        {
            run(time);
            mPhase = OUT;
            mLastTime = time;
        }
    }
}

void PetriNet::externalTransition(const devs::ExternalEventList& event,
                                  const devs::Time& time)
{
    devs::ExternalEventList::const_iterator it = event.begin();
    bool ok = false;

    while (it != event.end()) {
        const std::string& port = (*it)->getPortName();

        if (mInPlaceMarkings.find(port) != mInPlaceMarkings.end()) {
            Place* place = mPlaces[mInPlaceMarkings[port].first];
            const std::string& name = place->getName();
            unsigned int tokenNumber = mInPlaceMarkings[port].second;

            if (mMarkings.find(name) == mMarkings.end())
                mMarkings[name] = new Marking(place);
            for (unsigned int i = 0; i < tokenNumber; i++)
                putToken(mMarkings[name], time);
            mTokenNumber += tokenNumber;
            ok = true;
        } else if (mInTransitionMarkings.find(port) !=
                   mInTransitionMarkings.end()) {
            Transition* transition = mTransitions[mInTransitionMarkings[port]];
            goOutTransition(transition, time);
        }
        ++it;
    }
    computeEnabledTransition(time);
    run(time);
    mPhase = OUT;
    mLastTime = time;

    updateSigma(time);
    mLastTime = time;
}

value::Value PetriNet::observation(const devs::ObservationEvent& event) const
{
    if (event.onPort("token")) {
        return buildInteger(mTokenNumber);
    } else if (event.onPort("marking")) {
        value::Set markings = value::SetFactory::create();
        MarkingList::const_iterator it = mMarkings.begin();

        while (it != mMarkings.end()) {
            value::Set marking = value::SetFactory::create();

            marking->addValue(buildString(it->first));
            marking->addValue(buildInteger(it->second->getTokenNumber()));
            markings->addValue(marking);
            ++it;
        }
        return markings;
    }
    return Dynamics::observation(event);
}

}} // namespace vle extension

