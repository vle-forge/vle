/**
 * @file vle/oov/plugins/cairo/plot/Plot.cpp
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


#include <vle/oov/plugins/cairo/plot/Plot.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/XML.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/String.hpp>

namespace vle { namespace oov { namespace plugin {

Plot::Plot(const std::string& location):
    CairoPlugin(location),
    mFinish(false),
    mTime(-1.0),
    mReceive(0),
    mReceive2(0),
    mWidth(800),
    mHeight(400),
    mDashes(2)
{
    mDashes[0] = 2.0;
    mDashes[1] = 2.0;
}

void Plot::onNewObservable(const std::string& simulator,
                           const std::string& /* parent */,
                           const std::string& port,
                           const std::string& /* view */,
                           const double& /* time */)
{
    std::string name(buildname(simulator, port));

    Assert(utils::InternalError,mColumns2.find(name) == mColumns2.end(),
           boost::format("Plot: observable '%1%' already exist") % name);

    mColumns.push_back(name);
    mColumns2[name] = mReceive2;
    ++mReceive2;
}

void Plot::onDelObservable(const std::string& /* simulator */,
                           const std::string& /* parent */,
                           const std::string& /* port */,
                           const std::string& /* view */,
                           const double& /* time */)
{
}

void Plot::onValue(const std::string& simulator,
                   const std::string& /* parent */,
                   const std::string& port,
                   const std::string& /* view */,
                   const double& time,
                   value::Value* value)
{
    double min_date_curve;
    int nb_check_min_max = 0;
    bool calcul_new_min_max_no_scroll = false;

    mTime = time;

    std::string name(buildname(simulator, port));
    std::map < std::string,int >::iterator it;
    it = mColumns2.find(name);

    Assert(utils::InternalError,it != mColumns2.end(),boost::format(
            "Plot: columns '%1%' does not exist") % name);

    if (value->isDouble()) {
        (*m_it_dble)->add(mTime, value->toDouble().value());
        ++m_it_dble;
    } else if (value->isInteger()) {
        (*m_it_int)->add(mTime, value->toInteger().value());
        ++m_it_int;
    }
    delete value;
    mReceive++;

    if (mReceive == mColumns.size()) {
	mReceive = 0;
	mParameter.inc_max_draw_index();

	m_it_dble = mRealCurveList.begin();
	m_it_int = mIntCurveList.begin();

	{
	    int v_int;
	    std::list < IntCurve * >::const_iterator it_int_show;
	    for (it_int_show = mShowIntCurveList.begin();
		 it_int_show != mShowIntCurveList.end(); ++it_int_show) {
		v_int = (*it_int_show)->get_value(mParameter.get_max_draw_index()-1);
		if (mParameter.is_smaller(v_int))
		    mParameter.set_min_value(v_int);
		if (mParameter.is_higher(v_int))
		    mParameter.set_max_value(v_int);
	    }

	    double v_dble;
	    std::list < RealCurve * >::const_iterator it_dble_show;
	    for (it_dble_show = mShowRealCurveList.begin();
		 it_dble_show != mShowRealCurveList.end(); ++it_dble_show) {
		v_dble = (*it_dble_show)->get_value(mParameter.get_max_draw_index()-1);
		if (mParameter.is_smaller(v_dble))
		    mParameter.set_min_value(v_dble);
		if (mParameter.is_higher(v_dble))
		    mParameter.set_max_value(v_dble);
	    }
	}

	//recalcul la valeur de la 1ere date affichée
	if ((mTime - mParameter.get_min_draw_date()) >=
	    mParameter.get_number_drawn_date()) {
	    if (mParameter.get_scrolling())
		mParameter.set_min_draw_date(
		    mTime - mParameter.get_number_drawn_date());
	    else {
		mParameter.set_min_draw_date(mTime);
		calcul_new_min_max_no_scroll = true;
	    }
	}

	mParameter.set_max_draw_date(mTime);

	//recalcul l'index de la 1ere valeur affichée
	double min_draw_date;
	int min_index = mParameter.get_min_draw_index();

	if (!mRealCurveList.empty()) {
	    std::vector < RealCurve * >::iterator it_dble = mRealCurveList.begin();
	    min_draw_date = mParameter.get_min_draw_date();
	    while( (*it_dble)->get_date(min_index) < min_draw_date) {
		min_index++;
		nb_check_min_max++;
	    }
	    min_date_curve = (*it_dble)->get_date(min_index);
	} else if (!mIntCurveList.empty()) {
	    std::vector < IntCurve * > :: iterator it_int = mIntCurveList.begin();
	    min_draw_date = mParameter.get_min_draw_date();
	    while( (*it_int)->get_date(min_index) < min_draw_date) {
		min_index++;
		nb_check_min_max++;
	    }
	    min_date_curve = (*it_int)->get_date(min_index);
	}
	mParameter.set_min_draw_index(min_index);

	// verifie si l'on ne doit pas mettre a jour les valeur max et min
	// verifie que les n valeur passé (a cause du scroll) n'etait pas soit
	// la plus haute soit la moins haute
	if (calcul_new_min_max_no_scroll) {
	    // comme on efface tout on recalcule le min & max
	    // par rapport a la dernière valeur de chaque courbe
	    mParameter.set_min_value(getMinValueN(nb_check_min_max));
	    mParameter.set_max_value(getMaxValueN(nb_check_min_max));
	} else {
	    // on verifie que les n valeur passé a cause du scroll
	    // n'etaient pas les valeurs min et/ou max
	    double min, max;

	    min = getMinValueN(nb_check_min_max);
	    max = getMaxValueN(nb_check_min_max);

	    // on doit mettre a jour la valeur minimal
	    if (min == mParameter.get_min_back_value()) {
		updateMinValueDrawn();
	    }
	    // on doit mettre a jour la valeur maximal
	    if (max == mParameter.get_max_back_value()) {
		updateMaxValueDrawn();
	    }
	}
	updateStepHeight();
	draw();
        copy();
    }
}

