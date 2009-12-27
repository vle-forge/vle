/**
 * @file vle/graph/ModelPortList.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
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


#include <vle/graph/ModelPortList.hpp>
#include <vle/graph/Model.hpp>
#include <vle/utils/Debug.hpp>


namespace vle { namespace graph {

ModelPortList::~ModelPortList()
{
}

void ModelPortList::add(Model* model, const std::string& portname)
{
    if (not model) {
        throw utils::DevsGraphError(fmt(
                _("Cannot add model port %1% in an empty model")) %
            portname);
    }

    m_lst.insert(std::make_pair < Model*, std::string >(model, portname));
}

void ModelPortList::remove(Model* model, const std::string& portname)
{
    if (not model) {
        throw utils::DevsGraphError(fmt(
                _("Cannot remove model port %1% in an empty model")) %
            portname);
    }

    std::pair < iterator, iterator > its = m_lst.equal_range(model);
    iterator it = its.first;
    iterator previous = its.first;

    while (it != its.second) {
	if (it->second == portname) {
            if (it == previous) {
                it++;
                m_lst.erase(previous);
                previous = it;
            } else {
                m_lst.erase(it);
	        it = previous;
	    }
	} else {
	    previous = it;
	    it++;
	}
    }
}

void ModelPortList::merge(ModelPortList& lst)
{
    for (iterator it = lst.begin(); it != lst.end(); ++it) {
        m_lst.insert(std::make_pair < Model*, std::string >(
                it->first, it->second));
    }
}

bool ModelPortList::exist(Model* model, const std::string& portname) const
{
    std::pair < const_iterator, const_iterator > its(m_lst.equal_range(model));
    for (const_iterator it = its.first; it != its.second; ++it) {
        if (it->second == portname) {
            return true;
        }
    }
    return false;
}

bool ModelPortList::exist(const Model* model, const std::string& portname) const
{
    std::pair < const_iterator, const_iterator > its;
    its = m_lst.equal_range(const_cast < Model* >(model));
    for (const_iterator it = its.first; it != its.second; ++it) {
        if (it->second == portname) {
            return true;
        }
    }
    return false;
}

std::ostream& operator<<(std::ostream& out, const ModelPortList& lst)
{
    ModelPortList::const_iterator it;
    for (it = lst.begin(); it != lst.end(); ++it) {
        out << "(" << it->first->getName() << "," << it->second << ")\n";
    }
    return out;
}

}} // namespace vle graph
