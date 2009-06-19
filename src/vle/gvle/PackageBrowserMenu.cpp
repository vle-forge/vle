/**
 * @file vle/gvle/PackageBrowserMenu.cpp
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

#include <vle/gvle/PackageBrowserMenu.hpp>
#include <vle/gvle/PackageBrowserWindow.hpp>
#include <vle/utils/i18n.hpp>
#include <gtkmm/stock.h>

namespace vle { namespace gvle {

PackageBrowserMenu::PackageBrowserMenu(
    BaseObjectType* cobject,
    const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/) :
    Gtk::MenuBar(cobject)
{}

void PackageBrowserMenu::makeMenus()
{
    makeMenuFile();
    makeMenuPackage();

    items().push_back(
	Gtk::Menu_Helpers::MenuElem(_("_File"), mMenuFile));
    items().push_back(
	Gtk::Menu_Helpers::MenuElem(_("_Package"), mMenuPackage));
}

void PackageBrowserMenu::makeMenuFile()
{
    Gtk::Menu::MenuList& menuList = mMenuFile.items();

    menuList.push_back(Gtk::Menu_Helpers::StockMenuElem(
                           Gtk::Stock::NEW,
                           Gtk::AccelKey(GDK_N, Gdk::CONTROL_MASK)/*,
			   sigc::mem_fun(mParent, &PackageBrowserWindow::newFile)*/));

    menuList.push_back(Gtk::Menu_Helpers::SeparatorElem());

    menuList.push_back(Gtk::Menu_Helpers::StockMenuElem(
                           Gtk::Stock::OPEN,
                           Gtk::AccelKey(GDK_O, Gdk::CONTROL_MASK)/*,
                           sigc::mem_fun(mParent, &PackageBrowserWindow::openFile)*/));

    menuList.push_back(Gtk::Menu_Helpers::StockMenuElem(
                           Gtk::Stock::SAVE,
                           Gtk::AccelKey(GDK_S, Gdk::CONTROL_MASK)/*,
                           sigc::mem_fun(mParent, &PackageBrowserWindow::saveFile)*/));

    menuList.push_back(Gtk::Menu_Helpers::StockMenuElem(
                           Gtk::Stock::SAVE_AS,
                           Gtk::AccelKey(GDK_S, Gdk::CONTROL_MASK)/*,
                           sigc::mem_fun(mParent, &PackageBrowserWindow::saveFileAs)*/));

    menuList.push_back(Gtk::Menu_Helpers::StockMenuElem(
                           Gtk::Stock::CLOSE,
                           Gtk::AccelKey(GDK_W, Gdk::CONTROL_MASK)/*,
			   sigc::mem_fun(mParent, &PackageBrowserWindow::closeFile)*/));

    menuList.push_back(Gtk::Menu_Helpers::SeparatorElem());

    menuList.push_back(Gtk::Menu_Helpers::StockMenuElem(
                           Gtk::Stock::QUIT,
                           Gtk::AccelKey(GDK_Q, Gdk::CONTROL_MASK)/*,
			   sigc::mem_fun(mParent, &PackageBrowserWindow::hide)*/));
}

void PackageBrowserMenu::makeMenuPackage()
{
    Gtk::Menu::MenuList& menuList = mMenuPackage.items();

    menuList.push_back(Gtk::Menu_Helpers::MenuElem(
			   _("_Configure project")/*,
			   sigc::mem_fun(mParent, &PackageBrowserWindow::configureProject)*/));

    menuList.push_back(Gtk::Menu_Helpers::MenuElem(
			   _("_Build project")/*,
			   sigc::mem_fun(mParent, &PackageBrowserWindow::buildProject)*/));

    menuList.push_back(Gtk::Menu_Helpers::MenuElem(
			   _("_C_lean project")/*,
			   sigc::mem_fun(mParent, &PackageBrowserWindow::cleanProject)*/));

    menuList.push_back(Gtk::Menu_Helpers::MenuElem(
			   _("Create project _package")/*,
			   sigc::mem_fun(mParent, &PackageBrowserWindow::packageProject)*/));
}

}}