void Plot::onParameter(const std::string& /* plugin */,
                       const std::string& /* location */,
                       const std::string& /* file */,
                       const std::string& parameters,
                       const double& /* time */)
{
    Assert(utils::InternalError, m_ctx, "Cairo plot drawing error");

    mParameter.set_drawing_area_size(mWidth, mHeight);
    mParameter.set_screen_size(mWidth, mHeight);

    std::string s = "t";

    m_ctx->set_font_size(10);
    m_ctx->get_text_extents(s, mExtents);
    mParameter.set_text_height((int)(mExtents.height * 5));
    updateStepHeight();

    if (not parameters.empty()) {
        xmlpp::DomParser parser;

        parser.parse_memory(parameters);
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
		    mRealCurveList.push_back(rc);
		    mShowRealCurveList.push_back(rc);
		} else if (v_type == "integer") {
		    ic = new IntCurve(utils::xml::get_attribute(elt2,"name"),
				      utils::to_int(utils::xml::get_attribute(elt2,"color_red")),
				      utils::to_int(utils::xml::get_attribute(elt2,"color_green")),
				      utils::to_int(utils::xml::get_attribute(elt2,"color_blue")));
		    mIntCurveList.push_back(ic);
		    mShowIntCurveList.push_back(ic);
		}
		++it;
	    }
	}

	if (utils::xml::exist_children(root, "window")) {
	    xmlpp::Element* elt = utils::xml::get_children(root, "window");
	    mParameter.set_min_draw_date(0.0f);
	    mParameter.set_max_draw_date(utils::to_double(utils::xml::get_attribute(elt, "size")));
	    mParameter.set_number_drawn_date((int)mParameter.get_max_draw_date());
	}

	if (utils::xml::exist_children(root, "value")) {
	    xmlpp::Element* elt = utils::xml::get_children(root, "value");
	    if (utils::xml::exist_attribute(elt, "min"))
		mParameter.set_min_value(utils::to_double(utils::xml::get_attribute(elt, "min")),
					 true);
	    if (utils::xml::exist_attribute(elt, "max"))
		mParameter.set_max_value(utils::to_double(utils::xml::get_attribute(elt, "max")),
					 true);
	}

	if (utils::xml::exist_children(root, "scrolling")) {
	    xmlpp::Element* elt = utils::xml::get_children(root,"scrolling");
	    mParameter.set_scrolling(utils::to_boolean(utils::xml::get_attribute(elt,"value")));
	}

	if (utils::xml::exist_children(root, "limits")) {
	    xmlpp::Element* elt = utils::xml::get_children(root, "limits");
	    xmlpp::Node::NodeList lst = elt->get_children("limit");
	    xmlpp::Node::NodeList::iterator it = lst.begin();

	    double valueLimit;
	    while (it != lst.end()) {
		xmlpp::Element * elt2 = ( xmlpp::Element* )( *it );

		valueLimit = utils::to_double(utils::xml::get_attribute(elt2,"value"));
		mLimitList.push_back(new Limit(valueLimit,
					       utils::to_int(utils::xml::get_attribute(elt2,"color_red")),
					       utils::to_int(utils::xml::get_attribute(elt2,"color_green")),
					       utils::to_int(utils::xml::get_attribute(elt2,"color_blue"))));
		++it;
	    }
	}
    }

    m_it_dble = mRealCurveList.begin();
    m_it_int = mIntCurveList.begin();
}

