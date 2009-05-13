/**
 * @file vle/extension/PetriNet.cpp
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

bool operator<(const PetriNet::pairTimeTransition& x,
               const PetriNet::pairTimeTransition& y)
{
    return x.first < y.first;
}

PetriNet::Marking::~Marking()
{
    std::for_each(mTokens.begin(), mTokens.end(),
                  boost::checked_deleter < Token >());
}

void PetriNet::addEnabledTransition(Transition* transition)
{
    if (std::find(mEnabledTransitions.begin(),
                  mEnabledTransitions.end(),
                  transition) == mEnabledTransitions.end()) {
        mEnabledTransitions.push_back(transition);
    }
}

void PetriNet::computeEnabledTransition(const vle::devs::Time& time)
{
    vle::devs::Time min = vle::devs::Time::infinity;
    TransitionList::const_iterator it = mTransitions.begin();

    mEnabledTransitions.clear();
    while (it != mTransitions.end()) {
        vle::devs::Time t = getValidTime(it->second);

        if (t != devs::Time::infinity) {
            if (t == min)
                addEnabledTransition(it->second);
            else if (t < min) {
                mEnabledTransitions.clear();
                addEnabledTransition(it->second);
                min = t;
            }
        }
        ++it;
    }
    // some transitions are in waiting to fired
    if (not mWaitingTransitions.empty()) {
        if (mWaitingTransitions.front().first < min) {
            min = mWaitingTransitions.front().first;
            mEnabledTransitions.clear();
        }
    }
    // sigma in [0; +inf]
    mSigma = min - time;
}

void PetriNet::disableOutTransition()
{
    devsOutTransitionMarkingList::iterator it =
        mOutTransitionMarkings.begin();

    while (it != mOutTransitionMarkings.end()) {
        it->second.second = false;
        ++it;
    }
}

void PetriNet::disableOutPlace(const devs::Time& time)
{
    devsPlaceMarkingList::const_iterator it = mOutPlaceMarkings.begin();

    while (it != mOutPlaceMarkings.end()) {
        const std::string& placeName = it->second.first;
        unsigned int tokenNumber = it->second.second;
        MarkingList::const_iterator itm = mMarkings.find(placeName);

        if (itm != mMarkings.end() and
            itm->second->getTokenNumber() >= tokenNumber) {
            TokenList::const_iterator itt = itm->second->getTokens().
                begin();

            while (itt != itm->second->getTokens().end()) {
                if (not (*itt)->sent() and
                    (*itt)->getTime() == time) {
                    (*itt)->send();
                }
                ++itt;
            }
        }
        ++it;
    }
}

void PetriNet::fire(const devs::Time& time)
{
    if (mEnabledTransitions.empty()) {
        while (not mWaitingTransitions.empty()
               and mWaitingTransitions.front().first == time) {
            Transition* transition = mWaitingTransitions.front().second;

            goOutTransition(transition, time);
            mWaitingTransitions.pop_front();
        }
    }
    else {
        Transition* transition = selectTransition();

        if (goInTransition(transition)) {
            if (transition->getDelay() == 0)
                goOutTransition(transition, time);
            else {
                mWaitingTransitions.push_back(
                    pairTimeTransition(time + transition-> getDelay(),
                                       transition));
                mWaitingTransitions.sort();
            }
        }
        mEnabledTransitions.erase(std::find(mEnabledTransitions.begin(),
                                            mEnabledTransitions.end(),
                                            transition));
    }
}

bool PetriNet::goInTransition(Transition* transition)
{
    const InputList& inputs = transition->inputs();
    InputList::const_iterator it = inputs.begin();

    while (it != inputs.end()) {
        Place* place = (*it)->getPlace();
        MarkingList::const_iterator itm = mMarkings.find(place->getName());

        if ((*it)->getConsumedTokenNumber() > 0) {
            if (removeTokens(itm->second,
                             (*it)->getConsumedTokenNumber())) {
                delete itm->second;
                mMarkings.erase(place->getName());
            }
        }
        if (transition->outputs().size() == 0 and
            mOutTransitionMarkings.find(transition->getName()) !=
            mOutTransitionMarkings.end()) {
            mOutTransitionMarkings[transition->getName()].second = true;
        }
        ++it;
    }
    return true;
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

devs::Time PetriNet::getNextValid(Marking* marking,
                                  unsigned int tokenNumber)
{
    const TokenList& tokens = marking->getTokens();
    std::vector < vle::devs::Time > dates;

    for (TokenList::const_iterator it = tokens.begin();
         it != tokens.end(); it++)
        dates.push_back((*it)->getTime());
    std::sort < std::vector < vle::devs::Time >::iterator >(
        dates.begin(), dates.end());
    return dates[tokenNumber - 1];
}

// How time the transition can be fired ? Returns infinity if it
// is not possible.
devs::Time PetriNet::getValidTime(Transition* transition)
{
    const InputList& inputs = transition->inputs();

    if (inputs.empty()) return vle::devs::Time::infinity;

    InputList::const_iterator it = inputs.begin();
    vle::devs::Time time = 0;

    while (it != inputs.end() and time != vle::devs::Time::infinity) {
        Place* place = (*it)->getPlace();
        MarkingList::const_iterator itm = mMarkings.find(place->getName());

        if ((mMarkings.find(place->getName()) == mMarkings.end() and
             (*it)->getConsumedTokenNumber() == 0)
            or
            (mMarkings.find(place->getName()) != mMarkings.end() and
             (*it)->getConsumedTokenNumber() != 0 and
             (*it)->getConsumedTokenNumber() <=
             mMarkings[place->getName()]->getTokenNumber()))
        {
            vle::devs::Time validTime = ((*it)->getConsumedTokenNumber() == 0)
                ? devs::Time(0.0) : getNextValid(itm->second,
                                (*it)->getConsumedTokenNumber());
            if (validTime > time)
                time = validTime;
        }
        else time = vle::devs::Time::infinity;
        ++it;
    }
    return time;
}

void PetriNet::putToken(Marking* marking,
                        const devs::Time& time)
{
    marking->addToken(new Token(time + marking->getPlace()->getDelay()));
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

bool PetriNet::removeTokens(Marking* marking,
                            unsigned int tokenNumber)
{
    vle::devs::Time time = getNextValid(marking, tokenNumber);
    TokenList& tokens = marking->getTokens();
    TokenList::iterator it = tokens.begin();

    while (it != tokens.end()) {
        if ((*it)->getTime() <= time and tokenNumber != 0) {
            --tokenNumber;
            delete *it;
            it = tokens.erase(it);
        } else {
            ++it;
        }
    }
    return tokens.empty();
}

void PetriNet::run(const devs::Time& time)
{
    fire(time);
    computeEnabledTransition(time);
}

PetriNet::Transition* PetriNet::selectTransition()
{
    if (mEnabledTransitions.size() == 1)
        return mEnabledTransitions.at(0);

    enabledTransitionList::iterator it = mEnabledTransitions.begin();
    unsigned int priority = (*it++)->getPriority();
    bool change = false;

    while (it != mEnabledTransitions.end()) {
        if (priority > (*it)->getPriority()) {
            change = true;
            priority = (*it)->getPriority();
        }
        else if (priority != (*it)->getPriority()) {
            change = true;
        }
        ++it;
    }

    if (change) {
        enabledTransitionList list;

        it = mEnabledTransitions.begin();
        while (it != mEnabledTransitions.end()) {
            if (priority == (*it)->getPriority())
                list.push_back(*it);
            ++it;
        }

        int index = rand().getInt(0, list.size() - 1);

        return list.at(index);
    }
    else {
        int index = rand().getInt(0, mEnabledTransitions.size() - 1);

        return mEnabledTransitions.at(index);
    }
}

// Constructor and destructor
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

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

void PetriNet::addArc(const std::string& src,
                      const std::string& dst,
                      unsigned int tokenNumber)
{
    if (existPlace(src)) {
        Assert < utils::ModellingError >(existTransition(dst),
               fmt(_("Petri Net: Unknow transition: %1%")) % dst);
        Input* input = new Input(mPlaces[src],
                                 mTransitions[dst],
                                 tokenNumber);

        mTransitions[dst]->addInput(input);
        mPlaces[src]->addInput(input);
        mInputs.push_back(input);
    } else if (existPlace(dst)) {
        Assert < utils::ModellingError >(existTransition(src),
               fmt(_("Petri Net: Unknow transition: %1%")) % src);
        Output* output = new Output(mTransitions[src],
                                    mPlaces[dst],
                                    tokenNumber);

        mTransitions[src]->addOutput(output);
        mPlaces[dst]->addOutput(output);
        mOutputs.push_back(output);
    } else {
        throw utils::ModellingError(fmt(_(
                "Petri Net: unknow place: %1% or %2%")) % src % dst);
    }
}

void PetriNet::addInitialMarking(const std::string& placeName,
                                 unsigned int tokenNumber)
{
    Assert < utils::ModellingError >(existPlace(placeName),
           fmt(_("Petri Net: Unknow place: %1%")) % placeName);

    if (tokenNumber > 0)
        mInitialMarking[placeName] = tokenNumber;
}

void PetriNet::addInputTransition(const std::string& transitionName,
                                  const std::string& portName,
                                  double delay)
{
    Assert < utils::ModellingError >(not existTransition(transitionName),
           fmt(_("Petri Net: transition '%1%' already exists"))
           % transitionName);

    Assert < utils::ModellingError >(delay >= 0,
           fmt(_("Petri Net: delay on transition %1% is negative"))
           % transitionName);

    mTransitions[transitionName] = new Transition(transitionName, delay);
    mInTransitionMarkings[portName] = transitionName;
}

void PetriNet::addOutputPlace(const std::string& placeName,
                              const std::string& portName)
{
    Assert < utils::ModellingError >(not existPlace(placeName), fmt(_(
            "Petri Net: place '%1%' already exists")) % placeName);

    mPlaces[placeName] = new Place(placeName, 0.0);
    mOutPlaceMarkings[portName] = std::make_pair(placeName, 1);
}

void PetriNet::addOutputTransition(const std::string& transitionName,
                                   const std::string& portName,
                                   unsigned int priority)
{
    Assert < utils::ModellingError >(not existTransition(transitionName),
           fmt(_("Petri Net: transition '%1%' already exists"))
           % transitionName);

    mTransitions[transitionName] = new Transition(transitionName, 0.0,
                                                  priority);
    mOutTransitionMarkings[transitionName] = std::make_pair(portName,
                                                            false);
}

void PetriNet::addPlace(const std::string& placeName,
                        double delay)
{
    Assert < utils::ModellingError >(not existPlace(placeName), fmt(_(
            "Petri Net: place '%1%' already exists")) % placeName);

    Assert < utils::ModellingError >(delay >= 0,
           fmt(_("Petri Net: delay on place %1% is negative"))
           % placeName);

    mPlaces[placeName] = new Place(placeName, delay);
}

void PetriNet::addTransition(const std::string& transitionName,
                             double delay,
                             unsigned int priority)
{
    Assert < utils::ModellingError >(not existTransition(transitionName),
           fmt(_("Petri Net: transition '%1%' already exists"))
           % transitionName);

    Assert < utils::ModellingError >(delay >= 0,
           fmt(_("Petri Net: delay on transition %1% is negative"))
           % transitionName);

    mTransitions[transitionName] = new Transition(transitionName, delay,
                                                  priority);
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

void PetriNet::build()
{
    initPlaces(value::toSet(mInitEvents.get("places")));
    initTransitions(value::toSet(mInitEvents.get("transitions")));
    initArcs(value::toSet(mInitEvents.get("arcs")));
    initInitialMarking(value::toSet(mInitEvents.get("initialMarkings")));
}

void PetriNet::buildInitialMarking(const devs::Time& time)
{
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
        addArc(first, second, tokenNumber);
    }
}

void PetriNet::initInitialMarking(const value::VectorValue& initialMarkings)
{
    for(value::VectorValue::const_iterator it = initialMarkings.begin();
        it != initialMarkings.end(); it++) {
        value::VectorValue initialMarking = value::toSet(*it);
        std::string name = value::toString(initialMarking[0]);
        unsigned int tokenNumber = value::toInteger(initialMarking[1]);

        addInitialMarking(name, tokenNumber);
    }
}

void PetriNet::initPlaces(const value::VectorValue& places)
{
    for(value::VectorValue::const_iterator it = places.begin();
        it != places.end(); it++) {
        value::VectorValue place = value::toSet(*it);
        const std::string& name(value::toString(place[0]));
        double delay = 0.0;

        if (place.size() > 1) {
            if (place[1]->isString()) {
                const std::string& portname(value::toString(place[1]));

                addOutputPlace(name, portname);
            }
            else {
                delay = value::toDouble(place[1]);
                addPlace(name, delay);
            }
        }
        else addPlace(name, delay);
    }
}

void PetriNet::initTransitions(const value::VectorValue& transitions)
{
    for(value::VectorValue::const_iterator it = transitions.begin();
        it != transitions.end(); it++) {
        value::VectorValue transition = value::toSet(*it);
        const std::string& name(value::toString(transition[0]));

        if (transition.size() > 1) {
            if (transition[1]->isString()) {
                const std::string& type(value::toString(transition[1]));
                std::string portName(value::toString(transition[2]));

                if (type == "input") {
                    double delay = 0.0;

                    if (transition.size() == 4) {
                        delay = vle::value::toDouble(transition[3]);
                    }
                    addInputTransition(name, portName, delay);
                } else if (type == "output") {
                    unsigned int priority = 0;

                    if (transition.size() == 4) {
                        priority = value::toInteger(transition[3]);
                    }
                    addOutputTransition(name, portName, priority);
                }
            } else {
                double delay = 0.0;
                unsigned int priority = 0;

                if (transition[1]->isDouble()) {
                    delay = vle::value::toDouble(transition[1]);
                    if (transition.size() == 3)
                        priority = value::toInteger(transition[2]);
                }
                else priority = value::toInteger(transition[1]);
                addTransition(name, delay, priority);
            }
        }
        else addTransition(name);
    }
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

devs::Time PetriNet::init(const devs::Time& time)
{
    build();
    buildInitialMarking(time);
    computeEnabledTransition(time);
    if (mSigma == 0) mPhase = RUN;
    else mPhase = WAITING;
    return mSigma;
}

void PetriNet::output(const devs::Time& time,
                      devs::ExternalEventList& output) const
{
    if (mPhase == OUT or mPhase == OUT2) {
        {
            devsPlaceMarkingList::const_iterator it = mOutPlaceMarkings.begin();

            while (it != mOutPlaceMarkings.end()) {
                const std::string& portName = it->first;
                const std::string& placeName = it->second.first;
                unsigned int tokenNumber = it->second.second;
                MarkingList::const_iterator itm = mMarkings.find(placeName);

                if (itm != mMarkings.end() and
                    itm->second->getTokenNumber() >= tokenNumber) {
                    TokenList::const_iterator itt = itm->second->getTokens().
                        begin();

                    while (itt != itm->second->getTokens().end()) {
                        if (not (*itt)->sent() and
                            (*itt)->getTime() == time) {
                            output.addEvent(buildEvent(portName));
                        }
                        ++itt;
                    }
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
                    output.addEvent(buildEvent(portName));
                }
                ++it;
            }
        }

    }
}

devs::Time PetriNet::timeAdvance() const
{
    switch (mPhase) {
    case IDLE:
        return 0;
    case WAITING:
        return mSigma;
    case OUT:
        return 0;
    case OUT2:
        return 0;
    case RUN:
        return 0;
    }
    return devs::Time::infinity;
}

void PetriNet::internalTransition(const devs::Time& time)
{
    switch (mPhase) {
    case IDLE:
        {
            mPhase = OUT;
            break;
        }
    case OUT:
        {
            disableOutTransition();
            disableOutPlace(time);
            computeEnabledTransition(time);
            if (mSigma == 0) mPhase = RUN;
            else mPhase = WAITING;
            break;
        }
    case OUT2:
        {
            disableOutTransition();
            disableOutPlace(time);
            if (mSigma == 0) mPhase = RUN;
            else mPhase = WAITING;
            break;
        }
    case WAITING:
    case RUN:
        {
            run(time);
            mPhase = OUT2;
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

            if (transition->getDelay() == 0) {
                goOutTransition(transition, time);
                ok = true;
            } else {
                mWaitingTransitions.push_back(pairTimeTransition(
                        time + transition->getDelay(), transition));
                mWaitingTransitions.sort();
            }
        }
        ++it;
    }
    mPhase = IDLE;
}

value::Value* PetriNet::observation(const devs::ObservationEvent& event) const
{
    if (event.onPort("token")) {
        return buildInteger(mTokenNumber);
    } else if (event.onPort("marking")) {
        value::Set* markings = value::Set::create();
        MarkingList::const_iterator it = mMarkings.begin();

        while (it != mMarkings.end()) {
            value::Set* marking = value::Set::create();

            marking->add(buildString(it->first));
            marking->add(buildInteger(it->second->getTokenNumber()));
            markings->add(marking);
            ++it;
        }
        return markings;
    }
    return Dynamics::observation(event);
}

}} // namespace vle extension

