/**
 * @file vle/gvle/PackageBrowserMenu.hpp
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


#ifndef VLE_GVLE_PACKAGEBROWSERMENU_HPP
#define VLE_GVLE_PACKAGEBROWSERMENU_HPP

#include <gtkmm.h>
#include <libglademm.h>

namespace vle { namespace gvle {

    class PackageBrowserWindow;

/**
 * @brief A class to manage the GVLE main menu window.
 */
class PackageBrowserMenu : public Gtk::MenuBar
{
public:
    PackageBrowserMenu(BaseObjectType* cobject,
		       const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/);

    /** Nothing to delete. */
    virtual ~PackageBrowserMenu() {}

    inline void setParent(PackageBrowserWindow* parent)
    { mParent = parent; }

    void makeMenus();

private:
    PackageBrowserWindow*            mParent;
    Gtk::Menu                        mMenuFile;
    Gtk::Menu                        mMenuPackage;

    void makeMenuFile();
    void makeMenuPackage();
};

}} //namespace vle gvle

#endif
