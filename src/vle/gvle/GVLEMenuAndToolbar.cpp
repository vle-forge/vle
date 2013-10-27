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


#include <vle/gvle/GVLEMenuAndToolbar.hpp>
#include <vle/gvle/Editor.hpp>
#include <vle/gvle/GVLE.hpp>
#include <gtkmm/stock.h>
#include <gtkmm/radioaction.h>
#include <iostream>

#ifdef __WIN32__
# ifdef DELETE
#  undef DELETE
# endif
#endif

namespace vle { namespace gvle {

const Glib::ustring GVLEMenuAndToolbar::UI_DEFINITION =
    "<ui>"
    "    <menubar name='MenuBar'>"
    "        <menu action='MenuFile'>"
    "            <menuitem action='NewProject'/>"
    "            <menuitem action='NewVpz'/>"
    "            <separator/>"
    "            <menuitem action='OpenProject'/>"
    "            <menuitem action='OpenVpz'/>"
    "            <separator/>"
    "            <menuitem action='SaveFile'/>"
    "            <menuitem action='SaveFileAs'/>"
    "            <separator />"
    "            <menuitem action='ClearModel'/>"
    "            <menuitem action='ImportModel'/>"
    "            <menuitem action='ExportModel'/>"
    "            <menuitem action='ExportGraphic'/>"
    "            <separator />"
    "            <menuitem action='CloseTab'/>"
    "            <menuitem action='CloseProject'/>"
    "            <menuitem action='Quit'/>"
    "        </menu>"
    "        <menu action='MenuEdit'>"
    "            <menuitem action='Undo' />"
    "            <menuitem action='Redo' />"
    "            <separator />"
    "            <menuitem action='Cut' />"
    "            <menuitem action='Copy' />"
    "            <menuitem action='Paste' />"
    "            <menuitem action='Select All' />"
    "        </menu>"
    "        <menu action='MenuTools'>"
    "            <menuitem action='ArrowTool' />"
    "            <menuitem action='AddModelsTool' />"
    "            <menuitem action='AddLinksTool' />"
    "            <menuitem action='AddCoupledTool' />"
    "            <menuitem action='DeleteTool' />"
    "            <menuitem action='ZoomTool' />"
    "            <menuitem action='QuestionTool' />"
    "        </menu>"
    "        <menu action='MenuProject'>"
    "            <menuitem action='ConfigureProject' />"
    "            <menuitem action='BuildProject' />"
    "            <menuitem action='ProjectDependencies' />"
    "            <menuitem action='TestProject' />"
    "            <menuitem action='CleanSourceProject' />"
    "            <menuitem action='RemoveBinaryProject' />"
    "            <menuitem action='CreateProjectPackage' />"
    "        </menu>"
    "        <menu action='MenuView'>"
    "            <menuitem action='Order' />"
    "            <menuitem action='Refresh' />"
    "            <menuitem action='Complete View' />"
    "            <menuitem action='Simple View' />"
    "            <separator />"
    "            <menuitem action='Preferences' />"
    "        </menu>"
    "        <menu action='MenuSimulation'>"
    "            <menuitem action='Project' />"
    "            <menuitem action='Views' />"
    "            <menuitem action='Dynamics' />"
    "            <menuitem action='Conditions' />"
    "            <separator />"
    "            <menuitem action='LaunchSimulation' />"
    "        </menu>"
    "        <menu action='MenuZoom'>"
    "            <menuitem action='ZoomPlus' />"
    "            <menuitem action='ZoomMinus' />"
    "            <separator />"
    "            <menuitem action='Zoom11' />"
    "            <menuitem action='Zoom12' />"
    "            <menuitem action='Zoom21' />"
    "        </menu>"
    "        <menu action='MenuHelp'>"
    "            <menuitem action='About' />"
    "        </menu>"
    "    </menubar>"
    "    <toolbar name='Toolbar'>"
    "        <toolitem action='Undo' />"
    "        <toolitem action='Redo' />"
    "        <separator />"
    "        <toolitem action='ArrowTool' />"
    "        <toolitem action='AddModelsTool' />"
    "        <toolitem action='AddLinksTool' />"
    "        <toolitem action='AddCoupledTool' />"
    "        <toolitem action='DeleteTool' />"
    "        <toolitem action='ZoomTool' />"
    "        <toolitem action='QuestionTool' />"
    "    </toolbar>"
    "</ui>";

GVLEMenuAndToolbar::GVLEMenuAndToolbar(GVLE* gvle) :
    mParent(gvle)
{
    m_refActionGroup = Gtk::ActionGroup::create("GVLEMenuAndToolbar");
    m_refUIManager = Gtk::UIManager::create();
    init();
    showMinimalMenu();
}

GVLEMenuAndToolbar::~GVLEMenuAndToolbar()
{
}

void GVLEMenuAndToolbar::init()
{
    createFileActions();
    createEditActions();
    createToolsActions();
    createProjectActions();
    createViewActions();
    createSimulationActions();
    createZoomActions();
    createHelpActions();

    m_refUIManager->insert_action_group(m_refActionGroup);
    mParent->add_accel_group(m_refUIManager->get_accel_group());

    createUI();

    mMenuBar = (Gtk::MenuBar *) (
        m_refUIManager->get_widget("/MenuBar"));
    mToolbar = (Gtk::Toolbar *) (
        m_refUIManager->get_widget("/Toolbar"));
}

void GVLEMenuAndToolbar::showMinimalMenu()
{
    // Menu file
    m_refActionGroup->get_action("NewVpz")->set_sensitive(true);
    m_refActionGroup->get_action("NewProject")->set_sensitive(true);
    m_refActionGroup->get_action("OpenProject")->set_sensitive(true);
    m_refActionGroup->get_action("OpenVpz")->set_sensitive(false);
    m_refActionGroup->get_action("SaveFile")->set_sensitive(false);
    m_refActionGroup->get_action("SaveFileAs")->set_sensitive(false);
    hideFileModel();
    hideCloseTab();
    hideCloseProject();

    hideEditMenu();
    hideToolsMenu();
    hideProjectMenu();
    hideViewMenu();
    hideSimulationMenu();
    hideZoomMenu();
}

void GVLEMenuAndToolbar::hideFileModel()
{
    m_refActionGroup->get_action("ClearModel")->set_sensitive(false);
    m_refActionGroup->get_action("ImportModel")->set_sensitive(false);
    m_refActionGroup->get_action("ExportModel")->set_sensitive(false);
    m_refActionGroup->get_action("ExportGraphic")->set_sensitive(false);
}

void GVLEMenuAndToolbar::showOpenVpz()
{
    m_refActionGroup->get_action("OpenVpz")->set_sensitive(true);
}

void GVLEMenuAndToolbar::hideCopyCut()
{
    m_refActionGroup->get_action("Copy")->set_sensitive(false);
    m_refActionGroup->get_action("Cut")->set_sensitive(false);
}

void GVLEMenuAndToolbar::showCopyCut()
{
    m_refActionGroup->get_action("Copy")->set_sensitive(true);
    m_refActionGroup->get_action("Cut")->set_sensitive(true);
}

void GVLEMenuAndToolbar::showPaste()
{
    m_refActionGroup->get_action("Paste")->set_sensitive(true);
}

void GVLEMenuAndToolbar::hideCloseTab()
{
    m_refActionGroup->get_action("CloseTab")->set_sensitive(false);
}

void GVLEMenuAndToolbar::showCloseTab()
{
    m_refActionGroup->get_action("CloseTab")->set_sensitive(true);
}

void GVLEMenuAndToolbar::hideSave()
{
    m_refActionGroup->get_action("SaveFile")->set_sensitive(false);
}

void GVLEMenuAndToolbar::showSave()
{
    m_refActionGroup->get_action("SaveFile")->set_sensitive(true);
}

void GVLEMenuAndToolbar::hideSaveAs()
{
    m_refActionGroup->get_action("SaveFileAs")->set_sensitive(false);
}

void GVLEMenuAndToolbar::showSaveAs()
{
    m_refActionGroup->get_action("SaveFileAs")->set_sensitive(true);
}

void GVLEMenuAndToolbar::hideCloseProject()
{
    m_refActionGroup->get_action("CloseProject")->set_sensitive(false);
}

void GVLEMenuAndToolbar::showCloseProject()
{
    m_refActionGroup->get_action("CloseProject")->set_sensitive(true);
}

void GVLEMenuAndToolbar::showRefresh()
{
    m_refActionGroup->get_action("Refresh")->set_sensitive(true);
}

void GVLEMenuAndToolbar::hideImportExport()
{
    m_refActionGroup->get_action("ClearModel")->set_sensitive(false);
    m_refActionGroup->get_action("ImportModel")->set_sensitive(false);
    m_refActionGroup->get_action("ExportModel")->set_sensitive(false);
    m_refActionGroup->get_action("ExportGraphic")->set_sensitive(false);
}

void GVLEMenuAndToolbar::showImportExport()
{
    m_refActionGroup->get_action("ClearModel")->set_sensitive(true);
    m_refActionGroup->get_action("ImportModel")->set_sensitive(true);
    m_refActionGroup->get_action("ExportModel")->set_sensitive(true);
    m_refActionGroup->get_action("ExportGraphic")->set_sensitive(true);
}

void GVLEMenuAndToolbar::hideEditMenu()
{
    m_refActionGroup->get_action("MenuEdit")->set_sensitive(false);
    m_refActionGroup->get_action("Undo")->set_sensitive(false);
    m_refActionGroup->get_action("Redo")->set_sensitive(false);
    m_refActionGroup->get_action("Cut")->set_sensitive(false);
    m_refActionGroup->get_action("Copy")->set_sensitive(false);
    m_refActionGroup->get_action("Paste")->set_sensitive(false);
    m_refActionGroup->get_action("Select All")->set_sensitive(false);
}

void GVLEMenuAndToolbar::showEditMenu()
{
    m_refActionGroup->get_action("MenuEdit")->set_sensitive(true);
    m_refActionGroup->get_action("Undo")->set_sensitive(true);
    m_refActionGroup->get_action("Redo")->set_sensitive(true);
    m_refActionGroup->get_action("Cut")->set_sensitive(false);
    m_refActionGroup->get_action("Copy")->set_sensitive(false);
    m_refActionGroup->get_action("Paste")->set_sensitive(false);
    m_refActionGroup->get_action("Select All")->set_sensitive(true);
}

void GVLEMenuAndToolbar::hideToolsMenu()
{
    m_refActionGroup->get_action("MenuTools")->set_sensitive(false);
    m_refActionGroup->get_action("ArrowTool")->set_sensitive(false);
    m_refActionGroup->get_action("AddModelsTool")->set_sensitive(false);
    m_refActionGroup->get_action("AddLinksTool")->set_sensitive(false);
    m_refActionGroup->get_action("AddCoupledTool")->set_sensitive(false);
    m_refActionGroup->get_action("DeleteTool")->set_sensitive(false);
    m_refActionGroup->get_action("ZoomTool")->set_sensitive(false);
    m_refActionGroup->get_action("QuestionTool")->set_sensitive(false);
}

void GVLEMenuAndToolbar::showToolsMenu()
{
    m_refActionGroup->get_action("MenuTools")->set_sensitive(true);
    m_refActionGroup->get_action("ArrowTool")->set_sensitive(true);
    m_refActionGroup->get_action("AddModelsTool")->set_sensitive(true);
    m_refActionGroup->get_action("AddLinksTool")->set_sensitive(true);
    m_refActionGroup->get_action("AddCoupledTool")->set_sensitive(true);
    m_refActionGroup->get_action("DeleteTool")->set_sensitive(true);
    m_refActionGroup->get_action("ZoomTool")->set_sensitive(true);
    m_refActionGroup->get_action("QuestionTool")->set_sensitive(true);
}

void GVLEMenuAndToolbar::hideProjectMenu()
{
    m_refActionGroup->get_action("MenuProject")->set_sensitive(false);
    m_refActionGroup->get_action("ConfigureProject")->set_sensitive(false);
    m_refActionGroup->get_action("BuildProject")->set_sensitive(false);
    m_refActionGroup->get_action("ProjectDependencies")->set_sensitive(false);
    m_refActionGroup->get_action("TestProject")->set_sensitive(false);
    m_refActionGroup->get_action("CleanSourceProject")->set_sensitive(false);
    m_refActionGroup->get_action("RemoveBinaryProject")->set_sensitive(false);
    m_refActionGroup->get_action("CreateProjectPackage")->set_sensitive(false);
}

void GVLEMenuAndToolbar::showProjectMenu()
{
    m_refActionGroup->get_action("MenuProject")->set_sensitive(true);
    m_refActionGroup->get_action("ConfigureProject")->set_sensitive(true);
    m_refActionGroup->get_action("BuildProject")->set_sensitive(true);
    m_refActionGroup->get_action("ProjectDependencies")->set_sensitive(true);
    m_refActionGroup->get_action("TestProject")->set_sensitive(true);
    m_refActionGroup->get_action("CleanSourceProject")->set_sensitive(true);
    m_refActionGroup->get_action("RemoveBinaryProject")->set_sensitive(true);
    m_refActionGroup->get_action("CreateProjectPackage")->set_sensitive(true);
}

void GVLEMenuAndToolbar::hideViewMenu()
{
    m_refActionGroup->get_action("MenuView")->set_sensitive(true);
    m_refActionGroup->get_action("Order")->set_sensitive(false);
    m_refActionGroup->get_action("Refresh")->set_sensitive(false);
    m_refActionGroup->get_action("Complete View")->set_sensitive(false);
    m_refActionGroup->get_action("Simple View")->set_sensitive(false);
    m_refActionGroup->get_action("Preferences")->set_sensitive(true);
}

void GVLEMenuAndToolbar::showViewMenu()
{
    m_refActionGroup->get_action("MenuView")->set_sensitive(true);
    m_refActionGroup->get_action("Order")->set_sensitive(true);
    m_refActionGroup->get_action("Refresh")->set_sensitive(true);
    m_refActionGroup->get_action("Complete View")->set_sensitive(true);
    m_refActionGroup->get_action("Simple View")->set_sensitive(true);
    m_refActionGroup->get_action("Preferences")->set_sensitive(true);
}

void GVLEMenuAndToolbar::hideSimulationMenu()
{
    m_refActionGroup->get_action("MenuSimulation")->set_sensitive(false);
    m_refActionGroup->get_action("Project")->set_sensitive(false);
    m_refActionGroup->get_action("Conditions")->set_sensitive(false);
    m_refActionGroup->get_action("Views")->set_sensitive(false);
    m_refActionGroup->get_action("Dynamics")->set_sensitive(false);
    m_refActionGroup->get_action("LaunchSimulation")->set_sensitive(false);
}

void GVLEMenuAndToolbar::showSimulationMenu()
{
    m_refActionGroup->get_action("MenuSimulation")->set_sensitive(true);
    m_refActionGroup->get_action("Project")->set_sensitive(true);
    m_refActionGroup->get_action("Conditions")->set_sensitive(true);
    m_refActionGroup->get_action("Views")->set_sensitive(true);
    m_refActionGroup->get_action("Dynamics")->set_sensitive(true);
    m_refActionGroup->get_action("LaunchSimulation")->set_sensitive(true);
}

void GVLEMenuAndToolbar::hideZoomMenu()
{
    m_refActionGroup->get_action("MenuZoom")->set_sensitive(false);
    m_refActionGroup->get_action("ZoomPlus")->set_sensitive(false);
    m_refActionGroup->get_action("ZoomMinus")->set_sensitive(false);
    m_refActionGroup->get_action("Zoom11")->set_sensitive(false);
    m_refActionGroup->get_action("Zoom12")->set_sensitive(false);
    m_refActionGroup->get_action("Zoom21")->set_sensitive(false);
}

void GVLEMenuAndToolbar::showZoomMenu()
{
    m_refActionGroup->get_action("MenuZoom")->set_sensitive(true);
    m_refActionGroup->get_action("ZoomPlus")->set_sensitive(true);
    m_refActionGroup->get_action("ZoomMinus")->set_sensitive(true);
    m_refActionGroup->get_action("Zoom11")->set_sensitive(true);
    m_refActionGroup->get_action("Zoom12")->set_sensitive(true);
    m_refActionGroup->get_action("Zoom21")->set_sensitive(true);
}

void GVLEMenuAndToolbar::createUI()
{
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try {
        m_refUIManager->add_ui_from_string(UI_DEFINITION);
    } catch(const Glib::Error& ex) {
        std::cerr << "building menus failed: " <<  ex.what();
    }
#else
    std::auto_ptr<Glib::Error> ex;
    m_refUIManager->add_ui_from_string(UI_DEFINITION, ex);
    if(ex.get()) {
        std::cerr << "building menus failed: " <<  ex->what();
    }
#endif //GLIBMM_EXCEPTIONS_ENABLED
}

void GVLEMenuAndToolbar::createFileActions()
{
    m_refActionGroup->add(Gtk::Action::create("MenuFile", _("_File")));

    m_refActionGroup->add(
        Gtk::Action::create("NewProject", Gtk::Stock::DIRECTORY,
                            _("New Project"), _("Create a new project")),
        Gtk::AccelKey("<control>n"),
        sigc::mem_fun(mParent, &GVLE::onNewProject));
    m_refActionGroup->add(
        Gtk::Action::create("NewVpz", Gtk::Stock::NEW,
                            _("New _Vpz"), _("Create a new Vpz")),
        Gtk::AccelKey("<control><shift>n"),
        sigc::mem_fun(mParent, &GVLE::onNewVpz));
    m_refActionGroup->add(
        Gtk::Action::create("OpenProject", Gtk::Stock::OPEN,
                            _("Open P_roject"), _("Open a Package")),
        Gtk::AccelKey("<control>o"),
        sigc::mem_fun(mParent, &GVLE::onOpenProject));
    m_refActionGroup->add(
        Gtk::Action::create("OpenVpz", Gtk::Stock::OPEN,
                            _("Open Vp_z"), _("Open a Vpz from package")),
        Gtk::AccelKey("<control><shift>o"),
        sigc::mem_fun(mParent, &GVLE::onOpenVpz));
    m_refActionGroup->add(
        Gtk::Action::create("SaveFile", Gtk::Stock::SAVE,
                            _("Save File"), _("Save a file")),
        sigc::mem_fun(mParent, &GVLE::onSave));
    m_refActionGroup->add(
        Gtk::Action::create("SaveFileAs", Gtk::Stock::SAVE_AS,
                            _("Save File As"), _("Save a file as a "
                                                 "different name")),
        Gtk::AccelKey("<control><shift>s"),
        sigc::mem_fun(mParent, &GVLE::onSaveAs));
    m_refActionGroup->add(
        Gtk::Action::create("ClearModel", _("_Clear Model"),
                            _("Clear the current model")),
        Gtk::AccelKey("<control>l"),
        sigc::mem_fun(mParent, &GVLE::clearCurrentModel));
    m_refActionGroup->add(
        Gtk::Action::create("ImportModel", _("_Import Model"),
                            _("Import a model")),
        Gtk::AccelKey("<control>i"),
        sigc::mem_fun(mParent, &GVLE::importModel));
    m_refActionGroup->add(
        Gtk::Action::create("ExportModel", _("_Export Model"),
                            _("Create a new Package")),
        sigc::mem_fun(mParent, &GVLE::exportCurrentModel));
    m_refActionGroup->add(
        Gtk::Action::create("ExportGraphic", _("Export _Graphic"),
                            _("Create a new Package")),
        sigc::mem_fun(mParent, &GVLE::exportGraphic));
    m_refActionGroup->add(
        Gtk::Action::create("CloseTab", Gtk::Stock::CLOSE,
                            _("_Close Tab"), _("Close the current tab")),
        sigc::mem_fun(mParent, &GVLE::onCloseTab));
    m_refActionGroup->add(
        Gtk::Action::create("CloseProject", Gtk::Stock::CLOSE,
                            _("_Close Project"), _("Close the project")),
        Gtk::AccelKey(""),
        sigc::mem_fun(mParent, &GVLE::onCloseProject));
    m_refActionGroup->add(
        Gtk::Action::create("Quit", Gtk::Stock::QUIT,
                            _("_Quit"), _("Quit GVLE")),
        sigc::mem_fun(mParent, &GVLE::onQuit));
}

void GVLEMenuAndToolbar::createEditActions()
{
    m_refActionGroup->add(Gtk::Action::create("MenuEdit", _("_Edit")));

    m_refActionGroup->add(
        Gtk::Action::create("Undo", Gtk::Stock::UNDO),
        Gtk::AccelKey("<control>z"),
        sigc::mem_fun(mParent->getEditor(), &Editor::onUndo));
    m_refActionGroup->add(
        Gtk::Action::create("Redo", Gtk::Stock::REDO),
        Gtk::AccelKey("<control>y"),
        sigc::mem_fun(mParent->getEditor(), &Editor::onRedo));
    m_refActionGroup->add(
        Gtk::Action::create("Cut", Gtk::Stock::CUT),
        sigc::mem_fun(mParent, &GVLE::onCutModel));
    m_refActionGroup->add(
        Gtk::Action::create("Copy", Gtk::Stock::COPY),
        sigc::mem_fun(mParent, &GVLE::onCopyModel));
    m_refActionGroup->add(
        Gtk::Action::create("Paste", Gtk::Stock::PASTE),
        sigc::mem_fun(mParent, &GVLE::onPasteModel));
    m_refActionGroup->add(
        Gtk::Action::create("Select All", Gtk::Stock::SELECT_ALL),
        Gtk::AccelKey("<control>a"),
        sigc::mem_fun(mParent, &GVLE::onSelectAll));
}

void GVLEMenuAndToolbar::createToolsActions()
{
    m_refActionGroup->add(Gtk::Action::create("MenuTools", _("_Tools")));
    Gtk::RadioAction::Group toolsGroup;

    m_refActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "ArrowTool", Gtk::Stock::INDEX,
                                 _("Select"), _("Move models, open coupled "
                                                "models, show dynamics of "
                                                "models. (F1)")),
        Gtk::AccelKey("F1"),
sigc::mem_fun(mParent, &GVLE::onArrow));
    m_refActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "AddModelsTool", Gtk::Stock::ADD,
                                 _("Add Models"), _("Add empty atomics models. "
                                                    "You can specify inputs, "
                                                    "outputs, inits and states "
                                                    "ports. Select dynamics "
                                                    "plugins file and XML write "
                                                    "your XML Dynamics. (F2)")),
        Gtk::AccelKey("F2"),
        sigc::mem_fun(mParent, &GVLE::onAddModels));
    m_refActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "AddLinksTool",
                                 Gtk::Stock::DISCONNECT,
                                 _("Add Links"), _("Add connections between "
                                                   "models. (F3)")),
        Gtk::AccelKey("F3"),
        sigc::mem_fun(mParent, &GVLE::onAddLinks));
    m_refActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "AddCoupledTool",
                                 Gtk::Stock::LEAVE_FULLSCREEN, _("Add Coupled"),
                                 _("Add coupled models. (F4)")),
        Gtk::AccelKey("F4"),
        sigc::mem_fun(mParent, &GVLE::onAddCoupled));
    m_refActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "DeleteTool", Gtk::Stock::DELETE,
                                 _("Delete"), _("Delete models or connections. "
                                                "(F5)")),
        Gtk::AccelKey("F5"),
        sigc::mem_fun(mParent, &GVLE::onDelete));
    m_refActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "ZoomTool", Gtk::Stock::ZOOM_FIT,
                                 _("Zoom"), _("Zoom. (F6)")),
        Gtk::AccelKey("F6"),
        sigc::mem_fun(mParent, &GVLE::onZoom));
    m_refActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "QuestionTool", Gtk::Stock::HELP,
                                 _("Question"), _("Show XML Dynamics if atomic "
                                                  "model is selected, show XML "
                                                  "Structures if coupled "
                                                  "model. (F7)")),
        Gtk::AccelKey("F7"),
        sigc::mem_fun(mParent, &GVLE::onQuestion));
}

