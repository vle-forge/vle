/** 
 * @file Plot.cpp
 * @brief 
 * @author The vle Development Team
 * @date ven, 27 jan 2006 16:09:49 +0100
 */

/*
 * Copyright (C) 2006-2007 - The vle Development Team
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

#include <vle/oov/plugins/cairo/plot/Plot.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/XML.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>

namespace vle { namespace oov { namespace plugin {

//#define SHIFT 16 

Plot::Plot(const std::string& location):
    CairoPlugin(location),
    m_finish(false),
    m_time(-1.0), 
    m_receive(0),
    mWidth(600),
    mHeight(300)
{ 
    m_parameter.set_pango_height(20);
}

void Plot::onNewObservable(const vpz::NewObservableTrame& trame)
{
    std::string name(buildname(trame.name(),trame.port()));

    Assert(utils::InternalError,m_columns2.find(name) == m_columns2.end(), 
           boost::format("Observable %1% already exist") % name); 

    m_columns.push_back(name);
    m_columns2[name] = m_buffer.size();
    m_buffer.push_back(value::Value());
}

void Plot::onDelObservable(const vpz::DelObservableTrame& /* trame */)
{
}

void Plot::onValue(const vpz::ValueTrame& trame)
{
    double min_date_curve;
    int nb_check_min_max = 0;
    bool calcul_new_min_max_no_scroll = false;

    m_time = utils::to_double(trame.time());

    for (vpz::ModelTrameList::const_iterator it = trame.trames().begin();
         it != trame.trames().end(); ++it) {
        std::string name(buildname(it->simulator(),it->port()));
        std::map < std::string,int >::iterator jt;
        jt = m_columns2.find(name);

        Assert(utils::InternalError,jt != m_columns2.end(),boost::format(
                "The columns %1% does not exist. No new Observable ?") % 
            name);

        m_buffer[jt->second] = it->value();

	if (it->value()->isDouble()) {
	    (*m_it_dble)->add(m_time, toDouble(it->value()));
	    ++m_it_dble;
	}
	else if (it->value()->isInteger()) {
	    (*m_it_int)->add(m_time, toInteger(it->value()));
	    ++m_it_int;
	}
        m_receive++;
    }
    
    if (m_receive == m_columns.size()) {
	m_receive = 0;
	m_parameter.inc_max_draw_index();

	m_it_dble = m_data_dble.begin();
	m_it_int = m_data_int.begin();

	{
	    int v_int;
	    double v_dble;
	    std::list < IntCurve * > :: const_iterator it_int_show;
	    for (it_int_show = m_data_int_show.begin(); it_int_show !=
		     m_data_int_show.end(); ++it_int_show) {
		v_int =
		    (*it_int_show)->get_value(m_parameter.get_max_draw_index()-1);
		if (m_parameter.is_smaller(v_int))
		    m_parameter.set_min_value(v_int);
		if (m_parameter.is_higher(v_int))
		    m_parameter.set_max_value(v_int);		
	    }

	    std::list < RealCurve * > :: const_iterator it_dble_show;
	    for (it_dble_show = m_data_dble_show.begin();
		 it_dble_show != m_data_dble_show.end(); ++it_dble_show) {
		v_dble =
		    (*it_dble_show)->get_value(m_parameter.get_max_draw_index()-1);
		if ( m_parameter.is_smaller(v_dble))
		    m_parameter.set_min_value(v_dble);
		if ( m_parameter.is_higher(v_dble))
		    m_parameter.set_max_value(v_dble);		
	    }
	}
	
	//recalcul la valeur de la 1ere date affichée 
	if ((m_time - m_parameter.get_min_draw_date()) >=
	    m_parameter.get_number_drawn_date()) {
	    if (m_parameter.get_scrolling())
		m_parameter.set_min_draw_date(
		    m_time - m_parameter.get_number_drawn_date());
	    else {
		m_parameter.set_min_draw_date(m_time);
		calcul_new_min_max_no_scroll = true;
	    }
	}
	
	m_parameter.set_max_draw_date(m_time);
	
	//recalcul l'index de la 1ere valeur affichée
	double min_draw_date;
	int min_index = m_parameter.get_min_draw_index();
	
	if (!m_data_dble.empty()) {
	    std::vector < RealCurve * >::iterator it_dble = m_data_dble.begin();
	    min_draw_date = m_parameter.get_min_draw_date();
	    while( (*it_dble)->get_date(min_index) < min_draw_date) {
		min_index++; 
		nb_check_min_max++;
	    }
	    min_date_curve = (*it_dble)->get_date(min_index);
	} else if (!m_data_int.empty()) {
	    std::vector < IntCurve * > :: iterator it_int = m_data_int.begin();
	    min_draw_date = m_parameter.get_min_draw_date();
	    while( (*it_int)->get_date(min_index) < min_draw_date) {
		min_index++;
		nb_check_min_max++;	
	    }
	    min_date_curve = (*it_int)->get_date(min_index);
	}
	m_parameter.set_min_draw_index(min_index);
	
	// verifie si l'on ne doit pas mettre a jour les valeur max et min
	// verifie que les n valeur passé (a cause du scroll) n'etait pas soit 
	// la plus haute soit la moins haute
	if (calcul_new_min_max_no_scroll) {
	    // comme on efface tout on recalcul le min & max
	    // par rapport a la derniere valeur de chaque courbe
	    double m_min, m_max;
	    m_min = get_min_value_n(nb_check_min_max);
	    m_max = get_max_value_n(nb_check_min_max);
	    
	    m_parameter.set_min_value(m_min);
	    m_parameter.set_max_value(m_max);
	} else {
	    // on verifie que les n valeur passé a cause du scroll
	    // n'etaient pas les valeurs min et/ou max
	    double m_min, m_max;
	    
	    m_min = get_min_value_n(nb_check_min_max);
	    m_max = get_max_value_n(nb_check_min_max);
	    
	    // on doit mettre a jour la valeur minimal
	    if (m_min == m_parameter.get_min_back_value()) {
		update_min_value_drawn();
	    }
	    // on doit mettre a jour la valeur maximal
	    if (m_max == m_parameter.get_max_back_value()) {
		update_max_value_drawn();
	    }
	}
	draw();
    }
}

