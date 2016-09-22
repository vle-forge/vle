/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2016 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2016 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2016 INRA http://www.inra.fr
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

#include <vle/DllDefines.hpp>
#include <vle/utils/Array.hpp>
#include <vle/devs/Executive.hpp>
#include <vle/value/Map.hpp>

namespace vle { namespace translator {

/**
 * A struct that represents the result of the \c make_graph function. It
 * provides the adjacency graph, the list of node names, the list of
 * classes names and the number of nodes.
 */
struct graph_result {
    vle::utils::Array<bool> graph;
    std::vector<std::string> nodes;
    std::vector<std::string> classes;
    long int node_number;
};

/**
 * A translator to build a DEVS graph where nodes are vpz::Class. The
 * graph uses an adjacency matrix to build connections between nodes.
 *
 * \param executive The \c vle::devs::Executive model used to build
 * structure of model.
 *
 * \param map The parameters of the translator formatted using a \c
 * vle::value::Map.
 *
 * \code
 * <map>
 *  <key name="prefix">
 *   <string>node</string> <!-- build node-0, node-1 etc. Default is vertex -->
 *  </key>
 *  <key name="number">
 *   <integer>10</integer> <!-- number of node in the graph. -->
 *  </key>
 *  <key name="adjacency matrix">
 *   <string>
 *    <!-- build a input, output ports and a connection. For example, the first
 *    operation is:
 *      * node-0 add an output port node-1.
 *      * node-1 add an input port node-0.
 *      * add connection (node-0, node-1) to (node-1 port node-0).
 *      -->
 *    0 1 0 1 1 1 1
 *    0 0 0 0 1 1 1
 *    0 0 0 0 0 1 1
 *    0 0 0 0 0 0 1
 *    0 0 0 0 0 0 0
 *    0 0 1 0 1 0 0
 *    0 0 0 1 0 0 0
 *   </string>
 *  </key>
 *  <key name="classes">
 *   <string>
 *   <!-- one class per node -->
 *   class1 class2 class3 class4 class5
 *   class6 class7
 *   </string>
 *  </key>
 *  <key name="port">
 *   <string>
 *   <!-- Type of connection:
 *     * in-out: add an output port `out' for source model and an input port
 *     `in' for destination model.
 *     * in: add an output port with same name as the destination model, and an
 *     input port `in'.
 *     * out: add an output port `out' for source model and an input port with
 *     the same name as the source model.
 *     * other: add an output port with same name as the destination model and
 *     an input port with the same name as the source model.
 *   </string>
 *  </key>
 * </map>
 * \endcode
 *
 * \exception \c vle::utils::ArgError
 */
graph_result VLE_API make_graph(vle::devs::Executive& executive,
                                const vle::value::Map& map);

}} // namesapce vle translator

#endif
