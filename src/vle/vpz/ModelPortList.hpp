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

#ifndef VLE_GRAPH_MODELPORTLIST_HPP
#define VLE_GRAPH_MODELPORTLIST_HPP

#include <map>
#include <string>
#include <vle/DllDefines.hpp>

namespace vle {
namespace vpz {

class BaseModel;

class VLE_API ModelPortList
{
public:
    typedef std::multimap<BaseModel*, std::string> Values;
    typedef Values::iterator iterator;
    typedef Values::const_iterator const_iterator;
    typedef Values::size_type size_type;
    typedef Values::value_type value_type;

    ModelPortList()
    {
    }

    virtual ~ModelPortList();

    /**
     * @brief Add a new ModelPort to the vector. No check is
     * performed is a connection already exist.
     *
     * @param model The model to add.
     * @param portname The port of the model to add.
     */
    void add(BaseModel* model, const std::string& portname);

    /**
     * @brief Remove a ModelPort from the vector. Linear complexity.
     *
     * @param model The model to delete.
     * @param portname The port of the model to delete.
     */
    void remove(BaseModel* model, const std::string& portname);

    /**
     * @brief Remove the specified model.
     *
     * @param model Model to be removed.
     */
    void erase(BaseModel* model)
    {
        m_lst.erase(model);
    }

    /**
     * @brief Remove all ModelPort from the vector. Linear
     * complexity.
     */
    void clear()
    {
        m_lst.clear();
    }

    /**
     * @brief Merge the vector from ModelPort vector. Linera
     * complexity.
     *
     * @param lst The vector of ModelPort to merger.
     */
    void merge(ModelPortList& lst);

    /**
     * @brief Check if a ModelPort already exist in the vector.
     * Linear complexity.
     *
     * @param model The model to check.
     * @param portname The port of the model to check.
     *
     * @return True if the (model, portname) exist in vector, false
     * otherwise.
     */
    bool exist(BaseModel* model, const std::string& portname) const;

    /**
     * @brief Check if a ModelPort already exist in the vector.
     * Linear complexity.
     *
     * @param model The model to check.
     * @param portname The port of the model to check.
     *
     * @return True if the (model, portname) exist in vector, false
     * otherwise.
     */
    bool exist(const BaseModel* model, const std::string& portname) const;

    inline iterator begin()
    {
        return m_lst.begin();
    }
    inline const_iterator begin() const
    {
        return m_lst.begin();
    }
    inline iterator end()
    {
        return m_lst.end();
    }
    inline const_iterator end() const
    {
        return m_lst.end();
    }
    inline size_type size() const
    {
        return m_lst.size();
    }

private:
    Values m_lst;
};

std::ostream& operator<<(std::ostream& out, const ModelPortList& lst);
}
} // namespace vpz graph

#endif