void Plot::onParameter(const vpz::ParameterTrame& trame)
{
    if (not trame.data().empty()) {
        xmlpp::DomParser parser;

        parser.parse_memory(trame.data());
	xmlpp::Element* root = utils::xml::get_root_node(parser, "parameters");  
	IntCurve *ic;
	RealCurve *rc;

	if (utils::xml::exist_children(root,"curves")) {   
	    xmlpp::Element* elt = utils::xml::get_children(root,"curves");
	    xmlpp::Node::NodeList lst = elt->get_children("curve");
	    xmlpp::Node::NodeList::iterator it = lst.begin();

	    while (it != lst.end()) {
		xmlpp::Element * elt2 = ( xmlpp::Element* )( *it );
		std::string v_type = utils::xml::get_attribute(elt2,"type");

		if (v_type == "real") {
		    rc = new RealCurve(utils::xml::get_attribute(elt2,"name"), 
				       utils::to_int(utils::xml::get_attribute(elt2,"color_red")),
				       utils::to_int(utils::xml::get_attribute(elt2,"color_green")),
				       utils::to_int(utils::xml::get_attribute(elt2,"color_blue")));
		    m_data_dble.push_back(rc);
		    m_data_dble_show.push_back(rc);
		} else if (v_type == "integer") {
		    ic = new IntCurve(utils::xml::get_attribute(elt2,"name"),
				      utils::to_int(utils::xml::get_attribute(elt2,"color_red")),
				      utils::to_int(utils::xml::get_attribute(elt2,"color_green")),
				      utils::to_int(utils::xml::get_attribute(elt2,"color_blue")));
		    m_data_int.push_back(ic);
		    m_data_int_show.push_back(ic);
		}
		++it;
	    }
	}
	
	if (utils::xml::exist_children(root, "date")) {
	    xmlpp::Element* elt = utils::xml::get_children(root, "date");
	    m_parameter.set_min_draw_date(0.0f);
	    m_parameter.set_max_draw_date(utils::to_double(utils::xml::get_attribute(elt, "max")));
	    m_parameter.set_number_drawn_date((int)m_parameter.get_max_draw_date());
	}
	
	if (utils::xml::exist_children(root, "scrolling")) {
	    xmlpp::Element* elt = utils::xml::get_children(root,"scrolling");
	    m_parameter.set_scrolling(utils::to_boolean(utils::xml::get_attribute(elt,"value")));
	}
	
	if (utils::xml::exist_children(root, "limits")) {
	    xmlpp::Element* elt = utils::xml::get_children(root, "limits");
	    xmlpp::Node::NodeList lst = elt->get_children("limit");
	    xmlpp::Node::NodeList::iterator it = lst.begin();
	    
	    double valueLimit;
	    while (it != lst.end()) {
		xmlpp::Element * elt2 = ( xmlpp::Element* )( *it );

		valueLimit = utils::to_double(utils::xml::get_attribute(elt2,"value"));
		m_list_limit.push_back(new Limit(valueLimit,
						 utils::to_int(utils::xml::get_attribute(elt2,"color_red")),
						 utils::to_int(utils::xml::get_attribute(elt2,"color_green")),
						 utils::to_int(utils::xml::get_attribute(elt2,"color_blue"))));
		++it;
	    }
	}	
    }
    if (not context()) {
        m_img = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, mWidth,
                                            mHeight);
        setSurface(m_img);
    }
    m_parameter.set_drawing_area_size(mWidth, mHeight);
    m_parameter.set_screen_size(mWidth, mHeight);

    m_it_dble = m_data_dble.begin();
    m_it_int = m_data_int.begin();

