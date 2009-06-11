/**
 * @file vle/gvle/GVLEMenu.cpp
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


#include <vle/gvle/GVLEMenu.hpp>
#include <vle/gvle/GVLE.hpp>
#include <vle/utils/Debug.hpp>
#include <gtkmm/stock.h>

namespace vle { namespace gvle {

GVLEMenu::GVLEMenu(GVLE* gvle)
{
    makeMenuFile(gvle);
    makeMenuView(gvle);
    makeMenuSimulation(gvle);
    makeMenuHelp(gvle);

    items().push_back(Gtk::Menu_Helpers::MenuElem(_("_File"), mMenuFile));
    items().push_back(Gtk::Menu_Helpers::MenuElem(_("_View"), mMenuView));
    items().push_back(Gtk::Menu_Helpers::MenuElem(_("_Sim"), mMenuSimulation));
    items().push_back(Gtk::Menu_Helpers::MenuElem(_("_Help"), mMenuHelp));
}

GVLEMenu::~GVLEMenu()
{
    delete mMenuFileOpenVpz;
}

void GVLEMenu::makeMenuFile(GVLE* gvle)
{
    Gtk::Menu::MenuList& menulist = mMenuFile.items();

    menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_New Vpz"),
                           Gtk::AccelKey(GDK_N, Gdk::CONTROL_MASK),
                           sigc::mem_fun(gvle,&GVLE::onMenuNew)));

    menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_New Package"),
                           Gtk::AccelKey(GDK_N, Gdk::CONTROL_MASK |
					 Gdk::SHIFT_MASK),
                           sigc::mem_fun(gvle, &GVLE::onMenuNewProject)));

    menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

    menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Open Package"),
			   Gtk::AccelKey(GDK_O, Gdk::CONTROL_MASK |
					 Gdk::SHIFT_MASK),
                           sigc::mem_fun(gvle, &GVLE::onMenuOpenPackage)));

    mMenuFileOpenVpz = new Gtk::MenuItem(_("Open Vpz"));
    mMenuFileOpenVpz->set_sensitive(false);
    menulist.push_back(Gtk::Menu_Helpers::MenuElem(*mMenuFileOpenVpz));
    mMenuFileOpenVpz->signal_activate().connect(
	sigc::mem_fun(gvle, &GVLE::onMenuOpenVpz));

    menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Open Global Vpz"),
                           Gtk::AccelKey(GDK_O, Gdk::CONTROL_MASK),
			   sigc::mem_fun(gvle, &GVLE::onMenuLoad)));

    menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Save"),
			   Gtk::AccelKey(GDK_S, Gdk::CONTROL_MASK),
			   sigc::mem_fun(gvle, &GVLE::onMenuSave)));


    menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

    menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Quit"),
                           Gtk::AccelKey(GDK_Q, Gdk::CONTROL_MASK |
					 Gdk::SHIFT_MASK),
                           //sigc::mem_fun(gvle,
                           //&Gtk::Window::hide)));
			   sigc::mem_fun(gvle, &GVLE::onMenuQuit)));
}

void GVLEMenu::makeMenuView(GVLE* gvle)
{
    Gtk::Menu::MenuList& menulist = mMenuView.items();

    menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Root tree"),
                       Gtk::AccelKey(GDK_R, Gdk::CONTROL_MASK),
                       sigc::mem_fun(gvle, &GVLE::onShowModelTreeView)));

    menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Class models"),
                       Gtk::AccelKey(GDK_M, Gdk::CONTROL_MASK),
                       sigc::mem_fun(gvle, &GVLE::onShowModelClassView)));

    menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Package Browser"),
                       Gtk::AccelKey(GDK_P, Gdk::CONTROL_MASK),
                       sigc::mem_fun(gvle, &GVLE::onShowPackageBrowserWindow)));

    menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(_("_Iconify all views"),
                                    Gtk::AccelKey(GDK_H, Gdk::CONTROL_MASK),
            sigc::mem_fun(gvle, &GVLE::onIconifyAllViews)));

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(_("_Deiconify all views"),
                                    Gtk::AccelKey(GDK_D, Gdk::CONTROL_MASK),
            sigc::mem_fun(gvle, &GVLE::onDeiconifyAllViews)));

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(_("_Close all views"),
                                    Gtk::AccelKey(GDK_W, Gdk::CONTROL_MASK),
            sigc::mem_fun(gvle, &GVLE::onCloseAllViews)));

    menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(_("Pre_ferences"),
                                    Gtk::AccelKey(GDK_F, Gdk::CONTROL_MASK),
            sigc::mem_fun(gvle, &GVLE::onPreferences)));
}

void GVLEMenu::makeMenuSimulation(GVLE* gvle)
{
    Gtk::Menu::MenuList& menulist = mMenuSimulation.items();

    menulist.push_back(
	Gtk::Menu_Helpers::MenuElem(_("_Project"),
				    Gtk::AccelKey(GDK_P,
						  Gdk::CONTROL_MASK),
				    sigc::mem_fun(gvle,
						  &GVLE::onExperimentsBox)));

    menulist.push_back(Gtk::Menu_Helpers::MenuElem(
			   _("_Views"), Gtk::AccelKey(GDK_Z, Gdk::CONTROL_MASK),
			   sigc::mem_fun(gvle, &GVLE::onViewOutputBox)));

    menulist.push_back(Gtk::Menu_Helpers::MenuElem
		       (_("_Conditions"), Gtk::AccelKey(GDK_C, Gdk::CONTROL_MASK),
			sigc::mem_fun(gvle, &GVLE::onConditionsBox)));

    menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

    menulist.push_back(
	Gtk::Menu_Helpers::MenuElem(_("H_osts"),
				    sigc::mem_fun(gvle, &GVLE::onHostsBox)));

    menulist.push_back(
	Gtk::Menu_Helpers::MenuElem(
	    _("_Launch Simulation"), Gtk::AccelKey(GDK_L, Gdk::CONTROL_MASK),
	    sigc::mem_fun(gvle, &GVLE::onSimulationBox)));
}

void GVLEMenu::makeMenuHelp(GVLE* gvle)
{
    Gtk::Menu::MenuList& menulist = mMenuHelp.items();

    menulist.push_back(
	Gtk::Menu_Helpers::MenuElem(_("About"),
				    sigc::mem_fun(gvle, &GVLE::onShowAbout)));
}

void GVLEMenu::onPackageMode()
{
   mMenuFileOpenVpz->set_sensitive(true);
}

void GVLEMenu::onGlobalMode()
{
    mMenuFileOpenVpz->set_sensitive(false);
}

} } // namespace vle gvle
