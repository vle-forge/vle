/** 
 * @file extension/QSS2.cpp
 * @brief 
 * @author The vle Development Team
 * @date ven, 27 oct 2006 00:06:41 +0200
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

#include <vle/extension/QSS2.hpp>
#include <cmath>

using namespace vle::devs;
using namespace vle::extension;
using namespace vle::graph;
using namespace vle::value;

mfi::mfi(qss2* qss,unsigned int i,unsigned int n):m_qss(qss),index(i),m_functionNumber(n)
{
  c = new double[m_functionNumber];
  z = new double[m_functionNumber];
  mz = new double[m_functionNumber];
  xt = new double[m_functionNumber];
  yt = new double[m_functionNumber];
}

mfi::~mfi()
{
  delete[] yt;
  delete[] xt;
  delete[] mz;
  delete[] z;
  delete[] c;
}

Couple mfi::init()
{
  double* sav = new double[m_functionNumber];
  Couple ret;

  for(unsigned int j = 0;j < m_functionNumber;j++) 
    z[j] = m_qss->getValue(j);
  for(unsigned int j = 0;j < m_functionNumber;j++) 
    mz[j] =  m_qss->compute(j);
  fiz = mz[index];
  for(unsigned int j = 0;j < m_functionNumber;j++)  {
    for(unsigned int k = 0;k < m_functionNumber;k++) 
      xt[k] = z[k];
    xt[j] += m_qss->m_precision;
    
    for(unsigned int k = 0;k < m_functionNumber;k++) {
      sav[k] = m_qss->getValue(k);
      m_qss->setValue(k,xt[k]);
    }
    for(unsigned int k = 0;k < m_functionNumber;k++) 
      yt[k] = m_qss->compute(k);
    for(unsigned int k = 0;k < m_functionNumber;k++) 
      m_qss->setValue(k,sav[k]);
    c[j] = (yt[index] - fiz)/m_qss->m_precision;
  }
  ret.value = mz[index];
  ret.derivative = 0;
  for(unsigned int j = 0;j < m_functionNumber;j++) 
    ret.derivative += c[j]*mz[j];
  m_lastTime = devs::Time(0);
  return ret;
}

Couple mfi::ext(const devs::Time& p_time, unsigned int i,const Couple & input)
{
  Couple ret;
  double e = (p_time - m_lastTime).getValue();
  double normal;
  double* zz = new double[m_functionNumber];
  double* sav = new double[m_functionNumber];

  m_lastTime = p_time;
  normal = z[i]+mz[i]*e;
  for(unsigned int j = 0;j < m_functionNumber;j++) 
    zz[j] = z[j]+mz[j]*e;
  for(unsigned int j = 0;j < m_functionNumber;j++) 
    if (j==i) {
      z[j]=input.value;
      mz[j]=input.derivative;
    }
    else z[j]+=mz[j]*e;
  /*  for(unsigned int k = 0;k < m_functionNumber;k++) 
      yt[k] = m_qss->compute(k);
      ret.value = yt[index]; */
  
  for(unsigned int j = 0;j < m_functionNumber;j++) {
    sav[j] = m_qss->getValue(j);
    m_qss->setValue(j,z[j]);
  }
  ret.value = m_qss->compute(index);
  for(unsigned int j = 0;j < m_functionNumber;j++) 
    m_qss->setValue(j,sav[j]);

  if (fabs(normal - z[i]) >= m_qss->m_threshold) {
    for(unsigned int j = 0;j < m_functionNumber;j++) {
      sav[j] = m_qss->getValue(j);
      m_qss->setValue(j,zz[j]);
    }
    
    double w = m_qss->compute(index);
    
    for(unsigned int j = 0;j < m_functionNumber;j++) 
      m_qss->setValue(j,sav[j]);
    c[i] = (w-ret.value)/(normal-z[i]);
  }
  
  ret.derivative = 0;
  for(unsigned int j = 0;j < m_functionNumber;j++) 
    ret.derivative+=c[j]*mz[j];
  delete[] sav;
  delete[] zz;
  return ret;
}

