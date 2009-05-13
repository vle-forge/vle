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

ViewMenu::ViewMenu(View* v) :
    m_view(v)
{
    assert(v);

    makeMenuFile(v);
    makeMenuEdit(v);
    makeMenuTools();

    items().push_back(Gtk::Menu_Helpers::MenuElem(_("_File"), mMenuFile));
    items().push_back(Gtk::Menu_Helpers::MenuElem(_("Edit"), mMenuEdit));
    items().push_back(Gtk::Menu_Helpers::MenuElem(_("_Tools"), mMenuTools));
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
                           _("_Import Model"),
                           Gtk::AccelKey(GDK_I, Gdk::CONTROL_MASK),
                           sigc::mem_fun(v, &View::importModel)));

    menuList.push_back(Gtk::Menu_Helpers::MenuElem(
                           _("_Export Model"),
                           Gtk::AccelKey(GDK_E, Gdk::CONTROL_MASK),
                           sigc::mem_fun(v, &View::exportCurrentModel)));

    menuList.push_back(Gtk::Menu_Helpers::MenuElem(
			   _("_Export Graphic"),
			   Gtk::AccelKey(GDK_L, Gdk::CONTROL_MASK),
			   sigc::mem_fun(v, &View::exportGraphic)));

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

void ViewMenu::makeMenuTools()
{
    Gtk::Menu::MenuList& menuList = mMenuTools.items();

    menuList.push_back(Gtk::Menu_Helpers::MenuElem(
			   _("_Selection"),
			   Gtk::AccelKey("F1"),
			   sigc::mem_fun(this, &ViewMenu::onArrow)));

    menuList.push_back(Gtk::Menu_Helpers::MenuElem(
			   _("_AtomicModel"),
			   Gtk::AccelKey("F2"),
			   sigc::mem_fun(this, &ViewMenu::onAddModels)));

    menuList.push_back(Gtk::Menu_Helpers::MenuElem(
			   _("Add _links"),
			   Gtk::AccelKey("F3"),
			   sigc::mem_fun(this, &ViewMenu::onAddLinks)));

    menuList.push_back(Gtk::Menu_Helpers::MenuElem(
			   _("_CoupledModel"),
			   Gtk::AccelKey("F4"),
			   sigc::mem_fun(this, &ViewMenu::onAddCoupled)));

    menuList.push_back(Gtk::Menu_Helpers::MenuElem(
			   _("_Delete"),
			   Gtk::AccelKey("F5"),
			   sigc::mem_fun(this, &ViewMenu::onDelete)));

    menuList.push_back(Gtk::Menu_Helpers::MenuElem(
			   _("_Zoom"),
			   Gtk::AccelKey("F6"),
			   sigc::mem_fun(this, &ViewMenu::onZoom)));

    menuList.push_back(Gtk::Menu_Helpers::MenuElem(
			   _("_Question"),
			   Gtk::AccelKey("F7"),
			   sigc::mem_fun(this, &ViewMenu::onQuestion)));
}

void ViewMenu::onArrow()
{
    GVLE* gvle = m_view->getModeling()->getGVLE();
    gvle->getButtonRef(GVLE::POINTER)->set_active(true);
}

void ViewMenu::onAddModels()
{
    GVLE* gvle = m_view->getModeling()->getGVLE();
    gvle->getButtonRef(GVLE::ADDMODEL)->set_active(true);
}

void ViewMenu::onAddLinks()
{
    GVLE* gvle = m_view->getModeling()->getGVLE();
    gvle->getButtonRef(GVLE::ADDLINK)->set_active(true);
}

void ViewMenu::onDelete()
{
    GVLE* gvle = m_view->getModeling()->getGVLE();
    gvle->getButtonRef(GVLE::DELETE)->set_active(true);
}

void ViewMenu::onAddCoupled()
{
    GVLE* gvle = m_view->getModeling()->getGVLE();
    gvle->getButtonRef(GVLE::ADDCOUPLED)->set_active(true);
}

void ViewMenu::onZoom()
{
    GVLE* gvle = m_view->getModeling()->getGVLE();
    gvle->getButtonRef(GVLE::ZOOM)->set_active(true);
}

void ViewMenu::onQuestion()
{
    GVLE* gvle = m_view->getModeling()->getGVLE();
    gvle->getButtonRef(GVLE::QUESTION)->set_active(true);
}

}
} // namespace vle gvle
