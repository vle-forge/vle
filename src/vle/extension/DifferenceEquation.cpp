/**
 * @file vle/extension/DifferenceEquation.cpp
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


#include <vle/extension/DifferenceEquation.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Trace.hpp>
#include <cmath>

namespace vle { namespace extension {

using namespace devs;
using namespace graph;
using namespace value;

DifferenceEquation::DifferenceEquation(const AtomicModel& model,
                                       const InitEventList& events) :
    Dynamics(model, events),
    mTimeStep(0),
    mSynchro(false),
    mAllSynchro(false),
    mWaiting(0),
    mInitValue(false),
    mMode(NAME)
{
    if (events.exist("name")) {
        mVariableName = toString(events.get("name"));
    } else {
        mVariableName = getModelName();
    }

    if (events.exist("value")) {
        mInitialValue = toDouble(events.get("value"));
        mInitValue = true;
    }

    if (events.exist("time-step"))
        mTimeStep = toDouble(events.get("time-step"));
    else
        mTimeStep = 0;

    if (events.exist("mode")) {
        std::string str = toString(events.get("mode"));

        if (str == "name" ) mMode = NAME;
        else if (str == "port" ) mMode = PORT;
        else if (str == "mapping" ) {
            mMode = MAPPING;

            Assert(utils::InternalError,
                   events.exist("mapping"), (boost::format(
                           "[%1%] DifferenceEquation - mapping port not exists")
                       % getModelName()).str());

            const value::Map& mapping = value::toMapValue(events.get("mapping"));
            const value::MapValue& lst = mapping->getValue();
            for (value::MapValue::const_iterator it = lst.begin();
                 it != lst.end(); ++it) {
                std::string port = it->first;
                std::string name = toString(it->second);

                mMapping[port] = name;
            }

        }
    }
}

void DifferenceEquation::addValue(const std::string& name,
                                  const Time& time,
                                  double value)
{
    mValues[name].push_front(std::pair < double, double >(time.getValue(),
                                                          value));
    if (mSize[name] != -1 and (int)mValues[name].size() > mSize[name])
        mValues[name].pop_back();
}

void DifferenceEquation::displayValues()
{
    valueList::const_iterator it = mValues.begin();

    while (it != mValues.end()) {
        std::deque < std::pair < double, double > > list = it->second;
        std::deque < std::pair < double, double > >::const_iterator it2 =
            list.begin();

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
    Assert(utils::InternalError, shift <= 0, (boost::format(
                "[%1%] DifferenceEquation::getValue - positive shift") %
            getModelName()).str());

    if (shift == 0) return mValue.front();
    else {

        Assert(utils::InternalError, (int)(mValue.size() - 1) >= -shift,
               (boost::format("[%1%] - getValue - shift too large") %
                getModelName()).str());

        return mValue[-shift];
    }
}

bool DifferenceEquation::initExternalVariable(const std::string& name,
                                              const devs::ExternalEvent& event,
                                              double& timeStep)
{
    bool ok = true;

    if (mSynchros.find(name) == mSynchros.end()) {
        if (mAllSynchro) {
            mSynchros.insert(name);
            ++mSyncs;
        }
    }
    else ++mSyncs;

    // est-ce que cette variable externe possède une
    // limite de buffer ? si non alors ...
    if (mSize.find(name) == mSize.end())
        mSize[name] = 2;

    // est-ce que je connais ce modèle ? si non alors ...
    if (mValues.find(name) == mValues.end()) {
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

        --mWaiting;

        // création de la liste des valeurs reçues
        mValues[name] = std::deque < std::pair < double, double > >();
        // récupération du delta de ma dépendance,
        // c'est utilisé si delta n'a pas été initialisé
        timeStep = event.getDoubleAttributeValue("time-step");
    }
    // ma dépendance ne connait pas son pas de temps
    else {
        mWait.push_back(name);
        ok = false;
    }
    return ok;
}

void DifferenceEquation::beginNames()
{
    namesIt = mValues.begin();
}

bool DifferenceEquation::endNames()
{
    return namesIt == mValues.end();
}

void DifferenceEquation::nextName()
{
    ++namesIt;
}

std::string DifferenceEquation::name() const
{
    return namesIt->first;
}

double DifferenceEquation::getValue(const char* name,
                                    int shift) const
{
    valueList::const_iterator it = mValues.find(name);

    Assert(utils::InternalError, it != mValues.end(), (boost::format(
    "[%1%] DifferenceEquation::getValue - invalid variable name: %2%") %
            getModelName() % name).str());

    Assert(utils::InternalError, shift <= 0, (boost::format(
    "[%1%] DifferenceEquation::getValue - positive shift on %2%") %
            getModelName() % name).str());

    if (mState == INIT) {
        Assert(utils::InternalError, shift == 0, (boost::format(
    "[%1%] DifferenceEquation::getValue - not null shift on %2% in INIT state")
                % getModelName() % name).str());

        return it->second.front().second;
    } else {
        std::set < std::string >::const_iterator its = mSynchros.find(name);

        if (its == mSynchros.end()) ++shift;

        Assert(utils::InternalError, shift <= 0, (boost::format(
                    "[%1%] DifferenceEquation::getValue - wrong shift on %2%") %
                getModelName() % name).str());

        if (shift == 0) {
            return it->second.front().second;
        } else {
            std::deque < std::pair < double, double > > list = it->second;

            Assert(utils::InternalError, (int)(list.size() - 1) >= -shift,
                   (boost::format("[%1%] - getValue - shift too large") %
                    getModelName()).str());

            return list[-shift].second;
        }
    }
}

void DifferenceEquation::setSize(const std::string& name,
                                 int size)
{
    Assert(utils::InternalError, mSize.find(name) == mSize.end(),
           (boost::format(
                   "[%1%] DifferenceEquation::setSize - %2% already exists") %
            getModelName() % name).str());

    mSize[name] = size;
}

void DifferenceEquation::setSynchronizedVariable(const std::string& name)
{
    mSynchro = true;
    mSynchros.insert(name);
    ++mWaiting;
}

void DifferenceEquation::allSynchronizedVariable()
{
    mAllSynchro = true;
}

Time DifferenceEquation::init(const devs::Time& /* time */)
{
    if (mMode == NAME) mDependance = getModel().existInputPort("update");
    else mDependance = (getModel().getInputPortNumber() -
                        getModel().existInputPort("perturb")?1:0 -
                        getModel().existInputPort("request")?1:0) > 0;

    mActive = getModel().existOutputPort("update");
    mSyncs = 0;
    mReceive = 0;
    mLastTime = 0;
    if (mInitValue) {
        mValue.push_front(mInitialValue);
    }
    else if (!mDependance /*or !mSynchro*/) {
        mState = INIT;
        mValue.push_front(initValue());
        mInitValue = true;
    }

    // si la valeur n'est pas initialisée alors la valeur est invalide
    mInvalid = !mInitValue;

    // dois-je envoyer mes caractéristiques ?
    if (mActive and mInitValue) {
        mState = PRE_INIT;
        mSigma = 0;
        return Time(0);
    } else if (mDependance) { // est-ce que j'attends les caractéristiques de
        // quelqu'un ?
        mState = INIT;
        mSigma = Time::infinity;
        return Time::infinity;
    } else {                  // j'attends rien !
        mState = RUN;
        mSigma = mTimeStep;
        return Time(mTimeStep);
    }
}

