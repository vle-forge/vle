/** 
 * @file extension/CellQSS.cpp
 * @brief 
 * @author The vle Development Team
 * @date ven, 27 oct 2006 00:05:20 +0200
 */

/*
 * Copyright (C) 2006 - The vle Development Team
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

#include <vle/extension/CellQSS.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/graph/Model.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/value/Double.hpp>
#include <vle/utils/Exception.hpp>
#include <cmath>

using std::string;
using std::map;
using std::pair;
using std::vector;


namespace vle { namespace extension {
    
using namespace devs;
using namespace vle::value;

CellQSS::CellQSS(const vle::graph::AtomicModel& p_model):CellDevs(p_model),
							 m_gradient(0),m_index(0),
							 m_sigma(0),
							 m_lastTime(0),
							 m_currentTime(0),
							 m_state(0)
{
}

// bool
// CellQSS::parseXML(xmlpp::Element* p_dynamicsNode)
// {
//     if (CellDevs::parseXML(p_dynamicsNode))
//     {
// 	// PARAMETER node
// 	xmlpp::Element* v_parameterNode = get_children(p_dynamicsNode,
// 						       "PARAMETER");

// 	if (!v_parameterNode)
// 	    throw utils::ParseError("Excepted PARAMETER tag.");

// 	string v_precision = get_attribute(v_parameterNode,
// 					   "PRECISION");

// 	if (v_precision == "")
// 	    throw utils::ParseError("Excepted PRECISION attribute in"
// 				   "PARAMETER tag.");

// 	m_precision = atof(v_precision.data());
// 	m_epsilon = m_precision;

// 	xmlpp::Element* v_variablesNode = get_children(p_dynamicsNode,
// 						       "VARIABLES");
// 	xmlpp::Node::NodeList lst = v_variablesNode->get_children("VARIABLE");
// 	xmlpp::Node::NodeList::iterator it = lst.begin();

// 	m_functionNumber = 0;
// 	while ( it != lst.end() )
// 	{
// 	    xmlpp::Element * elt = ( xmlpp::Element* )( *it );
// 	    string v_name = get_attribute(elt,"NAME");
// 	    unsigned int v_index = to_int(get_attribute(elt,"INDEX"));

// 	    m_variableName[v_index] = v_name;
// 	    m_variableIndex[v_name] = v_index;
// 	    ++it;
// 	    m_functionNumber++;
// 	}

// 	m_gradient = new double[m_functionNumber];
// 	m_index = new long[m_functionNumber];
// 	m_sigma = new Time[m_functionNumber];
// 	m_lastTime = new Time[m_functionNumber];
// 	m_currentTime = new Time[m_functionNumber];
// 	m_state = new state[m_functionNumber];

// 	return true;
//     }
//     return false;
// }

double CellQSS::d(long p_index)
{
    return p_index * m_precision;
}

double CellQSS::getGradient(unsigned int i) const
{
    return m_gradient[i];
}

long CellQSS::getIndex(unsigned int i) const
{
    return m_index[i];
}

const Time & CellQSS::getCurrentTime(unsigned int i) const
{
    return m_currentTime[i];
}

const Time & CellQSS::getLastTime(unsigned int i) const
{
    return m_lastTime[i];
}

const Time & CellQSS::getSigma(unsigned int i) const
{
    return m_sigma[i];
}

CellQSS::state CellQSS::getState(unsigned int i) const
{
    return m_state[i];
}

double CellQSS::getValue(unsigned int i) const
{
    return getDoubleState(m_variableName.find(i)->second);
}

void CellQSS::setIndex(unsigned int i,long p_index)
{
    m_index[i] = p_index;
}

void CellQSS::setGradient(unsigned int i,double p_gradient)
{
    m_gradient[i] = p_gradient;
}

void CellQSS::setLastTime(unsigned int i,const devs::Time & p_time)
{
    m_lastTime[i] = p_time;
}

void CellQSS::setCurrentTime(unsigned int i,const devs::Time & p_time)
{
    m_currentTime[i] = p_time;
}

void CellQSS::setState(unsigned int i,state p_state)
{
    m_state[i] = p_state;
}

void CellQSS::setSigma(unsigned int i,const devs::Time & p_time)
{
    m_sigma[i] = p_time;
}

void CellQSS::setValue(unsigned int i,double p_value)
{
    setDoubleState(m_variableName[i],p_value);
}

void CellQSS::updateSigma(unsigned int i)
{
// Mise à jour de tous les sigma
    for (unsigned int j = 0;j < m_functionNumber;j++)
	if (i != j) setSigma(j,getSigma(j) - getSigma(i));

// Calcul du sigma de la ième fonction
    if (std::abs(getGradient(i)) < 1e-10)
	setSigma(i,Time::infinity);
    else
	if (getGradient(i) > 0)
	    setSigma(i,Time((d(getIndex(i)+1)-getValue(i))/getGradient(i)));
	else
	    setSigma(i,Time(((d(getIndex(i))-getValue(i))-m_epsilon)
			  /getGradient(i)));

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
    CellDevs::setSigma(getSigma(m_currentModel));

//    std::cout << " ====> " << m_currentModel << "/"
//	      << m_functionNumber << std::endl;

}

// DEVS Methods
void CellQSS::finish()
{
  delete[] m_gradient;
  delete[] m_index;
  delete[] m_sigma;
  delete[] m_currentTime;
  delete[] m_lastTime;
  delete[] m_state;
}

devs::Time CellQSS::init()
{
  vector < pair < unsigned int , double > >::const_iterator it =
    m_initialValueList.begin();
  
  while (it != m_initialValueList.end()) {
    initDoubleNeighbourhood(m_variableName[it->first],0.0);
    initDoubleState(m_variableName[it->first],it->second);
    ++it;
  }
  
  for(unsigned int i = 0;i < m_functionNumber;i++) {
    m_gradient[i] = 0.0;
    m_index[i] = (long)(floor(getValue(i)/m_precision));
    setSigma(i,Time(0));
    setCurrentTime(i,Time(0));
    setLastTime(i,Time(0));
    setState(i,INIT);
  }
  m_currentModel = 0;
  return Time(0);
}

// void CellQSS::processInitEvents(const InitEventList& event)
// {
//   string v_name = event->getPortName();
//   unsigned int i = m_variableIndex[v_name];
//   double v = event->getDoubleAttributeValue(event->getPortName());
  
//   m_initialValueList.push_back(pair < unsigned int , double >(i,v));
// }

void CellQSS::processInternalEvent(const InternalEvent& event)
{
  unsigned int i = m_currentModel;

//    std::cout << event->getTime().getValue() << std::endl;

  setCurrentTime(i,event.getTime());
  switch (getState(i)) {
  case INIT:
    setState(i,INIT2);
    setSigma(i,Time(0.00001));
    CellDevs::setSigma(Time(0.00001));
    break;
  case INIT2: // init du gradient
    setState(i,RUN);
    setDelay(0.0);
    setGradient(i,compute(i));
    updateSigma(i);
    break;
  case RUN:
    // Mise à jour de l'index
    if (getGradient(i) > 0) setIndex(i,getIndex(i)+1);
    else setIndex(i,getIndex(i)-1);
    // Mise à jour de x
    setValue(i,d(getIndex(i)));
    // Mise à jour du gradient
    setGradient(i,compute(i));
    // Mise à jour de sigma
    updateSigma(i);
  }
  setLastTime(i,event.getTime());
}

void CellQSS::processExternalEvents(const ExternalEventList& event,
				    const Time& time)
{
  CellDevs::processExternalEvents(event,time);

  ExternalEventList::const_iterator it = event.begin();

  while (it != event.end()) {
    if (getState(0) == RUN)
      for (unsigned int i = 0; i < m_functionNumber ; i++) {
	double e = time.getValue() -
	  getLastTime(i).getValue();

	setCurrentTime(i,time);
	// Mise à jour de la valeur de la fonction
	if (e > 0)
	  setValue(i,getValue(i)+e*getGradient(i));
	// Mise à jour du gradient
	setGradient(i,compute(i));
	// Mise à jour de sigma
	updateSigma(i);
	
	if (getSigma(i) < 0)
	  setSigma(i,Time(0));
	setLastTime(i,time);
      }
    ++it;
  }
}
   
void CellQSS::processPerturbation(const ExternalEvent& /* event */)
{
  for (unsigned int i = 0; i < m_functionNumber ; i++) {
    m_gradient[i] = 0.0;
    m_index[i] = (long)(floor(getValue(i)/m_precision));
    setSigma(i,Time(0));
    setState(i,INIT);
  }
  CellDevs::setSigma(0);
}

Value CellQSS::processStateEvent(const StateEvent& event) const
{
  return value::DoubleFactory::create(getDoubleState(event.getPortName()));
}

}} // namespace vle extension