//    m_parameter.init();
}
    
void Plot::close(const vpz::EndTrame& /*trame */)
{
    m_parameter.set_number_drawn_date((int)ceil(m_parameter.get_max_draw_date()));
    m_parameter.set_min_draw_index(0);
    m_parameter.set_min_draw_date(0.0f);

    // on calcul le max & le min sur l'ensemble de la simulation
    update_max_value_drawn();
    update_min_value_drawn();
    update_curve = true;
    m_finish = true;
}

void Plot::preferredSize(int& width, int& height)
{
    width = mWidth;
    height = mHeight;
}

void Plot::draw()
{
    Assert(utils::InternalError, context(), "Cairo plot drawing error");
    Cairo::RefPtr < Cairo::Context > ctx = context();

    if (ctx) {
	if ((m_data_dble.size() > 0 && m_data_dble[0]->get_size() > 1) ||
	    (m_data_int.size() > 0 && m_data_int[0]->get_size() > 0)) {
	    
	    ctx->rectangle(0, 0, mWidth, mHeight);
	    ctx->set_source_rgb(1, 1, 1);
	    ctx->fill();	
	    ctx->set_line_width(1);
	    draw_vertical_step(ctx);
	    draw_horizontal_step(ctx);
	    draw_axis(ctx);
	    draw_curves(ctx);
	    draw_limits(ctx);
	}
    }
}

void Plot::draw_vertical_step(Cairo::RefPtr < Cairo::Context > ctx)
{
    double m_stepX, date, m_firstDate, m_lastDate, rest;
    int v_x, m_maxY, m_minY;

    m_stepX = m_parameter.get_number_drawn_date() / 4.0;
    m_firstDate = m_parameter.get_min_draw_date();
    m_lastDate = m_parameter.get_max_draw_date();
    m_maxY = m_parameter.get_shift_top();
    m_minY = m_maxY + m_parameter.get_graph_zone_height();
    rest = m_firstDate / m_stepX;

    if (rest == 0)
        date = m_firstDate;
    else
        date = ceil(rest) * m_stepX;

    while(date <= m_lastDate) {
        v_x = m_parameter.to_pixel_width(date);

	ctx->set_source_rgb(0.745098, 0.745098, 0.745098);
	ctx->begin_new_path();
	ctx->move_to(v_x, m_minY);
	ctx->line_to(v_x, m_maxY);
	ctx->close_path();
	ctx->stroke();

	ctx->set_source_rgb(0., 0., 0.);
	ctx->move_to(v_x +1, m_minY);
	ctx->show_text(utils::to_string(date));

        date += m_stepX;
    }
}

