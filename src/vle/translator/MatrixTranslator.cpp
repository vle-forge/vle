/**
 * @file src/vle/translator/MatrixTranslator.cpp
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

#include <vle/translator/MatrixTranslator.hpp>
#include <vle/devs/Coordinator.hpp>
#include <vle/utils/Rand.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/value/XML.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <boost/format.hpp>

namespace vle { namespace translator {

MatrixTranslator::MatrixTranslator(graph::CoupledModel* model,
				   devs::Coordinator* coordinator) :
    mCoupledModel(model),
    mCoordinator(coordinator),
    m_dimension(0),
    m_init(0),
    m_symmetricport(false)
{
}

MatrixTranslator::~MatrixTranslator()
{
    if (m_init) {
        delete[] m_init;
    }
}

bool MatrixTranslator::existModel(unsigned int i, unsigned int j)
{
    if (m_dimension == 0)
        return true;
    else
        if (m_dimension == 1)
            return (!m_init or m_init[i + (j - 1) * m_size[0] - 1] != 0);
        else
            return false;
}

std::string MatrixTranslator::getDynamics(unsigned int i, unsigned int j)
{
    if (m_multipleLibrary)
        if (m_dimension == 0)
            return (boost::format("dyn_cell_%1%") % m_init[i]).str();
        else
            if (m_dimension == 1)
                return (boost::format("dyn_cell_%1%")
                        % (m_init[i+(j-1)*m_size[0] - 1])).str();
            else
                return "";
    else
        return "cell";
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

void MatrixTranslator::parseXML(const std::string& buffer)
{
    // XML parsing
    m_parser.parse_memory(buffer);
    m_root = utils::xml::get_root_node(m_parser, "celldevs");

    xmlpp::Element* grid = utils::xml::get_children(m_root, "grid");
    xmlpp::Node::NodeList lst = grid->get_children("dim");
    xmlpp::Node::NodeList::iterator it;

    for (it = lst.begin(); it != lst.end(); ++it) {
        unsigned int dim = boost::lexical_cast < unsigned int
            >((utils::xml::get_attribute((xmlpp::Element*)(*it), "axe")));
        unsigned int number = boost::lexical_cast < unsigned int
            >((utils::xml::get_attribute((xmlpp::Element*)(*it), "number")));

        m_size[dim] = number;

        if (m_dimension < dim) m_dimension = dim;
    }

    xmlpp::Element* cells = utils::xml::get_children(m_root, "cells");

    if (utils::xml::exist_attribute(cells , "connectivity")) {
        std::string s = utils::xml::get_attribute(cells , "connectivity");

        if (s == "von neumann") m_connectivity = VON_NEUMANN;
        else if (s == "moore") m_connectivity = MOORE;
    } else
        m_connectivity = LINEAR;

    if (utils::xml::exist_attribute(cells, "symmetricport")) {
        m_symmetricport = utils::to_boolean(
            utils::xml::get_attribute(cells, "symmetricport"));
    } else {
        m_symmetricport = false;
    }

    if (utils::xml::exist_attribute(cells , "library")) {
        m_library = utils::xml::get_attribute(cells , "library");

        if (utils::xml::exist_attribute(cells , "model"))
            m_model = utils::xml::get_attribute(cells , "model");

        m_multipleLibrary = false;
    } else {
        xmlpp::Element* libraries = utils::xml::get_children(cells,
                                                             "libraries");
        xmlpp::Node::NodeList lst = libraries->get_children("library");
        xmlpp::Node::NodeList::iterator it;

        for (it = lst.begin(); it != lst.end(); ++it) {
            unsigned int index = boost::lexical_cast < unsigned int >(
                (utils::xml::get_attribute((xmlpp::Element*)(*it)
                                           , "index")));
            std::string name = utils::xml::get_attribute((xmlpp::Element*)(*it),
                                                         "name").c_str();
            std::string model;

            if (utils::xml::exist_attribute((xmlpp::Element*)(*it), "model"))
                model = utils::xml::get_attribute((xmlpp::Element*)(*it),
                                                  "model").c_str();

            m_libraries[index] = std::pair < std::string, std::string >(name,
                                                                        model);
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

        boost::split(lst, init, boost::is_any_of(" \n\t"),
                     boost::algorithm::token_compress_on);

        std::vector < std:: string >::const_iterator it = lst.begin();

        unsigned int i = 0;

        while (it != lst.end()) {
            m_init[i] = boost::lexical_cast < unsigned int >(*it);
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
    translateDynamics();
    translateConditions();
    translateStructures();
}

void MatrixTranslator::translateModel(unsigned int i,
                                      unsigned int j)
{
    graph::AtomicModel* atomicModel = new
        graph::AtomicModel(getName(i, j), mCoupledModel);
    std::vector < std::string > conditions;

    conditions.push_back("cond_cell");
    conditions.push_back((boost::format("cond_%1%_%2%_%3%")
                          % m_prefix % i % j).str());
    m_models[getName(i,j)] = atomicModel;

    mCoordinator->createModel(atomicModel,
                              getDynamics(i, j),
                              conditions,
                              "obs_cell");

    if (i != 1) atomicModel->addInputPort("N");

    if (j != 1) atomicModel->addInputPort("W");

    if (i != m_size[0]) atomicModel->addInputPort("S");

    if (j != m_size[1]) atomicModel->addInputPort("E");

    if (m_connectivity == VON_NEUMANN) {
        if (i != 1 and j != 1)
            atomicModel->addInputPort("NW");

        if (i != 1 and j != m_size[1])
            atomicModel->addInputPort("NE");

        if (i != m_size[0] and j != 1)
            atomicModel->addInputPort("SW");

        if (i != m_size[0] and j != m_size[1])
            atomicModel->addInputPort("SE");
    }

    if (m_symmetricport) {
        if (i != 1) atomicModel->addOutputPort("N");

        if (j != 1) atomicModel->addOutputPort("W");

        if (i != m_size[0]) atomicModel->addOutputPort("S");

        if (j != m_size[1]) atomicModel->addOutputPort("E");

        if (m_connectivity == VON_NEUMANN) {
            if (i != 1 and j != 1)
                atomicModel->addOutputPort("NW");

            if (i != 1 and j != m_size[1])
                atomicModel->addOutputPort("NE");

            if (i != m_size[0] and j != 1)
                atomicModel->addOutputPort("SW");

            if (i != m_size[0] and j != m_size[1])
                atomicModel->addOutputPort("SE");
        }
    } else {
        atomicModel->addOutputPort("out");
    }
}

void MatrixTranslator::translateStructures()
{
    if (m_dimension == 0)
        for (unsigned int i = 1 ; i <= m_size[0] ; i++) {
            graph::AtomicModel* atomicModel = new graph::AtomicModel(getName(i),
                                                                     mCoupledModel);
            std::vector < std::string > conditions;

            conditions.push_back("cond_cell");
            conditions.push_back((boost::format("cond_%1%_%2%") % m_prefix %
                                  i).str());
            m_models[getName(i)] = atomicModel;
            mCoordinator->createModel(atomicModel, getDynamics(i), conditions,
                                      "obs_cell");

            if (i != 1) atomicModel->addInputPort("L");

            if (i != m_size[0]) atomicModel->addInputPort("R");

            if (m_symmetricport) {
                if (i != 1) atomicModel->addOutputPort("L");

                if (i != m_size[0]) atomicModel->addOutputPort("R");
            } else {
                atomicModel->addOutputPort("out");
            }
        }
    else
        if (m_dimension == 1)
            for (unsigned int j = 1 ; j <= m_size[1] ; j++)
                for (unsigned int i = 1 ; i <= m_size[0] ; i++)
                    if (existModel(i, j))
                        translateModel(i, j);

    // Connections
    if (m_dimension == 0)
        for (unsigned int i = 1 ; i <= m_size[0] ; i++) {
            if (m_symmetricport) {
                if (i != 1)
                    mCoupledModel->addInternalConnection(getName(i), "L",
                                                         getName(i - 1), "R");

                if (i != m_size[0])
                    mCoupledModel->addInternalConnection(getName(i), "R",
                                                         getName(i + 1), "L");
            } else {
                if (i != 1)
                    mCoupledModel->addInternalConnection(getName(i), "out",
                                                         getName(i - 1), "R");

                if (i != m_size[0])
                    mCoupledModel->addInternalConnection(getName(i), "out",
                                                         getName(i + 1), "L");
            }
        }
    else
        if (m_dimension == 1)
            if (m_symmetricport) {
                for (unsigned int j = 1 ; j <= m_size[1] ; j++) {
                    for (unsigned int i = 1 ; i <= m_size[0] ; i++) {
                        if (existModel(i, j))
                            translateSymmetricConnection2D(i, j);
                    }
                }
            }
            else
                for (unsigned int j = 1 ; j <= m_size[1] ; j++)
                    for (unsigned int i = 1 ; i <= m_size[0] ; i++)
                        if (existModel(i, j))
                            translateConnection2D(i, j);
}

void MatrixTranslator::translateSymmetricConnection2D(unsigned int i,
                                                      unsigned int j)
{
    if (j != 1 and existModel(i, j - 1))
        mCoupledModel->addInternalConnection
            (getName(i, j), "W", getName(i, j - 1),
             "E");

    if (i != 1 and existModel(i - 1, j))
        mCoupledModel->addInternalConnection(getName(i, j), "N",
                                             getName(i - 1, j), "S");

    if (j != m_size[1] and existModel(i, j + 1))
        mCoupledModel->addInternalConnection(getName(i, j), "E",
                                             getName(i, j + 1), "W");

    if (i != m_size[0] and existModel(i + 1, j))
        mCoupledModel->addInternalConnection(getName(i, j), "S",
                                             getName(i + 1, j), "N");

    if (m_connectivity == VON_NEUMANN) {
        if (j != 1 and i != 1 and existModel(i - 1, j - 1))
            mCoupledModel->addInternalConnection(getName(i, j), "SW",
                                                 getName(i - 1, j - 1), "SE");

        if (j != m_size[1] and i != 1 and existModel(i - 1, j + 1))
            mCoupledModel->addInternalConnection(getName(i, j), "SE",
                                                 getName(i - 1, j + 1), "SW");

        if (j != 1 and i != m_size[0] and existModel(i + 1, j - 1))
            mCoupledModel->addInternalConnection(getName(i, j), "NW",
                                                 getName(i + 1, j - 1), "NE");

        if (j != m_size[1] and i != m_size[0] and existModel(i + 1, j + 1))
            mCoupledModel->addInternalConnection(getName(i, j), "NE",
                                                 getName(i + 1, j + 1), "NW");
    }
}

void MatrixTranslator::translateConnection2D(unsigned int i,
                                             unsigned int j)
{
    if (j != 1 and existModel(i, j - 1))
        mCoupledModel->addInternalConnection(getName(i, j), "out",
                                             getName(i, j - 1), "E");

    if (i != 1 and existModel(i - 1, j))
        mCoupledModel->addInternalConnection(getName(i, j), "out",
                                             getName(i - 1, j), "S");

    if (j != m_size[1] and existModel(i, j + 1))
        mCoupledModel->addInternalConnection(getName(i, j), "out",
                                             getName(i, j + 1), "W");

    if (i != m_size[0] and existModel(i + 1, j))
        mCoupledModel->addInternalConnection(getName(i, j), "out",
                                             getName(i + 1, j), "N");

    if (m_connectivity == VON_NEUMANN) {
        if (j != 1 and i != 1 and existModel(i - 1, j - 1))
            mCoupledModel->addInternalConnection(getName(i, j), "out",
                                                 getName(i - 1, j - 1), "SE");

        if (j != m_size[1] and i != 1 and existModel(i - 1, j + 1))
            mCoupledModel->addInternalConnection(getName(i, j), "out",
                                                 getName(i - 1, j + 1), "SW");

        if (j != 1 and i != m_size[0] and existModel(i + 1, j - 1))
            mCoupledModel->addInternalConnection(getName(i, j), "out",
                                                 getName(i + 1, j - 1), "NE");

        if (j != m_size[1] and i != m_size[0] and existModel(i + 1, j + 1))
            mCoupledModel->addInternalConnection(getName(i, j), "out",
                                                 getName(i + 1, j + 1), "NW");
    }
}

void MatrixTranslator::translateDynamics()
{
    if (m_multipleLibrary) {
        std::map < unsigned int ,
            std::pair < std::string,
            std::string > >::const_iterator it = m_libraries.begin();

        while (it != m_libraries.end()) {
            vpz::Dynamic dynamics((boost::format("dyn_cell_%1%") %
                                   it->first).str());

            dynamics.setLibrary(it->second.first);

            if (it->second.second != "") dynamics.setModel(it->second.second);

            mCoordinator->addPermanent(dynamics);

            ++it;
        }
    } else {
        vpz::Dynamic dynamics("cell");

        dynamics.setLibrary(m_library);

        if (m_model != "") dynamics.setModel(m_model);

        mCoordinator->addPermanent(dynamics);
    }
}

void MatrixTranslator::translateCondition1D(unsigned int i)
{
    std::string name = (boost::format("cond_%1%_%2%")
                        % m_prefix % i).str();
    vpz::Condition condition(name);
    value::Set neighbourhood = value::SetFactory::create();

    if (i != 1)
        neighbourhood->addValue(value::StringFactory::create("L"));

    if (i != m_size[0])
        neighbourhood->addValue(value::StringFactory::create("R"));

    condition.addValueToPort("Neighbourhood", neighbourhood);

    condition.addValueToPort("_x", value::IntegerFactory::create(i));

    mCoordinator->addPermanent(condition);
}

void MatrixTranslator::translateCondition2D(unsigned int i,
                                            unsigned int j)
{
    std::string name = (boost::format("cond_%1%_%2%_%3%")
                        % m_prefix % i % j).str();
    vpz::Condition condition(name);
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

    condition.addValueToPort("Neighbourhood", neighbourhood);

    condition.addValueToPort("_x", value::IntegerFactory::create(i));
    condition.addValueToPort("_y", value::IntegerFactory::create(j));
    mCoordinator->addPermanent(condition);
}

void MatrixTranslator::translateConditions()
{
    if (m_dimension == 0) {
        for (unsigned int i = 1 ; i <= m_size[0] ; i++)
            if (!m_init || m_init[i-1] != 0)
                translateCondition1D(i);
    } else
        if (m_dimension == 1) {
            for (unsigned int i = 1 ; i <= m_size[0] ; i++)
                for (unsigned int j = 1 ; j <= m_size[1] ; j++)
                    if (existModel(i, j))
                        translateCondition2D(i, j);
        }
}

}
} // namespace vle translator
