/**
 * @file vle/gvle/ViewMenu.hpp
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


#ifndef GUI_VIEWMENU_HPP
#define GUI_VIEWMENU_HPP

#include <gtkmm/menubar.h>
#include <gtkmm/menu.h>

namespace vle
{
namespace gvle {

class View;

/**
 * @brief A Gtk::MenuBar to draw menu of the gui::View. This menu is
 * attached to the gui::View window and perform manunipulation on
 * gui::ViewDrawingArea.
 */
class ViewMenu : public Gtk::MenuBar
{
public:
    /**
     * Create menubar for View with menus file and edit.
     *
     * @param v a ptr to View to connect signals
     */
    ViewMenu(View* v);

    /** Nothing to delete */
    virtual ~ViewMenu() {}

private:
    /**
     * Make menu file and connect signals
     *
     * @param v a ptr to View to connect signals
     */
    void makeMenuFile(View* v);

    /**
     * Make menu edit and connect signals
     *
     * @param v a ptr to View to connect signals
     */
    void makeMenuEdit(View* v);

    /**
     * Make menu zoom and connect signals
     *
     */
    void makeMenuZoom(View* v);


    Gtk::Menu   mMenuFile;
    Gtk::Menu   mMenuEdit;
    Gtk::Menu   mMenuZoom;
    View*       m_view;

    Gtk::MenuItem* mMenuZoomPlus;
};

}
} // namespace vle gvle

#endif
