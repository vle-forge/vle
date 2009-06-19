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

GVLEMenu::GVLEMenu(BaseObjectType* cobject,
		   const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/) :
    Gtk::MenuBar(cobject)
{ }

void GVLEMenu::makeMenus()
{
    makeMenuFile();
    makeMenuEdit();
    makeMenuPackage();
    makeMenuView();
    makeMenuSimulation();
    makeMenuZoom();
    makeMenuHelp();

    items().push_back(Gtk::Menu_Helpers::MenuElem(_("_File"), mMenuFile));
    items().push_back(Gtk::Menu_Helpers::MenuElem(_("Edit"), mMenuEdit));
    items().push_back(Gtk::Menu_Helpers::MenuElem(_("_Package"),
						  mMenuPackage));
    items().push_back(Gtk::Menu_Helpers::MenuElem(_("_View"), mMenuView));
    items().push_back(Gtk::Menu_Helpers::MenuElem(_("_Sim"), mMenuSimulation));
    items().push_back(Gtk::Menu_Helpers::MenuElem(_("Zoom"), mMenuZoom));
    items().push_back(Gtk::Menu_Helpers::MenuElem(_("_Help"), mMenuHelp));
}

GVLEMenu::~GVLEMenu()
{
    delete mMenuFileOpenVpz;
}

void GVLEMenu::makeMenuFile()
{
    Gtk::Menu::MenuList& menulist = mMenuFile.items();

    menulist.push_back(Gtk::Menu_Helpers::StockMenuElem(
                           Gtk::Stock::NEW,
                           Gtk::AccelKey(GDK_N, Gdk::CONTROL_MASK),
                           sigc::mem_fun(mParent, &GVLE::newFile)));

    menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_New Vpz"),
                           Gtk::AccelKey(GDK_N, Gdk::CONTROL_MASK),
                           sigc::mem_fun(mParent,&GVLE::onMenuNew)));

    menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_New Package"),
                           Gtk::AccelKey(GDK_N, Gdk::CONTROL_MASK |
					 Gdk::SHIFT_MASK),
                           sigc::mem_fun(mParent, &GVLE::onMenuNewProject)));

    menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

    menulist.push_back(Gtk::Menu_Helpers::StockMenuElem(
                           Gtk::Stock::OPEN,
                           Gtk::AccelKey(GDK_O, Gdk::CONTROL_MASK),
                           sigc::mem_fun(mParent, &GVLE::openFile)));

    menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Open Package"),
			   Gtk::AccelKey(GDK_O, Gdk::CONTROL_MASK |
					 Gdk::SHIFT_MASK),
                           sigc::mem_fun(mParent, &GVLE::onMenuOpenPackage)));

    mMenuFileOpenVpz = new Gtk::MenuItem(_("Open Vpz"));
    mMenuFileOpenVpz->set_sensitive(false);
    menulist.push_back(Gtk::Menu_Helpers::MenuElem(*mMenuFileOpenVpz));
    mMenuFileOpenVpz->signal_activate().connect(
	sigc::mem_fun(mParent, &GVLE::onMenuOpenVpz));

    menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Open Global Vpz"),
                           Gtk::AccelKey(GDK_O, Gdk::CONTROL_MASK),
			   sigc::mem_fun(mParent, &GVLE::onMenuLoad)));

    menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Save"),
			   Gtk::AccelKey(GDK_S, Gdk::CONTROL_MASK),
			   sigc::mem_fun(mParent, &GVLE::onMenuSave)));

    menulist.push_back(Gtk::Menu_Helpers::StockMenuElem(
                           Gtk::Stock::SAVE,
                           Gtk::AccelKey(GDK_S, Gdk::CONTROL_MASK),
			   sigc::mem_fun(mParent, &GVLE::saveFile)));

    menulist.push_back(Gtk::Menu_Helpers::StockMenuElem(
                           Gtk::Stock::SAVE_AS,
                           Gtk::AccelKey(GDK_S, Gdk::CONTROL_MASK),
                           sigc::mem_fun(mParent,&GVLE::saveFileAs)));

    menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

    mMenuFileClear = new Gtk::MenuItem(_("Clear"));
    mMenuFileClear->set_sensitive(false);
    menulist.push_back(Gtk::Menu_Helpers::MenuElem(*mMenuFileClear));
    mMenuFileClear->signal_activate().connect(
	sigc::mem_fun(mParent, &GVLE::clearCurrentModel));


    mMenuFileImportModel = new Gtk::MenuItem(_("Import Model"));
    mMenuFileImportModel->set_sensitive(false);
    menulist.push_back(Gtk::Menu_Helpers::MenuElem(*mMenuFileImportModel));
    mMenuFileImportModel->signal_activate().connect(
	sigc::mem_fun(mParent, &GVLE::importModel));

    mMenuFileExportModel = new Gtk::MenuItem(_("Export Model"));
    mMenuFileExportModel->set_sensitive(false);
    menulist.push_back(Gtk::Menu_Helpers::MenuElem(*mMenuFileExportModel));
    mMenuFileExportModel->signal_activate().connect(
	sigc::mem_fun(mParent, &GVLE::exportCurrentModel));

    mMenuFileExportGraphic = new Gtk::MenuItem(_("Export Graphic"));
    mMenuFileExportGraphic->set_sensitive(false);
    menulist.push_back(Gtk::Menu_Helpers::MenuElem(*mMenuFileExportGraphic));
    mMenuFileExportGraphic->signal_activate().connect(
	sigc::mem_fun(mParent, &GVLE::exportGraphic));

    menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

    menulist.push_back(Gtk::Menu_Helpers::StockMenuElem(
                           Gtk::Stock::CLOSE,
                           Gtk::AccelKey(GDK_W, Gdk::CONTROL_MASK),
			   sigc::mem_fun(mParent, &GVLE::closeFile)));

    menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Quit"),
                           Gtk::AccelKey(GDK_Q, Gdk::CONTROL_MASK |
					 Gdk::SHIFT_MASK),
                           //sigc::mem_fun(gvle,
                           //&Gtk::Window::hide)));
			   sigc::mem_fun(mParent, &GVLE::onMenuQuit)));
}

