/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
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


#include <vle/vpz/Model.hpp>
#include <vle/vpz/BaseModel.hpp>
#include <vle/vpz/AtomicModel.hpp>

namespace vle { namespace vpz {

Model::Model(const Model& mdl) :
    Base(mdl)
{
    if (mdl.m_graph == 0) {
        m_graph = 0;
    } else {
        m_graph = mdl.m_graph->clone();
    }
}

void Model::write(std::ostream& out) const
{
    out << "<structures>\n";
    m_graph->write(out);
    out << "</structures>\n";
}

void Model::clear()
{
    // m_atomicmodels.clear();
    m_graph = 0;
}

void Model::setModel(BaseModel* mdl)
{
    m_graph = mdl;
}

BaseModel* Model::model()
{
    return m_graph;
}

BaseModel* Model::model() const
{
    return m_graph;
}

void Model::updateDynamics(const std::string& oldname,
                           const std::string& newname)
{
    m_graph->updateDynamics(oldname, newname);
}

void Model::purgeDynamics(const std::set < std::string >& dynamicslist)
{
    m_graph->purgeDynamics(dynamicslist);
}

void Model::updateObservable(const std::string& oldname,
                             const std::string& newname)
{
    m_graph->updateObservable(oldname, newname);
}

void Model::purgeObservable(const std::set < std::string >& observablelist)
{
    m_graph->purgeObservable(observablelist);
}

void Model::updateConditions(const std::string& oldname,
                             const std::string& newname)
{
    m_graph->updateConditions(oldname, newname);
}

void Model::purgeConditions(const std::set < std::string >& conditionlist)
{
    m_graph->purgeConditions(conditionlist);
}

void Model::getAtomicModelList(std::vector < AtomicModel* >& list) const
{
    list.clear();
    m_graph->getAtomicModelList(m_graph, list);
}


}} // namespace vle vpz
