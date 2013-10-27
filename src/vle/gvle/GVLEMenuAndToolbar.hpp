/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef VLE_GVLE_GVLEMENUANDTOOLBAR_HPP
#define VLE_GVLE_GVLEMENUANDTOOLBAR_HPP

#include <gtkmm/builder.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/menubar.h>
#include <gtkmm/menu.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/actiongroup.h>
#include <gdk/gdkkeysyms-compat.h>

namespace vle { namespace gvle {

class GVLE;

/**
 * @brief A class to manage the GVLE main menu window.
 */
class GVLEMenuAndToolbar
{
public:
    static const Glib::ustring UI_DEFINITION;

    GVLEMenuAndToolbar(GVLE* gvle);

    virtual ~GVLEMenuAndToolbar();

    void setParent(GVLE* gvle)
    { mParent = gvle; }

    Gtk::Toolbar* getToolbar()
    { return mToolbar; }

    Gtk::MenuBar* getMenuBar()
    { return mMenuBar; }

    Glib::RefPtr<Gtk::UIManager> getUIManager()
    { return m_refUIManager; }

    Glib::RefPtr<Gtk::ActionGroup> getActionGroup()
    { return  m_refActionGroup; }

    // Menu item
    void hideCopyCut();
    void showCopyCut();
    void showPaste();
    void showOpenVpz();
    void hideSave();
    void showSave();
    void hideSaveAs();
    void showSaveAs();
    void hideCloseTab();
    void showCloseTab();
    void hideCloseProject();
    void showCloseProject();
    void hideImportExport();
    void showImportExport();
    void showRefresh();

    // Menu
    void showMinimalMenu();

    void hideFileModel();

    void hideEditMenu();
    void showEditMenu();

    void hideToolsMenu();
    void showToolsMenu();

    void hideProjectMenu();
    void showProjectMenu();

    void hideViewMenu();
    void showViewMenu();

    void hideSimulationMenu();
    void showSimulationMenu();

    void hideZoomMenu();
    void showZoomMenu();

    // show/hide on actions
    void onOpenFile();
    void onOpenProject();
    void onOpenVpz();

    void onCloseTab(bool vpz, bool empty);

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