void GVLEMenu::makeMenuEdit()
{
    Gtk::Menu::MenuList& menuList = mMenuEdit.items();

    menuList.push_back(Gtk::Menu_Helpers::StockMenuElem(
                           Gtk::Stock::CUT,
                           Gtk::AccelKey(GDK_X, Gdk::CONTROL_MASK),
                           sigc::mem_fun(mParent, &GVLE::onCutModel)));

    menuList.push_back(Gtk::Menu_Helpers::StockMenuElem(
                           Gtk::Stock::COPY,
                           Gtk::AccelKey(GDK_C, Gdk::CONTROL_MASK),
                           sigc::mem_fun(mParent, &GVLE::onCopyModel)));

    menuList.push_back(Gtk::Menu_Helpers::StockMenuElem(
                           Gtk::Stock::PASTE,
                           Gtk::AccelKey(GDK_V, Gdk::CONTROL_MASK),
                           sigc::mem_fun(mParent, &GVLE::onPasteModel)));
}

void GVLEMenu::makeMenuPackage()
{
    Gtk::Menu::MenuList& menuList = mMenuPackage.items();

    menuList.push_back(Gtk::Menu_Helpers::MenuElem(
			   _("_Configure project"),
			   sigc::mem_fun(mParent, &GVLE::configureProject)));

    menuList.push_back(Gtk::Menu_Helpers::MenuElem(
			   _("_Build project"),
			   sigc::mem_fun(mParent, &GVLE::buildProject)));

    menuList.push_back(Gtk::Menu_Helpers::MenuElem(
			   _("_C_lean project"),
			   sigc::mem_fun(mParent, &GVLE::cleanProject)));

    menuList.push_back(Gtk::Menu_Helpers::MenuElem(
			   _("Create project _package"),
			   sigc::mem_fun(mParent, &GVLE::packageProject)));
}


void GVLEMenu::makeMenuView()
{
    Gtk::Menu::MenuList& menulist = mMenuView.items();

    menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Root tree"),
                       Gtk::AccelKey(GDK_R, Gdk::CONTROL_MASK),
                       sigc::mem_fun(mParent, &GVLE::onShowModelTreeView)));

    menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Class models"),
                       Gtk::AccelKey(GDK_M, Gdk::CONTROL_MASK),
                       sigc::mem_fun(mParent, &GVLE::onShowModelClassView)));

    menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Package Browser"),
                       Gtk::AccelKey(GDK_P, Gdk::CONTROL_MASK),
                       sigc::mem_fun(mParent, &GVLE::onShowPackageBrowserWindow)));

    menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(_("_Iconify all views"),
                                    Gtk::AccelKey(GDK_H, Gdk::CONTROL_MASK),
            sigc::mem_fun(mParent, &GVLE::onIconifyAllViews)));

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(_("_Deiconify all views"),
                                    Gtk::AccelKey(GDK_D, Gdk::CONTROL_MASK),
            sigc::mem_fun(mParent, &GVLE::onDeiconifyAllViews)));

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(_("_Close all views"),
                                    Gtk::AccelKey(GDK_W, Gdk::CONTROL_MASK),
            sigc::mem_fun(mParent, &GVLE::onCloseAllViews)));

    menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(_("Pre_ferences"),
                                    Gtk::AccelKey(GDK_F, Gdk::CONTROL_MASK),
            sigc::mem_fun(mParent, &GVLE::onPreferences)));
}