void GVLEMenuAndToolbar::createProjectActions()
{
    m_refActionGroup->add(Gtk::Action::create("MenuProject", _("_Project")));

    m_refActionGroup->add(
        Gtk::Action::create("ConfigureProject", _("Configure Project"),
                            _("Configure the project")),
        Gtk::AccelKey("<control>g"),
        sigc::mem_fun(mParent, &GVLE::configureProject));
    m_refActionGroup->add(
        Gtk::Action::create("BuildProject", _("Build Project"),
                            _("Build the project")),
        Gtk::AccelKey("<control>b"),
        sigc::mem_fun(mParent, &GVLE::buildProject));
    m_refActionGroup->add(
        Gtk::Action::create("ProjectDependencies", _("Project Dependencies"),
                            _("Display the project Dependencies")),
        sigc::mem_fun(mParent, &GVLE::displayDependencies));
    m_refActionGroup->add(
        Gtk::Action::create("TestProject", _("Test Project"),
                            _("Test the project")),
        sigc::mem_fun(mParent, &GVLE::testProject));
    m_refActionGroup->add(
        Gtk::Action::create("CleanSourceProject", _("Clean Project"),
                            _("Clean the source project")),
        sigc::mem_fun(mParent, &GVLE::cleanProject));
    m_refActionGroup->add(
        Gtk::Action::create("RemoveBinaryProject", _("Remove Binary  Project"),
                            _("Remove the binary project")),
        sigc::mem_fun(mParent, &GVLE::removeProject));
    m_refActionGroup->add(
        Gtk::Action::create("CreateProjectPackage", _("Create Project Package"),
                            _("Create the project package")),
        sigc::mem_fun(mParent, &GVLE::packageProject));
}

