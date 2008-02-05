/**
 * @file src/vle/oov/plugins/cairo/plot/Parameter.cpp
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




#include <vle/oov/plugins/cairo/plot/Parameter.hpp>
#include <cmath>
#include <iostream>

namespace vle { namespace oov { namespace plugin {
    
/************************************/
/* constructeur                     */
/* recupere la hauteur d'un text   */
/************************************/
Parameter::Parameter() : m_shift_top_default(5), m_shift_left(50), 
			 m_shift_left_text_width(5), m_axe_y0_show(false), 
			 m_min_already_initialized(false), 
			 m_max_already_initialized(false) 
{
    m_min_draw_index = 0;
    m_max_draw_index = 0;
    majoration_min = 0.05;
    majoration_max = 0.05;
}


void Parameter::printParameter()
{
    std::string barre = "**********************************************************************";

    std::cout << "detail Parameter" << std::endl << barre << std::endl;
    std::cout << barre << std::endl;
    std::cout << "fenetre" << std::endl;
    std::cout << "m_screen_width : " << m_screen_width << std::endl;
    std::cout << "m_screen_height : " << m_screen_height << std::endl;
    std::cout << barre << std::endl;
    std::cout << "drawing_area" << std::endl;
    std::cout << "m_da_width : " << m_da_width << std::endl;
    std::cout << "m_da_height : " << m_da_height << std::endl;
    std::cout << barre << std::endl;
    std::cout << "zone libre autour de la zone de dessin" << std::endl;
    std::cout << "m_shift_top : " << m_shift_top << std::endl;
    std::cout << "m_shift_top_default : " << m_shift_top_default << std::endl;
    std::cout << "m_shift_bottom : " << m_shift_bottom << std::endl;
    std::cout << "m_shift_left : " << m_shift_left << std::endl;
    std::cout << "m_shift_right : " << m_shift_right << std::endl;
    std::cout << "m_shift_left_text_width : " << m_shift_left_text_width << std::endl;
    std::cout << barre << std::endl;
    std::cout << "zone de dessin" << std::endl;
    std::cout << "m_number_drawn_date : " << m_number_drawn_date << std::endl;
    std::cout << "m_graph_zone_height : " << m_graph_zone_height << std::endl;
    std::cout << "m_graph_zone_width : " << m_graph_zone_width << std::endl;
    std::cout << "m_scrolling : " << m_scrolling << std::endl;
    std::cout << "m_unit_height : " << m_unit_height << std::endl;
    std::cout << "m_unit_width : " << m_unit_width << std::endl;
    std::cout << barre << std::endl;
    std::cout << "gestion des dates" << std::endl;
    std::cout << "m_min_draw_date : " << m_min_draw_date << std::endl;
    std::cout << "m_max_draw_date : " << m_max_draw_date << std::endl;
    std::cout << "m_min_draw_index : " << m_min_draw_index << std::endl;
    std::cout << "m_max_draw_index : " << m_max_draw_index << std::endl;
    std::cout << "m_text_height : " << m_text_height << std::endl;
    std::cout << "m_text_height_value : " << m_text_height_value << std::endl;
    std::cout << "m_y0 : " << m_y0 << std::endl;
    std::cout << barre << std::endl;
    std::cout << "gestion valeur minimum et maximum affiché" << std::endl;
    std::cout << "m_max_value : " << m_max_value << std::endl;
    std::cout << "m_max_value_not_increase : " << m_max_value_not_increase << std::endl;
    std::cout << "m_min_value : " << m_min_value << std::endl;
    std::cout << "m_min_value_not_increase : " << m_min_value_not_increase << std::endl;
    std::cout << "m_axe_y0_show : " << m_axe_y0_show << std::endl;
    std::cout << "get_y0_axe() : " << get_y0_axe() << std::endl;
    std::cout << barre << std::endl;
}



/***********************************************/
/* arrondi le double a l'entier le plus proche */
/* 1.5 --> 2                                   */
/***********************************************/
int Parameter::round_nearest(double value)
{
    return (int)floor(value + 0.5);
}


/*****************************************/
/* recalcul le largeur unitaire en pixel */
/*****************************************/
void Parameter::update_unit_width()
{
    if (m_number_drawn_date != 0) {
        m_unit_width = (m_da_width - m_shift_left) / (double)m_number_drawn_date;
        m_graph_zone_width = (int)(m_unit_width * m_number_drawn_date);
    }
}