void Plot::close(const double& /* time */)
{
    mParameter.set_number_drawn_date((int)ceil(mParameter.get_max_draw_date()));
    mParameter.set_min_draw_index(0);
    mParameter.set_min_draw_date(0.0f);

    // on calcul le max & le min sur l'ensemble de la simulation
    updateMaxValueDrawn();
    updateMinValueDrawn();
    mFinish = true;
}

void Plot::preferredSize(int& width, int& height)
{
    width = mWidth;
    height = mHeight;
}

void Plot::draw()
{
    if ((mRealCurveList.size() > 0 && mRealCurveList[0]->get_size() > 1) ||
        (mIntCurveList.size() > 0 && mIntCurveList[0]->get_size() > 0)) {

        m_ctx->rectangle(0, 0, mWidth, mHeight);
        m_ctx->set_source_rgb(1, 1, 1);
        m_ctx->fill();
        drawVerticalStep(m_ctx);
        drawHorizontalStep(m_ctx);
        drawAxis(m_ctx);
        drawCurves(m_ctx);
        drawLimits(m_ctx);
    }
}

void Plot::drawVerticalStep(Cairo::RefPtr < Cairo::Context > m_ctx)
{
    double m_stepX, date, m_firstDate, m_lastDate, rest;
    int v_x, m_maxY, m_minY;

    m_stepX = mParameter.get_number_drawn_date() / 4.0;
    m_firstDate = mParameter.get_min_draw_date();
    m_lastDate = mParameter.get_max_draw_date();
    m_maxY = mParameter.get_shift_top();
    m_minY = m_maxY + mParameter.get_graph_zone_height();
    rest = m_firstDate / m_stepX;

    if (rest == 0)
        date = m_firstDate;
    else
        date = ceil(rest) * m_stepX;

    m_ctx->set_source_rgb(0, 0, 0);
    m_ctx->set_line_width(1);
    while(date <= m_lastDate) {
        v_x = mParameter.to_pixel_width(date);

	m_ctx->begin_new_path();
	m_ctx->move_to(v_x, m_minY);
	m_ctx->line_to(v_x, m_maxY);
	m_ctx->close_path();
	m_ctx->stroke();

	m_ctx->move_to(v_x +1, m_minY);
	m_ctx->show_text(utils::to_string(date));

        date += m_stepX;
    }
}

void Plot::drawHorizontalStep(Cairo::RefPtr < Cairo::Context > m_ctx)
{
    int x_left, x_right, y_bottom;
    int y;
    double value_show = mParameter.get_min_value();
    double max_value_show = mParameter.get_max_value();

    m_ctx->set_dash(mDashes, 0);

    x_left = mParameter.get_shift_left();
    x_right = mParameter.get_shift_left() + mParameter.get_graph_zone_width();
    y_bottom = mParameter.get_graph_zone_height() + mParameter.get_shift_top();
    y = y_bottom;

    while(value_show <= max_value_show) {
	m_ctx->set_source_rgb(0., 0., 0.);
	m_ctx->move_to(x_left, y);
	m_ctx->line_to(x_right, y);
	m_ctx->set_line_width(1);

	m_ctx->move_to(0, y - 1);
	m_ctx->show_text(utils::to_string(value_show));
	m_ctx->stroke();

        y -= mParameter.get_text_height();
        value_show += mStepHeight;
    }
    m_ctx->unset_dash();
}

