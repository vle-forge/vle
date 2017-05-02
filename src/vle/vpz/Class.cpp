/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cassert>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/vpz/Class.hpp>

namespace vle {
namespace vpz {

Class::Class(const std::string& name)
  : Base()
  , m_name(name)
  , m_node(nullptr)
{
}

Class::Class(const Class& cls)
  : Base(cls)
  , m_name(cls.m_name)
  , m_node(nullptr)
{
    if (cls.m_graph) {
        m_graph = std::unique_ptr<BaseModel>(cls.m_graph->clone());
        m_node = m_graph.get();
    } else if (cls.m_node)
        m_node = cls.m_node->clone();
}

Class::~Class() = default;

void
Class::write(std::ostream& out) const
{
    if (m_graph) {
        out << "<class name=\"" << m_name.c_str() << "\" >\n";
        m_graph->write(out);
        out << "</class>\n";
    }
}

void
Class::setGraph(std::unique_ptr<BaseModel> graph)
{
    assert(m_graph.get() == m_node and
           "Can not assign vpz.project.model with a node");

    m_graph = std::move(graph);
    m_node = m_graph.get();
}

std::unique_ptr<BaseModel>
Class::graph()
{
    assert(m_graph.get() == m_node and
           "Can not assign vpz.project.model with a node");

    m_node = nullptr;
    return std::move(m_graph);
}

void
Class::setNode(BaseModel* mdl)
{
    assert(not m_graph && "Can not use setNode with vpz.project.model");

    m_node = mdl;
}

BaseModel*
Class::node()
{
    return m_node;
}

BaseModel*
Class::node() const
{
    return m_node;
}

void
Class::updateDynamics(const std::string& oldname, const std::string& newname)
{
    assert(not m_node && "Can not get node with a vpz.project.class");

    m_graph->updateDynamics(oldname, newname);
}

void
Class::purgeDynamics(const std::set<std::string>& dynamicslist)
{
    assert(not m_node && "Can not get node with a vpz.project.class");

    m_graph->purgeDynamics(dynamicslist);
}

void
Class::updateObservable(const std::string& oldname, const std::string& newname)
{
    assert(not m_node && "Can not get node with a vpz.project.class");
    m_graph->updateObservable(oldname, newname);
}

void
Class::purgeObservable(const std::set<std::string>& observablelist)
{
    assert(not m_node && "Can not get node with a vpz.project.class");
    m_graph->purgeObservable(observablelist);
}

void
Class::updateConditions(const std::string& oldname, const std::string& newname)
{
    assert(not m_node && "Can not get node with a vpz.project.class");
    m_graph->updateConditions(oldname, newname);
}

void
Class::purgeConditions(const std::set<std::string>& conditionlist)
{
    assert(not m_node && "Can not get node with a vpz.project.class");
    m_graph->purgeConditions(conditionlist);
}

void
Class::getAtomicModelList(std::vector<AtomicModel*>& list) const
{
    assert(not m_node && "Can not get node with a vpz.project.class");

    list.clear();
    m_graph->getAtomicModelList(m_graph.get(), list);
}
}
} // namespace vle vpz
