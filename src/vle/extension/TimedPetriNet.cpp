/**
 * @file src/vle/extension/TimedPetriNet.cpp
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


#include <vle/extension/TimedPetriNet.hpp>

namespace vle { namespace extension {

void TimedPetriNet::computeEnabledTransition(const vle::devs::Time& time)
{
    vle::devs::Time min = vle::devs::Time::infinity;
    TransitionList::const_iterator it = mTransitions.begin();

    mEnabledTransitions.clear();
    while (it != mTransitions.end()) {
        vle::devs::Time t = isEnabled((TimedTransition*)(it->second));

        if (t == min)
            addEnabledTransition(it->second);
        else if (t < min) {
            mEnabledTransitions.clear();
            addEnabledTransition(it->second);
            min = t;
        }
        ++it;
    }
    mSigma = min - time;
}

vle::devs::Time TimedPetriNet::getNextValid(Marking* marking,
                                            unsigned int tokenNumber)
{
    const TokenList& tokens = marking->getTokens();
    std::vector < vle::devs::Time > dates;

    for (TokenList::const_iterator it = tokens.begin();
         it != tokens.end(); it++)
        dates.push_back(((TimedToken*)(*it))->getTime());
    std::sort < std::vector < vle::devs::Time >::iterator >(
        dates.begin(), dates.end());
    return dates[tokenNumber - 1];
}


vle::devs::Time TimedPetriNet::isEnabled(TimedTransition* transition)
{
    const InputList& inputs = transition->inputs();
    InputList::const_iterator it = inputs.begin();
    vle::devs::Time time = 0;

    while (it != inputs.end() and time != vle::devs::Time::infinity) {
        Place* place = (*it)->getPlace();
        MarkingList::const_iterator itm = mMarkings.find(place->getName());

        if (itm == mMarkings.end() or
            itm->second->getTokenNumber() <
            (*it)->getConsumedTokenNumber())
            time = vle::devs::Time::infinity;
        else {
            vle::devs::Time validTime =
                getNextValid(itm->second,
                             (*it)->getConsumedTokenNumber());
            if (validTime > time)
                time = validTime;
        }
        ++it;
    }
    return time + transition->getDelay();
}

bool TimedPetriNet::removeTokens(Marking* marking,
                                 unsigned int tokenNumber)
{
    vle::devs::Time time = getNextValid(marking, tokenNumber);
    TokenList& tokens = marking->getTokens();
    TokenList::iterator it = tokens.begin();

    while (it != tokens.end()) {
        if (((TimedToken*)(*it))->getTime() <= time) {
            delete *it;
            it = tokens.erase(it);
        } else {
            ++it;
        }
    }
    return tokens.empty();
}	

void TimedPetriNet::initTransitions(const vle::value::VectorValue& transitions)
{
    for(vle::value::VectorValue::const_iterator it = transitions.begin();
        it != transitions.end(); it++) {
        vle::value::VectorValue transition = vle::value::toSet(*it);
        std::string name = vle::value::toString(transition[0]);
        std::string type = vle::value::toString(transition[1]);
        double delay = vle::value::toDouble(transition[2]);

        mTransitions[name] = new TimedTransition(name, delay);
        if (type != "internal") {
            std::string portName = vle::value::toString(transition[3]);

            if (type == "input") {
                mInTransitionMarkings[portName] = name;
            } else if (type == "output") {
                mOutTransitionMarkings[portName] = std::make_pair(name, false);
            }
        }
    }
}

vle::devs::Time TimedPetriNet::start(const vle::devs::Time& time)
{
    computeEnabledTransition(time);
    mLastTime = time;
    mPhase = RUN;
    return mSigma;
}

void TimedPetriNet::run(const vle::devs::Time& time)
{
    while (not mEnabledTransitions.empty()) {
        fire(time);
    }
    computeEnabledTransition(time);
}

}} // namespace vle extension

