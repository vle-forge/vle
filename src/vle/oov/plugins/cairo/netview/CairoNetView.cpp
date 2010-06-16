/**
 * @file vle/oov/plugins/cairo/netview/CairoNetView.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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


#include <vle/oov/plugins/cairo/netview/CairoNetView.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/value.hpp>

namespace vle { namespace oov { namespace plugin {

CairoNetView::CairoNetView(const std::string& location) :
    CairoPlugin(location),
    mPen(1),
    mWindowWidth(605),
    mWindowHeight(605),
    mTime(-1.0),
    mReceiveCell(0),
    mGraph(0)
{
}

CairoNetView::~CairoNetView()
{
    delete mGraph;
}

void CairoNetView::onParameter(const std::string& /* plugin */,
                               const std::string& /* location */,
                               const std::string& /* file */,
                               value::Value* parameters,
                               const double& /* time */)
{
    m_display_node_names = false;

    if (parameters) {

        if (not parameters->isMap()) {
            throw utils::ArgError(
                _("NetView: initialization failed, bad parameters"));
        }

        value::Map* init = dynamic_cast < value::Map* >(parameters);

        const value::Map& dimensions = init->getMap("dimensions");

        mMaxX = toDouble(dimensions.get("x"));
        mMaxY = toDouble(dimensions.get("y"));

        mExecutiveName = toString(init->get("executiveName"));

        const value::Map& nodes = init->getMap("nodes");

        std::string matrix = toString(init->get("adjacency_matrix"));

        std::string node_names;
        m_degree=0;
        if (toString(nodes.get("type")) == "set") {
            node_names = toString(nodes.get("names"));
        } else {
            node_names = toString(nodes.get("prefix"));
            if (nodes.exist("depth"))
                m_degree= toInteger(nodes.get("depth"));
        }
        // create the graph
        mGraph = new Graph(node_names, matrix);


        // set the positions
        const value::Map& positions = init->getMap("positions");
        if (toString(positions.get("type")) == "set") {
            mGraph->set_positions(toString(positions.get("values")));
        } else {
            mGraph->set_positions();
            mMaxX = 300; // arbitrary dimensions to scale the graph
            mMaxY = 300;
        }

        mGraph->scale_positions(mWindowWidth / mMaxX, mWindowHeight / mMaxY);

        const value::Map& states = init->getMap("states");

        if (states.exist("name")) {
            mStateName = toString(states.get("name"));
        }

        std::string type = toString(states.get("type"));
        const value::Set& values = states.getSet("values");

        mColors.build_color_list(type, values);

        if (init->exist("display_names")) {
            const value::Map& display_names = init->getMap("display_names");
            if (toString(display_names.get("activate")) == "yes") {
                m_display_node_names = true;
            }
        }
        //
        //// default color value for all vertices
        //
        std::vector<std::string> nodes_names = mGraph->get_all_vertice_names();
        for (unsigned int i = 0; i < nodes_names.size(); ++i){
            mValues[nodes_names[i]] = "0";
        }
    }

    delete parameters;
}

void CairoNetView::onNewObservable(const std::string& /* simulator */,
                                   const std::string& /* parent */,
                                   const std::string& /* port */,
                                   const std::string& /* view */,
                                   const double& /* time */)
{
}

void CairoNetView::onDelObservable(const std::string& /* simulator */,
                                   const std::string& /* parent */,
                                   const std::string& /* port */,
                                   const std::string& /* view */,
                                   const double& /* time */)
{
}

void CairoNetView::onValue(const std::string& simulator,
                           const std::string&  parent ,
                           const std::string& /*port*/,
                           const std::string& /* view */,
                           const double& time,
                           value::Value* value)
{
    mTime = time;
    boost::char_separator<char> sep(" \n\t");
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    if (simulator == mExecutiveName) {
        value::Set set(value::toSetValue(*value));
        mValues[simulator] = set.get(0)->writeToString();
        mReceiveCell++;

        if (set.getString(1) == "add") {
            tokenizer vertices_tok(set.getString(2), sep);
            tokenizer values_tok(set.getString(3), sep);
            tokenizer::iterator val_it = values_tok.begin();
            for (tokenizer::iterator v_it = vertices_tok.begin();
                 v_it != vertices_tok.end(); ++v_it) {
                mGraph->add_node(*v_it);
                mValues[*v_it] = *val_it;
                ++val_it;
            }

            tokenizer edges_tok(set.getString(4), sep);
            tokenizer::iterator it = edges_tok.begin();
            while (it != edges_tok.end()) {
                tokenizer::iterator it_origin = it;
                ++it;
                mGraph->push_back_edge(*it_origin, *it);
                ++it;
            }

        } else if (set.getString(1) == "delete") {
            tokenizer vertices_tok(set.getString(2), sep);

            for (tokenizer::iterator it = vertices_tok.begin();
                 it != vertices_tok.end(); ++it) {
                mGraph->remove_node(*it);
                mValues.erase(*it);
            }
        }

        mGraph->set_positions();
        mMaxX = 300;
        mMaxY = 300;
        mGraph->scale_positions(mWindowWidth / mMaxX, mWindowHeight / mMaxY);
        draw();
        copy();
    } else { // receive a node value
        if (m_degree == 0)
            mValues[simulator] = value->writeToString();
        else {
            std::string par=parent;
            std::reverse(par.begin(),par.end());
            boost::char_separator<char> sep(",");
            tokenizer tokens(par, sep);
            tokenizer::iterator tok_iter = tokens.begin();
            int i=0;

            while (i < m_degree-1) {
                ++tok_iter;
                ++i;
            }

            std::string nodename= *tok_iter;
            std::reverse(nodename.begin(),nodename.end());
            mValues[nodename] = value->writeToString();
        }
        ++mReceiveCell;
        if (mReceiveCell == mGraph->num_nodes()) {
            draw();
            copy();
            mReceiveCell = 0;
        }
    }
    delete value;
}

