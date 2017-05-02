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

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/bandwidth.hpp>
#include <boost/graph/erdos_renyi_generator.hpp>
#include <boost/graph/grid_graph.hpp>
#include <boost/graph/plod_generator.hpp>
#include <boost/graph/small_world_generator.hpp>
#include <random>
#include <vle/translator/GraphTranslator.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/i18n.hpp>

namespace vle {
namespace translator {

using graphT =
  boost::adjacency_list<boost::vecS,
                        boost::vecS,
                        boost::bidirectionalS,
                        boost::property<boost::vertex_name_t, std::string>>;

graph_generator::graph_generator(const parameter& params)
  : m_params(params)
  , m_metrics{ -1, -1, -1 }
{
}

graph_generator::graph_metrics
graph_generator::metrics() const
{
    return m_metrics;
}

graph_generator::graph_metrics
init_metrics(const graphT& g)
{
    graph_generator::graph_metrics ret;

    ret.vertices = boost::num_vertices(g);
    ret.edges = boost::num_edges(g);
    ret.bandwidth = boost::bandwidth(g);

    return ret;
}

void
build(const graph_generator::parameter& params,
      vle::devs::Executive& executive,
      graphT& g)
{
    graphT::vertex_iterator vi, vi_end;
    std::tie(vi, vi_end) = boost::vertices(g);

    auto modelnames = boost::get(boost::vertex_name, g);

    {
        std::string name, classname;

        int id = 0;
        graph_generator::node_metrics metrics{ 0, 0, 0 };
        graphT::vertex_iterator vi, vi_end;
        for (std::tie(vi, vi_end) = boost::vertices(g); vi != vi_end; ++vi) {
            metrics.id = id++;
            metrics.out_degree =
              utils::numeric_cast<int>(boost::out_degree(*vi, g));
            metrics.in_degree =
              utils::numeric_cast<int>(boost::in_degree(*vi, g));

            params.make_model(metrics, name, classname);

            executive.createModelFromClass(classname, name);
            modelnames[*vi] = name;
        }
    }

    {
        std::string src, dst;
        graphT::vertex_iterator i;
        graphT::out_edge_iterator ei, ei_end;

        switch (params.type) {
        case graph_generator::connectivity::IN_OUT:
            for (i = vi; i != vi_end; ++i) {
                std::tie(ei, ei_end) = boost::out_edges(*i, g);
                for (; ei != ei_end; ++ei) {
                    src = modelnames[boost::source(*ei, g)];
                    dst = modelnames[boost::target(*ei, g)];
                    executive.addOutputPort(src, "out");
                    executive.addInputPort(dst, "in");
                    executive.addConnection(src, "out", dst, "in");
                }
            }
            break;
        case graph_generator::connectivity::IN:
            for (i = vi; i != vi_end; ++i) {
                std::tie(ei, ei_end) = boost::out_edges(*i, g);
                for (; ei != ei_end; ++ei) {
                    src = modelnames[boost::source(*ei, g)];
                    dst = modelnames[boost::target(*ei, g)];
                    executive.addOutputPort(src, dst);
                    executive.addInputPort(dst, "in");
                    executive.addConnection(src, dst, dst, "in");
                }
            }
            break;
        case graph_generator::connectivity::OUT:
            for (i = vi; i != vi_end; ++i) {
                std::tie(ei, ei_end) = boost::out_edges(*i, g);
                for (; ei != ei_end; ++ei) {
                    src = modelnames[boost::source(*ei, g)];
                    dst = modelnames[boost::target(*ei, g)];
                    executive.addOutputPort(src, "out");
                    executive.addInputPort(dst, src);
                    executive.addConnection(src, "out", dst, src);
                }
            }
            break;
        case graph_generator::connectivity::OTHER:
            for (i = vi; i != vi_end; ++i) {
                std::tie(ei, ei_end) = boost::out_edges(*i, g);
                for (; ei != ei_end; ++ei) {
                    src = modelnames[boost::source(*ei, g)];
                    dst = modelnames[boost::target(*ei, g)];
                    executive.addOutputPort(src, dst);
                    executive.addInputPort(dst, src);
                    executive.addConnection(src, dst, dst, src);
                }
            }
            break;
        }
    }
}

void
graph_generator::make_graph(vle::devs::Executive& executive,
                            int number,
                            const vle::utils::Array<bool>& matrix)
{
    if (matrix.size() != utils::numeric_cast<std::size_t>(number * number))
        throw vle::utils::ArgError(_("graph_generator: bad user matrix size"));

    graphT g(number);

    for (std::size_t r = 0, r_end = matrix.rows(); r != r_end; ++r) {
        for (std::size_t c = 0, c_end = matrix.columns(); c != c_end; ++c) {
            if (matrix(c, r)) {
                auto vs = boost::vertices(g);
                graphT::vertex_iterator from = vs.first + r;
                graphT::vertex_iterator to = vs.first + c;

                boost::add_edge(*from, *to, g);
            }
        }
    }

    m_metrics = init_metrics(g);
    build(m_params, executive, g);
}

void
graph_generator::make_smallworld(vle::devs::Executive& executive,
                                 std::mt19937& gen,
                                 int number,
                                 int k,
                                 double probability,
                                 bool allow_self_loops)
{
    if (number <= 0)
        throw vle::utils::ArgError(_("graph_generator: bad model number"));

    using graphgenT = boost::small_world_iterator<std::mt19937, graphT>;

    graphT g(graphgenT(gen, number, k, probability, allow_self_loops),
             graphgenT(),
             number);

    m_metrics = init_metrics(g);
    build(m_params, executive, g);
}

void
graph_generator::make_scalefree(vle::devs::Executive& executive,
                                std::mt19937& gen,
                                int number,
                                double alpha,
                                double beta,
                                bool allow_self_loops)
{
    if (number <= 0)
        throw vle::utils::ArgError(_("graph_generator: bad model number"));

    using graphgenT = boost::plod_iterator<std::mt19937, graphT>;

    graphT g(graphgenT(gen, number, alpha, beta, allow_self_loops),
             graphgenT(),
             number);

    m_metrics = init_metrics(g);
    build(m_params, executive, g);
}

void
graph_generator::make_sorted_erdos_renyi(vle::devs::Executive& executive,
                                         std::mt19937& gen,
                                         int number,
                                         double probability,
                                         bool allow_self_loops)
{
    if (number <= 0)
        throw vle::utils::ArgError(_("graph_generator: bad model number"));

    using graphgenT = boost::sorted_erdos_renyi_iterator<std::mt19937, graphT>;

    graphT g(graphgenT(gen, number, probability, allow_self_loops),
             graphgenT(),
             number);

    m_metrics = init_metrics(g);
    build(m_params, executive, g);
}

void
graph_generator::make_erdos_renyi(vle::devs::Executive& executive,
                                  std::mt19937& gen,
                                  int number,
                                  double fraction,
                                  bool allow_self_loops)
{
    if (number <= 0)
        throw vle::utils::ArgError(_("graph_generator: bad model number"));

    using graphgenT = boost::erdos_renyi_iterator<std::mt19937, graphT>;

    graphT g(
      graphgenT(gen, number, fraction, allow_self_loops), graphgenT(), number);

    m_metrics = init_metrics(g);
    build(m_params, executive, g);
}

void
graph_generator::make_erdos_renyi(vle::devs::Executive& executive,
                                  std::mt19937& gen,
                                  int number,
                                  int edges_number,
                                  bool allow_self_loops)
{
    if (number <= 0)
        throw vle::utils::ArgError(_("graph_generator: bad model number"));

    using graphgenT = boost::erdos_renyi_iterator<std::mt19937, graphT>;

    graphT g(
      graphgenT(gen,
                number,
                utils::numeric_cast<graphT::edges_size_type>(edges_number),
                allow_self_loops),
      graphgenT(),
      number);

    m_metrics = init_metrics(g);
    build(m_params, executive, g);
}
}
} // namespace vle translator