void DifferenceEquation::output(const Time& /*time*/,
                                ExternalEventList& output) const
{
    if (mActive and (mState == PRE_INIT or mState == PRE_INIT2 or mState == POST
                     or mState == POST2)) {
        ExternalEvent* ee = new ExternalEvent("update");

        if (mState == PRE_INIT or mState == PRE_INIT2 or mState == POST or
            mState == POST2) {
            ee << attribute("name", mVariableName);
            ee << attribute("value", getValue());
        }

        if (mState == PRE_INIT or mState == PRE_INIT2) {
            ee << attribute("time-step", mTimeStep);
        }
        output.addEvent(ee);
    }
}

Time DifferenceEquation::timeAdvance() const
{
    return mSigma;
}

Event::EventType DifferenceEquation::confluentTransitions(
    const Time& /* internal */,
    const ExternalEventList& /* extEventlist */) const
{
    return Event::EXTERNAL;
}

void DifferenceEquation::internalTransition(const Time& time)
{
    switch (mState) {
    case PRE_INIT:
        // est-ce que j'attends les caractéristiques de quelqu'un ?
        if (mDependance) {
            mState = INIT;
            mSigma = Time::infinity;
        } else { // j'attends rien !
            mState = RUN;
            mSigma = mTimeStep;
        }
        break;
    case PRE_INIT2:
        //        if (mDependance) mState = PRE;
        if (mSynchro or mAllSynchro) mState = PRE;
        else mState = RUN;
        mSigma = mTimeStep;
        break;
    case INIT:
        break;
    case PRE:
        // si pas de cycle alors
        if (mSyncs != 0 and (mSynchro or mAllSynchro)) break;
    case RUN:
        // je calcule ma nouvelle valeur
        mLastTime = time;
        try {
            mValue.push_front(compute(time));
            mInvalid = false;
        } catch(utils::InternalError) {
            mInvalid = true;
        }
        // je vais propager ma nouvelle valeur à ceux qui
        // dépendent de moi
        if (mActive) {
            mState = POST;
            mSigma = 0;
        } else {
            if (mDependance) {
                mState = PRE;
            } else {
                mState = RUN;
            }
            mSigma = mTimeStep;
        }
        break;
    case POST:
        // ma nouvelle valeur est propagée

        // est-ce que je suis synchronisé ?
        if (mSynchro or mAllSynchro) mState = PRE;
        else {
            TraceDebug(boost::format("%1% %2%: fin") % time % getModelName());
            mState = RUN;
        }
        mSigma = mTimeStep;
        break;
    case POST2:
        // ma valeur a été propagée suite à la reception d'une
        // mise à jour d'une de mes dépendances non synchrones
        // cette propagation est utile pour la compatibilité avec
        // QSS
        if (mDependance) {
            mState = PRE;
        } else {
            mState = RUN;
        }
        mSigma = mSigma2;
    }
}