void GVLEMenuAndToolbar::createViewActions()
{
    m_refActionGroup->add(Gtk::Action::create("MenuView", _("_View")));

    m_refActionGroup->add(
        Gtk::Action::create("Order", _("Order"),
                            _("Arrange the model with randomly")),
        sigc::mem_fun(mParent, &GVLE::onOrder));

    m_refActionGroup->add(
        Gtk::Action::create("Refresh", _("Refresh"),
                            _("Refresh the package tree view")),
        Gtk::AccelKey("<control>r"),
        sigc::mem_fun(mParent, &GVLE::onRefresh));

    m_refActionGroup->add(
        Gtk::Action::create("Complete View", _("Complete View"),
                            _("Show models with ports")),
        sigc::mem_fun(mParent, &GVLE::onShowCompleteView));

    m_refActionGroup->add(
        Gtk::Action::create("Simple View", _("Simple View"),
                            _("Show models without ports")),
        sigc::mem_fun(mParent, &GVLE::onShowSimpleView));

    m_refActionGroup->add(
        Gtk::Action::create("Preferences", Gtk::Stock::PREFERENCES,
                            _("Preferences"), _("Create the project package")),
        Gtk::AccelKey("<control>p"),
        sigc::mem_fun(mParent, &GVLE::onPreferences));
}

