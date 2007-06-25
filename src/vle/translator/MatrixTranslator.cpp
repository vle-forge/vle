/**
 * @file   MatrixTranslator.cpp
 * @author The VLE Development Team.
 * @date   vendredi 22 juin 2007
 *
 * @brief	 Virtual Laboratory Environment - VLE Project
 * Copyright (C) 2004-2007 LIL Laboratoire d'Informatique du Littoral
 *
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <MatrixTranslator.hpp>
#include <vle/utils/Rand.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <boost/format.hpp>

using namespace vle;

namespace vle { namespace translator {

MatrixTranslator::MatrixTranslator(const vle::vpz::Project& prj) :
  Translator(prj),
  m_dimension(0)
{
}

MatrixTranslator::~MatrixTranslator()
{
}

std::string MatrixTranslator::getName(unsigned int i, unsigned int j) const
{
  if (m_dimension == 0) return (boost::format("%1%_%2%") % 
				m_prefix % i).str();
  else
    if (m_dimension == 1) return (boost::format("%1%_%2%_%3%") % 
				  m_prefix % i % j).str();
  return "";
}    

/********************************
<?xml version="1.0" ?>
<celldevs>
 <grid>
  <dim axe="0" number="11" />
  <dim axe="1" number="7" />
 </grid>
 <cells connectivity="von neumann | moore" library="libcellule" >
  <prefix>cell</prefix>
  <init>
0 0 0 1 1 1 1 1 1 1 0
0 0 0 0 1 1 1 1 1 1 0
0 0 0 0 0 1 1 1 1 0 0
0 0 0 0 0 0 1 1 0 0 0
0 0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0 0
  </init>
 </cells>
</celldevs>

von neumann = 8
moore = 4
*********************************/

void MatrixTranslator::parseXML(const std::string& buffer)
{
  // XML parsing
  m_parser.parse_memory(buffer);
  m_root = utils::xml::get_root_node(m_parser, "celldevs");

  xmlpp::Element* grid = utils::xml::get_children(m_root, "grid");
  xmlpp::Node::NodeList lst = grid->get_children("dim");
  xmlpp::Node::NodeList::iterator it;

  for (it = lst.begin(); it != lst.end(); ++it) {
    unsigned int dim = atoi(utils::xml::get_attribute((xmlpp::Element*)(*it), "axe").c_str());
    unsigned int number = atoi(utils::xml::get_attribute((xmlpp::Element*)(*it), "number").c_str());
    
    m_size[dim] = number;
    if (m_dimension < dim) m_dimension = dim;
  }
  
  xmlpp::Element* cells = utils::xml::get_children(m_root, "cells");
  
  if (utils::xml::exist_attribute(cells , "connectivity")) {
    std::string s = utils::xml::get_attribute(cells , "connectivity");
    
    if (s == "von neumann") m_connectivity = VON_NEUMANN;
    else if (s == "moore") m_connectivity = MOORE;
  }
  else 
    m_connectivity = LINEAR;
  m_library = utils::xml::get_attribute(cells , "library");

  xmlpp::Element* prefix = utils::xml::get_children(cells, "prefix");

  m_prefix = prefix->get_child_text()->get_content();

  if (utils::xml::exist_children(cells , "init")) {
    xmlpp::Element* init = utils::xml::get_children(cells, "init");

    m_init = init->get_child_text()->get_content();
  }
}

void MatrixTranslator::translate(const std::string& buffer)
{
  // XML parsing
  parseXML(buffer);

  // translate
  translateStructures();
  translateDynamics();
  translateTranslators();
  translateConditions();
  translateViews();
}