void DifferenceEquation::externalTransition(const ExternalEventList& event,
                                            const Time& time)
{
    Time e = time - mLastTime;
    // PROBLEMATIQUE !!!!!!! l'idéal serait : bool end = (e == mSigma);
    bool end = std::abs(e.getValue() - mSigma.getValue()) < 1e-10;
    bool begin = (e == 0);
    ExternalEventList::const_iterator it = event.begin();
    double timeStep;
    bool reset = false;

    while (it != event.end()) {
        if (mMode == NAME and (*it)->onPort("update")) {
            std::string name = (*it)->getStringAttributeValue("name");
            double value = (*it)->getDoubleAttributeValue("value");
            bool ok = true;

            if (mState == INIT)
                ok = initExternalVariable(name, **it, timeStep);

            // est-ce que je suis dans l'état de recevoir les
            // valeurs de mes dépendances
            if (ok) {
                addValue(name, time.getValue(), value);
                // si j'étais en attente de mes dépendances et
                // que je suis au tout début de l'attente alors
                // je comptabilise cette dépendance afin de
                // vérifier que j'ai bien tout reçu et je
                // pourrais alors lancer mon propre calcul
                if (mState == PRE and end
		    and mSynchros.find(name) != mSynchros.end())
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
        } else {

            Assert(utils::InternalError, mMode == PORT or mMode == MAPPING,
                   (boost::format("[%1%] - DifferenceEquation: invalid mode") %
                    getModelName()).str());

            std::string portName = (*it)->getPortName();
            std::string name = (mMode == PORT)?portName:mMapping[portName];
            double value = (*it)->getDoubleAttributeValue("value");
            bool ok = true;

            if (mState == INIT)
                ok = initExternalVariable(name, **it, timeStep);

            // est-ce que je suis dans l'état de recevoir les
            // valeurs de mes dépendances
            if (ok) {
                addValue(name, time.getValue(), value);
                // si j'étais en attente de mes dépendances et
                // que je suis au tout début de l'attente alors
                // je comptabilise cette dépendance afin de
                // vérifier que j'ai bien tout reçu et je
                // pourrais alors lancer mon propre calcul
                if (mState == PRE and end) ++mReceive;
            }	
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
            // si le pas de temps n'est pas défini
            if (mTimeStep == 0) mTimeStep = timeStep;
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
        else if (mWait.empty() and mWaiting <= 0) {
            if (!mInitValue) {
                mValue.push_front(initValue());
                mInvalid = false;
                // je propage mes caractéristiques si nécessaire
                mSigma = 0;
                mState = PRE_INIT2;
            }
            else {
                mSigma = mTimeStep;
                mState = PRE;
            }
        }
    }
    // j'ai tout reçu, je peux passer en mode calcul
    //    else if (mState == PRE and end and (mReceive == mSyncs or reset) and mSynchro) {
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

Value DifferenceEquation::observation(const ObservationEvent& event) const
{
    if (mInvalid) {
        return Value();
    } else {
        Assert(utils::InternalError, event.getPortName() == mVariableName,
               boost::format("Observation: %1% model, invalid variable" \
                             " name: %2%") % getModelName()
               % event.getPortName());
        return buildDouble(getValue());
    }
}

void DifferenceEquation::request(const RequestEvent& event,
                                 const Time& /*time*/,
                                 ExternalEventList& output) const
{
    Assert(utils::InternalError,
           event.getStringAttributeValue("name") == mVariableName,
           boost::format(
               "DifferenceEquation model, invalid variable name: %1%") %
           event.getStringAttributeValue("name"));

    ExternalEvent* ee = new ExternalEvent("response");

    ee << attribute("name", event.getStringAttributeValue("name"));
    ee << attribute("value", getValue());
    output.addEvent(ee);
}

}} // namespace vle extension
