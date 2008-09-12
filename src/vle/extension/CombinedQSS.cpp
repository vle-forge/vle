/**
 * @file vle/extension/CombinedQSS.cpp
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




#include <vle/extension/CombinedQSS.hpp>
#include <vle/value/Set.hpp>
#include <vle/utils/Debug.hpp>
#include <cmath>

namespace vle { namespace extension {

using namespace devs;
using namespace graph;
using namespace value;

CombinedQss::CombinedQss(const AtomicModel& model,
                         const InitEventList& events) :
    Dynamics(model, events),
    mActive(true),
    mDependance(true),
    mValue(0),
    mExternalVariableNumber(0),
    mExternalValues(false),
    mIndex(0),
    mGradient(0),
    mSigma(0),
    mLastTime(0),
    mState(0)
{
    const Value& threshold = events.get("threshold");
    mThreshold = toDouble(threshold);

    if (events.exist("active")) {
        const Value& active = events.get("active");
        mActive = toBoolean(active);
    }

    if (events.exist("dependance")) {
        const Value& dependance = events.get("dependance");
        mDependance = toBoolean(dependance);
    }

    const value::Set& variables = value::toSetValue(events.get("variables"));
    unsigned int index;
    
    for (index = 0; index < variables->size(); ++index) {
        const value::Set& tab(value::toSetValue(variables->getValue(index)));
	std::string name = value::toString(tab->getValue(0));
	double init = value::toDouble(tab->getValue(1));        
	double precision = value::toDouble(tab->getValue(2));        
        mVariableIndex[name] = index;
        mVariableName[index] = name;
        mVariablePrecision[index] = precision;
        mVariableEpsilon[index] = precision;
        mInitialValueList.push_back(std::pair < unsigned int, double >(
                index, init));
    }
    mVariableNumber = index;
    mValue = new double[index];
    mIndex = new long[index];
    mGradient = new double[index];
    mSigma = new Time[index];
    mLastTime = new Time[index];
    mState = new state[index];
}

void CombinedQss::updateSigma(unsigned int i)
{
    if (std::abs(getGradient(i)) < mThreshold) {
        setSigma(i,Time::infinity);
    } else {
        if (getGradient(i) > 0) {
            setSigma(i, Time((d(i, getIndex(i) + 1)-getValue(i)) /
                                   getGradient(i)));
        } else {
            setSigma(i, Time(((d(i, getIndex(i)) - getValue(i)) -
                                    mVariableEpsilon[i]) / getGradient(i)));
        }
    }
}

void CombinedQss::minSigma()
{
    // Recherche de la prochaine fonction à calculer
    unsigned int j = 1;
    unsigned int j_min = 0;
    double min = getSigma(0).getValue();

    while (j < mVariableNumber) {
        if (min > getSigma(j).getValue()) {
            min = getSigma(j).getValue();
            j_min = j;
        }
        ++j;
    }
    mCurrentModel = j_min;
}

void CombinedQss::reset(const Time& time, unsigned int i, double value)
{
    setValue(i, value);
    setIndex(i, (long)(floor(getValue(i)/mVariablePrecision[i])));
    setLastTime(i, time);
    setGradient(i, compute(i, time));
    updateSigma(i);    
}

// DEVS Methods

void CombinedQss::finish()
{
    delete[] mGradient;
    delete[] mValue;
    delete[] mIndex;
    delete[] mSigma;
    delete[] mLastTime;
    delete[] mState;
}

Time CombinedQss::init(const Time& /* time */)
{
    // Initialisation des variables internes
    std::vector < std::pair < unsigned int , double > >::const_iterator it =
        mInitialValueList.begin();

    while (it != mInitialValueList.end()) {
        mValue[it->first] = it->second;
        ++it;
    }

    for(unsigned int i = 0;i < mVariableNumber;i++) {
        mGradient[i] = 0.0;
        mIndex[i] = (long)(floor(getValue(i)/mVariablePrecision[i]));
        setSigma(i, Time(0));
        setLastTime(i, Time(0));
        setState(i, INIT);
    }
    mCurrentModel = 0;
    return Time(0);
}


