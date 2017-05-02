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

#ifndef VLE_TRANSLATOR_GRAPHTRANSLATOR_HPP
#define VLE_TRANSLATOR_GRAPHTRANSLATOR_HPP

#include <array>
#include <random>
#include <vle/DllDefines.hpp>
#include <vle/devs/Executive.hpp>
#include <vle/utils/Array.hpp>
#include <vle/vpz/Condition.hpp>

namespace vle {
namespace translator {

class VLE_API graph_generator
{
public:
    enum class connectivity
    {
        IN_OUT, /*!< @c in-out: add an output port @c out for source model and
                 an input port @c in for destination model. */
        IN,     /*!< @c in: add an output port with same name as the
                 destination model, and an input port @c in. */
        OUT,    /*!< @c out: add an output port @c out for source model and an
                 input port with the same name as the source model.*/
        OTHER   /*!< @c other: add an output port with same name as the
                 destination model and an input port with the same name as the
                 source model. */
    };

    struct graph_metrics
    {
        int vertices;  /*!< Number of vertices. */
        int edges;     /*!< Number of edges. */
        int bandwidth; /*!< The maximum distance between two adjacent
                            vertices,  with distance measured on a line upon
                            which the vertices have been placed at unit
                            intervals.*/
    };

    struct node_metrics
    {
        int id;         /*!< Unique identifier for node. */
        int in_degree;  /*!< Number of edges entering vertex. */
        int out_degree; /*|< Number of edges leaving vertex. */
    };

    struct parameter
    {
        /*!< Function to build the @c name, @c classname and @c condition for
         * the model defined by the @c metrics. */
        std::function<void(const node_metrics& metrics,
                           std::string& name,
                           std::string& classname)>
          make_model;

        /*!< The connectivity between each DEVS model.*/
        connectivity type;

        /*!< True is the connection should be directed or undirected. */
        bool directed;
    };

    graph_generator(const parameter& params);

    graph_metrics metrics() const;

    void make_graph(vle::devs::Executive& executive,
                    int number,
                    const vle::utils::Array<bool>& graph);

    void make_smallworld(vle::devs::Executive& executive,
                         std::mt19937& gen,
                         int number,
                         int k,
                         double probability,
                         bool allow_self_loops);

    void make_scalefree(vle::devs::Executive& executive,
                        std::mt19937& gen,
                        int number,
                        double alpha,
                        double beta,
                        bool allow_self_loops);

    void make_sorted_erdos_renyi(vle::devs::Executive& executive,
                                 std::mt19937& gen,
                                 int number,
                                 double probability,
                                 bool allow_self_loops);

    void make_erdos_renyi(vle::devs::Executive& executive,
                          std::mt19937& gen,
                          int number,
                          double fraction,
                          bool allow_self_loops);

    void make_erdos_renyi(vle::devs::Executive& executive,
                          std::mt19937& gen,
                          int number,
                          int edges_number,
                          bool allow_self_loops);

private:
    parameter m_params;
    graph_metrics m_metrics;
};
}
} // namespace vle translator

#endif
