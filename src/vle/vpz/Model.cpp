/*
 * @file vle/vpz/Model.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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
    m_atomicmodels.writeModel(m_graph, out);
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

}} // namespace vle vpz
