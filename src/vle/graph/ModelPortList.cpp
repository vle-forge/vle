/** 
 * @file graph/ModelPortList.cpp
 * @brief Represent a list of Model and input or output ports.
 * @author The vle Development Team
 * @date lun, 02 jui 2007 11:51:31 +0200
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
    for (iterator it = its.first; it != its.second; ++it) {
        if (it->second == portname) {
            erase(it);
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