void CombinedQss::output(const Time& time, ExternalEventList& output) const
{
    if (getState(mCurrentModel) == INIT or
        ((getState(mCurrentModel) == POST3 and mExternalValues) or
         (getState(mCurrentModel) == RUN and mActive))) {
        std::map < unsigned int , std::string >::const_iterator it;
        it = mVariableName.find(mCurrentModel);
        AssertI(it != mVariableName.end());
        ExternalEvent* ee = new ExternalEvent(it->second);
        double e = (time - getLastTime(mCurrentModel)).getValue();

        ee << attribute("value", getValue(mCurrentModel) + e
                              * getGradient(mCurrentModel));
        ee << attribute("gardient", getGradient(mCurrentModel));
        output.addEvent(ee);
    }
}

Time CombinedQss::timeAdvance() const
{
    return getSigma(mCurrentModel);
}

Event::EventType CombinedQss::confluentTransitions(
    const Time& /* internal */,
    const ExternalEventList& /* extEventlist */) const
{
    return Event::EXTERNAL;
}

void CombinedQss::internalTransition(const Time& time)
{
    switch (getState(mCurrentModel)) {
    case INIT: // init du gradient
        if (mDependance) {
            for (unsigned int i = 0; i < mVariableNumber; i++) {
                setState(i, POST_INIT);
                setSigma(i, Time::infinity);
            }
        }
        else {
            for (unsigned int i = 0; i < mVariableNumber; i++) {
                setState(i, RUN);
                setGradient(i, compute(i, time));
                updateSigma(i);
            }
            minSigma();
        }
        break;
    case POST2:
        // mise à jour du gradient après reception des valeurs de
        // mes variables externes
        for (unsigned int i = 0; i < mVariableNumber; i++) {
            setState(i, RUN);
            setLastTime(i, time);
            setGradient(i, compute(i, time));
            updateSigma(i);
        }
        minSigma();
        break;
    case POST3:
        // mise à jour du gradient après reception d'une nouvelle
        // valeur pour l'une de mes variables externes
        // et envoie de ma nouvelle valeur aux équations qui
        // dépendent de moi si elles existent
        setState(mCurrentModel, RUN);
        updateSigma(mCurrentModel);
        minSigma();
        break;
    case RUN:
        if (getGradient(mCurrentModel) >= 0) incIndex(mCurrentModel);
        else decIndex(mCurrentModel);
        for (unsigned int i = 0; i < mVariableNumber; i++)
            if (i != mCurrentModel)
                setValue(i, getValue(i) + (time -
                                           getLastTime(i)).getValue() *
                         getGradient(i));
            else 
                setValue(mCurrentModel, d(mCurrentModel, 
                                         getIndex(mCurrentModel)));

        // Propagation ou non de la nouvelle valeur
        if (mActive and mExternalValues) {
            // si oui alors on va attendre les valeurs
            // actualisées de toutes mes variables externes
            setState(mCurrentModel, POST);
            setSigma(mCurrentModel, Time::infinity); 
        } else {
            // sinon on passe au pas suivant

            for (unsigned int i = 0; i < mVariableNumber; i++) {
                setState(i, RUN);
                setLastTime(i, time);
                setGradient(i, compute(i, time));
                updateSigma(i);
            }
        }
        minSigma();
        break;
    case POST_INIT:
    case POST:
        break;
    }
}

