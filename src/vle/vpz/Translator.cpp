/** 
 * @file vpz/Translator.cpp
 * @brief Translator base class to develop translator plugin.
 * @author The vle Development Team
 * @date mar, 28 fév 2006 15:02:18 +0100
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

#include <vle/vpz/Translator.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Exception.hpp>

namespace vle { namespace vpz {

using namespace vle::utils;

Translator::Translator(xmlpp::Element* root) :
    m_structure(0),
    m_dynamics(0),
    m_experiment(0),
    m_graphics(0),
    m_root(root)
{
    Assert(utils::ParseError, root, "Translator with null XML element.");
}

Translator::~Translator()
{
    delete m_structure;
    delete m_dynamics;
    delete m_experiment;
    delete m_graphics;
}

void Translator::buildDocuments(const std::string& modelName,
                                const std::string& modelType)
{
    m_structure = new xmlpp::Document();
    xmlpp::Element* e = m_structure->create_root_node("MODEL");
    e->set_attribute("NAME", modelName);
    e->set_attribute("TYPE", modelType);

    xmlpp::Element* e2 = e->add_child("DESCRIPTION");
    e2->add_child_text("");

    m_dynamics = new xmlpp::Document();
    e = m_dynamics->create_root_node("MODELS");

    m_experiment = new xmlpp::Document();
    e = m_experiment->create_root_node("EXPERIMENT");

    m_graphics = new xmlpp::Document();
    e = m_graphics->create_root_node("MODELS");
    e->set_attribute("NAME", modelName);
    e->set_attribute("TYPE", modelType);
}

xmlpp::Element* Translator::getRoot() const
{
    return m_root;
}

xmlpp::Element* Translator::addAtomicModel(xmlpp::Element* node,
                                           const std::string& modelName)
{
    xmlpp::Element* modelNode = node->add_child("MODEL");

    modelNode->set_attribute("NAME", modelName);
    modelNode->set_attribute("TYPE", "atomic");
    return modelNode;
}


xmlpp::Element* Translator::addCoupledModel(xmlpp::Element* node,
                                            const std::string& modelName)
{
    xmlpp::Element* modelNode = node->add_child("MODEL");

    modelNode->set_attribute("NAME", modelName);
    modelNode->set_attribute("TYPE", "coupled");
    return modelNode;
}

void Translator::addInternalConnection(xmlpp::Element* node,
				       const std::string& originModelName,
				       const std::string& originPortName,
				       const std::string& destinationModelName,
				       const std::string& destinationPortName)
{
    xmlpp::Element * connection = node->add_child("CONNECTION");
    connection->set_attribute("TYPE", "internal");

    xmlpp::Element * origin = connection->add_child("ORIGIN");
    origin->set_attribute("MODEL", originModelName);
    origin->set_attribute("PORT", originPortName);

    xmlpp::Element * destination = connection->add_child("DESTINATION");
    destination->set_attribute("MODEL", destinationModelName);
    destination->set_attribute("PORT", destinationPortName);
}

void  Translator::addPort(xmlpp::Element* node , const std::string& portName)
{
    xmlpp::Element* portNode = node->add_child("PORT");

    portNode->set_attribute("NAME",portName);
}

xmlpp::Element* Translator::addInitPorts(xmlpp::Element* node)
{
    return node->add_child("INIT");
}

xmlpp::Element* Translator::addInPorts(xmlpp::Element* node)
{
    return node->add_child("IN");
}

xmlpp::Element* Translator::addOutPorts(xmlpp::Element* node)
{
    return node->add_child("OUT");
}

xmlpp::Element* Translator::addStatePorts(xmlpp::Element* node)
{
    return node->add_child("STATE");
}

xmlpp::Element*  Translator::addDoubleParameter(xmlpp::Element* node,
						const std::string& name,
						double value)
{
    xmlpp::Element* newnode = node->add_child(name);

    newnode->set_attribute("VALUE", utils::to_string(value));
    return newnode;
}

xmlpp::Element* Translator::addIntegerParameter(xmlpp::Element* node,
						const std::string& name,
						long value)
{
    xmlpp::Element* newnode = node->add_child(name);

    newnode->set_attribute("VALUE", utils::to_string(value));
    return newnode;
}

xmlpp::Element* Translator::addBooleanParameter(xmlpp::Element* node,
						const std::string& name,
						bool value)
{
    xmlpp::Element* newnode = node->add_child(name);

    newnode->set_attribute("VALUE",value?"true":"false");
    return newnode;
}

xmlpp::Element* Translator::addStringParameter(xmlpp::Element* node,
					       const std::string& name,
					       const std::string& value)
{
    xmlpp::Element* newnode = node->add_child(name);

    newnode->set_attribute("VALUE",value.c_str());
    return newnode;
}

xmlpp::Element* Translator::addModelDynamics(xmlpp::Element* node,
					     const std::string& name,
					     const std::string& formalismName,
					     const std::string& type)
{
    xmlpp::Element* newnode = node->add_child("MODEL");

    newnode->set_attribute("NAME",name.c_str());

    xmlpp::Element* dynamics = newnode->add_child("DYNAMICS");
    dynamics->set_attribute("FORMALISM", formalismName.c_str());
    dynamics->set_attribute("TYPE", type.c_str());
    return dynamics;
}

void Translator::setExperiment(xmlpp::Element* node,
			       const std::string& name,
			       const std::string& duration,
			       const std::string& date,
			       const std::string& seed)
{
    node->set_attribute("NAME",name);
    node->set_attribute("DURATION",duration);
    node->set_attribute("DATE",date);
    node->set_attribute("SEED",seed);
}

xmlpp::Element* Translator::addOutput(xmlpp::Element* node,
				      const std::string& name,
				      const std::string& type,
				      const std::string& format,
				      const std::string& plugin,
				      const std::string& location)
{
    xmlpp::Element* outputNode = node->add_child("OUTPUT");

    outputNode->set_attribute("NAME",name);
    outputNode->set_attribute("TYPE",type);
    outputNode->set_attribute("FORMAT",format);
    outputNode->set_attribute("PLUGIN",plugin);
    outputNode->set_attribute("LOCATION",location);
    return outputNode;
}


xmlpp::Element* Translator::addConditions(xmlpp::Element* node)
{
    return node->add_child("EXPERIMENTAL_CONDITIONS");
}

void Translator::addBooleanCondition(xmlpp::Element* node,
				     const std::string& modelName,
				     const std::string& portName,
				     bool value)
{
    xmlpp::Element* newnode = node->add_child("CONDITION");

    newnode->set_attribute("MODEL_NAME",modelName);
    newnode->set_attribute("PORT_NAME",portName);

    xmlpp::Element* newnode2 = newnode->add_child("BOOLEAN");

    newnode2->set_attribute("VALUE",(value ? "true":"false"));
}


void Translator::addDoubleCondition(xmlpp::Element* node,
				    const std::string& modelName,
				    const std::string& portName,
				    double value)
{
    xmlpp::Element* newnode = node->add_child("CONDITION");

    newnode->set_attribute("MODEL_NAME",modelName);
    newnode->set_attribute("PORT_NAME",portName);

    xmlpp::Element* newnode2 = newnode->add_child("DOUBLE");

    newnode2->set_attribute("VALUE", utils::to_string(value));
}


void Translator::addIntegerCondition(xmlpp::Element* node,
				     const std::string& modelName,
				     const std::string& portName,
				     long value)
{
    xmlpp::Element* newnode = node->add_child("CONDITION");

    newnode->set_attribute("MODEL_NAME",modelName);
    newnode->set_attribute("PORT_NAME",portName);

    xmlpp::Element* newnode2 = newnode->add_child("INTEGER");

    newnode2->set_attribute("VALUE", utils::to_string(value));
}

xmlpp::Element* Translator::addMeasures(xmlpp::Element* node)
{
    return node->add_child("MEASURES");
}

xmlpp::Element* Translator::addOutputs(xmlpp::Element* node)
{
    return node->add_child("OUTPUTS");
}

xmlpp::Element* Translator::addTimedMeasure(xmlpp::Element* node,
					    const std::string& measureName,
					    const std::string& timeStep,
					    const std::string& outputName)
{
    xmlpp::Element* measureNode = node->add_child("MEASURE");

    measureNode->set_attribute("NAME",measureName);
    measureNode->set_attribute("TYPE","timed");
    measureNode->set_attribute("TIME_STEP",timeStep);
    measureNode->set_attribute("OUTPUT",outputName);
    return measureNode;
}

xmlpp::Element* Translator::addEventMeasure(xmlpp::Element* node,
					    const std::string& measureName,
					    const std::string& outputName)
{
    xmlpp::Element* measureNode = node->add_child("MEASURE");

    measureNode->set_attribute("NAME",measureName);
    measureNode->set_attribute("TYPE","event");
    measureNode->set_attribute("OUTPUT",outputName);
    return measureNode;
}

void  Translator::addObservable(xmlpp::Element* node,
				const std::string& modelName,
				const std::string& portName,
				const std::string& group,
				int index)
{
    xmlpp::Element* observableNode = node->add_child("OBSERVABLE");

    observableNode->set_attribute("MODEL_NAME",modelName);
    observableNode->set_attribute("PORT_NAME",portName);

    if (group.empty() == false)
	observableNode->set_attribute("GROUP",group);

    if (index != -1)
	observableNode->set_attribute("INDEX",utils::to_string(index));
}

}} // namespace vle vpz
