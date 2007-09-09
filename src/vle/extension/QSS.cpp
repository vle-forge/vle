/** 
 * @file extension/QSS.cpp
 * @brief 
 * @author The vle Development Team
 * @date ven, 27 oct 2006 00:06:52 +0200
 */

/*
 * Copyright (C) 2006, 07 - The vle Development Team
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

#include <vle/extension/QSS.hpp>
#include <vle/value/Map.hpp>
#include <cmath>

namespace vle { namespace extension {

using namespace devs;
using namespace graph;
using namespace value;

qss::qss(const AtomicModel& model) :
    Dynamics(model),
    m_functionNumber(0),
    m_gradient(0),
    m_index(0),
    m_value(0),
    m_sigma(0),
    m_lastTime(0),
    m_state(0)
{
}

void qss::processInitEvents(const InitEventList& event)
{
    const value::Value& precision = event.get("precision");
    m_precision = value::toDouble(precision);
    m_epsilon = m_precision;

    const value::Value& threshold = event.get("threshold");
    m_threshold = value::toDouble(threshold);

    const value::Value& active = event.get("active");
    m_active = value::toBoolean(active);

    const value::Map& variables = value::toMapValue(event.get("variables"));
    const value::MapFactory::MapValue& lst = variables->getValue();

    m_functionNumber = lst.size();

    m_gradient = new double[m_functionNumber];
    m_value = new double[m_functionNumber];
    m_index = new long[m_functionNumber];
    m_sigma = new devs::Time[m_functionNumber];
    m_lastTime = new devs::Time[m_functionNumber];
    m_state = new state[m_functionNumber];

    for (value::MapFactory::MapValue::const_iterator it = lst.begin();
         it != lst.end(); ++it) {
        const value::Set& tab(value::toSetValue(it->second));

        unsigned int index = value::toInteger(tab->getValue(0));
        double init = value::toDouble(tab->getValue(1));
        m_variableIndex[it->first] = index;
        m_variableName[index] = it->first;
        m_initialValueList.push_back(std::pair < unsigned int, double >(
                index, init));
    }
}

void qss::updateSigma(unsigned int i)
{
    if (std::abs(getGradient(i)) < m_threshold) {
        setSigma(i,devs::Time::infinity);
    } else {
        if (getGradient(i) > 0) {
            setSigma(i, devs::Time((d(getIndex(i) + 1)-getValue(i)) /
                                   getGradient(i)));
        } else {
            setSigma(i,devs::Time(((d(getIndex(i))-getValue(i))-m_epsilon) /
                                  getGradient(i)));
        }
    }
}

void qss::minSigma()
{
    // Recherche de la prochaine fonction à calculer
    unsigned int j = 1;
    unsigned int j_min = 0;
    double min = getSigma(0).getValue();

    while (j < m_functionNumber) {
        if (min > getSigma(j).getValue()) {
            min = getSigma(j).getValue();
            j_min = j;
        }
        ++j;
    }
    m_currentModel = j_min;
}

// DEVS Methods

void qss::finish()
{
    delete[] m_gradient;
    delete[] m_value;
    delete[] m_index;
    delete[] m_sigma;
    delete[] m_lastTime;
    delete[] m_state;
}

Time qss::init()
{
    std::vector < std::pair < unsigned int , double > >::const_iterator it =
        m_initialValueList.begin();

    while (it != m_initialValueList.end()) {
        setValue(it->first, it->second);
        ++it;
    }

    for(unsigned int i = 0;i < m_functionNumber;i++) {
        m_gradient[i] = 0.0;
        m_index[i] = (long)(floor(getValue(i)/m_precision));
        setSigma(i,devs::Time(0));
        setLastTime(i,devs::Time(0));
        setState(i,INIT);
    }
    m_currentModel = 0;
    return devs::Time(0);
}


void qss::getOutputFunction(const Time& time,
                            ExternalEventList& output) 
{
    //  std::cout << "[" << getModelName() << "] " << "getOutputFunction at "
    //  << time << std::endl;
    if (getState(0) == POST3 or getState(0) == RUN) {
        devs::ExternalEvent* ee = new devs::ExternalEvent("out");

        for (unsigned int i = 0; i < m_functionNumber ; i++) {
            double e = (time - getLastTime(i)).getValue();

            // 	    std::cout << "e = " << e << std::endl; std::cout <<
            // 	    "gradient = " << getGradient(i) << std::endl; std::cout <<
            // 	    "value = " << getValue(i)+e*getGradient(i) << std::endl;

            //	    ee << devs::attribute(m_variableName[i],
            //	    getValue(i)+e*getGradient(i));
            ee << devs::attribute("name", m_variableName[i]);
            ee << devs::attribute("value", getValue(i)+e*getGradient(i));
        }
        output.addEvent(ee);
    }
}

Time qss::getTimeAdvance()
{
    return getSigma(m_currentModel);
}

Event::EventType qss::processConflict(const InternalEvent& /* internal */,
                    const ExternalEventList& /* extEventlist */) const
{
    return Event::EXTERNAL;
}