void Plot::draw_horizontal_step(Cairo::RefPtr < Cairo::Context > ctx)
{
    int x_left, x_right, y_bottom;
    int pango_height = m_parameter.get_pango_height();
    int semi_pango_height = pango_height / 2;
    int y;
    double value_show = m_parameter.get_min_value();
    double max_value_show = m_parameter.get_max_value();
    double step_value = m_parameter.get_pango_height_value();
    
    x_right = m_parameter.get_shift_left() + m_parameter.get_graph_zone_width();
    y_bottom = m_parameter.get_graph_zone_height() + m_parameter.get_shift_top();
    y = y_bottom;

    while(value_show <= max_value_show) {
	ctx->set_source_rgb(0., 0., 0.);
	ctx->move_to(0, y - semi_pango_height);
	ctx->show_text(utils::to_string(value_show));

	ctx->set_source_rgb(0.745098, 0.745098, 0.745098);
	ctx->begin_new_path();
	ctx->move_to(x_left, y);
	ctx->line_to(x_right, y);
	ctx->close_path();
	ctx->stroke();

        y -= pango_height;
        value_show += std::abs(step_value);
    }
}

void Plot::draw_curves(Cairo::RefPtr < Cairo::Context > ctx)
{
    std::list < RealCurve * > :: const_iterator it_dble;
    for (it_dble = m_data_dble_show.begin(); it_dble != m_data_dble_show.end();
         ++it_dble)
        (*it_dble)->draw(ctx, m_parameter);

    if(!m_data_int_show.empty())
        (*(m_data_int_show.begin()))->draw(ctx, m_parameter, m_data_int_show);
}

void Plot::draw_axis(Cairo::RefPtr < Cairo::Context > ctx)
{
    int x_left, x_right, y_top, y_bottom;

    x_left = m_parameter.get_shift_left();
    x_right = m_parameter.get_shift_left() + m_parameter.get_graph_zone_width();
    y_top = m_parameter.get_shift_top();
    y_bottom = m_parameter.get_graph_zone_height() + m_parameter.get_shift_top();

    ctx->begin_new_path();
    ctx->move_to(x_left, y_top);
    ctx->line_to(x_right, y_top);
    ctx->line_to(x_right, y_bottom);
    ctx->line_to(x_left, y_bottom);
    ctx->line_to(x_left, y_top);
    ctx->close_path();
    ctx->stroke();
}

void Plot::draw_limits(Cairo::RefPtr < Cairo::Context > ctx)
{
    std::list < Limit * > :: const_iterator it;
    int y_limit;
    int x_left, x_right;
    int semi_pango_height = m_parameter.get_pango_height() / 2;

    x_left = m_parameter.get_shift_left();
    x_right = m_parameter.get_shift_left() + m_parameter.get_graph_zone_width();	

    for (it = m_list_limit.begin(); it != m_list_limit.end(); ++it) {
        if (m_parameter.is_inside_min_max((*it)->get_value())) {

//            m_pango->set_markup(utils::to_string((*it)->get_value()));
            y_limit = m_parameter.to_pixel_height((*it)->get_value());
//            m_buffer->draw_layout(m_gc, 0, y_limit - semi_pango_height, m_pango);
	    ctx->set_source_rgb(0., 0., 0.);
	    ctx->move_to(0, y_limit - semi_pango_height);
	    ctx->show_text(utils::to_string((*it)->get_value()));

	    ctx->set_source_rgb((*it)->get_red_color()/65535., 
				(*it)->get_green_color()/65535., 
				(*it)->get_blue_color()/65535.);

//            m_buffer->draw_line(m_gc, x_left, y_limit, x_right, y_limit);	
	    ctx->begin_new_path();
	    ctx->move_to(x_left, y_limit);
	    ctx->line_to(x_right, y_limit);
	    ctx->close_path();
	    ctx->stroke();
        }
    }
}

