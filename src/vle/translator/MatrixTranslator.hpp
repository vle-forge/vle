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

#ifndef VLE_TRANSLATOR_MATRIXTRANSLATOR_HPP
#define VLE_TRANSLATOR_MATRIXTRANSLATOR_HPP

#include <vle/DllDefines.hpp>
#include <vle/devs/Executive.hpp>
#include <vle/utils/Array.hpp>
#include <vle/vpz/Condition.hpp>
#include <functional>
#include <array>

namespace vle {
namespace translator {

class VLE_API regular_graph_generator
{
public:
    enum class connectivity
    {
        IN_OUT, /*!< @c in-out: add an output port @c out for source model and
                     an input port @c in for destination model. */
        OTHER,  /*!< @c other: add an output port with same name as the
                     destination model and an input port with the same name as
                     the source model. */
        NAMED   /*!< @c named: use the mask named to provide output and input
                     port name. */
    };

    struct graph_metrics
    {
        int vertices; /*!< Number of vertices. */
    };

    struct node_metrics
    {
        int x; /*!< Identifier for 1d, 2d and 3d model. */
        int y; /*!< Used by regular_graph 2d amd 3d to build unique
                    identifier. */
        int z; /*!< Used by regular_graph 3d to build unique identifier. */
    };

    struct parameter
    {
        /*!< Function to build the @c name, @c classname and @c condition for
             the model defined by the @c metrics. */
        std::function<void(const node_metrics& n,
                           std::string& name,
                           std::string& classname)>
          make_model;

        connectivity type; /*!< The connectivity between each DEVS model.*/
    };

    regular_graph_generator(const parameter& params);

    graph_metrics metrics() const;

    void make_1d(vle::devs::Executive& executive,
                 int length,
                 bool wrap,
                 const std::vector<std::string>& mask,
                 int x_mask);

    void make_2d(vle::devs::Executive& executive,
                 const std::array<int, 2>& length,
                 const std::array<bool, 2>& wrap,
                 const utils::Array<std::string>& mask,
                 int x_mask,
                 int y_mask);

private:
    parameter m_params;
    graph_metrics m_metrics;
};
}
}

#endif
