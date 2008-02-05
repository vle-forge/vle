/**
 * @file src/vle/graph/ModelPortList.cpp
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




#include <vle/graph/ModelPortList.hpp>
#include <vle/utils/Debug.hpp>



namespace vle { namespace graph {

ModelPortList::~ModelPortList()
{
}

void ModelPortList::add(Model* model, const std::string& portname)
{
    AssertS(utils::DevsGraphError, model);

    insert(std::make_pair < Model*, std::string >(model, portname));
}

void ModelPortList::remove(Model* model, const std::string& portname)
{
    AssertS(utils::DevsGraphError, model);

    std::pair < iterator, iterator > its = equal_range(model);
    iterator it = its.first;
    iterator previous = its.first;

    while (it != its.second) {
	if (it->second == portname) {
            if (it == previous) {
                it++;
    	        erase(previous);
                previous = it;
            } else {
                erase(it);
	        it = previous;
	    }
	} else {
	    previous = it;
	    it++;
	}
    }
}

void ModelPortList::remove_all()
{
    clear();
}

void ModelPortList::merge(ModelPortList& lst)
{
    for (iterator it = lst.begin(); it != lst.end(); ++it) {
        insert(std::make_pair < Model*, std::string >(it->first, it->second));
    }
}

bool ModelPortList::exist(Model* model, const std::string& portname) const
{
    std::pair < const_iterator, const_iterator > its = equal_range(model);
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
    its = equal_range(const_cast < Model* >(model));
    for (const_iterator it = its.first; it != its.second; ++it) {
        if (it->second == portname) {
            return true;
        }
    }
    return false;
}

}} // namespace vle graph
