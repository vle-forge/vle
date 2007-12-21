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
#include <cmath>

namespace vle { namespace extension {

using namespace devs;
using namespace graph;
using namespace value;

DifferenceEquation::DifferenceEquation(const AtomicModel& model) :
    Dynamics(model), 
    mTimeStep(0),
    mDelta(0), 
    mMultiple(0),
    mSize(-1),
    mInitValue(false)
{
}

void DifferenceEquation::addValue(const std::string& name,
				  const vle::devs::Time& time,
				  double value)
{
    mValues[name].push_front(std::pair < double, double >(time.getValue(), value));
    if (mSize != -1 and (int)mValues[name].size() > mSize)
	mValues[name].pop_back();
}

void DifferenceEquation::displayValues()
{
    valueList::const_iterator it = mValues.begin();

    while (it != mValues.end()) {
	std::deque < std::pair < double, double > > list = it->second;
	std::deque < std::pair < double, double > >::const_iterator it2 = list.begin();
	
	std::cout << it->first << ":";
	while (it2 != list.end()) {
	    std::cout << "(" << it2->first << "," << it2->second << ") ";
	    ++it2;
	}    
	std::cout << std::endl;
	++it;
    }
}

double DifferenceEquation::getValue(int shift) const 
{ 
    if (shift > 0) Throw(vle::utils::InternalError, "getValue - positive shift");
    if (shift == 0) return mValue.front(); 
    else {
	std::deque < double >::const_iterator it = mValue.begin();

	if ((int)(mValue.size() - 1) >= -shift) return mValue[-shift];
	else Throw(vle::utils::InternalError, 
		   (boost::format("%1% - getValue - shift too large") % getModelName()).str());
    }
}
double DifferenceEquation::getValue(const char* name, int shift)
{
    if (mValues.find(name) == mValues.end())
	Throw(vle::utils::InternalError, 
	      (boost::format("%1% - getValue - invalid variable name: %2%") % getModelName() % name).str());

    if (shift > 0) Throw(vle::utils::InternalError, "getValue - positive shift");
    if (shift == 0) return mValues[name].front().second;
    else {
	std::deque < std::pair < double, double > > list = mValues[name];

	if ((int)(list.size() - 1) >= -shift) return list[-shift].second;
	else Throw(vle::utils::InternalError, 
		   (boost::format("%1% - getValue - shift too large") % getModelName()).str());
    }
}

void DifferenceEquation::processInitEvents(const InitEventList& event)
{
    if (event.exist("name")) mVariableName = vle::value::toString(event.get("name"));
    else mVariableName = getModelName();
    if (event.exist("size")) mSize = (int)vle::value::toInteger(event.get("size"));
    else mSize = -1;
    if (event.exist("value")) {
	mInitialValue = vle::value::toDouble(event.get("value"));
	mInitValue = true;
    }
    if (event.exist("delta")) {
	mDelta = vle::value::toDouble(event.get("delta"));
	if (event.exist("multiple")) mMultiple = vle::value::toInteger(event.get("multiple"));
	else mMultiple = 1;
    }
    else {
	mDelta = 0;
	mMultiple = 0;
    }

    mTimeStep = mMultiple * mDelta;
}

// DEVS Methods

Time DifferenceEquation::init()
{
    mDependance = getModel().existInputPort("update");
    mActive = getModel().existOutputPort("update");
    mSyncs = 0;
    mReceive = 0;
    mLastTime = 0;
    if (mInitValue) mValue.push_front(mInitialValue);
    // si la valeur n'est initialisée alors la valeur est invalide
    mInvalid = !mInitValue;
    // dois-je envoyer mes caractéristiques ?
    if (mActive) {
	mState = PRE_INIT;
	mSigma = 0;
	return vle::devs::Time(0);
    }
    // est-ce que j'attends les caractéristiques de quelqu'un ?
    else if (mDependance) {
	mState = INIT;
	mSigma = vle::devs::Time::infinity;
	return vle::devs::Time::infinity;
    }
    // j'attends rien !
    else {
	mState = RUN;
	mSigma = mTimeStep;
	return vle::devs::Time(mTimeStep);
    }
}

void DifferenceEquation::getOutputFunction(const Time& /*time*/,
					   ExternalEventList& output) 
{
    if (mActive and (mState == PRE_INIT or mState == PRE_INIT2 or mState == POST or mState == POST2)) {
	vle::devs::ExternalEvent* ee = new vle::devs::ExternalEvent("update");

	if (mState == PRE_INIT or mState == PRE_INIT2 or mState == POST or mState == POST2) {
	    ee << vle::devs::attribute("name", mVariableName);
	    ee << vle::devs::attribute("value", getValue());
	}
	if (mState == PRE_INIT or mState == PRE_INIT2) {
	    ee << vle::devs::attribute("multiple", (int)mMultiple);
	    ee << vle::devs::attribute("delta", mDelta);
	}
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
    switch (mState) 
    {
    case PRE_INIT:
	// est-ce que j'attends les caractéristiques de quelqu'un ?
	if (mDependance) {
	    mState = INIT;
	    mSigma = vle::devs::Time::infinity;
	}
	// j'attends rien !
	else {
	    mState = RUN;
	    mSigma = mTimeStep;
	}
	break;
    case PRE_INIT2:
	if (mDependance) mState = PRE;
	else mState = RUN;
	mSigma = mTimeStep;
	break;
    case INIT:
	break;
    case PRE:
// si pas de cycle alors
	if (mSyncs != 0) break;
    case RUN:
	// je calcule ma nouvelle valeur
	mLastTime = event.getTime();
	try {
	    mValue.push_front(compute(vle::devs::Time(event.getTime().getValue())));
	    mInvalid = false;
	}
	catch(vle::utils::InternalError) {
	    mInvalid = true;
	}
	// je vais propager ma nouvelle valeur à ceux qui
	// dépendent de moi
	if (mActive) {
	    mState = POST;
	    mSigma = 0;
	}
	else {
	    if (mDependance) mState = PRE;
	    else mState = RUN;
	    mSigma = mTimeStep;
	}
	break;
    case POST:
	// ma nouvelle valeur est propagée
	if (mDependance) mState = PRE;
	else mState = RUN;
	mSigma = mTimeStep;
	break;
    case POST2:
	// ma valeur a été propagée suite à la reception d'une
	// mise à jour d'une de mes dépendances non synchrones
	// cette propagation est utile pour la compatibilité avec
	// QSS
	if (mDependance) mState = PRE;
	else mState = RUN;
	mSigma = mSigma2;
    }
}

void DifferenceEquation::processExternalEvents(const ExternalEventList& event,
					       const Time& time)
{
    vle::devs::Time e = time - mLastTime;
    // PROBLEMATIQUE !!!!!!! l'idéal serait : bool end = (e == mSigma);
    bool end = fabs(e.getValue() - mSigma.getValue()) < 1e-10;
    bool begin = (e == 0);
    vle::devs::ExternalEventList::const_iterator it = event.begin();
    double delta;
    bool reset = false;
      
    while (it != event.end()) {
	if ((*it)->onPort("update")) {
	    std::string name = (*it)->getStringAttributeValue("name");
	    double value = (*it)->getDoubleAttributeValue("value");
	    bool ok = true;
	    
	    // est-ce que je connais ce modèle ? si non alors ...
	    if (mValues.find(name) == mValues.end() and 
		(*it)->existAttributeValue("multiple")) {
		unsigned int multiple = (*it)->getIntegerAttributeValue("multiple");
		
		// ma dépendance connait son pas de temps
		if (multiple != 0) {
		    // est-ce que je suis en attente des
		    // caractéristiques d'un modèle non défini ?
		    // si oui alors recherche du modèle et
		    // suppression de celui-ci de la liste des attentes
		    if (!mWait.empty()) {
			std::vector < std::string >::iterator itw = mWait.begin();
			bool found = false;
			
			while (!found and itw != mWait.end()) {
			    found = (*itw == name);
			    ++itw;
			}
			if (found) mWait.erase(itw);
		    }
		    // création de la liste des valeurs reçues
		    mValues[name] = std::deque < std::pair < double, double > >();
		    // récupération du delta de ma dépendance,
		    // c'est utilisé si delta n'a pas été initialisé
		    delta = (*it)->getDoubleAttributeValue("delta");
		    // initialisation du facteur multiplicateur
		    // de ma dépendance
		    mMultiples[name] = multiple;
		    // si ma multiplicité est définie alors je
		    // regarde si c'est un multiple de ma
		    // multiplicité, si c'est le cas alors la
		    // réception des valeurs de cette dépendance
		    // se produiront en même temps que mes
		    // changements de valeur
		    if (mMultiple != 0)
			if ((multiple > mMultiple and (multiple % mMultiple == 0)) or 
			    (multiple < mMultiple and (mMultiple % multiple == 0)) or
			    (multiple == mMultiple))
			    ++mSyncs;
		}
		// ma dépendance ne connait pas son pas de temps
		else {
		    mWait.push_back(name);
		    ok = false;
		}
	    }
	    // est-ce que je suis dans l'état de recevoir les
	    // valeurs de mes dépendances
	    if (ok) {
		addValue(name, time.getValue(), value);
		// si j'étais en attente de mes dépendances et
		// que je suis au tout début de l'attente alors
		// je comptabilise cette dépendance afin de
		// vérifier que j'ai bien tout reçu et je
		// pourrais alors lancer mon propre calcul
		if (mState == PRE and end) 
		    ++mReceive;
	    }
	}
	// c'est une perturbation d'un modèle externe sur la
	// variable du modèle
	else if ((*it)->onPort("perturb")) {
	    std::string name = (*it)->getStringAttributeValue("name");
	    double value = (*it)->getDoubleAttributeValue("value");

	    mValue.push_front(value);
	    reset = true;
	}
	++it;	
    }
    // je suis en phase d'initialisation
    if (mState == INIT) {
	// si le modèle se synchronise sur ses dépendances alors
	// il faut calculer son pas de temps
	// ce pas de temps est égal au plus petit pas de temps
	// des dépendances
	if (mTimeStep == 0 and mWait.empty()) {
	    std::map < std::string, unsigned int >::const_iterator it = mMultiples.begin();
	    unsigned int min = it->second;

	    while (it != mMultiples.end()) {
		unsigned int multiple = it->second;

		if (min > multiple) min = multiple;
		++it;
	    }
	    mMultiple = min;
	    it = mMultiples.begin();
	    while (it != mMultiples.end()) {
		unsigned int multiple = it->second;

		if ((multiple > mMultiple and (multiple % mMultiple == 0)) or 
		    (multiple < mMultiple and (mMultiple % multiple == 0)) or
		    (multiple == mMultiple))
		    ++mSyncs;
		++it;
	    }
	    // si le pas de temps n'est pas défini
	    if (mDelta == 0) mDelta = delta;

	    mTimeStep = mMultiple * mDelta;
	    
	    // si ma valeur initiale n'est pas définie alors je
	    // la calcule
	    if (!mInitValue) {
		mValue.push_front(initValue());
		mInvalid = false;
	    }
	    // je propage mes caractéristiques si nécessaire
	    mSigma = 0;
	    mState = PRE_INIT2;
	}
	// est-ce que j'ai reçu les caractéristiques de toutes
	// mes dépendances ? si oui, je calcule ma valeur initiale
	else if (mWait.empty()) {
	    if (!mInitValue) {
		mValue.push_front(initValue());
		mInvalid = false;
	    }
	    mSigma = mTimeStep;
	    mState = PRE;
	}
    }
    // j'ai tout reçu, je peux passer en mode calcul
    else if (mState == PRE and end and (mReceive == mSyncs or reset)) {
	mState = RUN;
	mSigma = 0;
	mReceive = 0;
    }
    // je reçois la valeur d'autres modèles lorsque je ne suis
    // pas en phase de calcul
    else if ((mState == RUN or mState == PRE) and !end and !begin) {
	mLastTime = time;
	mState = POST2;
	mSigma2 = mSigma - e;
	mSigma = 0;
    }
}

Value DifferenceEquation::processStateEvent(const StateEvent& event) const
{
    if (mInvalid) return vle::value::Value();
    else {
	Assert(vle::utils::InternalError, event.getPortName() == mVariableName,
	       boost::format("DifferenceEquation model, invalid variable name: %1%") % event.getPortName());

	return buildDouble(getValue());
    }
}

void DifferenceEquation::processInstantaneousEvent(
    const InstantaneousEvent& event,
    const Time& /*time*/,
    ExternalEventList& output) const
{
    Assert(vle::utils::InternalError, event.getStringAttributeValue("name") ==
           mVariableName, boost::format(
               "DifferenceEquation model, invalid variable name: %1%") %
           event.getStringAttributeValue("name"));

    vle::devs::ExternalEvent* ee = new vle::devs::ExternalEvent("response");
      
    ee << vle::devs::attribute("name", event.getStringAttributeValue("name"));
    ee << vle::devs::attribute("value", getValue());
    output.addEvent(ee);
}

}} // namespace vle extension
