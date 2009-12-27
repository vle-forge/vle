/**
 * @file vle/oov/plugins/cairo/netview/Graph.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
 * Copyright (C) 2003-2009 ULCO http://www.univ-littoral.fr
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


#include <vle/oov/plugins/cairo/netview/Graph.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace oov { namespace plugin {

Graph::Graph(const std::string &vertice_names,
             const std::string &matrix)
{
    boost::char_separator<char> sep(" \n\t");
    tokenizer tokens_links(matrix, sep);
    tokenizer tokens_names(vertice_names, sep);

    // create all the vertices with their name
    unsigned int nb_vertices = 0;
    for (tokenizer::iterator tok_iter = tokens_names.begin();
        tok_iter != tokens_names.end(); ++tok_iter) {
        v_descriptor vd = add_vertex(m_G);
        m_G[vd].name = *tok_iter;
        ++nb_vertices;
    }


    if(nb_vertices > 1) { // all vertices names are define
        p_vertex_it pv_it_src = vertices(m_G);
        vertex_it pv_it_dest  = pv_it_src.first;
        vertex_it pv_it_begin = pv_it_src.first;

        unsigned int i = 0;
        unsigned int modulo;
        for (tokenizer::iterator tok_iter = tokens_links.begin();
            tok_iter != tokens_links.end(); ++tok_iter) {
            modulo = i % nb_vertices;
            if(modulo == 0 and i > 0) {
                ++pv_it_src.first;
                pv_it_dest = pv_it_begin;
            }
            if(*tok_iter != "0") {
                add_edge(*pv_it_src.first, *pv_it_dest, m_G);
            }
            ++pv_it_dest;
            ++i;
        }
    }
    else { // construction of vertices names
        Graph::p_vertex_it pv_it = vertices(m_G);
        std::string prefix = m_G[*pv_it.first].name;
        remove_vertex(*pv_it.first, m_G);
        nb_vertices = 0;

        unsigned int i = 0;

        for (tokenizer::iterator tok_iter = tokens_links.begin();
            tok_iter != tokens_links.end(); ++tok_iter) {
            ++i;
        }
        nb_vertices = (unsigned int) sqrt(i);

        for (i = 0; i <nb_vertices; ++i) {
            v_descriptor vd = add_vertex(m_G);
            m_G[vd].name = (fmt("%1%%2%") % prefix % i).str();
        }

        p_vertex_it pv_it_src = vertices(m_G);
        vertex_it pv_it_dest  = pv_it_src.first;
        vertex_it pv_it_begin = pv_it_src.first;


        i = 0;
        unsigned int modulo;
        for (tokenizer::iterator tok_iter = tokens_links.begin();
            tok_iter != tokens_links.end(); ++tok_iter) {
            modulo = i % nb_vertices;
            if(modulo == 0 and i > 0) {
                ++pv_it_src.first;
                pv_it_dest = pv_it_begin;
            }
            if(*tok_iter != "0") {
                add_edge(*pv_it_src.first, *pv_it_dest, m_G);
            }
            ++pv_it_dest;
            ++i;
        }
    }
}

void
Graph::push_back_edge(const std::string &source,
                                  const std::string &out_edge)
{
    Graph::v_descriptor v1;
    Graph::v_descriptor v2;

    Graph::p_vertex_it pv_it = vertices(m_G);
    while (pv_it.first != pv_it.second) {
        if(m_G[*pv_it.first].name == source){
            v1 = *pv_it.first;
        }
        else if (m_G[*pv_it.first].name == out_edge){
            v2 = *pv_it.first;
        }
        ++pv_it.first;
    }
    add_edge(v1,v2,m_G);
}

std::vector<std::string> Graph::get_all_vertice_names()
{
    std::vector<std::string> sv;
    for (Graph::p_vertex_it pv_it = vertices(m_G);
        pv_it.first != pv_it.second; ++pv_it.first)
        sv.push_back(m_G[*pv_it.first].name);
    return sv;
}

void Graph::set_positions()
{
    boost::square_topology<boost::mt19937> m_topology(m_rand.gen(), 300);
    boost::associative_property_map<IndexMap> map(m_index_map);

    int nb_iter = num_vertices(m_G);
    if (nb_iter < 200)
        nb_iter=200;
    gursoy_atun_layout(m_G, m_topology, map, nb_iter);

    for (Graph::p_vertex_it pv_it = vertices(m_G);
         pv_it.first != pv_it.second; ++pv_it.first) {
        m_G[*pv_it.first].position.first  = m_index_map[*pv_it.first][0];
        m_G[*pv_it.first].position.second = m_index_map[*pv_it.first][1];
    }
}

void Graph::set_positions(const std::string &p)
{
    boost::char_separator<char> sep(" \n\t");
    tokenizer tokens(p, sep);

    Graph::p_vertex_it vp = vertices(m_G);

    tokenizer::iterator tok_iter = tokens.begin();
    while (tok_iter != tokens.end()) {

        if (vp.first == vp.second) {
            throw utils::InternalError(
                "The number of vertice does not match the number of names!");
        }

        double x = utils::toDouble(*tok_iter);
        ++tok_iter;

        if (tok_iter == tokens.end()) {
            throw utils::InternalError(
                "netview plugin error: The number of position must be pair!");
        }

        double y = utils::toDouble(*tok_iter);
        m_G[*vp.first].position.first  = x;
        m_G[*vp.first].position.second = y;

        ++tok_iter;
        ++vp.first;
    }
}


Graph::v_descriptor Graph::get_vertex_descriptor(const std::string &vertex)
{
    Graph::v_descriptor vd;

    Graph::p_vertex_it pv_it = vertices(m_G);

    while (pv_it.first != pv_it.second) {
        vd = *pv_it.first;
        if(m_G[vd].name == vertex)
            break;
        pv_it.first++;
    }

    return vd;
}

std::vector<std::string> Graph::get_adjacent_vertices(const std::string &v)
{
    Graph::v_descriptor vd = get_vertex_descriptor(v);

    Graph::p_adjacency_it p = adjacent_vertices(vd, m_G);

    std::vector<std::string> vertices;

    for (Graph::adjacency_it it = p.first; it != p.second; ++it)
        vertices.push_back(m_G[*it].name);

    return vertices;
}

}}} // vle::oov::plugin
