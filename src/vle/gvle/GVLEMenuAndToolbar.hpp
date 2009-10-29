/**
 * @file vle/gvle/GVLEMenuAndToolbar.hpp
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

#ifndef VLE_GVLE_GVLEMENUANDTOOLBAR_HPP
#define VLE_GVLE_GVLEMENUANDTOOLBAR_HPP

#include <gtkmm.h>
#include <libglademm.h>

#include <iostream>

namespace vle {
namespace gvle {

class GVLE;

/**
 * @brief A class to manage the GVLE main menu window.
 */
class GVLEMenuAndToolbar
{
public:
    static const Glib::ustring UI_DEFINITION;

    GVLEMenuAndToolbar(GVLE* gvle);

    ~GVLEMenuAndToolbar();

    void setParent(GVLE* gvle)
    { mParent = gvle; }

    Gtk::Toolbar* getToolbar()
    { return mToolbar; }

    Gtk::MenuBar* getMenuBar()
    { return mMenuBar; }

    /**
     * @brief Change to Package Mode to access to the Menu item
     */
    void onPackageMode();

    /**
     * @brief Change to Gobal Mode to lock the Menu item
     */
    void onGlobalMode();

    /**
     * @brief Change to View Mode to access to the Menu Edit
     */
    void onViewMode();

    /**
     * @brief Change to File Mode to access to the Menu Edit
     */
    void onFileMode();

    void hideCloseTab();
    void showCloseTab();
    void hideCloseProject();
    void showCloseProject();

    void hidePackageMenu();
    void showPackageMenu();

private:
    void init();
    void makeButtonsTools();
    void createUI();

    /* Populate actions */
    void createFileActions();
    void createEditActions();
    void createToolsActions();
    void createProjectActions();
    void createViewActions();
    void createSimulationActions();
    void createZoomActions();
    void createHelpActions();

    void on_my_post_activate(const Glib::RefPtr<Gtk::Action>& action);

    /* class members */
    GVLE*            mParent;
    Gtk::Toolbar*    mToolbar;
    Gtk::MenuBar*    mMenuBar;

    Glib::RefPtr<Gtk::UIManager>   m_refUIManager;
    Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;
};

}} // namespace vle gvle

#endif
