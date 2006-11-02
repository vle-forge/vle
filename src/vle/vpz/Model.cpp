/** 
 * @file vpz/Model.cpp
 * @brief 
 * @author The vle Development Team
 * @date lun, 27 fév 2006 15:47:06 +0100
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

#include <vle/vpz/Model.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/XML.hpp>
#include <vle/graph/CoupledModel.hpp>

namespace vle { namespace vpz {

using namespace vle::utils;

Model::Model() :
    m_graph(0)
{
}

Model::Model(const Model& model) :
    Base(model),
    m_graph(model.model())
{
}

Model& Model::operator=(const Model& model)
{
    if (this != &model) {
        delete m_graph;
        m_graph = model.model();
    }
    return *this;
}

Model::~Model()
{
    delete m_graph;
}

void Model::init(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "STRUCTURES");

    delete m_graph;
    m_graph = 0;

    if (not xml::exist_children(elt, "MODEL"))
        return;

    xmlpp::Element* model = xml::get_children(elt, "MODEL");
    m_graph = graph::Model::parseXMLmodel(model, 0);
}

void Model::write(xmlpp::Element* elt) const
{
    AssertI(elt);

    if (m_graph) {
        xmlpp::Element* structures = elt->add_child("STRUCTURES");
        xmlpp::Element* model = structures->add_child("MODEL");
        m_graph->writeXML(model);
    }
}

void Model::initFromModel(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "MODEL");

    delete m_graph;
    m_graph = 0;

    m_graph = graph::Model::parseXMLmodel(elt, 0);
}

void Model::clear()
{
    delete m_graph;
    m_graph = 0;
}

void Model::addModel(const std::string& modelname, const Model& m)
{
    graph::Model* mdl = m_graph->findModel(modelname);
    Assert(utils::InternalError, mdl,
           boost::format("Model '%1%' not found\n") % modelname);
    Assert(utils::InternalError, mdl->isNoVLE(),
           boost::format("Model '%1%' is not a NoVLE model\n") % modelname);

    graph::CoupledModel* parent = mdl->getParent();
    if (parent == 0) {
        delete mdl;
        m_graph = m.model();
    } else {
        parent->replace(mdl, m.model());
    }
}

void Model::setModel(graph::Model* mdl)
{
    delete m_graph;
    m_graph = mdl;
}

graph::Model* Model::model() const
{
    return (m_graph == 0) ? 0 : m_graph->clone();
}

graph::Model* Model::modelRef()
{
    return m_graph;
}

const graph::Model* Model::modelRef() const
{
    return m_graph;
}

}} // namespace vle vpz
