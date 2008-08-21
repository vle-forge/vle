/**
 * @file vle/gvle/GVLEMenu.hpp
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


#ifndef GUI_GVLEMENU_HPP
#define GUI_GVLEMENU_HPP

#include <gtkmm/menubar.h>
#include <gtkmm/menu.h>

namespace vle
{
namespace gvle {

class GVLE;

/**
 * @brief A class to manage the GVLE main menu window.
 */
class GVLEMenu : public Gtk::MenuBar
{
public:
    /**
     * Create menubar for GVLE with menus file, view, simulation and help.
     *
     * @param gvle a ptr to GVLE to connect signals
     */
    GVLEMenu(GVLE* gvle);

    /** Nothing to delete. */
    virtual ~GVLEMenu() {}

private:
    /**
     * Make menu file and connect signals.
     *
     * @param gvle a ptr to GVLE to connect signals.
     */
    void makeMenuFile(GVLE* gvle);

    /**
     * Make menu view and connect signals.
     *
     * @param gvle a ptr to GVLE to connect signals.
     */
    void makeMenuView(GVLE* gvle);

    /**
     * Make menu simulation and connect signals.
     *
     * @param gvle a ptr to GVLE to connect signals.
     */
    void makeMenuSimulation(GVLE* gvle);

    /**
     * Make menu help and connect signals.
     *
     * @param gvle a ptr to GVLE to connect signals.
     */
    void makeMenuHelp(GVLE* gvle);

    Gtk::Menu   mMenuFile;
    Gtk::Menu   mMenuView;
    Gtk::Menu   mMenuSimulation;
    Gtk::Menu   mMenuHelp;
};

}
} // namespace vle gvle

#endif
