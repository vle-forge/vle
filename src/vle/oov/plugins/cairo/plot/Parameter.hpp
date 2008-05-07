/**
 * @file src/vle/oov/plugins/cairo/plot/Parameter.hpp
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




#ifndef VLE_OOV_PLUGINS_CAIRO_PLOT_PARAMETER_HPP
#define VLE_OOV_PLUGINS_CAIRO_PLOT_PARAMETER_HPP

#include <vle/utils/Tools.hpp>

namespace vle { namespace oov { namespace plugin {

class Parameter
{
private :
    /*****************************************/
    /* recalcul le largeur unitaire en pixel */
    /*****************************************/
    void update_unit_width();
    
    /*****************************************/
    /* recalcul le hauteur unitaire en pixel */
    /*****************************************/
    void update_unit_height();
    
    /******************************/
    /* gestion largeur et hauteur */
    /******************************/
    /***********/
    /* fenetre */
    /***********/
    /* largeur */
    int m_screen_width;
    int m_screen_height;
    
    /****************/
    /* drawing_area */
    /****************/
    /* largeur du drawing area */
    int m_da_width; 
    int m_da_height;
    
    /******************************************/
    /* zone libre autour de la zone de dessin */
    /******************************************/
    int m_shift_top, m_shift_top_default; /* largeur en haut pour une meilleure vision */
    int m_shift_bottom; /* largeur en bas pour pouvoir afficher les textes */
    int m_shift_left; /* largeur a gauche pour pouvoir afficher les textes */
    int m_shift_right; /* largeur a droite pour pouvoir afficher rien du tout :D */
    int m_shift_left_text_width; /* espace entre la fin des textes et l'axe des ordonnées */
    
    /******************/
    /* zone de dessin */
    /******************/
    int m_number_drawn_date; /* nombre de date affiché */
    int m_graph_zone_height; /* hauteur de la zone des graphiques */
    int m_graph_zone_width; /* largeur de la zone des graphiques */	
    bool m_scrolling; /* scroll ou non */
    double m_unit_height, m_unit_width; /* largeur unitaire */
    
    /********************/
    /* gestion des date */
    /********************/
    double m_min_draw_date; /* la date minimum affiché */
    double m_max_draw_date; /* la date maximum affiché */
    int m_min_draw_index; /* indice de la premiere valeur affiché */
    int m_max_draw_index; /* indice de la derniere valeur affiché */
    int m_text_height;
    double m_text_height_value; /* conversion de la hauteur pixel --> valeur de l'axe des ordonn�es */
    int m_y0; /* axe y = 0 */
    
    /*********************************************/
    /* gestion valeur minimum et maximum affiché */
    /*********************************************/
    double m_max_value, m_max_value_not_increase; // zoogaku // zougaku
    double m_min_value, m_min_value_not_increase;
    bool m_axe_y0_show;
    
    /*************************************************/
    /* majoration pour calculer la valeur min et max */
    /*************************************************/
    double majoration_min, majoration_max;
    
    /*****************************************************/
    /* permet de savoir si l'on a deja parse des données */
    /* pour calculer le min et le max                    */
    /*****************************************************/
    bool m_min_already_initialized;
    bool m_max_already_initialized;
    
    /**************************************/
    /* recalcul la valeur du text_height */
    /**************************************/
    void update_text_height_value();
    
    /*************************************/
    /* recalcul si l'axe y=0 est visible */
    /*************************************/
    void update_axe_y0_show();
    
    /***********************************************/
    /* arrondi le double a l'entier le plus proche */
    /* 1.5 --> 2                                   */
    /***********************************************/
    //	int round_nearest(double value);
    
public :
    Parameter();
    virtual ~Parameter() { }
    
    void printParameter();
    int round_nearest(double value);
    void set_screen_size(int width, int height);
    void get_screen_size(int & width, int & height);
    void set_drawing_area_size(int width, int height);
    void get_drawing_area_size(int & width, int & height);
    void set_shift_left(int value);
    int get_shift_left();
    void set_shift_right(int value);
    int get_shift_right();
    void set_shift_top(int value);
    int get_shift_top();
    void set_shift_bottom(int value);
    int get_shift_bottom();
    void set_number_drawn_date(int value);
    int get_number_drawn_date();
    void set_text_height(int height);
    int get_text_height();
    void set_graph_zone_height(int value);
    int get_graph_zone_height();
    void set_graph_zone_width(int value);
    int get_graph_zone_width();
    void set_min_draw_date(double value);
    double get_min_draw_date();
    void set_max_draw_date(double value);
    double get_max_draw_date();
    void set_max_value(double value, bool absolute = false);
    double get_max_value();
    double get_max_back_value();
    void set_min_value(double value, bool absolute = false);
    double get_min_value();
    double get_min_back_value();
    void set_scrolling(bool value);
    bool get_scrolling();
    void init();
    void set_min_draw_index(int index);
    int get_min_draw_index();
    void set_max_draw_index(int index);
    int get_max_draw_index();
    void inc_max_draw_index();
    bool is_smaller(double value);
    bool is_higher(double value);
    bool is_smaller_back(double value);
    bool is_higher_back(double value);
    double get_text_height_value();
    bool get_axe_y0_show();
    int to_pixel_height(double value);
    int to_pixel_height(int value);
    int to_pixel_width(double date);
    int get_height_bottom_axe();
    double pixel_to_date(int pixel);
    double pixel_to_value_height(int pixel);
    int get_y0_axe();
    void update_m_y0();
    bool is_inside_min_max(double value);
};
	    
}}} // namespace vle oov plugin
    
#endif