void CombinedQss::externalTransition(const ExternalEventList& event,
                                     const Time& time)
{
    if (getState(0) == POST_INIT) { // réception de la définition des variables
                                    // externes
        ExternalEventList::const_iterator it = event.begin();
        unsigned int index = 0;
        unsigned int linear = 0;

        while (it != event.end()) {
            std::string name = (*it)->getStringAttributeValue("name");
            double value = (*it)->getDoubleAttributeValue("value");

            mExternalVariableIndex[name] = index;
            mExternalVariableValue[index] = value;
            if ((mIsGradient[index] = (*it)->existAttributeValue("gradient"))) {
                mExternalVariableGradient[index] =
                    (*it)->getDoubleAttributeValue("gradient");
                ++linear;
            }
            ++index;
            ++it;
        }
        mExternalValues = (linear < mExternalVariableIndex.size());
        mExternalVariableNumber = linear;
        for (unsigned int i = 0; i < mVariableNumber; i++) {
            setState(i, RUN);
            setGradient(i, compute(i, time));
            updateSigma(i);
        }
        minSigma();
    }
    else {
        ExternalEventList::const_iterator it = event.begin();
        bool _reset = false;

        while (it != event.end()) {
            std::string name = (*it)->getStringAttributeValue("name");
            double value = (*it)->getDoubleAttributeValue("value");

            // c'est une variable externe numérique
            if ((*it)->onPort("update")) {
                Assert(utils::InternalError, mVariableIndex.find(name) !=
                       mVariableIndex.end(), boost::format(
                           "Combined Qss update, invalid variable name: %1%") %
                       name);
                setExternalValue(name, value);		
                if (mIsGradient[mExternalVariableIndex[name]])
                    setExternalGradient(name,
                                        (*it)->getDoubleAttributeValue("gradient"));
            }	    
            // c'est une perturbation sur une variable interne
            if ((*it)->onPort("perturb")) {
                Assert(utils::InternalError, mVariableIndex.find(name) !=
                       mVariableIndex.end(),
                       boost::format("Combined Qss update, invalid variable name: %1%") % name);

                reset(time, mVariableIndex[name], value);
                _reset = true;
            }
            ++it;
        }
        // Mise à jour des valeurs de variables externes
        if (mExternalVariableNumber > 1) {
            for (unsigned int j = 0 ; j < mExternalVariableNumber ; j++)
                mExternalVariableValue[j] += (time -
                                              getLastTime(0)).getValue()*mExternalVariableGradient[j];
        }
        for (unsigned int i = 0; i < mVariableNumber; i++) {
            if (getState(i) == POST) {
                setState(i, POST2);
                setSigma(i, 0);
            }
            else if (getState(i) == RUN) {
                if (_reset) setSigma(i, 0);
                else {
                    // Mise à jour de la valeur
                    setValue(i, getValue(i) + (time - mLastTime[i]).getValue()*getGradient(i));
                    setLastTime(i, time);
                    setGradient(i, compute(i, time));

                    setSigma(i, 0);
                    setState(i, POST3);		
                }
            }
        }
        minSigma();	
    }
}

Value CombinedQss::observation(const ObservationEvent& event) const
{
    unsigned int i = mVariableIndex.find(event.getPortName())->second;
    double e = (event.getTime() - getLastTime(i)).getValue();

    return DoubleFactory::create(getValue(i)+e*getGradient(i));
}

void CombinedQss::request(const RequestEvent& event,
                          const Time& time,
                          ExternalEventList& output) const
{
    unsigned int i = mVariableIndex.find(event.getStringAttributeValue("name"))->second;
    double e = (time - getLastTime(i)).getValue();
    ExternalEvent* ee = new ExternalEvent("response");

    ee << attribute("name", event.getStringAttributeValue("name"));
    ee << attribute("value", getValue(i)+e*getGradient(i));
    output.addEvent(ee);
}

int CombinedQss::getVariable(const std::string& name) const
{
    std::map < std::string, unsigned int >::const_iterator it;

    it = mVariableIndex.find(name);
    if (it != mVariableIndex.end()) return it->second;
    else return -1;
}

const std::string& CombinedQss::getVariable(unsigned int index) const
{
    std::map < unsigned int, std::string >::const_iterator it;
    it = mVariableName.find(index);

    Assert(utils::InternalError, it != mVariableName.end(), boost::format(
            "CombinedQss model, unknow variable index %1%") % index);

    return it->second;
}

}} // namespace vle extension