qss2::qss2(const AtomicModel& p_model) :
  Dynamics(p_model),m_value(0),
  m_derivative(0),m_derivative2(0),m_index(0),m_index2(0),
  m_sigma(0),m_lastTime(0),m_mfi(0)
{
}

/*bool qss2::parseXML(xmlpp::Element* p_dynamicsNode)
{
  // PARAMETER node
  xmlpp::Element* v_parameterNode = get_children(p_dynamicsNode, "PARAMETER");

  if (!v_parameterNode) throw utils::ParseError("Excepted PARAMETER tag.");

  std::string v_precision = get_attribute(v_parameterNode, "PRECISION");

  if (v_precision == "")
    throw utils::ParseError("Excepted PRECISION attribute in PARAMETER "
			   "tag.");

  m_precision = to_double(v_precision);
  m_epsilon = m_precision;

  std::string v_threshold = get_attribute(v_parameterNode, "THRESHOLD");

  if (v_threshold == "")
    throw utils::ParseError("Excepted THRESHOLD attribute in PARAMETER "
			   "tag.");

  m_threshold = to_double(v_threshold);

  std::string v_active = get_attribute(v_parameterNode, "ACTIVE");

  if (v_active == "true") m_active = true;
  else m_active = false;

  xmlpp::Element* v_variablesNode = get_children(p_dynamicsNode,
						 "VARIABLES");
  xmlpp::Node::NodeList lst = v_variablesNode->get_children("VARIABLE");
  xmlpp::Node::NodeList::iterator it = lst.begin();
    
  m_functionNumber = 0;
  while ( it != lst.end() )
    {
      xmlpp::Element * elt = ( xmlpp::Element* )( *it );
      std::string v_name = get_attribute(elt,"NAME");
      unsigned int v_index = to_int(get_attribute(elt,"INDEX"));
	
      m_variableName[v_index] = v_name;
      m_variableIndex[v_name] = v_index;
      ++it;
      m_functionNumber++;
    }
    
  m_value = new double[m_functionNumber];
  m_derivative = new double[m_functionNumber];
  m_derivative2 = new double[m_functionNumber];
  m_index = new double[m_functionNumber];
  m_index2 = new double[m_functionNumber];
  m_sigma = new devs::Time[m_functionNumber];
  m_lastTime = new devs::Time[m_functionNumber];
  m_mfi = new pmfi[m_functionNumber];
  for(unsigned int i = 0;i < m_functionNumber;i++)
    m_mfi[i] = new mfi(this,i,m_functionNumber);
  return true;
} 

double qss2::parseParameter(const std::string & p_name, 
			   xmlpp::Element* p_dynamicsNode)
{
  xmlpp::Element* v_node;
    
  v_node = utils::xml::get_children(p_dynamicsNode,p_name);
  if (v_node) 
    return utils::to_double(utils::xml::get_attribute(v_node,"VALUE"));
  else
    return 0;
} */

double qss2::getDerivative(unsigned int i) const
{
  return m_derivative[i];
}

double qss2::getDerivative2(unsigned int i) const
{
  return m_derivative2[i];
}

double qss2::getIndex(unsigned int i) const
{
  return m_index[i];
}

double qss2::getIndex2(unsigned int i) const
{
  return m_index2[i];
}

const Time & qss2::getLastTime(unsigned int i) const
{
  return m_lastTime[i];
}

const Time & qss2::getSigma(unsigned int i) const
{
  return m_sigma[i];
}

void qss2::setIndex(unsigned int i,double p_index)
{
  m_index[i] = p_index;
}

void qss2::setIndex2(unsigned int i,double p_index)
{
  m_index2[i] = p_index;
}

void qss2::setDerivative(unsigned int i,double p_derivative)
{
  m_derivative[i] = p_derivative;
}

void qss2::setDerivative2(unsigned int i,double p_derivative)
{
  m_derivative2[i] = p_derivative;
}

void qss2::setLastTime(unsigned int i,const Time & p_time)
{
  m_lastTime[i] = p_time;
}

void qss2::setSigma(unsigned int i,const Time & p_time)
{
  m_sigma[i] = p_time;
}

void qss2::setValue(unsigned int i,double p_value)
{  
  m_value[i] = p_value;
}

