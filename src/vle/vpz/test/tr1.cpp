/** 
 * @file vle/vpz/tr1.hpp
 * @brief Translator test.
 * @author The vle Development Team
 * @date jeu, 14 jun 2007 18:48:27 +0200
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

#include <vle/vpz/test/tr1.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/graph/Model.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/NoVLEModel.hpp>
#include <vle/utils/Tools.hpp>
#include <iostream>

namespace vle { namespace vpz { namespace testunit {

void tr1::translate(const std::string& buffer)
{
    xmlpp::DomParser dom;
    dom.parse_memory(buffer);
    xmlpp::Document* doc(dom.get_document());
    xmlpp::Element* root(doc->get_root_node());

    xmlpp::Node::NodeList lst = root->get_children("models");
    xmlpp::Element* xmlmdls(dynamic_cast < xmlpp::Element* >(lst.front()));
    xmlpp::Attribute* att(xmlmdls->get_attribute("number"));
    const int nbModels(utils::to_int(att->get_value()));
    if (nbModels != 2) {
        throw(std::exception());
    }

    graph::CoupledModel* top(new graph::CoupledModel("toto", 0));
    graph::AtomicModel* atom1(top->addAtomicModel("tutu"));
    atom1->addOutputPort("out");
    graph::AtomicModel* atom2(top->addAtomicModel("tata"));
    atom2->addInputPort("in");
    top->addInternalConnection("tutu", "out", "tata", "in");
    m_model.set_model(top);

    m_model.atomicModels().add(atom1, AtomicModel("cond1", "dyn1", "", ""));
    m_model.atomicModels().add(atom2, AtomicModel("cond1", "dyn1", "", ""));

    m_dynamics.add(Dynamic("dyn1"));
    m_conditions.add(Condition("cond1"));
    m_measures.addTextStreamOutput("output1");
    m_measures.addEventMeasure("measure1", "output1");
    m_measures.addObservableToMeasure("measure1", "obs1");
}

}}} // namespace vle vpz testunit
