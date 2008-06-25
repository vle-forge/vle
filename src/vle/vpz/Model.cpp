/**
 * @file src/vle/vpz/Model.cpp
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




#include <vle/vpz/Model.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <stack>

namespace vle { namespace vpz {

Model::Model(const Model& mdl) :
    Base(mdl)
{
    if (mdl.m_graph == 0) {
        m_graph = 0;
    } else {
        m_graph = mdl.m_graph->clone();
        std::for_each(mdl.m_atomicmodels.begin(), mdl.m_atomicmodels.end(),
                      CopyAtomicModel(m_atomicmodels, *m_graph));
    }
}

void Model::write(std::ostream& out) const
{
    out << "<structures>\n";
    writeModel(out);
    out << "</structures>\n";
}

void Model::clear()
{
    m_atomicmodels.clear();
    m_graph = 0;
}

void Model::setModel(graph::Model* mdl)
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

//
///
//// Write graph information to stream.
///
//

void Model::writeModel(std::ostream& out) const
{
    if (m_graph) {
        if (m_graph->isAtomic()) {
            writeAtomic(out, static_cast < const graph::AtomicModel*
                        >(m_graph));
        } else {
            writeCoupled(out, static_cast < const graph::CoupledModel* >
                         (m_graph));
        }
    }
}

void Model::writeCoupled(std::ostream& out, const graph::CoupledModel* mdl) const
{
    typedef std::stack < const graph::CoupledModel* > CoupledModelList;
    typedef std::stack < bool > IsWritedCoupledModel;

    CoupledModelList cms;
    IsWritedCoupledModel writed;

    cms.push(mdl);
    writed.push(false);

    while (not cms.empty()) {
        const graph::CoupledModel* top(cms.top());

        if (not writed.top()) {
            writed.top() = true;
            out << "<model name=\"" << top->getName() << "\" "
                << "type=\"coupled\" ";
            writeGraphics(out, top);
            out << " >\n";
            writePort(out, top);
            out << "<submodels>\n";

            const graph::ModelList& childs(top->getModelList());
            for (graph::ModelList::const_iterator it = childs.begin(); 
                 it != childs.end(); ++it) {
                if (it->second->isCoupled()) {
                    cms.push(static_cast < graph::CoupledModel* >(it->second));
                    writed.push(false);
                } else if (it->second->isAtomic()) {
                    writeAtomic(out, static_cast < graph::AtomicModel* >
                                (it->second));
                }
            }
        } else {
            out << "</submodels>\n";
            writeConnection(out, top);
            cms.pop();
            writed.pop();
            out << "</model>\n";
        }
    }
}

void Model::writeAtomic(std::ostream& out, const graph::AtomicModel* mdl) const
{
    const AtomicModel& vpzatom(atomicModels().get(mdl));

    out << "<model name=\"" << mdl->getName() << "\" "
        << "type=\"atomic\" ";

    if (not vpzatom.conditions().empty()) {
	out << "conditions=\"";

        for (StringVector::const_iterator it = vpzatom.conditions().begin();
             it != vpzatom.conditions().end(); ++it) {
            out << *it;
            if (it + 1 != vpzatom.conditions().end()) {
                out << ",";
            }
        }

        out << "\" ";
    }
    out << "dynamics=\"" << vpzatom.dynamics() << "\" ";

    if (not vpzatom.observables().empty()) {
        out << "observables=\"" << vpzatom.observables() << "\" ";
    }

    writeGraphics(out, mdl);

    out << ">\n";

    writePort(out, mdl);

    out << "</model>\n";
}

void Model::writeGraphics(std::ostream& out, const graph::Model* mdl) const
{
    if (mdl->x() >= 0) {
        out << "x=\"" << mdl->x() << "\" ";
    }
    if (mdl->y() >= 0) {
        out << "y=\"" << mdl->y() << "\" ";
    }
    if (mdl->width() >= 0) {
        out << "width=\"" << mdl->width() << "\" ";
    }
    if (mdl->height() >= 0) {
        out << "height=\"" << mdl->height() << "\" ";
    }
}

void Model::writePort(std::ostream& out, const graph::Model* mdl) const
{
    const graph::ConnectionList& ins(mdl->getInputPortList());
    if (not ins.empty()) {
        out << "<in>\n";
        for (graph::ConnectionList::const_iterator it = ins.begin(); 
             it != ins.end(); ++it) {
            out << " <port name=\"" << it->first << "\" />\n";
        }
        out << "</in>\n";
    }
    
    const graph::ConnectionList& outs(mdl->getOutputPortList());
    if (not outs.empty()) {
        out << "<out>\n";
        for (graph::ConnectionList::const_iterator it = outs.begin(); 
             it != outs.end(); ++it) {
            out << " <port name=\"" << it->first << "\" />\n";
        }
        out << "</out>\n";
    }

}

void Model::writeConnection(std::ostream& out, const graph::CoupledModel* mdl) const
{
    out << "<connections>\n";
    mdl->writeConnections(out);
    out << "</connections>\n";
}

}} // namespace vle vpz