/*****************************************/
/* recalcul le hauteur unitaire en pixel */
/*****************************************/
void Parameter::update_unit_height()
{
    if (m_number_drawn_date != 0) {
        m_unit_height = (m_da_height - m_shift_top_default - m_text_height)  / (m_max_value - m_min_value);
        m_graph_zone_height = (int)(m_unit_height * (m_max_value - m_min_value));

        m_shift_bottom = m_da_height - m_text_height;
        m_shift_top = m_shift_bottom - m_graph_zone_height;

        update_text_height_value();
    }
}

/*************************************/
/* recalcul si l'axe y=0 est visible */
/*************************************/
void Parameter::update_axe_y0_show()
{
    m_axe_y0_show = (m_min_value < 0 && m_max_value > 0);
}

/**********************************/
/* defini la taille de la fenetre */
/**********************************/
void Parameter::set_screen_size(int width, int height)
{
    m_screen_width = width;
    m_screen_height = height;
}

/************************************/
/* retourne la taille de la fenetre */
/************************************/
const void Parameter::get_screen_size(int & width, int & height)
{
    width = m_screen_width;
    height = m_screen_height;
}

/************************************/
/* defini la taille du drawing_area */
/************************************/
void Parameter::set_drawing_area_size(int width, int height)
{
    if (width != m_da_width) {
        m_da_width = width;
        update_unit_width();
    }
    if(height != m_da_height) {
        m_da_height = height;
        update_unit_height();
    } 
    update_m_y0();
}

/**************************************/
/* retourne la taille du drawing_area */
/**************************************/
const void Parameter::get_drawing_area_size(int & width, int & height)
{
    width = m_da_width;
    height = m_da_height;
}

/***************************************************************/
/* defini la largeur necessaire a dessiner l'axe des ordonnées */
/***************************************************************/
void Parameter::set_shift_left(int value)
{
    m_shift_left = value;
}

/*****************************************************************/
/* retourne la largeur necessaire a dessiner l'axe des ordonnées */
/*****************************************************************/
const int Parameter::get_shift_left()
{
    return m_shift_left;
}

/*********************************************************/
/* defini la largeur necessaire a droite pour rien faire */
/*********************************************************/
void Parameter::set_shift_right(int value)
{
    m_shift_right = value;
}

/***********************************************************/
/* retourne la largeur necessaire a droite pour rien faire */
/***********************************************************/
const int Parameter::get_shift_right()
{
    return m_shift_right;
}

/***********************************************/
/* defini la hauteur necessaire pour la vision */
/***********************************************/
void Parameter::set_shift_top(int value)
{
    m_shift_top = value;
}

/*************************************************/
/* retourne la hauteur necessaire pour la vision */
/*************************************************/
const int Parameter::get_shift_top()
{
    return m_shift_top;
}

/***************************************************************************/
/* defini la hauteur necessaire a dessiner les text de l'axe des abscisse */
/***************************************************************************/
void Parameter::set_shift_bottom(int value)
{
    m_shift_bottom = value;
}

/*****************************************************************************/
/* retourne la hauteur necessaire a dessiner les text de l'axe des abscisse */
/*****************************************************************************/
const int Parameter::get_shift_bottom()
{
    return m_shift_bottom;
}

/************************************************/
/* defini le nombre de date affiché sur l'ecran */
/************************************************/
void Parameter::set_number_drawn_date(int value)
{
    m_number_drawn_date = value;
    update_unit_width();
}

/**************************************************/
/* retourne le nombre de date affiché sur l'ecran */
/**************************************************/
const int Parameter::get_number_drawn_date()
{
    return m_number_drawn_date;
}

/*****************************************/
/* defini la hauteur en pixel d'un text */
/*****************************************/
void Parameter::set_text_height(int height)
{
    m_text_height = height;
    update_text_height_value();
}

/**************************************/
/* recalcul la valeur du text_height */
/**************************************/
void Parameter::update_text_height_value()
{
    m_text_height_value = (m_text_height * (m_max_value - m_min_value)) / (double)m_graph_zone_height;
}

