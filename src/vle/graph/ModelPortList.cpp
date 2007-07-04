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



namespace vle { namespace graph {


void ModelPortList::add(Model* model, const std::string& portname)
{
    push_back(new ModelPort(model, portname));
}

void ModelPortList::remove(Model* model, const std::string& portname)
{
    for (iterator it = begin(); it != end(); ++it) {
        if ((*it)->model() == model and (*it)->port() == portname) {
            delete *it;
            erase(it);
            return;
        }
    }
}

void ModelPortList::merge(ModelPortList& lst)
{
    for (iterator it = lst.begin(); it != lst.end(); ++it) {
        push_back(new ModelPort((*it)->model(), (*it)->port()));
    }
}

bool ModelPortList::exist(Model* model, const std::string& portname) const
{
    for (const_iterator it = begin(); it != end(); ++it) {
        if ((*it)->model() == model and (*it)->port() == portname) {
            return true;
        }
    }
    return false;
}

bool ModelPortList::exist(const Model* model, const std::string& portname) const
{
    for (const_iterator it = begin(); it != end(); ++it) {
        if ((*it)->model() == model and (*it)->port() == portname) {
            return true;
        }
    }
    return false;
}

}} // namespace vle graph