void qss::processInternalEvent(const InternalEvent& event)
{
    unsigned int i = m_currentModel;

    switch (getState(i)) {
    case INIT: // init du gradient
        setState(i,RUN);
        setGradient(i,compute(i));
        updateSigma(i);
        minSigma();
        break;
    case POST:
        //       std::cout << "POST -> POST2 [" << getModelName() << "] " <<
        //       "internal event at " << event.getTime() << std::endl;
        for (unsigned int j = 0;j < m_functionNumber;j++) {
            setState(j, POST2);
            setSigma(j, Time(0));    
        }
        minSigma();
        break;
    case POST2:
        //       std::cout << "POST2 -> RUN [" << getModelName() << "] " <<
        //       "internal event at " << event.getTime() << std::endl;
        for (unsigned int j = 0;j < m_functionNumber;j++) {
            setState(j, RUN);
            setLastTime(j,event.getTime());
            // Mise à jour du gradient
            setGradient(j,compute(j));
            // Mise à jour de sigma
            updateSigma(j);
        }
        minSigma();
        break;
    case POST3:
        //       std::cout << "POST3 -> RUN [" << getModelName() << "] " <<
        //       "internal event at " << event.getTime() << std::endl;
        for (unsigned int j = 0;j < m_functionNumber;j++) {
            setState(j, RUN);
            setGradient(j,compute(j));
            updateSigma(j);
        }
        minSigma();
        break;
    case RUN:
        // Mise à jour de l'index

        if (getGradient(i) >= 0) setIndex(i,getIndex(i)+1);
        else setIndex(i,getIndex(i)-1);

        for (unsigned int j = 0;j < m_functionNumber;j++) {
            double e = (event.getTime() - getLastTime(j)).getValue();
            setValue(j,getValue(j)+e*getGradient(j));
        }

        if (m_active) {
            setState(i, POST);
            setSigma(i, Time::infinity); 
        } else {
            for (unsigned int j = 0;j < m_functionNumber;j++) {
                setState(j, RUN);
                setLastTime(j,event.getTime());
                setGradient(j,compute(j));
                updateSigma(j);
            }
            minSigma();
        }

        // 	std::cout << "RUN -> POST [" << getModelName() << "] " <<
        // 	"internal event at " << event.getTime() << " : " <<
        // 	getVariable(i) << " = " << getValue(i) << std::endl;

        minSigma();
    }
}

void qss::processExternalEvents(const ExternalEventList& event,
                                const Time& time)
{
    //   std::cout << "external event [" << getModelName() << "] at " << time
    //   << std::endl;

    if (getState(0) == POST) {
        ExternalEventList::const_iterator it = event.begin();

        while (it != event.end()) {
            if ((*it)->onPort("parameter")) {
                std::string name = (*it)->getStringAttributeValue("name");
                double value = (*it)->getDoubleAttributeValue("value");
                // 	std::cout << "POST [" << getModelName() << "] " <<
                // 	"perturbation at " << time <<  " : " << name << " , "
                // 	<< value << std::endl;
                processPerturbation(name, value);
            }
            ++it;
        }

        for (unsigned int j = 0;j < m_functionNumber;j++) {
            setState(j, POST2);
            setSigma(j, Time(0));    
        }
        minSigma();
    } else {
        if (getState(0) == RUN) {
            ExternalEventList::const_iterator it = event.begin();

            while (it != event.end()) {
                if ((*it)->onPort("parameter")) {
                    std::string name = (*it)->getStringAttributeValue("name");
                    double value = (*it)->getDoubleAttributeValue("value");
                    // 	    std::cout << "RUN [" << getModelName() << "] " <<
                    // 	    "perturbation at " << time <<  " : " << name << " ,
                    // 	    " << value << std::endl;
                    processPerturbation(name, value);
                    for (unsigned int j = 0;j < m_functionNumber;j++) {
                        double e = (time - getLastTime(j)).getValue();

                        setValue(j, getValue(j)+e*getGradient(j));
                    }
                    for (unsigned int j = 0;j < m_functionNumber;j++) {
                        setLastTime(j, time);
                        setSigma(j, 0);
                        setState(j, POST3);
                    }
                    minSigma();
                } else {
                    size_t i = getVariable((*it)->getPortName());
                    double value = (*it)->getDoubleAttributeValue("value");

                    if (getState(i) == RUN) {
                        double e = (time - getLastTime(i)).getValue();

                        for (unsigned int j = 0;j < m_functionNumber;j++) {
                            if (i == j) {
                                setValue(i, value);
                                m_gradient[i] = 0.0;
                                m_index[i] = (long)(floor(getValue(i) /
                                                          m_precision));
                                setSigma(i, Time(0));
                                setState(i, INIT);
                            } else {
                                setLastTime(j, time);
                                if (e > 0) {
                                    setValue(j,getValue(j)+e*getGradient(j));
                                }
                                setGradient(j, compute(j));
                                updateSigma(j);
                                if (getSigma(j) < 0) {
                                    setSigma(j, Time(0));
                                }
                            }
                        }
                        minSigma();
                    }
                }
                ++it;
            }
        }
    } 
}

Value qss::processStateEvent(const StateEvent& event) const
{
    unsigned int i = m_variableIndex.find(event.getPortName())->second;
    double e = (event.getTime() - getLastTime(i)).getValue();

    return value::DoubleFactory::create(getValue(i)+e*getGradient(i));
}

unsigned int qss::getVariable(const std::string& name) const
{
    std::map < std::string, unsigned int >::const_iterator it;
    it = m_variableIndex.find(name);

    Assert(utils::InternalError, it != m_variableIndex.end(), boost::format(
            "Qss model, unknow variable %1%") % name);

    return it->second;
}

const std::string& qss::getVariable(unsigned int index) const
{
    std::map < unsigned int, std::string >::const_iterator it;
    it = m_variableName.find(index);

    Assert(utils::InternalError, it != m_variableName.end(), boost::format(
            "Qss model, unknow variable index %1%") % index);

    return it->second;
}

}} // namespace vle extension