/*******************************************/
/* retourne la hauteur en pixel d'un text */
/*******************************************/
const int Parameter::get_text_height()
{
    return m_text_height;
}

/**********************************************************************/
/* return la valeur dble correspondant a la hauteur en pixel du text */
/**********************************************************************/
const double Parameter::get_text_height_value()
{
    return m_text_height_value;
}

/****************************************************/
/* defini la hauteur effective de la zone de dessin */
/****************************************************/
void Parameter::set_graph_zone_height(int value)
{
    m_graph_zone_height = value;
}

/******************************************************/
/* retourne la hauteur effective de la zone de dessin */
/******************************************************/
const int Parameter::get_graph_zone_height()
{
    return m_graph_zone_height;
}

/****************************************************/
/* defini la largeur effective de la zone de dessin */
/****************************************************/
void Parameter::set_graph_zone_width(int value)
{
    m_graph_zone_width = value;
}

/******************************************************/
/* retourne la largeur effective de la zone de dessin */
/******************************************************/
const int Parameter::get_graph_zone_width()
{
    return m_graph_zone_width;
}

/***********************************/
/* defini la date minimum affichée */
/***********************************/
void Parameter::set_min_draw_date(double value)
{
    m_min_draw_date = value;
}

/*************************************/
/* retourne la date minimum affichée */
/*************************************/
const double Parameter::get_min_draw_date()
{
    return m_min_draw_date;
}

/***********************************/
/* defini la date maximum affichée */
/***********************************/
void Parameter::set_max_draw_date(double value)
{
    m_max_draw_date = value;
}

/*************************************/
/* retourne la date maximum affichée */
/*************************************/
const double Parameter::get_max_draw_date()
{
    return m_max_draw_date;
}

/**************************************/
/* defini la valeur maximale affichée */
/**************************************/
void Parameter::set_max_value(double value, bool absolute)
{
    m_max_already_initialized = true;
    m_max_value_not_increase = value;
    if (absolute) m_max_value = value;
    else m_max_value = value  + majoration_max * fabs(value);

    update_m_y0();

    update_unit_height();
    update_text_height_value();
    update_axe_y0_show();
}

/****************************************/
/* retourne la valeur maximale affichée */
/****************************************/
const double Parameter::get_max_value()
{
    return m_max_value;
}

/**************************************/
/* defini la valeur minimale affichée */
/**************************************/
void Parameter::set_min_value(double value, bool absolute)
{
    m_min_already_initialized = true; 
    m_min_value_not_increase = value;
    if (absolute) m_min_value = value;
    else if (value != 0.0) m_min_value = value - majoration_min * fabs(value);
    else m_min_value = 0.0;
 
    update_m_y0();
    update_unit_height();
    update_text_height_value(); 
    update_axe_y0_show();
}

/****************************************/
/* retourne la valeur minimale affichée */
/****************************************/
const double Parameter::get_min_value()
{
    return m_min_value;
}

/**********************************************/
/* defini la valeur du scroll : true ou false */
/**********************************************/
void Parameter::set_scrolling(bool value)
{
    m_scrolling = value;
}

/************************************************/
/* retourne la valeur du scroll : true ou false */
/************************************************/
const bool Parameter::get_scrolling()
{
    return m_scrolling;
}

/*************************************************************************/
/* initialise l'ensemble des attribut suite a une mise a jour de ceux ci */
/*************************************************************************/
void Parameter::init()
{
    // TODO
}


/***********************************************/
/* defini la valeur de l'index minimum affiché */
/***********************************************/
void Parameter::set_min_draw_index(int index)
{
    m_min_draw_index = index;
}

/*************************************************/
/* retourne la valeur de l'index minimum affiché */
/*************************************************/
const int Parameter::get_min_draw_index()
{
    return m_min_draw_index;
}

/***********************************************/
/* defini la valeur de l'index maximum affiché */
/***********************************************/
void Parameter::set_max_draw_index(int index)
{
    m_max_draw_index = index;
}

/*************************************************/
/* retourne la valeur de l'index maximum affiché */
/*************************************************/
const int Parameter::get_max_draw_index()
{
    return m_max_draw_index;
}


/***********************************************/
/* incremente de 1 m_max_draw_index            */
/***********************************************/
void Parameter::inc_max_draw_index()
{
    m_max_draw_index++;
}