void GVLEMenuAndToolbar::createSimulationActions()
{
    m_refActionGroup->add(Gtk::Action::create("MenuSimulation",
                                              _("_Simulation")));

    m_refActionGroup->add(
        Gtk::Action::create("Project", _("Project"),
                            _("Configure _experiment")),
        Gtk::AccelKey("<control><shift>e"),
        sigc::mem_fun(mParent, &GVLE::onExperimentsBox));
    m_refActionGroup->add(
        Gtk::Action::create("Views", _("Views"), _("Manage the _views")),
        Gtk::AccelKey("<control><shift>v"),
        sigc::mem_fun(mParent, &GVLE::onViewOutputBox));
    m_refActionGroup->add(
        Gtk::Action::create("Dynamics", _("Dynamics"), _("Manage the _dynamics")),
        Gtk::AccelKey("<control><shift>d"),
        sigc::mem_fun(mParent, &GVLE::onDynamicsBox));
    m_refActionGroup->add(
        Gtk::Action::create("Conditions", _("Conditions"),
                            _("Manage the conditions")),
        Gtk::AccelKey("<control><shift>c"),
        sigc::mem_fun(mParent, &GVLE::onConditionsBox));
    m_refActionGroup->add(
        Gtk::Action::create("LaunchSimulation", _("Launch Simulation"),
                            _("Launch the simulation")),
        Gtk::AccelKey("<control><shift>l"),
        sigc::mem_fun(mParent, &GVLE::configureToSimulate));
}