void qss2::updateSigma(unsigned int) //i)
{
  // Calcul du sigma de la ième fonction
//   if (std::abs(getGradient(i)) < m_threshold)
//     setSigma(i,devs::Time::infinity);
//   else
//     if (getGradient(i) > 0)
//       setSigma(i,devs::Time((d(getIndex(i)+1)-getValue(i))/getGradient(i)));
//     else
//       setSigma(i,devs::Time(((d(getIndex(i))-getValue(i))-m_epsilon)
// 			    /getGradient(i)));
}

void qss2::minSigma()
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

bool qss2::delta(double a,double b,double c,double& s)
{
  if (a!=0)
    {
      double d = b*b-4*a*c;
      double s1,s2,temp;

      if (d<0) return false;
      s1 = (-b+sqrt(d))/(2*a);
      s2 = (-b-sqrt(d))/(2*a);
      if (s2<s1)
	{
	  temp = s1;
	  s1 = s2;
	  s2 = temp;
	}
      if (s1>0)
	{
	  s = s1;
	  return true;
	}
      else
	if (s2>0)
	  {
	    s = s2;
	    return true;
	  }
    }
  else
    {
      if (b == 0) return false;
      s = -c/b;
      if (s>0) return true;
    }
  return false;
}

bool qss2::intermediaire(double a,double b,double c,double cp,double& s)
{
  double s1,s2;

  if (delta(a,b,c,s1))
    if (delta(a,b,cp,s2))
      if (s1<s2)
	{
	  s = s1;
	  return true;
	}
      else 
	{
	  s = s2;
	  return true;
	}
    else
      {
	s = s1;
	return true;
      }
  else
    {
      if (delta(a,b,cp,s2))
	{
	  s = s2;
	  return true;
	}
    }
  return false;
}

// DEVS Methods

void qss2::finish()
{
  delete[] m_value;
  delete[] m_derivative;
  delete[] m_derivative2;
  delete[] m_index;
  delete[] m_index2;
  delete[] m_sigma;
  delete[] m_lastTime;
  for(unsigned int i = 0;i < m_functionNumber;i++)
    delete m_mfi[i];
  delete[] m_mfi;
}

void qss2::init2()
{
  for(unsigned int i = 0;i < m_functionNumber;i++)
    {
      m_index[i] = getValue(i);

      Couple res = m_mfi[i]->init();

      m_derivative[i] = res.value;
      m_derivative2[i] = res.derivative;

      m_index2[i] = m_derivative[i];
      if (fabs(m_derivative2[i]) < m_threshold) 
	setSigma(i,devs::Time::infinity);
      else
	setSigma(i,devs::Time(sqrt(2*m_precision/fabs(m_derivative2[i]))));
      setLastTime(i,devs::Time(0));
    }
  minSigma();
}

Time qss2::init()
{
  std::vector < std::pair < unsigned int , double > >::const_iterator it =
    m_initialValueList.begin();
  
  while (it != m_initialValueList.end()) {
    setValue(it->first,it->second);
    ++it;
  }
  init2();
  return getSigma(m_currentModel);
}

void qss2::getOutputFunction(const Time& /* time */,
			     ExternalEventList& output) 
{
  if (m_active) {
    devs::ExternalEvent* ee = new devs::ExternalEvent("out");
      
    for (unsigned int i = 0; i < m_functionNumber ; i++)
      ee << devs::attribute(m_variableName[i], getValue(i));
    output.addEvent(ee);
  }
}

Time qss2::getTimeAdvance()
{
  return getSigma(m_currentModel);
}

Event::EventType qss2::processConflict(const InternalEvent& /* internal */,
				       const ExternalEventList& /* extEventlist */) const
{
  return Event::EXTERNAL;
}

// void qss2::processInitEvent(devs::InitEvent* event)
// {
//   unsigned int i = m_variableIndex.find(event->getPortName())->second;  
//   double v = event->getDoubleAttributeValue(event->getPortName());

//   m_initialValueList.push_back(std::pair < unsigned int , double >(i,v));
// }