/**************************************************************/
/* calcul la valeur minimal affiché de l'ensemble des courbes */
/**************************************************************/
void Plot::update_min_value_drawn()
{
    std::vector < int > v_int;
    std::vector < double > v_dble;
    std::vector < int > :: const_iterator it_int_min;
    std::vector < double > :: const_iterator it_dble_min;
    std::list < IntCurve * > :: const_iterator it_int_curve;
    std::list < RealCurve * > :: const_iterator it_dble_curve;
    int number_value = m_parameter.get_max_draw_index() - m_parameter.get_min_draw_index();
    double m_min = 0.0f; // par defaut, si jamais on n'affiche pas de courbe
    bool dble_empty, int_empty;

    for(it_dble_curve = m_data_dble_show.begin(); it_dble_curve != m_data_dble_show.end(); ++it_dble_curve) {
        v_dble.push_back((*it_dble_curve)->get_min_value(number_value, m_parameter));
    }

    for(it_int_curve = m_data_int_show.begin(); it_int_curve != m_data_int_show.end(); ++it_int_curve) {
        v_int.push_back((*it_int_curve)->get_min_value(number_value, m_parameter));
    }

    int_empty = v_int.empty();
    dble_empty = v_dble.empty();
    if (!dble_empty & !int_empty) {
        it_dble_min = min_element(v_dble.begin(), v_dble.end());
        it_int_min = min_element(v_int.begin(), v_int.end());
        m_min = ( *it_dble_min < *it_int_min) ? *it_dble_min : *it_int_min;
    } else if (!dble_empty) {
        it_dble_min = min_element(v_dble.begin(), v_dble.end());
        m_min = *it_dble_min;
    } else if (!int_empty) {
        it_int_min = min_element(v_int.begin(), v_int.end());
        m_min = (double)(*it_int_min);
    }
    m_parameter.set_min_value(m_min);
}

/**************************************************************/
/* calcul la valeur maximal affiché de l'ensemble des courbes */
/**************************************************************/
void Plot::update_max_value_drawn()
{
    std::vector < int > v_int;
    std::vector < double > v_dble;
    std::vector < int > :: const_iterator it_int_max;
    std::vector < double > :: const_iterator it_dble_max;
    std::list < IntCurve * > :: const_iterator it_int_curve;
    std::list < RealCurve * > :: const_iterator it_dble_curve;
    int number_value = m_parameter.get_max_draw_index() - m_parameter.get_min_draw_index();
    double m_max = 1.0f; // par defaut, si jamais on n'affiche pas de courbe
    bool dble_empty, int_empty;

    for(it_dble_curve = m_data_dble_show.begin(); it_dble_curve != m_data_dble_show.end(); ++it_dble_curve) {
        v_dble.push_back((*it_dble_curve)->get_max_value(number_value, m_parameter));
    }

    for(it_int_curve = m_data_int_show.begin(); it_int_curve != m_data_int_show.end(); ++it_int_curve) {
        v_int.push_back((*it_int_curve)->get_max_value(number_value, m_parameter));
    }

    int_empty = v_int.empty();
    dble_empty = v_dble.empty();
    if (!dble_empty & !int_empty) {
        it_dble_max = max_element(v_dble.begin(), v_dble.end());
        it_int_max = max_element(v_int.begin(), v_int.end());

        m_max = ( *it_dble_max > *it_int_max) ? *it_dble_max : *it_int_max;
    } else if (!dble_empty) {
        it_dble_max = max_element(v_dble.begin(), v_dble.end());
        m_max = *it_dble_max;
    } else if (!int_empty) {
        it_int_max = max_element(v_int.begin(), v_int.end());
        m_max = (double)(*it_int_max);
    }
    m_parameter.set_max_value(m_max);
}


/****************************************************************************/
/* calcul la valeur maximale des n derniere valeur de l'ensemble des courbe */
/****************************************************************************/
double Plot::get_max_value_n(int nb_check_value)
{
    std::vector < int > v_int_max;
    std::vector < double > v_dble_max;
    std::vector < int > :: const_iterator it_int_max;
    std::vector < double > :: const_iterator it_dble_max;
    bool dble_empty, int_empty;
    std::list < IntCurve * > :: const_iterator it_int;
    std::list < RealCurve * > :: const_iterator it_dble;
    double m_max = 1.0f; // par defaut, si jamais on n'affiche pas de courbe
    int index_min = m_parameter.get_min_draw_index();

    for(it_dble = m_data_dble_show.begin(); it_dble != m_data_dble_show.end(); ++it_dble) {
        v_dble_max.push_back((*it_dble)->get_max_value_prec_index(index_min, nb_check_value));
    }

    for(it_int = m_data_int_show.begin(); it_int != m_data_int_show.end(); ++it_int) {
        v_int_max.push_back((*it_int)->get_max_value_prec_index(index_min, nb_check_value));
    }

    dble_empty = v_dble_max.empty();
    int_empty = v_int_max.empty();

    if (!dble_empty && !int_empty) {
        it_dble_max = max_element(v_dble_max.begin(), v_dble_max.end());
        it_int_max = max_element(v_int_max.begin(), v_int_max.end());
        m_max = ( *it_dble_max > *it_int_max) ? *it_dble_max : *it_int_max;
    } else if (!dble_empty) {
        it_dble_max = max_element(v_dble_max.begin(), v_dble_max.end());
        m_max = *it_dble_max;
    } else if (!int_empty) {
        it_int_max = max_element(v_int_max.begin(), v_int_max.end());
        m_max = (double)(*it_int_max);
    }
    return m_max;
}

