/** 
 * @file extension/DifferenceEquation.cpp
 * @brief 
 * @author The vle Development Team
 * @date ven, 27 oct 2006 00:05:33 +0200
 */

/*
 * Copyright (C) 2005, 2006 - The vle Development Team
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
#include <vle/utils/XML.hpp>



namespace vle { namespace extension {

using namespace devs;
using namespace utils::xml;

bool
DifferenceEquation::parseXML(xmlpp::Element* p_dynamicsNode)
{
  // DELAY node
  xmlpp::Element* v_delayNode = get_children(p_dynamicsNode,"DELAY");

  if (!v_delayNode)
    throw utils::ParseError("Excepted DELAY tag.");

  m_delay = utils::to_double(get_attribute(v_delayNode,"VALUE"));

  // PARAMETER node
  xmlpp::Element* v_parameterNode = get_children(p_dynamicsNode,"PARAMETERS");

  if (!v_parameterNode)
    throw utils::ParseError("Excepted PARAMETERS tag.");
  
  xmlpp::Node::NodeList lst = v_parameterNode->get_children("PARAMETER");
  xmlpp::Node::NodeList::iterator it = lst.begin();
  
  while (it != lst.end()) {
    xmlpp::Element * elt = (xmlpp::Element*)(*it);
    std::string v_name = get_attribute(elt,"NAME");
    std::string v_size = get_attribute(elt,"SIZE");
    std::string v_value = get_attribute(elt,"VALUE");

    m_parameters[v_name] = std::list < double >();
    m_listMaxSize[v_name] = utils::to_int(v_size);
	 if (master)
		 m_parameters[v_name].push_back( utils::to_double(v_value));
    ++it;
  }
  return true;
}

ExternalEventList * 
DifferenceEquation::getOutputFunction(devs::Time const &time)
{
  if (m_state == _S_CALCUL) {
		//std::cout<<"~~emission~~"<<(time).getValue()<<" "<< getModelName()<<"  m_r:"<<m_result<<endl;

    ParameterList::iterator it = m_parameters.begin();

    while (it != m_parameters.end()) {
      m_lastValue[it->first] = it->second.front();
      it->second.pop_front();
      ++it;
    }
    return buildEventWithADouble("out", "V", m_result, time);
  }
  else return noEvent();
}

devs::Time DifferenceEquation::getTimeAdvance()
{
  switch (m_state) {
  case _S_PARAMS:
    return 0;
    break;
  case _S_PARAMS2:
    return Time::infinity;
    break;
  case _S_CALCUL:
    return 0;
    break;
  case _S_PAUSE:
    return Time(m_delay);
    break;
  }
  return Time(0);
}

devs::Time DifferenceEquation::init()
{
  if (master) {
    m_state = _S_PARAMS;
    return 0;
  }
  else {
    m_state = _S_PARAMS2;
    return Time::infinity;
  }
}

void DifferenceEquation::processInternalEvent( devs::InternalEvent * )
{
	//std::cout<<"çççç"<<(p_event->getTime()).getValue()<<" IE sur "<<getModelName()<<m_state<<endl;
  switch (m_state) {
  case _S_PARAMS: {
    ParameterList::iterator it = m_parameters.begin();

    while (it != m_parameters.end()) {
      if (m_listMaxSize[it->first] > it->second.size())
	it->second.push_front(m_lastValue[it->first]);
      ++it;
    }
    m_result = compute();
  //  				std::cout<<"MAJ \tm_result sur "<<getModelName()<<" "<<m_result<<endl;
    m_state = _S_CALCUL;
    break;
  }
  case _S_CALCUL:
    m_state = _S_PAUSE;
    break;
  case _S_PAUSE:
    if (master)
      m_state = _S_PARAMS;
    else
      m_state = _S_PARAMS2;
    break;
  default:
    break;
  }
}

void DifferenceEquation::processExternalEvent(devs::ExternalEvent * event)
{
//std::cout<<"####"<<(event->getTime()).getValue()<<" EE sur "<<getModelName()<<" \t origine:"<<event->getSourceModelName()<<"     \t contenu:"<<event->getDoubleAttributeValue( "V")<<endl;

  std::string portName = event->getPortName();

  if (m_parameters[portName].size() < m_listMaxSize[portName]) 
    m_parameters[portName].
      push_back(event->getDoubleAttributeValue( "V"));
  
  if (m_state == _S_PARAMS2)
    m_state = _S_PARAMS;
}

value::Value DifferenceEquation::processStateEvent(devs::StateEvent * event) const
{
//std::cout<<(se->getTime()).getValue()<<"------------------------------"<<m_result<<endl;
  if (event->onPort("result"))
    return buildDouble(m_result);
  return value::ValueBase::empty;
}

void DifferenceEquation::processInitEvent(devs::InitEvent *event)
{
  std::string portName = event->getPortName();

  if (master)
	  m_parameters[portName].pop_front();
  m_parameters[portName].push_back(event->getDoubleAttributeValue(portName));

  /*  Value * b = event->getAttributeValue(port1);

  switch (b->getType()) {
  case 0:
    //BOOLEAN
    b = new Boolean(event->getBooleanAttributeValue(port1));
    break;
  case 1:
    //INTEGER
    b = new Integer(event->getIntegerAttributeValue(port1));
    break;
  case 2:
    //DOUBLE
    b = new Double(event->getDoubleAttributeValue(port1));
    break;
  case 3:
    //STRING
    b = new String(event->getStringAttributeValue(port1));
    break;
  case 6:
  case 4:
  case 5:
  case 7:
  case 8:
  case 9:
    //SET, MAP, POINTER, SPEED, COORDINATE, DIRECTION
    cerr << "Type d'attribut non reconnu" << endl;
  }
  init_v.addValue(port1,b); */
}

double DifferenceEquation::getValue(const char* p_name,unsigned int p_delta) {
  std::list < double >& list = m_parameters[p_name];
  std::list < double >::iterator it = list.begin();
  
  while (p_delta != 0 && it != list.end()) {
    ++it;
    --p_delta;
  }
  return *it;
}

}} // namespace vle extension