void GVLEMenu::makeMenuSimulation()
{
    Gtk::Menu::MenuList& menulist = mMenuSimulation.items();

    menulist.push_back(
	Gtk::Menu_Helpers::MenuElem(_("_Project"),
				    Gtk::AccelKey(GDK_P,
						  Gdk::CONTROL_MASK),
				    sigc::mem_fun(mParent,
						  &GVLE::onExperimentsBox)));

    menulist.push_back(Gtk::Menu_Helpers::MenuElem(
			   _("_Views"), Gtk::AccelKey(GDK_Z, Gdk::CONTROL_MASK),
			   sigc::mem_fun(mParent, &GVLE::onViewOutputBox)));

    menulist.push_back(Gtk::Menu_Helpers::MenuElem
		       (_("_Conditions"), Gtk::AccelKey(GDK_C, Gdk::CONTROL_MASK),
			sigc::mem_fun(mParent, &GVLE::onConditionsBox)));

    menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

    menulist.push_back(
	Gtk::Menu_Helpers::MenuElem(_("H_osts"),
				    sigc::mem_fun(mParent, &GVLE::onHostsBox)));

    menulist.push_back(
	Gtk::Menu_Helpers::MenuElem(
	    _("_Launch Simulation"), Gtk::AccelKey(GDK_L, Gdk::CONTROL_MASK),
	    sigc::mem_fun(mParent, &GVLE::onSimulationBox)));
}

void GVLEMenu::makeMenuZoom()
{
    Gtk::Menu::MenuList& menuList = mMenuZoom.items();
    menuList.push_back(Gtk::Menu_Helpers::MenuElem(
			   _("Zoom +"),
			   Gtk::AccelKey(GDK_plus, Gdk::CONTROL_MASK),
			   sigc::mem_fun(mParent, &GVLE::addCoefZoom)));


    menuList.push_back(Gtk::Menu_Helpers::MenuElem(
			   _("Zoom -"),
			   Gtk::AccelKey(GDK_minus, Gdk::CONTROL_MASK),
			   sigc::mem_fun(mParent, &GVLE::delCoefZoom)));

    menuList.push_back(Gtk::Menu_Helpers::SeparatorElem());

    menuList.push_back(Gtk::Menu_Helpers::MenuElem(
			   _("Zoom 1:1"),
			   Gtk::AccelKey(GDK_equal, Gdk::CONTROL_MASK),
			   sigc::bind(sigc::mem_fun(
					  mParent, &GVLE::setCoefZoom), 1.0)));

    menuList.push_back(Gtk::Menu_Helpers::MenuElem(
			   _("Zoom 1:2"),
			   Gtk::AccelKey(GDK_1, Gdk::CONTROL_MASK),
			   sigc::bind(sigc::mem_fun(
					  mParent, &GVLE::setCoefZoom), 0.5)));

    menuList.push_back(Gtk::Menu_Helpers::MenuElem(
			   _("Zoom 2:1"),
			   Gtk::AccelKey(GDK_2, Gdk::CONTROL_MASK),
			   sigc::bind(sigc::mem_fun(
					  mParent, &GVLE::setCoefZoom), 2)));
}

void GVLEMenu::makeMenuHelp()
{
    Gtk::Menu::MenuList& menulist = mMenuHelp.items();

    menulist.push_back(
	Gtk::Menu_Helpers::MenuElem(_("About"),
				    sigc::mem_fun(mParent, &GVLE::onShowAbout)));
}

void GVLEMenu::onPackageMode()
{
   mMenuFileOpenVpz->set_sensitive(true);
}

void GVLEMenu::onGlobalMode()
{
    mMenuFileOpenVpz->set_sensitive(false);
}

void GVLEMenu::onViewMode()
{
    mMenuEdit.set_sensitive(true);
    mMenuZoom.set_sensitive(true);
    mMenuFileClear->set_sensitive(true);
    mMenuFileImportModel->set_sensitive(true);
    mMenuFileExportModel->set_sensitive(true);
    mMenuFileExportGraphic->set_sensitive(true);
}

void GVLEMenu::onFileMode()
{
    mMenuEdit.set_sensitive(false);
    mMenuZoom.set_sensitive(false);
    mMenuFileClear->set_sensitive(false);
    mMenuFileImportModel->set_sensitive(false);
    mMenuFileExportModel->set_sensitive(false);
    mMenuFileExportGraphic->set_sensitive(false);
}

} } // namespace vle gvle