/**************************************/
/* return true if value < m_min_value */
/**************************************/
const bool Parameter::is_smaller(double value)
{
    if (!m_min_already_initialized) {
        m_min_already_initialized = true;
        return true;
    } else {
        return (value < m_min_value);
    }
}

/**************************************/
/* return true if value > m_max_value */
/**************************************/
const bool Parameter::is_higher(double value)
{
    if (!m_max_already_initialized) {
        m_max_already_initialized = true;
        return true;
    } else {
        return (value > m_max_value);
    }
}

/*****************************************************************/
/* return true if value < m_min_value pour les anciennes valeurs */
/*****************************************************************/
const bool Parameter::is_smaller_back(double value)
{
    return ((value - majoration_min * fabs(value)) < m_min_value);
}
/*****************************************************************/
/* return true if value > m_max_value pour les anciennes valeurs */
/*****************************************************************/
const bool Parameter::is_higher_back(double value)
{
    return ((value + majoration_max * fabs(value)) > m_max_value);
}

/****************************************/
/* return true si l'axe Y=0 est visible */
/* false sinon                          */
/****************************************/
const bool Parameter::get_axe_y0_show()
{
    return m_axe_y0_show;
}

/*************************************************/
/* return la valeur pixel de la valeur numerique */
/*************************************************/
const int Parameter::to_pixel_height(double value)
{
    return round_nearest(m_shift_top + m_graph_zone_height - fabs(m_min_value - value) * m_unit_height);
}

/*************************************************/
/* return la valeur pixel de la valeur numerique */
/*************************************************/
const int Parameter::to_pixel_height(int value)
{
    return round_nearest(m_shift_bottom - fabs(m_min_value - value) * m_unit_height);
}

/*************************************************/
/* return la valeur pixel de la valeur numerique */
/* sert pour les dates                           */
/*************************************************/
const int Parameter::to_pixel_width(double date)
{
    return round_nearest(m_shift_left + (date - m_min_draw_date) * m_unit_width);
}

/*******************************************************/
/* return la valeur pixel de l'axe y=0 ou dans le cas  */
/* ech�ant la valeur minimal si l'axe y=0 n'existe pas */
/*******************************************************/
const int Parameter::get_height_bottom_axe()
{
    if(m_axe_y0_show)
        return to_pixel_height(0.0f);
    else
        return to_pixel_height(m_min_value);
}

/***************************************************************/
/* retourne la valeur minimale affichée mais ss la majoration */
/***************************************************************/
const double Parameter::get_min_back_value()
{
    return m_min_value_not_increase;
}

/***************************************************************/
/* retourne la valeur maximale affichée mais ss la majoration */
/***************************************************************/
const double Parameter::get_max_back_value()
{
    return m_max_value_not_increase;

}

/************************************************************/
/* retourne la date equivalente au pixel pass� en parametre */
/************************************************************/
const double Parameter::pixel_to_date(int pixel)
{
    return (m_min_draw_date + (pixel - m_shift_left) / m_unit_width);
}

/**************************************************************/
/* retourne la valeur equivalente au pixel pass� en parametre */
/**************************************************************/
const double Parameter::pixel_to_value_height(int pixel)
{
    return ((m_shift_bottom - pixel) / m_unit_height + m_min_value);
}


/************************************************************/
/* retourne la valeur pixel de l'axe y=0 --> m_shift_bottom */
/************************************************************/
const int Parameter::get_y0_axe()
{
    return m_y0;
}

/***************/
/* maj de m_y0 */
/***************/
void Parameter::update_m_y0()
{
    if (m_min_value < 0 && m_max_value > 0)
        m_y0 = to_pixel_height(0.0);
    else if (m_min_value < 0 && m_max_value < 0)
        m_y0 = m_shift_top;
    else if (m_min_value > 0 && m_max_value > 0)
        m_y0 = m_shift_bottom;

}


/*************************************/
/* retourne true si la valeur pass�e */
/* en parametre est comprise         */
/* entre le min et le max            */
/*************************************/
const bool Parameter::is_inside_min_max(double value)
{
    return (value >= m_min_value && value <= m_max_value);
}

}}} // namespace vle oov plugin
