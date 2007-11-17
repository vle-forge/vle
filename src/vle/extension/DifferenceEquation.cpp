/** 
 * @file extension/DifferenceEquation.cpp
 * @author The vle Development Team
 */

/*
 * Copyright (C) 2007 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <vle/extension/DifferenceEquation.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace extension {

using namespace devs;
using namespace graph;
using namespace value;

DifferenceEquation::DifferenceEquation(const AtomicModel& model) :
    Dynamics(model),
    mActive(true),
    mDependance(true)
{
}

void DifferenceEquation::processInitEvents(const InitEventList& event)
{
    const value::Value& timeStep = event.get("timeStep");
    mTimeStep = value::toDouble(timeStep);

    if (event.exist("active")) {
	const value::Value& active = event.get("active");
	mActive = value::toBoolean(active);
    }

    if (event.exist("dependance")) {
	const value::Value& dependance = event.get("dependance");
	mDependance = value::toBoolean(dependance);
    }

    const value::Value& value = event.get("value");
    mInitialValue = value::toDouble(value);

    const value::Value& name = event.get("name");
    mVariableName = value::toString(name);
}

void DifferenceEquation::reset(const Time& time, double value)
{
    mValue = value;
    mLastTime = time;
    mSigma = mTimeStep;
}

// DEVS Methods

void DifferenceEquation::finish()
{
}

Time DifferenceEquation::init()
{
    mValue = mInitialValue;
    mLastTime = devs::Time(0);
    mState = INIT;
    mSigma = devs::Time(0);
    return devs::Time(0);
}


void DifferenceEquation::getOutputFunction(const Time& /*time*/,
					   ExternalEventList& output) 
{
    if (mState == INIT or ((mState == POST3 or mState == RUN) 
			   and mActive)) {
	devs::ExternalEvent* ee = new devs::ExternalEvent("update");
	
	ee << devs::attribute("name", mVariableName);
	ee << devs::attribute("value", mValue);
	output.addEvent(ee);
    }
}

Time DifferenceEquation::getTimeAdvance()
{
    return mSigma;
}

Event::EventType DifferenceEquation::processConflict(
    const InternalEvent& /* internal */,
    const ExternalEventList& /* extEventlist */) const
{
    return Event::EXTERNAL;
}

void DifferenceEquation::processInternalEvent(const InternalEvent& event)
{
    switch (mState) {
    case INIT: // init du gradient
	if (mDependance) {
	    mState = POST_INIT;
	    mSigma = Time::infinity;
	}
	else {
	    mState = RUN;
	    mSigma = mTimeStep;
	}
        break;
    case POST2:
	// mise à jour du gradient après reception des valeurs de
	// mes variables externes
	mState = RUN;
	mLastTime = event.getTime();
	mSigma = mTimeStep;
        break;
    case POST3:
	// mise à jour du gradient après reception d'une nouvelle
	// valeur pour l'une de mes variables externes
	// et envoie de ma nouvelle valeur aux équations qui
	// dépendent de moi si elles existent
	mState = RUN;
	mSigma = mTimeStep - (event.getTime() - mLastTime).getValue();
        break;
    case RUN:
	mValue = compute();
	// Propagation ou non de la nouvelle valeur
        if (mActive) {
	    // si oui alors on va attendre les valeurs
	    // actualisées de toutes mes variables externes
            mState = POST;
            mSigma = Time::infinity; 
        } else {
	    // sinon on passe au pas suivant
	    mState = RUN;
	    mLastTime = event.getTime();
	    mSigma = mTimeStep;
	}
	break;
    case POST_INIT:
    case POST:
	break;
    }
}

void DifferenceEquation::processExternalEvents(const ExternalEventList& event,
					       const Time& time)
{
    if (mState == POST_INIT) { // reception de la definition des variables
                               // externes.
	ExternalEventList::const_iterator it = event.begin();
	unsigned int index = 0;

	while (it != event.end()) {
	    std::string name = (*it)->getStringAttributeValue("name");
	    double value = (*it)->getDoubleAttributeValue("value");

	    mExternalVariableIndex[name] = index;
	    mExternalVariableValue[index] = value;
	    ++index;
	    ++it;
	}
	mState = RUN;
	mSigma = mTimeStep;
    } else {
	ExternalEventList::const_iterator it = event.begin();
	bool _reset = false;
	
	while (it != event.end()) {
	    std::string name = (*it)->getStringAttributeValue("name");
	    double value = (*it)->getDoubleAttributeValue("value");
	    
	    // c'est une variable externe DifferenceEquation
	    if ((*it)->onPort("update")) {
                Assert(utils::InternalError, name != mVariableName,
                       boost::format(
                           "DifferenceEquation update, invalid variable name:" \
                           "%1%") % name);

		setValue(name, value);		
	    }	    
	    // c'est une perturbation sur une variable interne
	    if ((*it)->onPort("perturb")) {
		Assert(utils::InternalError, name == mVariableName,
                       boost::format("DifferenceEquation perturbation," \
                                     " invalid variable name: %1%") % name);

		reset(time, value);
		_reset = true;
	    }
	    ++it;
	}
	if (mState == POST) {
	    mState = POST2;
	    mSigma = devs::Time(0);
	}
	else if (mState == RUN) {
            if (_reset) {
                mSigma = devs::Time(0);
            } else {
		mSigma = devs::Time(0);
		mState = POST3;
	    }
	}
    }
}

Value DifferenceEquation::processStateEvent(const StateEvent& event) const
{
    Assert(utils::InternalError, event.getPortName() == mVariableName,
           boost::format("DifferenceEquation model, invalid variable name: %1%")
           % event.getStringAttributeValue("name"));

    return value::DoubleFactory::create(mValue);
}

void DifferenceEquation::processInstantaneousEvent(
    const InstantaneousEvent& event,
    const Time& /*time*/,
    ExternalEventList& output) const
{
    Assert(utils::InternalError, event.getStringAttributeValue("name") ==
           mVariableName, boost::format(
               "DifferenceEquation model, invalid variable name: %1%") %
           event.getStringAttributeValue("name"));

    devs::ExternalEvent* ee = new devs::ExternalEvent("response");
      
    ee << devs::attribute("name", event.getStringAttributeValue("name"));
    ee << devs::attribute("value", mValue);
    output.addEvent(ee);
}

}} // namespace vle extension
