/** 
 * @file IntCurve.cpp
 * @brief 
 * @author The vle Development Team
 * @date ven, 27 jan 2006 16:08:33 +0100
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

#include <vle/oov/plugins/cairo/plot/IntCurve.hpp>

namespace vle { namespace oov { namespace plugin {

struct SortByNumber { 
    bool operator ()(const std::pair < double, int > & p1, const std::pair < double, int
		     > & p2) const 
	{ 
	    return p1.second < p2.second; 
	} 
};
	    
struct SortByNumberValue { 
    bool operator ()(const std::pair < int, int > & p1, const std::pair < int, int > & p2) const 
	{ 
	    return p1.first > p2.first; 
	} 
};

void IntCurve::swap_int(int &value1, int &value2)
{
    int temp;
    temp = value1;
    value1 = value2;
    value2 = temp;
}

void IntCurve::draw(Cairo::RefPtr < Cairo::Context > ctx, Parameter& m_parameter,
		    std::list < IntCurve * > m_list)
{
    std::vector < color > v_colors;
    int index = 0, v_value, v_x, v_x_next, v_y_top, v_y_bottom;
    double v_date, next_date, m_firstDate;
    bool draw_raccord = false;
    int lastIndex, firstIndex;
    std::list < IntCurve * > :: const_iterator it;
    std::vector < std::pair < int, int > > v_triValue, v_triRaccord;

    for(it = m_list.begin(); it != m_list.end(); ++it)
        v_colors.push_back(color((*it)->get_red_color()/65535.,
				 (*it)->get_green_color()/65535.,
				 (*it)->get_blue_color()/65535.));

    index = 0;
    m_firstDate = m_parameter.get_min_draw_date();
    firstIndex = m_parameter.get_min_draw_index();
    lastIndex = m_parameter.get_max_draw_index();

    for(int cpt = lastIndex - 2; cpt >= firstIndex; cpt--)
    {
        draw_raccord = false;

        for(it = m_list.begin(); it != m_list.end(); ++it) 
        {
            v_triValue.push_back(std::pair < int, int >((*it)->get_value(cpt), index));
            v_date = (*it)->get_date(cpt);
            next_date = (*it)->get_date(cpt+1);

            v_x = m_parameter.to_pixel_width(v_date);
            v_x_next = m_parameter.to_pixel_width(next_date);
            if (cpt == firstIndex && firstIndex != 0 && v_date != m_firstDate) {
                v_triRaccord.push_back(std::pair < int, int >((*it)->get_value(cpt-1) ,index)); 
                draw_raccord = true;
            }
            index++;
        }
        index = 0;

        // tri les valeurs
        std::sort(v_triValue.begin(), v_triValue.end(), SortByNumberValue());

        for(int k = 0; k < (int)v_triValue.size(); k++) {
            v_value = v_triValue[k].first;
            v_y_bottom = m_parameter.get_y0_axe();
            v_y_top = m_parameter.to_pixel_height(v_value);

            if(v_y_bottom > v_y_top) {
                swap_int(v_y_bottom, v_y_top);
            }
//            m_buffer->draw_rectangle(v_gc[v_triValue[k].second],
//            false, v_x, v_y_bottom, v_x_next-v_x, v_y_top-v_y_bottom);
	    ctx->set_source_rgb(v_colors[v_triValue[k].second].red, 
				v_colors[v_triValue[k].second].green,
				v_colors[v_triValue[k].second].blue);
	    ctx->begin_new_path();
	    ctx->move_to(v_x, v_y_bottom);
	    ctx->line_to(v_x_next, v_y_bottom);
	    ctx->line_to(v_x_next, v_y_top);
	    ctx->line_to(v_x, v_y_top);
	    ctx->line_to(v_x, v_y_bottom);
	    ctx->close_path();
	    ctx->stroke();
        }
        v_triValue.clear();
    }
}

void IntCurve::add(double date, int value)
{
    m_valueList.push_back(std::pair<double,int>(date, value));
}

const int IntCurve::get_last_value()
{
    if (m_valueList.size() == 0)
        return 0;
    else
        return m_valueList[m_valueList.size()-1].second;
}

/********************************************************************************************/
/* retourne la valeur minimum et maximum de la courbe sur les number_value derniere valeur */
/********************************************************************************************/		
const void IntCurve::get_min_max_value(int number_value, int & min, int & max, Parameter & m_parameter)
{
    min = get_min_value(number_value, m_parameter);
    max = get_max_value(number_value, m_parameter);
}

/********************************************************************************/
/* retourne la valeur minimum de la courbe sur les number_value derniere valeur */
/* en fonction du dernier index affich�                                         */
/********************************************************************************/		
const int IntCurve::get_min_value(int number_value, Parameter & m_parameter)
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

    std::vector < std::pair < double, int > > :: const_iterator it_begin, it_end, it_min;
    it_begin = m_valueList.begin() + index_begin;
    it_end = it_begin + (index_end - index_begin)+1;

    it_min = min_element(it_begin, it_end, SortByNumber());
    return (*it_min).second;
}

/********************************************************************************/
/* retourne la valeur maximum de la courbe sur les number_value derniere valeur */
/********************************************************************************/		
const int IntCurve::get_max_value(int number_value, Parameter & m_parameter)
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

    std::vector < std::pair < double, int > > :: const_iterator it_begin, it_end, it_max;
    it_begin = m_valueList.begin() + index_begin;
    it_end = it_begin + (index_end - index_begin)+1;

    it_max = max_element(it_begin, it_end, SortByNumber());
    return (*it_max).second;
}


/*********************************************/
/* retourne le nombre de valeur de la courbe */
/*********************************************/
const int IntCurve::get_number_value()
{
    return m_valueList.size();
}

/**************************/
/* retourne la nieme date */
/**************************/		
const double IntCurve::get_date(int index)
{
    return m_valueList.at(index).first;
}

/****************************/
/* retourne la nieme valeur */
/****************************/		
const int IntCurve::get_value(int index)
{
    return m_valueList.at(index).second;
}

/****************************************************************************/
/* retourne la valeur minimum et maximum de la courbe sur les number_value */
/* précédente valeur l'indice passé en parametre                            */
/****************************************************************************/
const void IntCurve::get_min_max_value_prec_index(int indice, int number_value, int & min, int & max)
{
    min = get_min_value_prec_index(indice, number_value);
    max = get_max_value_prec_index(indice, number_value);
}	


/****************************************************************/
/* retourne la valeur minimum de la courbe sur les number_value */
/* précédente valeur l'indice passé en parametre             */
/****************************************************************/
const int IntCurve::get_min_value_prec_index(int indice, int number_value)
{
    int nb_inc = 0; // nbre de fois qu'il faut incrementer l'iterateur de debut
    std::vector < std::pair < double, int > > :: const_iterator it_deb, it_end, it_min;

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
const int IntCurve::get_max_value_prec_index(int indice, int number_value)
{
    int nb_inc = 0; // nbre de fois qu'il faut incrementer l'iterateur de debut
    std::vector < std::pair < double, int > > :: const_iterator it_deb, it_end, it_max;

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
