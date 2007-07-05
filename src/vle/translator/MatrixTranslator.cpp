/** 
 * @file MatrixTranslator.cpp
 * @brief 
 * @author The vle Development Team
 * @date jeu, 05 jui 2007 17:13:49 +0200
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

namespace vle { namespace translator {

MatrixTranslator::MatrixTranslator(const vpz::Project& prj) :
    Translator(prj),
    m_dimension(0),
    m_init(0)
{
}

MatrixTranslator::~MatrixTranslator()
{
    if (m_init) delete[] m_init;
}

bool MatrixTranslator::existModel(unsigned int i, unsigned int j)
{
    if (m_dimension == 0)
        return true;
    else
        if (m_dimension == 1)
            return (!m_init or m_init[i+(j-1)*m_size[0]-1] != 0);
        else
            return false;
}

std::string MatrixTranslator::getDynamics(unsigned int i, unsigned int j)
{
    if (m_multipleLibrary) 
        if (m_dimension == 0) 
            return (boost::format("cell_%1%") % m_init[i]).str();
        else
            if (m_dimension == 1) 
                return (boost::format("cell_%1%")
                        % (m_init[i+(j-1)*m_size[0]-1])).str();
            else
                return "";
    else return "cell";
}

std::string MatrixTranslator::getName(unsigned int i, unsigned int j) const
{
    if (m_dimension == 0)
        return (boost::format("%1%_%2%") % m_prefix % i).str();
    else
        if (m_dimension == 1) return
            (boost::format("%1%_%2%_%3%") % m_prefix % i % j).str();
    return "";
}    

/********************************
  <?xml version="1.0" ?>
  <celldevs>
  <grid>
  <dim axe="0" number="11" />
  <dim axe="1" number="7" />
  </grid>
  <cells connectivity="neuman|moore" library="libcellule" >
  <libraries>
  <library index="1" name="lib1" />
  <library index="2" name="lib2" />
  </libraries>
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
        unsigned int dim = atoi(utils::xml::get_attribute((xmlpp::Element*)(*it), 
                                                          "axe").c_str());
        unsigned int number = atoi(utils::xml::get_attribute((xmlpp::Element*)(*it), 
                                                             "number").c_str());

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
    if (utils::xml::exist_attribute(cells , "library")) {
        m_library = utils::xml::get_attribute(cells , "library");
        m_multipleLibrary = false;
    }
    else {
        xmlpp::Element* libraries = utils::xml::get_children(cells, "libraries");
        xmlpp::Node::NodeList lst = libraries->get_children("library");
        xmlpp::Node::NodeList::iterator it;

        for (it = lst.begin(); it != lst.end(); ++it) {
            unsigned int index = atoi(utils::xml::get_attribute((xmlpp::Element*)(*it)
                                                                , "index").c_str());
            std::string name = utils::xml::get_attribute((xmlpp::Element*)(*it), 
                                                         "name").c_str();

            m_libraries[index] = name;
        }
        m_multipleLibrary = true;
    }

    xmlpp::Element* prefix = utils::xml::get_children(cells, "prefix");

    m_prefix = prefix->get_child_text()->get_content();

    if (utils::xml::exist_children(cells , "init")) {
        if (m_dimension == 0) m_init = new unsigned int[m_size[0]];
        else m_init = new unsigned int[m_size[0]*m_size[1]];

        std::string init = utils::xml::get_children(cells, "init")
            ->get_child_text()->get_content();
        std::vector < std:: string > lst;

        boost::trim(init);
        boost::split(lst, init, boost::is_any_of(" \n"),
                     boost::algorithm::token_compress_on);

        std::vector < std:: string >::const_iterator it = lst.begin();
        unsigned int i = 0;

        while (it != lst.end()) {
            m_init[i] = atoi(it->c_str());
            ++it;
            ++i;
        }
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
    translateFinish();
}

void MatrixTranslator::translateStructures()
{
    graph::CoupledModel* root = new graph::CoupledModel(m_prefix,0);

    m_model.set_model(root);

    // Models + ports
    if (m_dimension == 0) 
        for (unsigned int i = 1 ; i <= m_size[0] ; i++) {
            graph::AtomicModel* atomicModel = root
                ->addAtomicModel(getName(i));

            m_models[getName(i)] = &m_model.atomicModels()
                .add(atomicModel, 
                     vpz::AtomicModel((boost::format("c_%1%_%2%") 
                                       % m_prefix % i).str(), 
                                      getDynamics(i), "", ""));

            if (i != 1) atomicModel->addInputPort("L");
            if (i != m_size[0]) atomicModel->addInputPort("R");

            atomicModel->addOutputPort("out");
        }
    else
        if (m_dimension == 1)
            for (unsigned int j = 1 ; j <= m_size[1] ; j++) {
                for (unsigned int i = 1 ; i <= m_size[0] ; i++) {
                    if (existModel(i,j)) {
                        graph::AtomicModel* atomicModel = root
                            ->addAtomicModel(getName(i,j));

                        m_models[getName(i,j)] = &m_model.atomicModels()
                            .add(atomicModel, 
                                 vpz::AtomicModel((boost::format("c_%1%_%2%_%3%") 
                                                   % m_prefix % i % j).str(), 
                                                  getDynamics(i,j), "", ""));

                        if (i != 1) atomicModel->addInputPort("N");
                        if (j != 1) atomicModel->addInputPort("W");
                        if (i != m_size[0]) atomicModel->addInputPort("S");
                        if (j != m_size[1]) atomicModel->addInputPort("E");
                        if (m_connectivity == VON_NEUMANN) {
                            if (i != 1 and j != 1) atomicModel->addInputPort("NW");
                            if (i != 1 and j != m_size[1]) atomicModel->addInputPort("NE");
                            if (i != m_size[0] and j != 1) atomicModel->addInputPort("SW");
                            if (i != m_size[0] and j != m_size[1]) 
                                atomicModel->addInputPort("SE");
                        }

                        atomicModel->addOutputPort("out");
                    }
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
        if (m_dimension == 1) {
            for (unsigned int j = 1 ; j <= m_size[1] ; j++) {
                for (unsigned int i = 1 ; i <= m_size[0] ; i++) {
                    if (existModel(i,j)) {
                        if (j != 1 and existModel(i,j-1))
                            root->addInternalConnection(getName(i,j),"out",getName(i,j-1),"E");
                        if (i != 1 and existModel(i-1,j)) 
                            root->addInternalConnection(getName(i,j),"out",getName(i-1,j),"S");
                        if (j != m_size[1] and existModel(i,j+1))
                            root->addInternalConnection(getName(i,j),"out",getName(i,j+1),"W");
                        if (i != m_size[0] and existModel(i+1,j))
                            root->addInternalConnection(getName(i,j),"out",getName(i+1,j),"N");
                        if (m_connectivity == VON_NEUMANN) {
                            if (j != 1 and i != 1 and existModel(i-1,j-1)) 
                                root->addInternalConnection(getName(i,j),"out",
                                                            getName(i-1,j-1),"SE");		
                            if (j != m_size[1] and i != 1 and existModel(i-1,j+1)) 
                                root->addInternalConnection(getName(i,j),"out",
                                                            getName(i-1,j+1),"SW");		
                            if (j != 1 and i != m_size[0] and existModel(i+1,j-1)) 
                                root->addInternalConnection(getName(i,j),"out",
                                                            getName(i+1,j-1),"NE");		
                            if (j != m_size[1] and i != m_size[0] and existModel(i+1,j+1)) 
                                root->addInternalConnection(getName(i,j),"out",
                                                            getName(i+1,j+1),"NW");		
                        }
                    }
                }
            }
        }
}

void MatrixTranslator::translateDynamics()
{
    if (m_multipleLibrary) {
        std::map < unsigned int , std::string >::const_iterator it = 
            m_libraries.begin();

        while (it != m_libraries.end()) {
            vpz::Dynamic dynamics((boost::format("cell_%1%") % it->first).str());

            dynamics.setLibrary(it->second);
            m_dynamics.add(dynamics);
            ++it;
        }
    }
    else {
        vpz::Dynamic dynamics("cell");

        dynamics.setLibrary(m_library);
        m_dynamics.add(dynamics);
    }
}

void MatrixTranslator::translateConditions()
{
    if (m_dimension == 0) {
        for (unsigned int i = 1 ; i <= m_size[0] ; i++) 
            if (!m_init || m_init[i-1] != 0) {
                vpz::Condition condition((boost::format("c_%1%_%2%") 
                                          % m_prefix % i).str());
                value::Set neighbourhood = value::SetFactory::create();

                if (i != 1) 
                    neighbourhood->addValue(value::StringFactory::create("L"));
                if (i != m_size[0]) 
                    neighbourhood->addValue(value::StringFactory::create("R"));
                condition.addValueToPort("Neighbourhood",neighbourhood);
                m_conditions.add(condition);
            }
    }
    else
        if (m_dimension == 1) {
            for (unsigned int i = 1 ; i <= m_size[0] ; i++) 
                for (unsigned int j = 1 ; j <= m_size[1] ; j++) 
                    if (existModel(i,j)) {
                        vpz::Condition condition((boost::format("c_%1%_%2%_%3%") 
                                                  % m_prefix % i % j).str());
                        value::Set neighbourhood = value::SetFactory::create();

                        if (i != 1) 
                            neighbourhood->addValue(value::StringFactory::create("N"));
                        if (j != m_size[1]) 
                            neighbourhood->addValue(value::StringFactory::create("E"));
                        if (i != m_size[0]) 
                            neighbourhood->addValue(value::StringFactory::create("S"));
                        if (j != 1) 
                            neighbourhood->addValue(value::StringFactory::create("W"));
                        if (m_connectivity == VON_NEUMANN) {
                            if (i != 1 and j != 1) 
                                neighbourhood->addValue(value::StringFactory::create("NW"));
                            if (i != 1 and j != m_size[1]) 
                                neighbourhood->addValue(value::StringFactory::create("NE"));
                            if (i != m_size[0] and j != 1) 
                                neighbourhood->addValue(value::StringFactory::create("SW"));
                            if (i != m_size[0] and j != m_size[1]) 
                                neighbourhood->addValue(value::StringFactory::create("SE"));
                        }
                        condition.addValueToPort("Neighbourhood",neighbourhood);
                        m_conditions.add(condition);
                    }
        }
}

}} // namespace vle translator