/****************************************************************************/
/* calcul la valeur minimale des n derniere valeur de l'ensemble des courbe */
/****************************************************************************/
double Plot::get_min_value_n(int nb_check_value)
{
    std::vector < int > v_int_min;
    std::vector < double > v_dble_min;
    std::vector < int > :: const_iterator it_int_min;
    std::vector < double > :: const_iterator it_dble_min;
    bool dble_empty, int_empty;
    std::list < IntCurve * > :: const_iterator it_int;
    std::list < RealCurve * > :: const_iterator it_dble;
    double m_min = -1.0f;
    int index_min = m_parameter.get_min_draw_index();

    for(it_dble = m_data_dble_show.begin(); it_dble != m_data_dble_show.end(); ++it_dble) {
        v_dble_min.push_back((*it_dble)->get_min_value_prec_index(index_min, nb_check_value));
    }

    for(it_int = m_data_int_show.begin(); it_int != m_data_int_show.end(); ++it_int) {
        v_int_min.push_back((*it_int)->get_min_value_prec_index(index_min, nb_check_value));
    }

    dble_empty = v_dble_min.empty();
    int_empty = v_int_min.empty();

    if (!dble_empty && !int_empty) {
        it_dble_min = min_element(v_dble_min.begin(), v_dble_min.end());
        it_int_min = min_element(v_int_min.begin(), v_int_min.end());
        m_min = ( *it_dble_min > *it_int_min) ? *it_dble_min : *it_int_min;
    } else if (!dble_empty) {
        it_dble_min = min_element(v_dble_min.begin(), v_dble_min.end());
        m_min = *it_dble_min;
    } else if (!int_empty) {
        it_int_min = min_element(v_int_min.begin(), v_int_min.end());
        m_min = (double)(*it_int_min);
    }
    return m_min;
}

// /*************************************************************/
// /* ouvre un boite de dialogue pour choisir le nom du fichier */
// /*************************************************************/
// void Plot::on_button_save()
// {
//     Gtk::FileChooserWidget filechooser(Gtk::FILE_CHOOSER_ACTION_SAVE);
//     Gtk::FileFilter filter;
//     filter.set_name("Plot save");
//     filter.add_pattern("*.plotsave");
//     filechooser.add_filter(filter);

//     Gtk::Dialog dialog("Save as...");
//     dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
//     dialog.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

//     Gtk::VBox* vbox = dialog.get_vbox();
//     vbox->pack_start(filechooser, true, true);

//     dialog.show_all();

//     if (dialog.run() == Gtk::RESPONSE_OK) {
//         Glib::ustring filename = filechooser.get_filename();
//         if (filename.rfind(".plotsave", filename.length()) !=
//             (filename.length() - 9))
//             filename += ".plotsave";
//         save_to_file(filename);
//     }	
// }


// /**********************************************************************/
// /* Sauvegarde les données des différentes courbes dans un fichier txt */
// /**********************************************************************/
// void Plot::save_to_file(Glib::ustring const & filename)
// {
//     std::ofstream m_file;
//     std::vector < RealCurve * >::iterator it_dble;
//     std::vector < IntCurve * > :: iterator it_int;
//     std::vector < Curve * > temp_save;

//     m_file.open(filename.raw().c_str(), std::ios::in | std::ios::trunc);
//     if(!m_file) {
//         cout << "Erreur lors de la creation du fichier" << endl;
//     } else {
//         m_file << "DATE";
//         for(it_dble = m_data_dble.begin(); it_dble != m_data_dble.end(); ++it_dble) {
//             m_file << "\t" << (*it_dble)->get_name();
//             temp_save.push_back(*it_dble);
//         }
//         for(it_int = m_data_int.begin(); it_int != m_data_int.end(); ++it_int) {
//             m_file << "\t" << (*it_int)->get_name();
//             temp_save.push_back(*it_int);
//         }

