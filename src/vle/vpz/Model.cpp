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
#include <vle/graph/CoupledModel.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/NoVLEModel.hpp>
#include <stack>

namespace vle { namespace vpz {

void AtomicModelList::add(const AtomicModelList& atoms)
{
    for (const_iterator it = atoms.begin(); it != atoms.end(); ++it) {
        add(it->first, it->second);
    }
}

AtomicModel& AtomicModelList::add(graph::Model* mdl,
                                  const AtomicModel& atom)
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
    const_iterator it = find(atom);
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

const AtomicModel& AtomicModelList::get(const graph::Model* atom) const
{
    const_iterator it = find(const_cast < graph::Model* >(atom));
    if (it == end()) {
        Throw(utils::InternalError, boost::format(
                "The atomic model %s have not dynamics?") %
            atom->getName());
    }
    return it->second;
}

AtomicModel& AtomicModelList::get(const graph::Model* atom)
{
    iterator it = find(const_cast < graph::Model* >(atom));
    if (it == end()) {
        Throw(utils::InternalError, boost::format(
                "The atomic model %s have not dynamics?") %
                    atom->getName());
    }
    return it->second;
}

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
    delete m_graph;
    m_graph = 0;
}

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
        } else if (m_graph->isCoupled()) {
            writeCoupled(out, static_cast < const graph::CoupledModel* >
                         (m_graph));
        } else {
            writeNovle(out, static_cast < const graph::NoVLEModel* >
                        (m_graph));
        }
    }
}

void Model::writeCoupled(std::ostream& out, const graph::CoupledModel* mdl) const
{
    typedef std::list < const graph::CoupledModel* > CoupledModelList;
    typedef std::list < CoupledModelList > StackList;

    StackList stack;
    CoupledModelList toclosestack;

    stack.push_front(CoupledModelList());
    stack.front().push_back(mdl);
    toclosestack.push_front(mdl);

    while (not stack.empty()) {
        CoupledModelList current(stack.front());
        const graph::CoupledModel* top(current.front());
        stack.pop_front();
        while (not current.empty()) {
            bool firstpush = true;
            current.pop_front();

            out << "<model name=\"" << top->getName() << "\" "
                << "type=\"coupled\" >\n";
            writePort(out, top);

            out << " <submodels>\n";
            const graph::ModelList& childs(top->getModelList());
            for (graph::ModelList::const_iterator it = childs.begin(); 
                 it != childs.end(); ++it) {
                if (it->second->isCoupled()) {
                    if (firstpush) {
                        stack.push_back(CoupledModelList());
                        firstpush = false;
                    }
                    stack.back().push_back(
                        static_cast < graph::CoupledModel* >(it->second));
                    toclosestack.push_front(
                        static_cast < graph::CoupledModel* >(it->second));
                } else if (it->second->isAtomic()) {
                    writeAtomic(out, static_cast < graph::AtomicModel* >
                                (it->second));
                } else if (it->second->isNoVLE()) {
                    writeNovle(out, static_cast < graph::NoVLEModel* >
                               (it->second));
                }
            }

            if (toclosestack.front() == top) {
                out << " </submodels>\n";
                writeConnection(out, top);
                toclosestack.pop_front();
                out << "</model>\n";
            }
        }
    }
    while (not toclosestack.empty()) {
        out << " </submodels>\n";
        writeConnection(out, toclosestack.front());
        toclosestack.pop_front();
        out << "</model>\n";
    }
}

void Model::writeAtomic(std::ostream& out, const graph::AtomicModel* mdl) const
{
    const AtomicModel& vpzatom(atomicModels().get(mdl));

    out << "<model name=\"" << mdl->getName() << "\" "
        << "type=\"atomic\" "
        << "conditions=\"" << vpzatom.conditions() << "\" "
        << "dynamics=\"" << vpzatom.dynamics() << "\" "
        << "observables=\"" << vpzatom.observables() << "\" >\n";

    writePort(out, mdl);

    out << "</model>\n";
}

void Model::writeNovle(std::ostream& out, const graph::NoVLEModel* mdl) const
{
    const AtomicModel& vpzatom(atomicModels().get(mdl));

    out << "<model name=\"" << mdl->getName() << "\" "
        << "type=\"novle\" "
        << "translator=\"" << vpzatom.translator() << "\" >\n";

    writePort(out, mdl);

    out << "</model>\n";
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