void MatrixTranslator::translateStructures()
{
  vle::graph::CoupledModel* root = new vle::graph::CoupledModel(m_prefix,0);

  m_model.set_model(root);

  // Models + ports
  if (m_dimension == 0) 
    for (unsigned int i = 1 ; i <= m_size[0] ; i++) {
      vle::graph::AtomicModel* atomicModel = root
	->addAtomicModel(getName(i));
      
      m_models[getName(i)] = &m_model.atomicModels()
	.add(atomicModel, 
	     vle::vpz::AtomicModel((boost::format("c_%1%_%2%") 
				    % m_prefix % i).str(), 
				   "cell", "", ""));

      if (i != 1) atomicModel->addInputPort("L");
      if (i != m_size[0]) atomicModel->addInputPort("R");
      
      atomicModel->addOutputPort("out");
    }
  else
    if (m_dimension == 1)
      for (unsigned int j = 1 ; j <= m_size[1] ; j++) {
	for (unsigned int i = 1 ; i <= m_size[0] ; i++) {
	  vle::graph::AtomicModel* atomicModel = root
	    ->addAtomicModel(getName(i,j));
	  
	  m_models[getName(i,j)] = &m_model.atomicModels()
	    .add(atomicModel, 
		 vle::vpz::AtomicModel((boost::format("c_%1%_%2%_%3%") 
					% m_prefix % i % j).str(), 
				       "cell", "", ""));
	  
	  if (i != 1) atomicModel->addInputPort("N");
	  if (j != 1) atomicModel->addInputPort("O");
	  if (i != m_size[0]) atomicModel->addInputPort("S");
	  if (j != m_size[1]) atomicModel->addInputPort("E");
	  if (m_connectivity == VON_NEUMANN) {
	    if (i != 1 and j != 1) atomicModel->addInputPort("NO");
	    if (i != 1 and j != m_size[1]) atomicModel->addInputPort("NE");
	    if (i != m_size[0] and j != 1) atomicModel->addInputPort("SO");
	    if (i != m_size[0] and j != m_size[1]) atomicModel->addInputPort("SE");
	  }
	  
	  atomicModel->addOutputPort("out");
	}
      }
  
  // Connections
  if (m_dimension == 0) 
    for (unsigned int i = 1 ; i <= m_size[0] ; i++) {
      if (i != 1)
	root->addInternalConnection(getName(i),"out",getName(i-1),"R");
      if (i != m_size[0])
	root->addInternalConnection(getName(i),"out",getName(i+1),"L");
    }
  else
    if (m_dimension == 1)
      for (unsigned int j = 1 ; j <= m_size[1] ; j++) {
	for (unsigned int i = 1 ; i <= m_size[0] ; i++) {
	  if (j != 1)
	    root->addInternalConnection(getName(i,j),"out",getName(i,j-1),"E");
	  if (i != 1) 
	    root->addInternalConnection(getName(i,j),"out",getName(i-1,j),"S");
	  if (j != m_size[1])
	    root->addInternalConnection(getName(i,j),"out",getName(i,j+1),"O");
	  if (i != m_size[0])
	    root->addInternalConnection(getName(i,j),"out",getName(i+1,j),"N");
	  if (m_connectivity == VON_NEUMANN) {
	    if (j != 1 and i != 1) 
	      root->addInternalConnection(getName(i,j),"out",getName(i-1,j-1),"SE");		
	    if (j != m_size[1] and i != 1) 
	      root->addInternalConnection(getName(i,j),"out",getName(i-1,j+1),"SO");		
	    if (j != 1 and i != m_size[0]) 
	      root->addInternalConnection(getName(i,j),"out",getName(i+1,j-1),"NE");		
	    if (j != m_size[1] and i != m_size[0]) 
	      root->addInternalConnection(getName(i,j),"out",getName(i+1,j+1),"NO");		
	  }
	}
      }
}
    
void MatrixTranslator::translateDynamics()
{
  vle::vpz::Dynamic dynamics("cell");
      
  dynamics.setLibrary(m_library);
  m_dynamics.add(dynamics);
}

void MatrixTranslator::translateConditions()
{
  if (m_dimension == 0)
    for (unsigned int i = 1 ; i <= m_size[0] ; i++) {
      vle::vpz::Condition condition((boost::format("c_%1%_%2%") 
				     % m_prefix % i).str());
      vle::value::Set neighbourhood = vle::value::SetFactory::create();
      
      if (i != 1) 
	neighbourhood->addValue(vle::value::StringFactory::create("L"));
      if (i != m_size[0]) 
	neighbourhood->addValue(vle::value::StringFactory::create("R"));
      condition.addValueToPort("Neighbourhood",neighbourhood);
      m_conditions.add(condition);
    }
  else
    if (m_dimension == 1)
      for (unsigned int i = 1 ; i <= m_size[0] ; i++) 
	for (unsigned int j = 1 ; j <= m_size[1] ; j++) {
	  vle::vpz::Condition condition((boost::format("c_%1%_%2%_%3%") 
					 % m_prefix % i % j).str());
	  vle::value::Set neighbourhood = vle::value::SetFactory::create();
	  
	  if (i != 1) 
	    neighbourhood->addValue(vle::value::StringFactory::create("N"));
	  if (j != m_size[1]) 
	    neighbourhood->addValue(vle::value::StringFactory::create("E"));
	  if (i != m_size[0]) 
	    neighbourhood->addValue(vle::value::StringFactory::create("S"));
	  if (j != 1) 
	    neighbourhood->addValue(vle::value::StringFactory::create("O"));
	  if (m_connectivity == VON_NEUMANN) {
	    if (i != 1 and j != 1) 
	      neighbourhood->addValue(vle::value::StringFactory::create("NO"));
	    if (i != 1 and j != m_size[1]) 
	      neighbourhood->addValue(vle::value::StringFactory::create("NE"));
	    if (i != m_size[0] and j != 1) 
	      neighbourhood->addValue(vle::value::StringFactory::create("SO"));
	    if (i != m_size[0] and j != m_size[1]) 
	      neighbourhood->addValue(vle::value::StringFactory::create("SE"));
	  }
	  condition.addValueToPort("Neighbourhood",neighbourhood);
	  m_conditions.add(condition);
	}
}
    
}}