void qss2::processInternalEvent(const InternalEvent& event)
{
  unsigned int i = m_currentModel;
  Couple input;
  
  //  std::cout << "[" << i << "] int at " 
  //	    << event->getTime().getValue() << std::endl;

  input.value = m_value[i]+m_derivative[i]*m_sigma[i].getValue()
    +m_derivative2[i]/2*m_sigma[i].getValue()*m_sigma[i].getValue();
  input.derivative = m_derivative[i]+m_derivative2[i]*m_sigma[i].getValue();

//   std::cout << event->getTime().getValue() 
// 	    << " : " << input.value << " ; " 
// 	    << input.derivative << std::endl;


  // Mise à jour des sigma
//   for (unsigned int j = 0;j < m_functionNumber;j++)
//     m_sigma[j]-=(event->getTime() - m_lastTime[j]).getValue();

  // Mise à jour de l'index
  m_value[i]+=m_derivative[i]*m_sigma[i].getValue()
    +(m_derivative2[i]/2)*m_sigma[i].getValue()*m_sigma[i].getValue();
  m_index[i] = m_value[i];
  m_index2[i] = m_derivative[i]+ m_derivative2[i]*m_sigma[i].getValue();
  m_derivative[i]+=m_derivative2[i]*m_sigma[i].getValue();
  
  if (fabs(m_derivative2[i]) < m_threshold)
    m_sigma[i] = Time::infinity;
  else m_sigma[i] = Time(sqrt(2*m_precision/fabs(m_derivative2[i])));
  setLastTime(i,event.getTime());

  // Mise à jour des autres équations    
  for (unsigned int j = 0;j < m_functionNumber;j++) {
    double e = (event.getTime() - getLastTime(j)).getValue();
    
    if (e==0) m_mfi[j]->ext(event.getTime(),i,input);
    else {
      setLastTime(j,event.getTime());
      
      Couple ret = m_mfi[j]->ext(event.getTime(),i,input);
      
      m_value[j]+=m_derivative[j]*e+(m_derivative2[j]/2)*e*e;
      m_index[j]+=m_index2[j]*e;
      
      m_derivative[j] = ret.value;
      m_derivative2[j] = ret.derivative;
      
      double a = m_derivative2[j]/2;
      double b = m_derivative[j] - m_index2[j];
      double c = m_value[j] - m_index[j] + m_precision;
      double cp = m_value[j] - m_index[j] - m_precision;
      double s;
      
      if (intermediaire(a,b,c,cp,s)) m_sigma[j] = Time(s);
      else m_sigma[j] = Time::infinity;
    }
    
    //      std::cout << " -> [" << j << "] = " << m_sigma[j].getValue() << std::endl;
    
  }
  minSigma();
}
 
void qss2::processExternalEvents(const ExternalEventList& event,
				 const Time& /* time */)
{
  ExternalEventList::const_iterator it = event.begin();

  while (it != event.end()) {
    if ((*it)->onPort("parameter")) {
      std::string v_name = (*it)->getStringAttributeValue("name");
      double v_value = (*it)->getDoubleAttributeValue("value");
      
      processPerturbation(v_name,v_value);
      
      init2();
    }
    //   else
//     {
//       unsigned int i = (unsigned int)event->getIntegerAttributeValue("index");
      
//       if (getState(i) == RUN) {
//         double e = (event->getTime() - getLastTime(i)).getValue();

// 	for (unsigned int j = 0;j < m_functionNumber;j++)
// 	  {
// 	    setLastTime(j,event->getTime());
// 	    // Mise à jour de la valeur de la fonction
// 	    if (e > 0) setValue(j,getValue(j)+e*getGradient(j));
// 	    // Mise à jour du gradient
// 	    setGradient(j,compute(j));
// 	    // Mise à jour de sigma
// 	    updateSigma(j);
// 	    if (getSigma(j) < 0) setSigma(j,devs::Time(0));
// 	  }
// 	minSigma();
//       }
    ++it;
  }
}

Value qss2::processStateEvent(const StateEvent& event) const
{
  //  unsigned int i = to_int(event->getPortName());
  unsigned int i = m_variableIndex.find(event.getPortName())->second;
  double e = (event.getTime()-m_lastTime[i]).getValue();
  double x = m_value[i]+m_derivative[i]*e+m_derivative2[i]/2*e*e;

  return value::DoubleFactory::create(x);
}
