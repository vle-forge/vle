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
#include <vle/graph/AtomicModel.hpp>

namespace vle { namespace vpz {

using namespace vle::utils;

void AtomicModelList::add(const AtomicModelList& atoms)
{
    for (const_iterator it = atoms.begin(); it != atoms.end(); ++it) {
        add(it->first, it->second);
    }
}

AtomicModel& AtomicModelList::add(graph::Model* mdl, const AtomicModel& atom)
{
    const_iterator it = find(mdl);
    Assert(utils::InternalError, it == end(),
           (boost::format("The model %1% already have external information")
            % mdl->getName()));

    return (*insert(std::make_pair < graph::Model*, AtomicModel >(
                mdl, atom)).first).second;
}

const AtomicModel& AtomicModelList::get(graph::Model* atom) const
{
    AtomicModelList::const_iterator it = find(atom);
    if (it == end()) {
        Throw(utils::InternalError, boost::format(
                "The atomic model %s have not dynamics?") %
            atom->getName());
    }
    return it->second;
}

AtomicModel& AtomicModelList::get(graph::Model* atom)
{
    iterator it = find(atom);
    if (it == end()) {
        Throw(utils::InternalError, boost::format(
                "The atomic model %s have not dynamics?") %
                    atom->getName());
    }
    return it->second;
}

void Model::write(std::ostream& out) const
{
    out << "<structures><model>";
    m_graph->writeXML(out);
    out << "</model></structures>";
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

//void Model::addModel(const std::string& modelname, const Model& m)
//{
//graph::Model* mdl = m_graph->findModel(modelname);
//Assert(utils::InternalError, mdl,
//boost::format("Model '%1%' not found\n") % modelname);
//Assert(utils::InternalError, mdl->isNoVLE(),
//boost::format("Model '%1%' is not a NoVLE model\n") % modelname);
//
//graph::CoupledModel* parent = mdl->getParent();
//if (parent == 0) {
//delete mdl;
//m_graph = m.model();
//} else {
//parent->replace(mdl, m.model());
//}
//}

void Model::set_model(graph::Model* mdl)
{
    m_graph = mdl;
}

graph::Model* Model::model()
{
    return m_graph;
}

graph::Model* Model::model() const
{
    return m_graph;
}

}} // namespace vle vpz
