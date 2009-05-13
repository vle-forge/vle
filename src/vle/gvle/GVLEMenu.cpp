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

    items().push_back(Gtk::Menu_Helpers::MenuElem("_File", mMenuFile));
    items().push_back(Gtk::Menu_Helpers::MenuElem("_View", mMenuView));
    items().push_back(Gtk::Menu_Helpers::MenuElem("_Sim", mMenuSimulation));
    items().push_back(Gtk::Menu_Helpers::MenuElem("_Help", mMenuHelp));
}

void GVLEMenu::makeMenuFile(GVLE* gvle)
{
    Gtk::Menu::MenuList& menulist = mMenuFile.items();

    menulist.push_back(Gtk::Menu_Helpers::StockMenuElem(
                           Gtk::Stock::NEW,
                           Gtk::AccelKey(GDK_N, Gdk::CONTROL_MASK),
                           sigc::mem_fun(gvle,&GVLE::onMenuNew)));

    menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

    menulist.push_back(Gtk::Menu_Helpers::StockMenuElem(
                           Gtk::Stock::OPEN,
                           Gtk::AccelKey(GDK_O, Gdk::CONTROL_MASK),
                           sigc::mem_fun(gvle, &GVLE::onMenuLoad)));

    menulist.push_back(Gtk::Menu_Helpers::StockMenuElem(
                           Gtk::Stock::SAVE,
                           Gtk::AccelKey(GDK_S, Gdk::CONTROL_MASK),
                           sigc::mem_fun(gvle, &GVLE::onMenuSave)));

    menulist.push_back(Gtk::Menu_Helpers::StockMenuElem(
                           Gtk::Stock::SAVE_AS,
                           Gtk::AccelKey(GDK_S, Gdk::CONTROL_MASK),
                           sigc::mem_fun(gvle, &GVLE::onMenuSaveAs)));

    menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

    menulist.push_back(Gtk::Menu_Helpers::StockMenuElem(
                           Gtk::Stock::QUIT,
                           Gtk::AccelKey(GDK_Q, Gdk::CONTROL_MASK |
					 Gdk::SHIFT_MASK),
                           sigc::mem_fun(gvle, &Gtk::Window::hide)));
}

void GVLEMenu::makeMenuView(GVLE* gvle)
{
    Gtk::Menu::MenuList& menulist = mMenuView.items();

    menulist.push_back(Gtk::Menu_Helpers::MenuElem("_Root tree",
                       Gtk::AccelKey(GDK_R, Gdk::CONTROL_MASK),
                       sigc::mem_fun(gvle, &GVLE::onShowModelTreeView)));

    menulist.push_back(Gtk::Menu_Helpers::MenuElem("_Class models",
                       Gtk::AccelKey(GDK_M, Gdk::CONTROL_MASK),
                       sigc::mem_fun(gvle, &GVLE::onShowModelClassView)));

    menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem("_Iconify all views",
                                    Gtk::AccelKey(GDK_H, Gdk::CONTROL_MASK),
            sigc::mem_fun(gvle, &GVLE::onIconifyAllViews)));

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem("_Deiconify all views",
                                    Gtk::AccelKey(GDK_D, Gdk::CONTROL_MASK),
            sigc::mem_fun(gvle, &GVLE::onDeiconifyAllViews)));

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem("_Close all views",
                                    Gtk::AccelKey(GDK_W, Gdk::CONTROL_MASK),
            sigc::mem_fun(gvle, &GVLE::onCloseAllViews)));

    menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem("Pre_ferences",
                                    Gtk::AccelKey(GDK_F, Gdk::CONTROL_MASK),
            sigc::mem_fun(gvle, &GVLE::onPreferences)));
}

void GVLEMenu::makeMenuSimulation(GVLE* gvle)
{
    Gtk::Menu::MenuList& menulist = mMenuSimulation.items();

    menulist.push_back(
	Gtk::Menu_Helpers::MenuElem("_Project",
				    Gtk::AccelKey(GDK_P,
						  Gdk::CONTROL_MASK),
				    sigc::mem_fun(gvle,
						  &GVLE::onExperimentsBox)));

    menulist.push_back(Gtk::Menu_Helpers::MenuElem(
			   "_Views", Gtk::AccelKey(GDK_Z, Gdk::CONTROL_MASK),
			   sigc::mem_fun(gvle, &GVLE::onViewOutputBox)));

    menulist.push_back(Gtk::Menu_Helpers::MenuElem
		       ("_Conditions", Gtk::AccelKey(GDK_C, Gdk::CONTROL_MASK),
			sigc::mem_fun(gvle, &GVLE::onConditionsBox)));

    menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

    menulist.push_back(
	Gtk::Menu_Helpers::MenuElem("H_osts",
				    sigc::mem_fun(gvle, &GVLE::onHostsBox)));

    menulist.push_back(
	Gtk::Menu_Helpers::MenuElem(
	    "_Launch Simulation", Gtk::AccelKey(GDK_L, Gdk::CONTROL_MASK),
	    sigc::mem_fun(gvle, &GVLE::onSimulationBox)));
}

void GVLEMenu::makeMenuHelp(GVLE* gvle)
{
    Gtk::Menu::MenuList& menulist = mMenuHelp.items();

    menulist.push_back(
	Gtk::Menu_Helpers::MenuElem("About",
				    sigc::mem_fun(gvle, &GVLE::onShowAbout)));
}

} } // namespace vle gvle