void Plot::drawCurves(Cairo::RefPtr < Cairo::Context > m_ctx)
{
    std::list < RealCurve * > :: const_iterator it_dble;
    for (it_dble = mShowRealCurveList.begin(); it_dble != mShowRealCurveList.end();
         ++it_dble)
        (*it_dble)->draw(m_ctx, mParameter);

    if(!mShowIntCurveList.empty())
        (*(mShowIntCurveList.begin()))->draw(m_ctx, mParameter, mShowIntCurveList);
}

void Plot::drawAxis(Cairo::RefPtr < Cairo::Context > m_ctx)
{
    int x_left, x_right, y_top, y_bottom;

    x_left = mParameter.get_shift_left();
    x_right = mParameter.get_shift_left() + mParameter.get_graph_zone_width();
    y_top = mParameter.get_shift_top();
    y_bottom = mParameter.get_graph_zone_height() + mParameter.get_shift_top();

    m_ctx->set_line_width(1);
    m_ctx->set_source_rgb(0, 0, 0);
    m_ctx->begin_new_path();
    m_ctx->move_to(x_left, y_top);
    m_ctx->line_to(x_right, y_top);
    m_ctx->line_to(x_right, y_bottom);
    m_ctx->line_to(x_left, y_bottom);
    m_ctx->line_to(x_left, y_top);
    m_ctx->close_path();
    m_ctx->stroke();
}

void Plot::drawLimits(Cairo::RefPtr < Cairo::Context > m_ctx)
{
    int y_limit;
    int x_left, x_right;

    x_left = mParameter.get_shift_left();
    x_right = mParameter.get_shift_left() + mParameter.get_graph_zone_width();

    for (std::list < Limit * >::const_iterator it = mLimitList.begin();
	 it != mLimitList.end(); ++it) {
        if (mParameter.is_inside_min_max((*it)->get_value())) {

            y_limit = mParameter.to_pixel_height((*it)->get_value());
	    m_ctx->get_text_extents(utils::to_string((*it)->get_value()), mExtents);

	    m_ctx->set_source_rgba(1, 1, 0, 0.5);
	    m_ctx->begin_new_path();
	    m_ctx->move_to(x_left + 2, y_limit - 2);
	    m_ctx->rel_line_to(0, -mExtents.height - 4);
	    m_ctx->rel_line_to(mExtents.width + 4, 0);
	    m_ctx->rel_line_to(0, mExtents.height + 4);
	    m_ctx->rel_line_to(-mExtents.width - 4, 0);
	    m_ctx->close_path();
	    m_ctx->fill();
	    m_ctx->stroke();

	    m_ctx->set_source_rgba(0., 0., 0., 1.);
	    m_ctx->move_to(x_left + 4, y_limit - 4);
	    m_ctx->show_text(utils::to_string((*it)->get_value()));

	    m_ctx->set_source_rgba((*it)->get_red_color()/65535.,
				 (*it)->get_green_color()/65535.,
				 (*it)->get_blue_color()/65535.,
				 0.5);
	    m_ctx->set_line_width(1);
	    m_ctx->begin_new_path();
	    m_ctx->move_to(x_left, y_limit);
	    m_ctx->line_to(x_right, y_limit);
	    m_ctx->close_path();
	    m_ctx->stroke();
        }
    }
}

