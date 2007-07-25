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

double qss::d(long p_index)
{
    return p_index * m_precision;
}

double qss::getGradient(unsigned int i) const
{
    return m_gradient[i];
}

long qss::getIndex(unsigned int i) const
{
    return m_index[i];
}

const Time & qss::getLastTime(unsigned int i) const
{
    return m_lastTime[i];
}

const Time & qss::getSigma(unsigned int i) const
{
    return m_sigma[i];
}

qss::state qss::getState(unsigned int i) const
{
    return m_state[i];
}

void qss::setIndex(unsigned int i,long p_index)
{
    m_index[i] = p_index;
}

void qss::setGradient(unsigned int i,double p_gradient)
{
    m_gradient[i] = p_gradient;
}

void qss::setLastTime(unsigned int i,const Time & p_time)
{
    m_lastTime[i] = p_time;
}

void qss::setState(unsigned int i,state p_state)
{
    m_state[i] = p_state;
}

void qss::setSigma(unsigned int i,const Time & p_time)
{
    m_sigma[i] = p_time;
}

void qss::setValue(unsigned int i,double p_value)
{  
    m_value[i] = p_value;
}

void qss::updateSigma(unsigned int i)
{
    // Calcul du sigma de la ième fonction
    if (std::abs(getGradient(i)) < m_threshold)
        setSigma(i,devs::Time::infinity);
    else
        if (getGradient(i) > 0)
            setSigma(i,devs::Time((d(getIndex(i)+1)-getValue(i))/getGradient(i)));
        else
            setSigma(i,devs::Time(((d(getIndex(i))-getValue(i))-m_epsilon)
                                  /getGradient(i)));
}

void qss::minSigma()
{
    // Recherche de la prochaine fonction à calculer
    unsigned int j = 1;
    unsigned int j_min = 0;
    double v_min = getSigma(0).getValue();

    while (j < m_functionNumber)
    {
        if (v_min > getSigma(j).getValue())
        {
            v_min = getSigma(j).getValue();
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
        setValue(it->first,it->second);
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


void qss::getOutputFunction(const Time& /* time */,
                            ExternalEventList& output) 
{
    if (getState(0) == RUN and m_active) {
        devs::ExternalEvent* ee = new devs::ExternalEvent("out");

        for (unsigned int i = 0; i < m_functionNumber ; i++)
            ee << devs::attribute(m_variableName[i], getValue(i));
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

    //  setLastTime(i,event->getTime());
    switch (getState(i)) {
    case INIT: // init du gradient
        setState(i,RUN);
        setGradient(i,compute(i));
        updateSigma(i);
        minSigma();
        break;
    case RUN:
        // Mise à jour de l'index
        if (getGradient(i) >= 0) setIndex(i,getIndex(i)+1);
        else setIndex(i,getIndex(i)-1);

        for (unsigned int j = 0;j < m_functionNumber;j++)
            if (j != i) {
                double e = (event.getTime() - getLastTime(j)).getValue();

                setValue(j,getValue(j)+e*getGradient(j));
            }
            else setValue(i,getValue(i)+getSigma(i).getValue()*getGradient(i));

        for (unsigned int j = 0;j < m_functionNumber;j++) {
            setLastTime(j,event.getTime());
            // Mise à jour du gradient
            setGradient(j,compute(j));
            // Mise à jour de sigma
            updateSigma(j);
        }
        minSigma();
    }
}

void qss::processExternalEvents(const ExternalEventList& event,
                                const Time& time)
{
    ExternalEventList::const_iterator it = event.begin();

    while (it != event.end()) {
        if ((*it)->onPort("parameter")) {
            std::string v_name = (*it)->getStringAttributeValue("name");
            double v_value = (*it)->getDoubleAttributeValue("value");

            processPerturbation(v_name,v_value);

            for (unsigned int j = 0;j < m_functionNumber;j++) {
                double e = (time - getLastTime(j)).getValue();

                setLastTime(j,time);
                // Mise à jour de la valeur de la fonction
                if (e > 0) setValue(j,getValue(j)+e*getGradient(j));
                // Mise à jour du gradient
                setGradient(j,compute(j));
                // Mise à jour de sigma
                updateSigma(j);
                if (getSigma(j) < 0) setSigma(j,Time(0));
            }
            minSigma();
        }
        else {
            unsigned int i = (unsigned int)(*it)->getIntegerAttributeValue("index");

            if (getState(i) == RUN) {
                double e = (time - getLastTime(i)).getValue();

                for (unsigned int j = 0;j < m_functionNumber;j++) {
                    setLastTime(j,time);
                    // Mise à jour de la valeur de la fonction
                    if (e > 0) setValue(j,getValue(j)+e*getGradient(j));
                    // Mise à jour du gradient
                    setGradient(j,compute(j));
                    // Mise à jour de sigma
                    updateSigma(j);
                    if (getSigma(j) < 0) setSigma(j,Time(0));
                }
                minSigma();
            }
        }
        ++it;
    }
}

Value qss::processStateEvent(const StateEvent& event) const
{
    //  unsigned int i = to_int(event->getPortName());
    unsigned int i = m_variableIndex.find(event.getPortName())->second;

    return value::DoubleFactory::create(getValue(i));
}

}} // namespace vle extension
