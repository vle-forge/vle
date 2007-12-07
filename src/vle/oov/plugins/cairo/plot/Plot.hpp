/** 
 * @file Plot.hpp
 * @brief 
 * @author The vle Development Team
 * @date ven, 27 jan 2006 16:09:59 +0100
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

#ifndef VLE_OOV_PLUGINS_CAIRO_PLOT_HPP
#define VLE_OOV_PLUGINS_CAIRO_PLOT_HPP

#include <vle/oov/CairoPlugin.hpp>
#include <list>
#include <map>
#include <vector>
#include <vle/oov/plugins/cairo/plot/Limit.hpp>
#include <vle/oov/plugins/cairo/plot/RealCurve.hpp>
#include <vle/oov/plugins/cairo/plot/IntCurve.hpp>
#include <vle/oov/plugins/cairo/plot/Parameter.hpp>

namespace vle { namespace oov { namespace plugin {

class Plot : public CairoPlugin
{
    typedef struct { int x; int y; double value; bool show; } pango_limit;
    typedef struct { int x; int y; } pointeur;

public:
    Plot(const std::string& location);
    virtual ~Plot() { }
    
    virtual void onParameter(const vpz::ParameterTrame& trame);
    
    virtual void onNewObservable(const vpz::NewObservableTrame& trame);
    
    virtual void onDelObservable(const vpz::DelObservableTrame& trame);
    
    virtual void onValue(const vpz::ValueTrame& trame);
    
    virtual void close(const vpz::EndTrame& trame);
    
    virtual void preferredSize(int& width, int& height);
    
    virtual void onSize(int width, int height)
        { mWidth = width; mHeight = height; }

private:
    std::vector < IntCurve * > m_data_int; // vector des courbes entieres
    std::vector < RealCurve * > m_data_dble; // vector des courbes réelles
    std::list < IntCurve * > m_data_int_show; // liste des courbes entieres affichées
    std::list < RealCurve * > m_data_dble_show; // liste des courbes érelles affichées
    std::list < Limit * > m_list_limit; // liste des limites
    Parameter m_parameter;
    bool update_curve;
    bool m_finish;

    std::vector < std::string > m_columns;
    std::map < std::string, int > m_columns2;
    std::vector < value::Value > m_buffer;
    double m_time;
    unsigned int m_receive;

    int mWidth;
    int mHeight;

    std::vector < RealCurve * >::iterator m_it_dble;
    std::vector < IntCurve * > :: iterator m_it_int;

    Cairo::RefPtr < Cairo::ImageSurface > m_img;

    void draw();
    void draw_axis(Cairo::RefPtr < Cairo::Context > ctx);
    void draw_vertical_step(Cairo::RefPtr < Cairo::Context > ctx);
    void draw_horizontal_step(Cairo::RefPtr < Cairo::Context > ctx);
    void draw_curves(Cairo::RefPtr < Cairo::Context > ctx);
    void draw_limits(Cairo::RefPtr < Cairo::Context > ctx);

    void update_min_value_drawn();
    void update_max_value_drawn();
    double get_max_value_n(int nb_check_value);
    double get_min_value_n(int nb_check_value);
    
    std::string buildname(const std::string& simulator,
			  const std::string& port)
        {
            std::string result(simulator);
            result += '_';
            result += port;
            return result;
        }

//     int indexScreenshot;
//     void screenshot();
    
//     /* choix de la zone a afficher */
//     pango_limit m_pango_limit_left, m_pango_limit_right;
//     bool m_finish;
//     void draw_selection_box();
//     pointeur pt_box_1, pt_box_2;
//     bool pt_box_press, pt_box_release, m_show_box;
//     bool key_shift;			
    
//     /* interface graphique */
//     Gtk::DrawingArea m_da;
//     Gtk::Button m_btn_save;
//     Gtk::HBox m_hbox;
//     Gtk::VBox m_vbox;
//     Gtk::HPaned m_hpaned;
//     Gtk::HSeparator m_hseparator;
//     Gtk::Label m_lbl;
    
