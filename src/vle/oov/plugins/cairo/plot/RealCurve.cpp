/**
 * @file vle/oov/plugins/cairo/plot/RealCurve.cpp
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


#include <vle/oov/plugins/cairo/plot/RealCurve.hpp>

namespace vle { namespace oov { namespace plugin {

struct SortByNumber {
    bool operator()
    (const std::pair < double, double > & p1,
     const std::pair < double, double > & p2) const
	{
	    return p1.second < p2.second;
	}
};


void RealCurve::add(double date, double value)
{
    m_valueList.push_back(std::pair<double,double>(date, value));
}

void RealCurve::draw(Cairo::RefPtr < Cairo::Context > ctx,
		     Parameter & m_parameter)
{
    // dessine de droite  gauche
    int v_y, v_y2, v_x, v_x2;
    double v_date, v_date2, v_value, v_value2, m_firstDate;
    int firstIndex, lastIndex;

    m_firstDate = m_parameter.get_min_draw_date();

    firstIndex = m_parameter.get_min_draw_index();
    lastIndex = m_parameter.get_max_draw_index();

    v_date = m_valueList[lastIndex - 1].first;
    v_value = m_valueList[lastIndex - 1].second;
    v_x = m_parameter.to_pixel_width(v_date);

    // change la couleur en la couleur de la courbe
    ctx->set_source_rgb(get_red_color()/65535.,
			get_green_color()/65535.,
			get_blue_color()/65535.);
    ctx->set_line_width(1);

    for(int cpt = lastIndex - 2; cpt >= firstIndex; cpt--) {
        v_date2 = m_valueList[cpt].first;
        v_value2 = m_valueList[cpt].second;

        v_x2 = m_parameter.to_pixel_width(v_date2);

        v_y = m_parameter.to_pixel_height(v_value);
        v_y2 = m_parameter.to_pixel_height(v_value2);

	ctx->begin_new_path();
	ctx->move_to(v_x, v_y);
	ctx->line_to(v_x2, v_y2);
	ctx->close_path();
	ctx->stroke();

        v_date = v_date2;
        v_value = v_value2;
        v_x = v_x2;
        v_y = v_y2;

        if (cpt == firstIndex && firstIndex != 0 && v_date != m_firstDate) {
            v_date2 = m_valueList[cpt - 1].first;
            v_value2 = m_valueList[cpt - 1].second;
            v_date = v_date - m_firstDate;
            v_date2 = m_firstDate - v_date2;

            v_x2 = m_parameter.to_pixel_width(m_firstDate);
            v_y2 = m_parameter.to_pixel_height((v_date * v_value2 + v_date2 * v_value) /  (v_date + v_date2));

	    ctx->begin_new_path();
	    ctx->move_to(v_x, v_y);
	    ctx->line_to(v_x2, v_y2);
	    ctx->close_path();
	    ctx->stroke();
	}
    }
}

double RealCurve::get_last_value() const
{
    if (m_valueList.size() == 0)
        return 0.0f;
    else
        return m_valueList[m_valueList.size()-1].second;
}

/********************************************************************************************/
/* retourne la valeur minimum et maximum de la courbe sur les number_value derniere valeur */
/********************************************************************************************/
void RealCurve::get_min_max_value(int number_value, double & min, double & max, Parameter & m_parameter)
{
    min = get_min_value(number_value, m_parameter);
    max = get_max_value(number_value, m_parameter);
}

/********************************************************************************/
/* retourne la valeur minimum de la courbe sur les number_value derniere valeur */
/********************************************************************************/
double RealCurve::get_min_value(int number_value, Parameter & m_parameter)
{
    int index_data_end = m_parameter.get_max_draw_index(); // index de fin de recherche
    int vector_size = m_valueList.size();
    int index_end, index_begin;
    if (index_data_end >= 0)
        index_end = (vector_size <= index_data_end) ? vector_size - 1 : index_data_end;
    else
        index_end = vector_size - 1;

    if (number_value >= 1)
        index_begin = ((index_end - number_value) < 0) ? 0 : index_end - number_value + 1;
    else
        index_begin = index_end;

    std::vector < std::pair < double, double > > :: const_iterator it_begin, it_end, it_min;
    it_begin = m_valueList.begin() + index_begin;
    it_end = it_begin + (index_end - index_begin)+1;

    it_min = min_element(it_begin, it_end, SortByNumber());
    return (*it_min).second;
}