void GVLEMenuAndToolbar::createZoomActions()
{
    m_refActionGroup->add(Gtk::Action::create("MenuZoom", _("_Zoom")));

    m_refActionGroup->add(
        Gtk::Action::create("ZoomPlus", Gtk::Stock::ZOOM_IN,
                            _("Zoom +"), _("Zoom in")),
        Gtk::AccelKey(GDK_plus, Gdk::CONTROL_MASK),
        sigc::mem_fun(mParent, &GVLE::addCoefZoom));
    m_refActionGroup->add(
        Gtk::Action::create("ZoomMinus", Gtk::Stock::ZOOM_OUT,
                            _("Zoom -"), _("Zoom out")),
        Gtk::AccelKey(GDK_minus, Gdk::CONTROL_MASK),
        sigc::mem_fun(mParent, &GVLE::delCoefZoom));
    m_refActionGroup->add(
        Gtk::Action::create("Zoom11", Gtk::Stock::ZOOM_100,
                            _("Zoom 1:1"), _("Restore standard zoom")),
        Gtk::AccelKey("<control>1"),
        sigc::bind(sigc::mem_fun(mParent, &GVLE::setCoefZoom), 1.0));
    m_refActionGroup->add(
        Gtk::Action::create("Zoom21", _("Zoom 2:1"), _("Zoom twice")),
        sigc::bind(sigc::mem_fun(mParent, &GVLE::setCoefZoom), 2.0));
    m_refActionGroup->add(
        Gtk::Action::create("Zoom12", _("Zoom 1:2"), _("Zoom half")),
        sigc::bind(sigc::mem_fun(mParent, &GVLE::setCoefZoom), 0.5));
}