/**************************************************************/
/* calcul la valeur minimal affiché de l'ensemble des courbes */
/**************************************************************/
void Plot::updateMinValueDrawn()
{
    std::vector < int > v_int;
    std::vector < double > v_dble;
    std::vector < int > :: const_iterator it_int_min;
    std::vector < double > :: const_iterator it_dble_min;
    std::list < IntCurve * > :: const_iterator it_int_curve;
    std::list < RealCurve * > :: const_iterator it_dble_curve;
    int number_value = mParameter.get_max_draw_index() - mParameter.get_min_draw_index();
    double m_min = 0.0f; // par defaut, si jamais on n'affiche pas de courbe
    bool dble_empty, int_empty;

    for(it_dble_curve = mShowRealCurveList.begin(); it_dble_curve != mShowRealCurveList.end(); ++it_dble_curve) {
        v_dble.push_back((*it_dble_curve)->get_min_value(number_value, mParameter));
    }

    for(it_int_curve = mShowIntCurveList.begin(); it_int_curve != mShowIntCurveList.end(); ++it_int_curve) {
        v_int.push_back((*it_int_curve)->get_min_value(number_value, mParameter));
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
    mParameter.set_min_value(m_min);
}

/**************************************************************/
/* calcul la valeur maximal affiché de l'ensemble des courbes */
/**************************************************************/
void Plot::updateMaxValueDrawn()
{
    std::vector < int > v_int;
    std::vector < double > v_dble;
    std::vector < int > :: const_iterator it_int_max;
    std::vector < double > :: const_iterator it_dble_max;
    std::list < IntCurve * > :: const_iterator it_int_curve;
    std::list < RealCurve * > :: const_iterator it_dble_curve;
    int number_value = mParameter.get_max_draw_index() - mParameter.get_min_draw_index();
    double m_max = 1.0f; // par defaut, si jamais on n'affiche pas de courbe
    bool dble_empty, int_empty;

    for(it_dble_curve = mShowRealCurveList.begin(); it_dble_curve != mShowRealCurveList.end(); ++it_dble_curve) {
        v_dble.push_back((*it_dble_curve)->get_max_value(number_value, mParameter));
    }

    for(it_int_curve = mShowIntCurveList.begin(); it_int_curve != mShowIntCurveList.end(); ++it_int_curve) {
        v_int.push_back((*it_int_curve)->get_max_value(number_value, mParameter));
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
    mParameter.set_max_value(m_max);
}


/****************************************************************************/
/* calcul la valeur maximale des n derniere valeur de l'ensemble des courbe */
/****************************************************************************/
double Plot::getMaxValueN(int nb_check_value)
{
    std::vector < int > v_int_max;
    std::vector < double > v_dble_max;
    std::vector < int > :: const_iterator it_int_max;
    std::vector < double > :: const_iterator it_dble_max;
    bool dble_empty, int_empty;
    std::list < IntCurve * > :: const_iterator it_int;
    std::list < RealCurve * > :: const_iterator it_dble;
    double m_max = 1.0f; // par defaut, si jamais on n'affiche pas de courbe
    int index_min = mParameter.get_min_draw_index();

    for(it_dble = mShowRealCurveList.begin(); it_dble != mShowRealCurveList.end(); ++it_dble) {
        v_dble_max.push_back((*it_dble)->get_max_value_prec_index(index_min, nb_check_value));
    }

    for(it_int = mShowIntCurveList.begin(); it_int != mShowIntCurveList.end(); ++it_int) {
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
double Plot::getMinValueN(int nb_check_value)
{
    std::vector < int > v_int_min;
    std::vector < double > v_dble_min;
    std::vector < int > :: const_iterator it_int_min;
    std::vector < double > :: const_iterator it_dble_min;
    bool dble_empty, int_empty;
    std::list < IntCurve * > :: const_iterator it_int;
    std::list < RealCurve * > :: const_iterator it_dble;
    double m_min = -1.0f;
    int index_min = mParameter.get_min_draw_index();

    for(it_dble = mShowRealCurveList.begin(); it_dble != mShowRealCurveList.end(); ++it_dble) {
        v_dble_min.push_back((*it_dble)->get_min_value_prec_index(index_min, nb_check_value));
    }

    for(it_int = mShowIntCurveList.begin(); it_int != mShowIntCurveList.end(); ++it_int) {
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

void Plot::updateStepHeight()
{
    mStepHeight = (5. * mExtents.height) * (mParameter.get_max_value() - mParameter.get_min_value())
	/ mParameter.get_graph_zone_height();
}

}}} // namespace vle oov plugin