/********************************************************************************/
/* retourne la valeur maximum de la courbe sur les number_value derniere valeur */
/********************************************************************************/
double RealCurve::get_max_value(int number_value, Parameter & m_parameter)
{
    int index_data_end = m_parameter.get_max_draw_index(); // index de fin de recherche
    int vector_size = m_valueList.size();
    int index_end, index_begin;
    if (index_data_end >= 0)
        index_end = (vector_size <= index_data_end) ? vector_size - 1 : index_data_end;
    else
        index_end = vector_size - 1;

    if (number_value >= 1)
        index_begin = ((index_end - number_value) < 0) ? 0 : index_end - number_value + 1;
    else
        index_begin = index_end;

    std::vector < std::pair < double, double > > :: const_iterator it_begin, it_end, it_max;
    it_begin = m_valueList.begin() + index_begin;
    it_end = it_begin + (index_end - index_begin)+1;

    it_max = max_element(it_begin, it_end, SortByNumber());
    return (*it_max).second;
}


/*********************************************/
/* retourne le nombre de valeur de la courbe */
/*********************************************/
int RealCurve::get_number_value()
{
    return m_valueList.size();
}

/**************************/
/* retourne la nieme date */
/**************************/
double RealCurve::get_date(int index)
{
    return m_valueList.at(index).first;
}

/****************************/
/* retourne la nieme valeur */
/****************************/
double RealCurve::get_value(int index)
{
    return m_valueList.at(index).second;
}

/****************************************************************************/
/* retourne la valeur minimum et maximum de la courbe sur les number_value */
/* précédente valeur l'indice passé en parametre                            */
/****************************************************************************/
void RealCurve::get_min_max_value_prec_index(int indice, int number_value, double & min, double & max)
{
    min = get_min_value_prec_index(indice, number_value);
    max = get_min_value_prec_index(indice, number_value);
}

/****************************************************************/
/* retourne la valeur minimum de la courbe sur les number_value */
/* précédente valeur l'indice passé en parametre             */
/****************************************************************/
double RealCurve::get_min_value_prec_index(int indice, int number_value)
{
    int nb_inc = 0; // nbre de fois qu'il faut incrementer l'iterateur de debut
    std::vector < std::pair < double, double > > :: const_iterator it_deb, it_end, it_min;

    if((indice - number_value) > 0)
        nb_inc = indice - number_value;

    it_deb = m_valueList.begin();
    it_end = m_valueList.begin();

    it_deb = it_deb + nb_inc;
    it_end = it_end + indice; // borne fin non prise ne compte

    it_min = min_element(it_deb, it_end, SortByNumber());
    return (*it_min).second;
}

/****************************************************************/
/* retourne la valeur maximum de la courbe sur les number_value */
/* précédente valeur l'indice passé en parametre             */
/****************************************************************/
double RealCurve::get_max_value_prec_index(int indice, int number_value)
{
    int nb_inc = 0; // nbre de fois qu'il faut incrementer l'iterateur de debut
    std::vector < std::pair < double, double > > :: const_iterator it_deb, it_end, it_max;

    if((indice - number_value) > 0)
        nb_inc = indice - number_value;

    it_deb = m_valueList.begin();
    it_end = m_valueList.begin();

    it_deb = it_deb + nb_inc;
    it_end = it_end + indice; // borne fin non prise ne compte

    it_max = max_element(it_deb, it_end, SortByNumber());
    return (*it_max).second;

}

}}} // namespace vle oov plugin