void GVLEMenuAndToolbar::createHelpActions()
{
    m_refActionGroup->add(Gtk::Action::create("MenuHelp", _("_Help")));

    m_refActionGroup->add(
        Gtk::Action::create("About", Gtk::Stock::ABOUT, _("About"), _("About")),
        sigc::mem_fun(mParent, &GVLE::onShowAbout));
}

void GVLEMenuAndToolbar::onOpenFile()
{
    hideImportExport();
    showEditMenu();
    showCopyCut();

    Glib::RefPtr<Gtk::Clipboard> clipboard(Gtk::Clipboard::get());
    if(clipboard->wait_is_text_available()) {
        showPaste();
    }

    hideToolsMenu();
    hideViewMenu();
    hideSimulationMenu();
    hideZoomMenu();
    showSaveAs();
    showCloseTab();
}

void GVLEMenuAndToolbar::onOpenProject()
{
    showOpenVpz();
    showCloseProject();
    showProjectMenu();
    showRefresh();
    hideToolsMenu();
}

void GVLEMenuAndToolbar::onOpenVpz()
{
    showImportExport();
    showEditMenu();

    if(not mParent->paste_is_empty()) {
        showPaste();
    }
    showToolsMenu();
    showViewMenu();
    showSimulationMenu();
    showZoomMenu();
    showSaveAs();
    showCloseTab();
}

void GVLEMenuAndToolbar::onCloseTab(bool vpz, bool empty)
{
    if (vpz) {
        hideImportExport();
        hideToolsMenu();
        hideViewMenu();
        hideSimulationMenu();
        hideZoomMenu();
    }
    if (empty) {
        hideSave();
        hideSaveAs();
        hideEditMenu();
        hideCloseTab();
    }
}

}} // namespace vle gvle
