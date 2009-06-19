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

#include <gtkmm.h>
#include <libglademm.h>

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
    GVLEMenu(BaseObjectType* cobject,
	     const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/);

    /** Delete the item. */
    virtual ~GVLEMenu();

    /**
     *
     * Change to Package Mode to access to the Menu item
     *
     */
    void onPackageMode();

    /**
     *
     * Change to Gobal Mode to lock the Menu item
     *
     */
    void onGlobalMode();


    /**
     *
     * Change to View Mode to access to the Menu Edit
     *
     */
    void onViewMode();

    /**
     *
     * Change to File Mode to access to the Menu Edit
     *
     */

    void onFileMode();


    inline void setParent(GVLE* parent)
    { mParent = parent; }

    void makeMenus();


private:
    /**
     * Make menu file and connect signals.
     *
     */
    void makeMenuFile();

     /**
     * Make menu edit and connect signals.
     *
     */
    void makeMenuEdit();

    /**
     * Make menu package and connect signals.
     *
     */
    void makeMenuPackage();

    /**
     * Make menu view and connect signals.
     *
     */
    void makeMenuView();

    /**
     * Make menu simulation and connect signals.
     *
     */
    void makeMenuSimulation();

    /**
     * Make menu zoom and connect signals.
     *
     */
    void makeMenuZoom();

    /**
     * Make menu help and connect signals.
     *
     */
    void makeMenuHelp();

    GVLE*       mParent;

    Gtk::Menu   mMenuFile;
    Gtk::Menu   mMenuEdit;
    Gtk::Menu   mMenuPackage;
    Gtk::Menu   mMenuView;
    Gtk::Menu   mMenuSimulation;
    Gtk::Menu   mMenuZoom;
    Gtk::Menu   mMenuHelp;

    Gtk::MenuItem* mMenuFileOpenVpz;
    Gtk::MenuItem* mMenuFileClear;
    Gtk::MenuItem* mMenuFileImportModel;
    Gtk::MenuItem* mMenuFileExportModel;
    Gtk::MenuItem* mMenuFileExportGraphic;

};

}
} // namespace vle gvle

#endif