//         m_file << "\n";
//         // meme nombre de donne dans toute les courbes */
//         {
//             int vector_size = temp_save.size();
//             Glib::ustring temp;
//             if (vector_size > 0) {
//                 int nb_value = temp_save[0]->get_number_value();
//                 for(int i = 0; i < nb_value; i++) {
//                     m_file << temp_save[0]->get_date(i);
//                     temp = utils::to_string(temp_save[0]->get_date(i));
//                     for(int j = 0; j < vector_size; j++) {
//                         if(temp_save[j]->is_integer()) {
//                             m_file << "\t" << ((IntCurve * )temp_save[j])->get_value(i);
//                             temp += "\t" + to_string(((IntCurve * )temp_save[j])->get_value(i));
//                         } else if(temp_save[j]->is_double()) {
//                             m_file << "\t" << ((RealCurve * )temp_save[j])->get_value(i);
//                             temp += "\t" + to_string(((RealCurve * )temp_save[j])->get_value(i));
//                         }
//                     }
//                     m_file << "\n";
//                 }
//             }
//         }
//         m_file.close();
//     }
// }


// void Plot::finished()
// {
// }

// bool Plot::button_press_event(GdkEventButton* event)
// {
//     int x_begin, y_begin, x_end;
//     x_begin = (int)event->x;
//     y_begin = (int)event->y;

//     if (event->type == GDK_BUTTON_PRESS) {
//         if (event->button == 1) {
//             if (x_begin >= m_parameter.get_shift_left() 
//                 && x_begin <= (m_parameter.get_shift_left() + m_parameter.get_graph_zone_width())) {
//                 pt_box_1.x = x_begin;
//                 pt_box_2.x = x_begin;
//             }
//             if (y_begin >= m_parameter.get_shift_top() 
//                 && y_begin <= (m_parameter.get_shift_top() + m_parameter.get_graph_zone_height())) {
//                 pt_box_1.y = y_begin;
//                 pt_box_2.y = y_begin;
//             }
//             pt_box_press = m_show_box = true;
//             pt_box_release = false;
//             update_curve = false;
//         } else 	if (event->button == 2) {
//             if (pt_box_1.x != pt_box_2.x) {
//                 double min_date, max_date;
//                 double min_draw_date;
//                 double max_draw_date;
//                 int min_index = 0;
//                 int max_index;

//                 m_show_box = false;

//                 x_begin = (pt_box_1.x < pt_box_2.x) ? pt_box_1.x : pt_box_2.x;
//                 x_end = (pt_box_1.x > pt_box_2.x) ? pt_box_1.x : pt_box_2.x;

//                 min_date = m_parameter.pixel_to_date(x_begin);
//                 max_date = m_parameter.pixel_to_date(x_end);
//                 m_parameter.set_min_draw_date(min_date);
//                 m_parameter.set_max_draw_date(max_date);

//                 vector < RealCurve * >::iterator it_dble = m_data_dble.begin();
//                 vector < IntCurve * > :: iterator it_int = m_data_int.begin();

//                 // recalcul l'index de la 1ere valeur affichée
//                 if (!m_data_dble.empty()) {
//                     it_dble = m_data_dble.begin();
//                     min_draw_date = m_parameter.get_min_draw_date();
//                     while( (*it_dble)->get_date(min_index) < min_draw_date) {
//                         min_index++; 	
//                     }
//                 } else if (!m_data_int.empty()) {
//                     it_int = m_data_int.begin();
//                     min_draw_date = m_parameter.get_min_draw_date();
//                     while( (*it_int)->get_date(min_index) < min_draw_date) {
//                         min_index++;
//                     }
//                 }
//                 m_parameter.set_min_draw_index(min_index);

//                 // recalcul l'index de la derniere valeur affichée
//                 max_index = min_index;

//                 if (!m_data_dble.empty()) {
//                     it_dble = m_data_dble.begin();
//                     max_draw_date = m_parameter.get_max_draw_date();
//                     while( (*it_dble)->get_date(max_index) < max_draw_date) {
//                         max_index++; 
//                     }
//                 } else if (!m_data_int.empty()) {
//                     it_int = m_data_int.begin();
//                     max_draw_date = m_parameter.get_min_draw_date();
//                     while( (*it_int)->get_date(max_index) < max_draw_date) {
//                         max_index++;
//                     }
//                 }
//                 m_parameter.set_max_draw_index(max_index);
//                 m_parameter.set_number_drawn_date((int)(max_date - min_date));
//                 update_max_value_drawn();
//                 update_min_value_drawn();
//                 update_curve = true;
//             }			
//         } else if (event->button == 3) {
//             m_show_box = false;

