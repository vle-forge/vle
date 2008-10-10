/**
 * @file vle/gvle/ViewMenu.cpp
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


#include <vle/gvle/ViewMenu.hpp>
#include <vle/gvle/View.hpp>
#include <vle/utils/Debug.hpp>
#include <gtkmm/stock.h>

namespace vle
{
namespace gvle {

ViewMenu::ViewMenu(View* v)
{
    assert(v);

    makeMenuFile(v);
    makeMenuEdit(v);

    items().push_back(Gtk::Menu_Helpers::MenuElem("_File", mMenuFile));
    items().push_back(Gtk::Menu_Helpers::MenuElem("Edit", mMenuEdit));
}

void ViewMenu::makeMenuFile(View* v)
{
    Gtk::Menu::MenuList& menuList = mMenuFile.items();

    menuList.push_back(Gtk::Menu_Helpers::StockMenuElem(
                           Gtk::Stock::CLEAR,
                           Gtk::AccelKey(GDK_L, Gdk::CONTROL_MASK),
                           sigc::mem_fun(v, &View::clearCurrentModel)));

    menuList.push_back(Gtk::Menu_Helpers::SeparatorElem());

    menuList.push_back(Gtk::Menu_Helpers::MenuElem(
                           "_Import Model",
                           Gtk::AccelKey(GDK_I, Gdk::CONTROL_MASK),
                           sigc::mem_fun(v, &View::importModel)));

    menuList.push_back(Gtk::Menu_Helpers::MenuElem(
                           "_Export Model",
                           Gtk::AccelKey(GDK_E, Gdk::CONTROL_MASK),
                           sigc::mem_fun(v, &View::exportCurrentModel)));

    menuList.push_back(Gtk::Menu_Helpers::SeparatorElem());

    menuList.push_back(Gtk::Menu_Helpers::StockMenuElem(
                           Gtk::Stock::CLOSE,
                           Gtk::AccelKey(GDK_W, Gdk::CONTROL_MASK),
                           sigc::mem_fun(v, &View::hide)));

}

void ViewMenu::makeMenuEdit(View* v)
{
    Gtk::Menu::MenuList& menuList = mMenuEdit.items();

    menuList.push_back(Gtk::Menu_Helpers::StockMenuElem(
                           Gtk::Stock::CUT,
                           Gtk::AccelKey(GDK_X, Gdk::CONTROL_MASK),
                           sigc::mem_fun(v, &View::onCutModel)));

    menuList.push_back(Gtk::Menu_Helpers::StockMenuElem(
                           Gtk::Stock::COPY,
                           Gtk::AccelKey(GDK_C, Gdk::CONTROL_MASK),
                           sigc::mem_fun(v, &View::onCopyModel)));

    menuList.push_back(Gtk::Menu_Helpers::StockMenuElem(
                           Gtk::Stock::PASTE,
                           Gtk::AccelKey(GDK_V, Gdk::CONTROL_MASK),
                           sigc::mem_fun(v, &View::onPasteModel)));
}

}
} // namespace vle gvle
