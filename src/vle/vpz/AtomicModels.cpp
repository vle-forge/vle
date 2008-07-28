/**
 * @file src/vle/vpz/AtomicModels.cpp
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

#include <vle/vpz/AtomicModels.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {

AtomicModel::AtomicModel(const std::string& conditions,
			 const std::string& dynamics,
			 const std::string& observables) :
    m_dynamics(dynamics),
    m_observables(observables)
{
    std::string conditionList(conditions);
    boost::trim(conditionList);

    if (not conditionList.empty()) {
        boost::split(m_conditions, conditionList, boost::is_any_of(","),
                     boost::algorithm::token_compress_on);
        if (m_conditions.front().empty()) {
            m_conditions.pop_back();
        }
    }
}

void AtomicModelList::add(const AtomicModelList& atoms)
{
    for (const_iterator it = atoms.atomicmodels().begin();
         it != atoms.atomicmodels().end(); ++it) {
        add(it->first, it->second);
    }
}

AtomicModel& AtomicModelList::add(graph::Model* mdl,
                                  const AtomicModel& atom)
{
    const_iterator it = m_lst.find(mdl);
    Assert(utils::ArgError, it == end(), boost::format(
            "The model [%1%] already have external information")
            % mdl->getName());

    return (*m_lst.insert(std::make_pair < graph::Model*, AtomicModel >(
                mdl, atom)).first).second;
}

const AtomicModel& AtomicModelList::get(graph::Model* atom) const
{
    const_iterator it = m_lst.find(atom);
    if (it == end()) {
        Throw(utils::ArgError, boost::format(
                "The information about atomic model [%1%] does not exist") %
            atom->getName());
    }
    return it->second;
}

AtomicModel& AtomicModelList::get(graph::Model* atom)
{
    iterator it = m_lst.find(atom);
    if (it == end()) {
        Throw(utils::ArgError, boost::format(
                "The information about atomic model [%1%] does not exist") %
                    atom->getName());
    }
    return it->second;
}

const AtomicModel& AtomicModelList::get(const graph::Model* atom) const
{
    const_iterator it = m_lst.find(const_cast < graph::Model* >(atom));
    if (it == end()) {
        Throw(utils::ArgError, boost::format(
                "The information about atomic model [%1%] does not exist") %
            atom->getName());
    }
    return it->second;
}

AtomicModel& AtomicModelList::get(const graph::Model* atom)
{
    iterator it = m_lst.find(const_cast < graph::Model* >(atom));
    if (it == end()) {
        Throw(utils::ArgError, boost::format(
                "The information about atomic model [%1%] does not exist") %
                    atom->getName());
    }
    return it->second;
}

}} // namespace vle vpz