//             double min_date, max_date;
//             int max_index, min_index = 0;

//             vector < RealCurve * >::iterator it_dble = m_data_dble.begin();
//             vector < IntCurve * > :: iterator it_int = m_data_int.begin();


//             if (!m_data_dble.empty()) {
//                 it_dble = m_data_dble.begin();
//                 min_date = 0.0f;
//                 max_index = (*it_dble)->get_size()-1;
//                 max_date = (*it_dble)->get_date(max_index);
//             } else if (!m_data_int.empty()) {
//                 it_int = m_data_int.begin();
//                 min_date = 0.0f;
//                 max_index = (*it_int)->get_size()-1;
//                 max_date = (*it_int)->get_date(max_index);
//             }
//             m_parameter.set_min_draw_date(min_date);
//             m_parameter.set_max_draw_date(max_date);
//             m_parameter.set_number_drawn_date((int)(max_date - min_date));
//             m_parameter.set_min_draw_index(min_index);
//             m_parameter.set_max_draw_index(max_index);
//             update_max_value_drawn();
//             update_min_value_drawn();
//             update_curve = true;
//         }
//         m_da.queue_draw();
//     }
//     return true;
// }

// bool Plot::button_release_event(GdkEventButton* event)
// {
//     if (event->type == GDK_BUTTON_RELEASE)
//         ;
//     pt_box_release = true;
//     update_curve = false;
//     return true;
// }

// bool Plot::motion_notify_event(GdkEventMotion* event)
// {
//     if (key_shift) {
//         //set_title(compose("Plot -- Date : %1\tValue : %2", m_parameter.pixel_to_date((int)(event->x)), m_parameter.pixel_to_value_height((int)(event->y))));
//         // cout << "x : " << event->x << "\ty : " << event->y << endl;
//     } else {
//         if (pt_box_press && !pt_box_release) {
//             int x = (int)event->x, y = (int)event->y;
//             if (x >= m_parameter.get_shift_left() && x <= (m_parameter.get_shift_left() + m_parameter.get_graph_zone_width()))
//                 pt_box_2.x = x;
//             else if (x < m_parameter.get_shift_left())
//                 x = m_parameter.get_shift_left();
//             else if (x > (m_parameter.get_shift_left() + m_parameter.get_graph_zone_width()))
//                 x = (m_parameter.get_shift_left() + m_parameter.get_graph_zone_width());

//             if (y >= m_parameter.get_shift_top() && y <= (m_parameter.get_shift_top() + m_parameter.get_graph_zone_height()))
//                 pt_box_2.y = y;
//             else if (y < m_parameter.get_shift_top())
//                 y = m_parameter.get_shift_top();
//             else if (y > (m_parameter.get_shift_top() + m_parameter.get_graph_zone_height()))
//                 y = (m_parameter.get_shift_top() + m_parameter.get_graph_zone_height());

//             update_curve = false;
//             queue_draw();
//         }
//     }


//     return true;
// }


// bool Plot::on_key_press_event (GdkEventKey* event)
// {
//     if (event->state == SHIFT)
//         key_shift = true;
//     else
//         key_shift = false;

//     if (event->keyval == 's')
//         screenshot();


//     if (event->keyval == 'v') {
//         m_parameter.printParameter();


//     }

//     cout << event->keyval << endl;
//     if (event->keyval == 32) // space
//         show_hide_treeview();
//     return true;
// }

// bool Plot::on_key_release_event (GdkEventKey* event)
// {
//     if (event->state)
//         ;
//     key_shift = false;
//     return true;
// }

// /********************************/
// /* affiche ou cache le treeview */
// /********************************/
// void Plot::show_hide_treeview()
// {
//     int width, height;

//     m_parameter.get_screen_size(width, height);
//     m_show_treeview = !m_show_treeview;
//     if (m_show_treeview)
//         m_hpaned.set_position(width - 200);
//     else
//         m_hpaned.set_position(width);
//     update_curve = true;
// }

}}} // namespace vle oov plugin