void CairoNetView::close(const double& /* time */)
{
    delete mGraph;
    mGraph = 0;
}

void CairoNetView::preferredSize(int& width, int& height)
{
    width = mWindowWidth;
    height = mWindowHeight;
}


void CairoNetView::onSize(int width, int height)
{
    mMaxX = mWindowWidth;
    mMaxY = mWindowHeight;
    mWindowWidth = width;
    mWindowHeight = height;
    mGraph->scale_positions(mWindowWidth / mMaxX, mWindowHeight / mMaxY);
}



void CairoNetView::draw()
{
    m_ctx->rectangle(0, 0, mWindowWidth, mWindowHeight);
    m_ctx->set_source_rgb(0.74, 0.74, 0.74);
    m_ctx->fill();

    int rayon = mWindowWidth / (3 * mGraph->num_nodes());
    if (rayon < 2) rayon = 2;

    double xx, yy;
    Graph::p_vertex_it pv_it;
    for (pv_it = mGraph->get_first_last_vertice_iterators();
         pv_it.first != pv_it.second; ++pv_it.first) {
        Graph::p_adjacency_it pa_it;
        for (pa_it = mGraph->get_first_last_adjacent_iterators(pv_it.first);
             pa_it.first != pa_it.second; ++pa_it.first){
            double Ax, Ay, Bx, By;
            mGraph->get_vertex_position(pv_it.first, Ax, Ay);
            mGraph->get_vertex_position(pa_it.first, Bx, By);
            draw_arrow(static_cast<int>(Ax), static_cast<int>(Ay),
                       static_cast<int>(Bx), static_cast<int>(By),
                       rayon);
        }
        std::string node_name = mGraph->get_vertex_name(pv_it.first);
        mColors.build_color(mValues[node_name]);
        m_ctx->set_source_rgb(mColors.r, mColors.g, mColors.b);
        mGraph->get_vertex_position(pv_it.first, xx ,yy);
        int x = static_cast<int>(xx);
        int y = static_cast<int>(yy);
        m_ctx->arc(x, y, rayon, 0, 2*M_PI);
        m_ctx->fill();
        if (m_display_node_names)
            draw_node_name(node_name, x, y, 2.*rayon/3.);
    }
}

void CairoNetView::draw_arrow(const int start_x, const int start_y,
                              int end_x, int end_y, int rayon)
{

    double cos_teta = (start_x - end_x) /
        get_length(start_x, start_y, end_x, end_y);
    double sin_teta = (start_y - end_y) /
        get_length(start_x, start_y, end_x, end_y);
    double teta = acos(cos_teta);

    end_x += (int)(cos_teta * rayon);
    end_y += (int)(sin_teta * rayon);

    m_ctx->set_source_rgb(0.4, 0.0, 0.0);
    m_ctx->set_line_width(0.8);
    m_ctx->begin_new_path();
    m_ctx->move_to (start_x, start_y);
    m_ctx->line_to (end_x, end_y);
    m_ctx->close_path();
    m_ctx->stroke();

    double teta0 = teta + M_PI / 6.;
    double teta1 = teta - M_PI / 6.;

    double x;
    double y;

    x = end_x + rayon * cos(teta0);
    if (end_y < start_y) {
        y = end_y + rayon * sin(teta0);
    } else {
        y = end_y - rayon * sin(teta0);
    }
    m_ctx->line_to(static_cast<int>(x), static_cast<int>(y));

    x = end_x + (rayon*0.7) * cos_teta;
    y = end_y + (rayon*0.7) * sin_teta;

    m_ctx->line_to(static_cast<int>(x), static_cast<int>(y));
    x = end_x + rayon * cos(teta1);
    if (end_y < start_y) {
        y = end_y + rayon * sin(teta1);
    } else {
        y = end_y - rayon * sin(teta1);
    }
    m_ctx->line_to(static_cast<int>(x), static_cast<int>(y));

    m_ctx->line_to (end_x, end_y);
    m_ctx->fill();
}

double CairoNetView::get_length(const int Ax, const int Ay,
                                const int Bx, const int By)
{
    return sqrt((Bx-Ax)*(Bx-Ax) + (By-Ay)*(By-Ay));
}


void CairoNetView::draw_node_name(const std::string &node_name,
                                  double x, double y, double font_size)
{
    m_ctx->set_source_rgb(0.0, 0.0, 0.0);
    m_ctx->select_font_face("Sans", Cairo::FONT_SLANT_NORMAL,
                            Cairo::FONT_WEIGHT_BOLD);

    m_ctx->set_font_size(font_size);

    Cairo::TextExtents extents;
    m_ctx->get_text_extents(node_name, extents);
    x -= extents.width/2 + extents.x_bearing;
    y -= extents.height/2 + extents.y_bearing;

    m_ctx->move_to(x, y);
    m_ctx->show_text(node_name);
}

}}} // namespace vle oov plugin