//     /* treeview */
//     Gtk::ScrolledWindow m_ScrolledWindow;
//     Gtk::TreeView m_TreeViewCurve;
//     Glib::RefPtr<TreeModelCurve> m_refTreeModelCurve;
//     bool m_show_treeview;
    
//    Glib::RefPtr<Gdk::GC> m_gc, m_gc_dash, m_gc_pango, m_gc_axis, m_gc_limit_show, m_gc_limit;
//    Pango::FontDescription m_font;
//    Glib::RefPtr < Pango::Layout > m_pango;

    
//     /***************************************/
//     /* Gdk::Window associé au drawing_area */
//     /***************************************/
//     Glib::RefPtr<Gdk::Window> m_wnd;
    
//     /*****************************/
//     /* buffer servant a dessiner */
//     /*****************************/
//     Glib::RefPtr <Gdk::Pixmap> m_buffer;
    
//     /********************/
//     /* efface le buffer */
//     /********************/
//     void clear_buffer();
    
    
//     /*******************************************************/
//     /* creer une couleur a partir de 3 chaine de caractere */
//     /* representant les composante RGB de la couleur       */
//     /*******************************************************/
//     Gdk::Color createColor(Glib::ustring r, Glib::ustring g, Glib::ustring b);
    
    
//     /***********************************/
//     /* ajoute les widgets a la fenetre */	
//     /***********************************/
//     void init_widget();
    
//     /***********************************/
//     /* initialise le model du treeview */
//     /***********************************/
//     void init_treeviewModel();
    
//     /********************************/
//     /* ajoute une ligne au treeview */
//     /********************************/
//     void add_row(std::string const & name, double value, Gdk::Color const & color, Curve * c, bool show);
    
//     /*************************************************************/
//     /* retourne la ligne correspondant au nom passé en parametre */
//     /*************************************************************/
//     Gtk::TreeModel::Row get_row(std::string const & name);
    
//     /**********************************************/
//     /* rempli le treeview avec le nom des courbes */
//     /**********************************************/
//     void init_treeview();
    
//     /***************************************************************/
//     /* met a jour le treeview avec la derniere valeurs des courbes */
//     /***************************************************************/
//     void update_treeview();
    
//     /*************************************************************/
//     /* ouvre un boite de dialogue pour choisir le nom du fichier */
//     /*************************************************************/
//     void on_button_save();
    
//     /**********************************************************************/
//     /* Sauvegarde les données des différentes courbes dans un fichier txt */
//     /**********************************************************************/
//     void save_to_file(Glib::ustring const & filename);
    
//     /***********************************************************************/
//     /* recupere le signal lorsque l'on clique sur une checkbox du treeview */
//     /***********************************************************************/
//     void on_checkbox_toggled(const Glib::ustring& path);
    
//     /**************************************************/
//     /* change la sensibilite des checkbox du treeview */
//     /**************************************************/	
//     void set_checkbox_sensitive(bool sensitive);
    
//     /**************************************************************/
//     /* calcul la valeur minimal affiché de l'ensemble des courbes */
//     /**************************************************************/
   
    
    
//     /***************************/
//     /* creer les differents gc */
//     /***************************/
//     void create_gc();
    
//     /*********/
//     /* MOUSE */
//     /*********/
//     bool button_press_event(GdkEventButton* event);
//     bool button_release_event(GdkEventButton* event);
//     bool motion_notify_event(GdkEventMotion* event);
    
//     /************/
//     /* KEYBOARD */
//     /************/
//     bool on_key_press_event (GdkEventKey* event);
//     bool on_key_release_event (GdkEventKey* event);
    
//     /********************************/
//     /* affiche ou cache le treeview */
//     /********************************/
//     void show_hide_treeview();
    
    
};
	    
}}} // namespace vle oov plugin

DECLARE_OOV_PLUGIN(vle::oov::plugin::Plot);

#endif
