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

#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/vpz/BaseModel.hpp>
#include <vle/vpz/ModelPortList.hpp>

namespace vle {
namespace vpz {

ModelPortList::~ModelPortList()
{
}

void
ModelPortList::add(BaseModel* model, const std::string& portname)
{
    if (not model) {
        throw utils::DevsGraphError(
          (fmt(_("Cannot add model port %1% in an empty model")) % portname)
            .str());
    }

    m_lst.insert(value_type(model, portname));
}

void
ModelPortList::remove(BaseModel* model, const std::string& portname)
{
    if (not model) {
        throw utils::DevsGraphError(
          (fmt(_("Cannot remove model port %1% in an empty model")) % portname)
            .str());
    }

    std::pair<iterator, iterator> its = m_lst.equal_range(model);
    auto it = its.first;
    auto previous = its.first;

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

void
ModelPortList::merge(ModelPortList& lst)
{
    for (auto& elem : lst) {
        m_lst.insert(value_type(elem.first, elem.second));
    }
}

bool
ModelPortList::exist(BaseModel* model, const std::string& portname) const
{
    std::pair<const_iterator, const_iterator> its(m_lst.equal_range(model));
    for (auto it = its.first; it != its.second; ++it) {
        if (it->second == portname) {
            return true;
        }
    }
    return false;
}

bool
ModelPortList::exist(const BaseModel* model, const std::string& portname) const
{
    std::pair<const_iterator, const_iterator> its;
    its = m_lst.equal_range(const_cast<BaseModel*>(model));
    for (auto it = its.first; it != its.second; ++it) {
        if (it->second == portname) {
            return true;
        }
    }
    return false;
}

std::ostream&
operator<<(std::ostream& out, const ModelPortList& lst)
{
    ModelPortList::const_iterator it;
    for (it = lst.begin(); it != lst.end(); ++it) {
        out << "(" << it->first->getName() << "," << it->second << ")\n";
    }
    return out;
}
}
} // namespace vle vpz
