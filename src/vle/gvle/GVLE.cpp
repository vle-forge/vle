/**
 * @file vle/gvle/GVLE.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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


#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/About.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/Editor.hpp>
#include <vle/gvle/ExperimentBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/ExperimentBox.hpp>
#include <vle/gvle/HelpBox.hpp>
#include <vle/gvle/HostsBox.hpp>
#include <vle/gvle/GVLEMenuAndToolbar.hpp>
#include <vle/gvle/PreferencesBox.hpp>
#include <vle/gvle/Settings.hpp>
#include <vle/gvle/ViewDrawingArea.hpp>
#include <vle/gvle/ViewOutputBox.hpp>
#include <vle/gvle/View.hpp>
#include <vle/gvle/LaunchSimulationBox.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Path.hpp>
#include <vle/vpz/Vpz.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <gtkmm/filechooserdialog.h>
#include <glibmm/spawn.h>
#include <glibmm/miscutils.h>
#include <gtkmm/stock.h>

namespace vle { namespace gvle {

const std::string GVLE::WINDOW_TITLE =
    "GVLE  " +
    std::string(VLE_VERSION) +
    std::string(VLE_EXTRA_VERSION);

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

GVLE::FileTreeView::FileTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/) :
    Gtk::TreeView(cobject),
    mDelayTime(0)
{
    mRefTreeModel = Gtk::TreeStore::create(mColumns);
    set_model(mRefTreeModel);
    mColumnName = append_column(_("Project"), mColumns.m_col_name);

    mCellrenderer = dynamic_cast<Gtk::CellRendererText*>(
	get_column_cell_renderer(mColumnName - 1));

    mRefTreeSelection = get_selection();
    mIgnoredFilesList.push_front("build");

    {
	Gtk::Menu::MenuList& menulist = mMenuPopup.items();

	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Open with..."),
		sigc::mem_fun(
		    *this,
		    &GVLE::FileTreeView::onOpen)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("New _Directory"),
		sigc::mem_fun(
		    *this,
		    &GVLE::FileTreeView::onNewDirectory)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("New _File"),
		sigc::mem_fun(
		    *this,
		    &GVLE::FileTreeView::onNewFile)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Remove"),
		sigc::mem_fun(
		    *this,
		    &GVLE::FileTreeView::onRemove)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Rename"),
		sigc::mem_fun(
		    *this,
		    &GVLE::FileTreeView::onRename)));
    }
    mMenuPopup.accelerate(*this);
}

GVLE::FileTreeView::~FileTreeView()
{
}

void GVLE::FileTreeView::buildHierarchyDirectory(
    const Gtk::TreeModel::Row* parent, const std::string& dirname)
{
    Glib::Dir dir(dirname);
    std::list<std::string> entries (dir.begin(), dir.end());
    entries.sort();
    std::list <std::string> ::iterator it;
    for (it = entries.begin(); it != entries.end(); ++it) {
	if (((*it)[0] != '.') //Don't show hidden files
	    and (std::find(mIgnoredFilesList.begin(),
			   mIgnoredFilesList.end(), *it)
	         == mIgnoredFilesList.end())) {
	    std::string nextpath = Glib::build_filename(dirname, *it);
	    if (isDirectory(nextpath)) {
		if (parent) {
		    Gtk::TreeModel::Row row =
			*(mRefTreeModel->append(parent->children()));
		    row[mColumns.m_col_name] = *it;
		    buildHierarchy(&(*row), nextpath);
		} else {
		    Gtk::TreeModel::Row row =
			*(mRefTreeModel->append());
		    row[mColumns.m_col_name] = *it;
		    buildHierarchy(&(*row), nextpath);
		}
	    }
	}
    }
}

void GVLE::FileTreeView::buildHierarchyFile(
    const Gtk::TreeModel::Row* parent, const std::string& dirname)
{
    Glib::Dir dir(dirname);
    std::list<std::string> entries (dir.begin(), dir.end());
    entries.sort();
    std::list<std::string>::iterator it;
    for (it = entries.begin(); it != entries.end(); ++it) {
	if (((*it)[0] != '.') //Don't show hidden files
	    and (std::find(mIgnoredFilesList.begin(),
			   mIgnoredFilesList.end(), *it)
	         == mIgnoredFilesList.end())) {
	    std::string nextpath = Glib::build_filename(dirname, *it);
	    if (not isDirectory(nextpath)) {
		if (parent) {
		    Gtk::TreeModel::Row row =
			*(mRefTreeModel->append(parent->children()));
		    row[mColumns.m_col_name] = *it;
		} else {
		    Gtk::TreeModel::Row row =
			*(mRefTreeModel->append());
		    row[mColumns.m_col_name] = *it;
		}
	    }
	}
    }
}


void GVLE::FileTreeView::buildHierarchy(
    const Gtk::TreeModel::Row* parent, const std::string& dirname)
{
    buildHierarchyDirectory(parent, dirname);
    buildHierarchyFile(parent, dirname);
}

void GVLE::FileTreeView::clear()
{
    mRefTreeModel->clear();
}

void GVLE::FileTreeView::build()
{
    remove_all_columns();
    if (not mPackage.empty()) {
        mColumnName = append_column(utils::Path::filename(mPackage),
                                    mColumns.m_col_name);
	mCellrenderer = dynamic_cast<Gtk::CellRendererText*>(
	    get_column_cell_renderer(mColumnName - 1));
	buildHierarchy(0, mPackage);
    } else {
	mColumnName = append_column("Project", mColumns.m_col_name);
	mCellrenderer = dynamic_cast<Gtk::CellRendererText*>(
	    get_column_cell_renderer(mColumnName - 1));
    }
}

bool GVLE::FileTreeView::isDirectory(const std::string& dirname)
{
    return Glib::file_test(dirname, Glib::FILE_TEST_IS_DIR);
}

void GVLE::FileTreeView::on_row_activated(const Gtk::TreeModel::Path&,
					  Gtk::TreeViewColumn*)
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection	= get_selection();
    Gtk::TreeModel::const_iterator it = refSelection->get_selected();
    const Gtk::TreeModel::Row row = *it;
    std::list<std::string> lstpath;

    projectFilePath(row, lstpath);

    std::string absolute_path =
	Glib::build_filename(mPackage, Glib::build_filename(lstpath));

    if (not isDirectory(absolute_path)) {
	if (mParent->getEditor()->existTab(absolute_path)) {
	    mParent->getEditor()->focusTab(absolute_path);
	} else {
            if (utils::Path::extension(absolute_path) == ".vpz") {
                Editor::Documents::const_iterator it =
                    mParent->getEditor()->getDocuments().begin();
                bool found = false;

                while (not found and
                       it != mParent->getEditor()->getDocuments().end()) {
                    if (utils::Path::extension(it->first) == ".vpz") {
                        found = true;
                    } else {
                        ++it;
                    }
                }
                if (not found or (found and mParent->closeTab(it->first))) {
                    mParent->getEditor()->openTab(absolute_path);
                    mParent->redrawModelTreeBox();
                    mParent->redrawModelClassBox();
                    mParent->getMenu()->onOpenVpz();
                }
	    } else {
		mParent->getEditor()->openTab(absolute_path);
                mParent->getMenu()->onOpenFile();
	    }
	}
    } else {
	if (not row_expanded(Gtk::TreePath(it))) {
	    expand_row(Gtk::TreePath(it), false);
	} else {
	    collapse_row(Gtk::TreePath(it));
	}
    }
}

void GVLE::FileTreeView::projectFilePath(const Gtk::TreeRow& row,
					 std::list<std::string>& lst)
{
    if (row.parent()) {
	projectFilePath(*row.parent(), lst);
    }
    lst.push_back(std::string(row.get_value(mColumns.m_col_name)));
}

bool GVLE::FileTreeView::on_button_press_event(GdkEventButton* event)
{
    bool return_value = TreeView::on_button_press_event(event);
    if (event->type == GDK_BUTTON_PRESS and event->button == 3
	and not vle::utils::Path::path().getPackageDir().empty()) {
	mMenuPopup.popup(event->button, event->time);
    }
    return return_value;
}

void GVLE::FileTreeView::onOpen()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection =  get_selection();
    SimpleTypeBox box(_("Path of the program ?"), "");
    std::string prg = Glib::find_program_in_path(boost::trim_copy(box.run()));

    if (refSelection and box.valid() and not prg.empty()) {
	Gtk::TreeModel::Row row = *refSelection->get_selected();
	std::list < std::string > argv;
	std::list<std::string> lstpath;
	argv.push_back(prg);
	projectFilePath(row, lstpath);
	std::string filepath = Glib::build_filename(
	    mPackage, Glib::build_filename(lstpath));
	argv.push_back(filepath);

	try {
	    Glib::spawn_async(utils::Path::path().getParentPath(filepath),
			     argv,
			     Glib::SpawnFlags(0),
			     sigc::slot < void >());
	} catch(const Glib::SpawnError& e) {
	    Error(_("The program can not be lanched"));
	}
    }
}

void GVLE::FileTreeView::onNewFile()
{
    SimpleTypeBox box(_("Name of the File ?"), "");
    std::string name = boost::trim_copy(box.run());
    std::string filepath;

    if (box.valid() and not name.empty()) {
	Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();

	if (refSelection) {
	    Gtk::TreeModel::const_iterator it = refSelection->get_selected();
	    const Gtk::TreeModel::Row row = *it;
	    std::list<std::string> lstpath;
	    projectFilePath(row, lstpath);
	    filepath = Glib::build_filename(
		mPackage, Glib::build_filename(lstpath));
	    if (not isDirectory(filepath)) {
                filepath = utils::Path::dirname(filepath);
	    }
	} else {
	    filepath = mPackage;
	}
	utils::Package::package().addFile(filepath, name);
    }
    mParent->refreshPackageHierarchy();
}

void GVLE::FileTreeView::onNewDirectory()
{
    SimpleTypeBox box(_("Name of the Directory ?"), "");
    std::string name = boost::trim_copy(box.run());
    std::string directorypath;

    if (box.valid() and not name.empty()) {
	Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();

	if (refSelection) {
	    Gtk::TreeModel::const_iterator it = refSelection->get_selected();
	    const Gtk::TreeModel::Row row = *it;
	    std::list<std::string> lstpath;
	    projectFilePath(row, lstpath);
	    directorypath = Glib::build_filename(
		mPackage, Glib::build_filename(lstpath));
	    if (not isDirectory(directorypath)) {
                directorypath = utils::Path::dirname(directorypath);
	    }
	} else {
	    directorypath = mPackage;
	}
	utils::Package::package().addDirectory(directorypath, name);
    }
    mParent->refreshPackageHierarchy();
}


void GVLE::FileTreeView::onRemove()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection	= get_selection();

    if (refSelection) {
	Gtk::TreeModel::const_iterator it = refSelection->get_selected();
	const Gtk::TreeModel::Row row = *it;
	std::list < std::string > lstpath;
	bool is_directory;

	projectFilePath(row, lstpath);
	is_directory = isDirectory(
	    Glib::build_filename(mPackage, Glib::build_filename(lstpath)));
	if ((not is_directory and
	     gvle::Question(_("Do you really want remove this file ?\n"))) or
	    (is_directory and gvle::Question(
		_("Do you really want remove this directory ?\n")))) {
	    std::string oldName(Glib::build_filename(lstpath));
	    utils::Package::package().removeFile(oldName);
	    mParent->refreshEditor(oldName, "");
	    if (mParent->getEditor()->getDocuments().empty()) {
		mParent->setTitle();
	    }
	}
    }
    mParent->refreshPackageHierarchy();
}

void GVLE::FileTreeView::onRename()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();

    if (refSelection) {
	Gtk::TreeModel::const_iterator it = refSelection->get_selected();

	if (it) {
	    const Gtk::TreeModel::Row row = *it;
	    std::string oldName = row.get_value(mColumns.m_col_name);
	    SimpleTypeBox box(_("Name of the file ?"), oldName);
	    std::string newName = boost::trim_copy(box.run());

	    if (box.valid() and not newName.empty()) {
		std::list < std::string > lstpath;
		std::string oldName;

		projectFilePath(row, lstpath);
		oldName = Glib::build_filename(lstpath);

		std::string oldAbsolutePath =
		    Glib::build_filename(utils::Path::path().getPackageDir(),
					 oldName);

		if (utils::Path::extension(newName) == "") {
		    newName += utils::Path::extension(oldAbsolutePath);
		}

		std::string newAbsolutePath =
		    Glib::build_filename(
			utils::Path::path().getParentPath(oldAbsolutePath),
			newName);

		if (not utils::Path::exist(newAbsolutePath)) {
		    utils::Package::package().renameFile(oldName, newName);

		    mParent->refreshEditor(oldName, newName);
		}
	    }
	    mParent->refreshPackageHierarchy();
	}
    }
}

void GVLE::FileTreeView::refresh()
{
    refreshHierarchy(0, mPackage);
}

void GVLE::FileTreeView::refreshHierarchy(
    const Gtk::TreeModel::Row* parent,
    const std::string& dirname)
{
    refreshHierarchyDirectory(parent, dirname);
    refreshHierarchyFile(parent, dirname);
}

void GVLE::FileTreeView::refreshHierarchyDirectory(
    const Gtk::TreeModel::Row* parent,
    const std::string& dirname)
{
    Glib::Dir dir(dirname);
    std::list < std::string > entries(dir.begin(), dir.end());

    entries.sort();
    removeFiles(parent, entries);

    for (std::list < std::string >::iterator it = entries.begin();
         it != entries.end(); ++it) {
	if (((*it)[0] != '.') //Don't show hidden files
	    and (std::find(mIgnoredFilesList.begin(),
			   mIgnoredFilesList.end(), *it)
	         == mIgnoredFilesList.end())) {
	    std::string nextpath = Glib::build_filename(dirname, *it);
	    if (isDirectory(nextpath)) {
		if (parent) {
                    Gtk::TreeModel::const_iterator itt =
                        parent->children().begin();

                    while (itt != parent->children().end()
                           and (*itt)[mColumns.m_col_name] != *it) {
                        ++itt;
                    }
                    if (itt == parent->children().end()) {
                        Gtk::TreeModel::Row row =
                            *(mRefTreeModel->append(parent->children()));
                        row[mColumns.m_col_name] = *it;
                        refreshHierarchy(&(*row), nextpath);
                    } else {
                        refreshHierarchy(&(*itt), nextpath);
                    }
		} else {
                    Gtk::TreeModel::const_iterator itt =
                        mRefTreeModel->children().begin();

                    while (itt != mRefTreeModel->children().end()
                           and (*itt)[mColumns.m_col_name] != *it) {
                        ++itt;
                    }
                    if (itt == mRefTreeModel->children().end()) {
                        Gtk::TreeModel::Row row =
                            *(mRefTreeModel->append());
                        row[mColumns.m_col_name] = *it;
                        refreshHierarchy(&(*row), nextpath);
                    } else {
                        refreshHierarchy(&(*itt), nextpath);
                    }
		}
	    }
	}
    }
}

void GVLE::FileTreeView::refreshHierarchyFile(
    const Gtk::TreeModel::Row* parent,
    const std::string& dirname)
{
    Glib::Dir dir(dirname);
    std::list < std::string > entries(dir.begin(), dir.end());

    entries.sort();
    removeFiles(parent, entries);

    for (std::list < std::string >::iterator it = entries.begin();
         it != entries.end(); ++it) {
	if (((*it)[0] != '.') //Don't show hidden files
	    and (std::find(mIgnoredFilesList.begin(),
			   mIgnoredFilesList.end(), *it)
	         == mIgnoredFilesList.end())) {
	    std::string nextpath = Glib::build_filename(dirname, *it);

	    if (not isDirectory(nextpath)) {
		if (parent) {
                    Gtk::TreeModel::const_iterator itt =
                        parent->children().begin();

                    while (itt != parent->children().end()
                           and (*itt)[mColumns.m_col_name] != *it) {
                        ++itt;
                    }
                    if (itt == parent->children().end()) {
                        Gtk::TreeModel::Row row =
                            *(mRefTreeModel->append(parent->children()));
                        row[mColumns.m_col_name] = *it;
                    }
		} else {
                    Gtk::TreeModel::const_iterator itt =
                        mRefTreeModel->children().begin();

                    while (itt != mRefTreeModel->children().end()
                           and (*itt)[mColumns.m_col_name] != *it) {
                        ++itt;
                    }
                    if (itt == mRefTreeModel->children().end()) {
                        Gtk::TreeModel::Row row =
                            *(mRefTreeModel->append());
                        row[mColumns.m_col_name] = *it;
                    }
		}
	    }
	}
    }
}

void GVLE::FileTreeView::removeFiles(const Gtk::TreeModel::Row* parent,
                                     const std::list < std::string >& entries)
{
    std::list < Gtk::TreeModel::RowReference > rows;
    Gtk::TreeModel::const_iterator itt = parent ? parent->children().begin() :
        mRefTreeModel->children().begin();

    while (itt != (parent ? parent->children().end() :
                   mRefTreeModel->children().end())) {
        if (std::find(entries.begin(), entries.end(),
                      (*itt)[mColumns.m_col_name]) == entries.end()) {
            rows.push_back(Gtk::TreeRowReference(
                               mRefTreeModel,
                               mRefTreeModel->get_path(itt)));
        }
        ++itt;
    }

    for (std::list < Gtk::TreeModel::RowReference >::iterator it =
             rows.begin(); it != rows.end(); ++it) {
        Gtk::TreeModel::iterator treeiter =
            mRefTreeModel->get_iter(it->get_path());

        mRefTreeModel->erase(treeiter);
    }
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

GVLE::GVLE(BaseObjectType* cobject,
	   const Glib::RefPtr<Gnome::Glade::Xml> xml):
    Gtk::Window(cobject),
    mModeling(new Modeling(this)),
    mCurrentButton(POINTER),
    mCurrentTab(-1)
{
    mRefXML = xml;
    mModeling->setGlade(mRefXML);

    Settings::settings().load();

    mGlobalVpzPrevDirPath = "";

    mConditionsBox = new ConditionsBox(mRefXML, this);
    mPreferencesBox = new PreferencesBox(mRefXML);
    mOpenPackageBox = new OpenPackageBox(mRefXML, mModeling);
    mOpenVpzBox = new OpenVpzBox(mRefXML, mModeling);
    mNewProjectBox = new NewProjectBox(mRefXML, mModeling, this);
    mSaveVpzBox = new SaveVpzBox(mRefXML, mModeling);
    mQuitBox = new QuitBox(mRefXML, mModeling);

    if (Settings::settings().getFont().empty()) {
        Settings::settings().setFont(mPreferencesBox->getGraphicsFont());
    }

    if (Settings::settings().getFontEditor().empty()) {
        Settings::settings().setFontEditor(mPreferencesBox->getEditorFont());
    }

    mRefXML->get_widget("MenuAndToolbarVbox", mMenuAndToolbarVbox);
    mRefXML->get_widget("StatusBarPackageBrowser", mStatusbar);
    mRefXML->get_widget("TextViewLogPackageBrowser", mLog);
    mRefXML->get_widget_derived("FileTreeViewPackageBrowser", mFileTreeView);
    mFileTreeView->setParent(this);
    mRefXML->get_widget_derived("NotebookPackageBrowser", mEditor);
    mEditor->setParent(this);
    mRefXML->get_widget_derived("TreeViewModel", mModelTreeBox);
    mModelTreeBox->setModeling(mModeling);
    mRefXML->get_widget_derived("TreeViewClass", mModelClassBox);
    mModelClassBox->createNewModelBox(mModeling);

    mMenuAndToolbar = new GVLEMenuAndToolbar(this);
    mMenuAndToolbarVbox->pack_start(*mMenuAndToolbar->getMenuBar());
    mMenuAndToolbarVbox->pack_start(*mMenuAndToolbar->getToolbar());
    mMenuAndToolbar->getToolbar()->set_toolbar_style(Gtk::TOOLBAR_BOTH);

    mModeling->setModified(false);
    set_title(WINDOW_TITLE);
    resize(900, 550);
    show();
}

GVLE::~GVLE()
{
    delete mModeling;

    delete mConditionsBox;
    delete mPreferencesBox;
    delete mOpenPackageBox;
    delete mOpenVpzBox;
    delete mNewProjectBox;
    delete mSaveVpzBox;
    delete mQuitBox;
    delete mMenuAndToolbar;

    Settings::settings().kill();
}

void GVLE::show()
{
    buildPackageHierarchy();
    show_all();
}

void GVLE::setModifiedTitle(const std::string& name)
{
    if (not name.empty() and utils::Path::extension(name) == ".vpz") {
	Editor::Documents::const_iterator it =
	    mEditor->getDocuments().find(name);
	if (it != mEditor->getDocuments().end())
	    it->second->setTitle(name ,getModeling()->getTopModel(), true);
    }
}

void GVLE::buildPackageHierarchy()
{
    mPackage = vle::utils::Path::path().getPackageDir();
    mFileTreeView->clear();
    mFileTreeView->setPackage(mPackage);
    mFileTreeView->build();
}

void GVLE::refreshPackageHierarchy()
{
    mFileTreeView->refresh();
}

void GVLE::refreshEditor(const std::string& oldName, const std::string& newName)
{
    if (newName.empty()) { // the file is removed
	mEditor->closeTab(Glib::build_filename(
			      mPackage, oldName));
    } else {
	std::string filePath = utils::Path::buildFilename(mPackage, oldName);
        std::string newFilePath = utils::Path::buildFilename(mPackage, newName);

        mEditor->changeFile(filePath, newFilePath);
	mModeling->setFileName(newFilePath);
    }
}

void GVLE::setFileName(std::string name)
{
    if (not name.empty() and utils::Path::extension(name) == ".vpz") {
	mModeling->parseXML(name);
	mMenuAndToolbar->showEditMenu();
	mMenuAndToolbar->showSimulationMenu();
	redrawModelTreeBox();
    }
    mModeling->setModified(false);
}

void GVLE::insertLog(const std::string& text)
{
    mLog->get_buffer()->insert(
	mLog->get_buffer()->end(), text);
}

void GVLE::redrawView()
{
    View* currentView = dynamic_cast<DocumentDrawingArea*>(
	mEditor->get_nth_page(mCurrentTab))->getView();
    currentView->redraw();
}

void GVLE::redrawModelTreeBox()
{
    assert(mModeling->getTopModel());
    mModelTreeBox->parseModel(mModeling->getTopModel());
}

void GVLE::redrawModelClassBox()
{
    mModelClassBox->parseClass();
}

void GVLE::clearModelTreeBox()
{
    mModelTreeBox->clear();
}

void GVLE::clearModelClassBox()
{
    mModelClassBox->clear();
}

void GVLE::showRowTreeBox(const std::string& name)
{
    mModelTreeBox->showRow(name);
}

void GVLE::showRowModelClassBox(const std::string& name)
{
    mModelClassBox->showRow(name);
}

bool GVLE::on_delete_event(GdkEventAny* event)
{
    if (event->type == GDK_DELETE) {
	onQuit();
	return true;
    }
    return false;
}

void GVLE::onArrow()
{
    mCurrentButton = POINTER;
    mStatusBar.push(_("Selection"));
}

void GVLE::onAddModels()
{
    mCurrentButton = ADDMODEL;
    mStatusBar.push(_("Add models"));
}

void GVLE::onAddLinks()
{
    mCurrentButton = ADDLINK;
    mStatusBar.push(_("Add links"));
}

void GVLE::onDelete()
{
    mCurrentButton = DELETE;
    mStatusBar.push(_("Delete object"));
}

void GVLE::onAddCoupled()
{
    mCurrentButton = ADDCOUPLED;
    mStatusBar.push(_("Coupled Model"));
}

void GVLE::onZoom()
{
    mCurrentButton = ZOOM;
    mStatusBar.push(_("Zoom"));
}

void GVLE::onQuestion()
{
    mCurrentButton = QUESTION;
    mStatusBar.push(_("Question"));
}

void GVLE::onNewFile()
{
    mEditor->createBlankNewFile();
    mMenuAndToolbar->onOpenFile();
    mMenuAndToolbar->showSave();
}

void GVLE::onNewVpz()
{
    if (not mModeling->isModified() or mModeling->getFileName().empty() or
	(mModeling->isModified() and
	 gvle::Question(_("Do you really want load a new Model ?\nCurrent "
			 "model will be destroy and not save")))) {
        mModeling->start();
        mMenuAndToolbar->onOpenVpz();
        mMenuAndToolbar->showSave();
    }
}

void GVLE::onNewProject()
{
    mNewProjectBox->show();
}

void GVLE::onOpenFile()
{
    Gtk::FileChooserDialog file(_("Choose a file"),
				Gtk::FILE_CHOOSER_ACTION_OPEN);
    file.set_transient_for(*this);
    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

    if (file.run() == Gtk::RESPONSE_OK) {
	std::string selected_file = file.get_filename();
	mEditor->openTab(selected_file);
	mMenuAndToolbar->onOpenFile();
    }
}

void GVLE::onOpenProject()
{
    if (mOpenPackageBox->run() == Gtk::RESPONSE_OK) {
        onCloseProject();
        utils::Package::package().select(mOpenPackageBox->name());
        mPluginFactory.update();
        buildPackageHierarchy();
	mMenuAndToolbar->onOpenProject();
        setTitle("");
    }
}

void GVLE::onOpenVpz()
{
    if (not mModeling->isModified() or mModeling->getFileName().empty() or
	(mModeling->isModified() and
	 gvle::Question(_("Do you really want load a new Model ?\nCurrent "
			 "model will be destroy and not save")))) {
	try {
	    if (mOpenVpzBox->run() == Gtk::RESPONSE_OK) {
                redrawModelTreeBox();
                redrawModelClassBox();
                mMenuAndToolbar->onOpenVpz();
            }
	} catch(utils::InternalError) {
            Error((fmt(_("No experiments in the package '%1%'")) %
                  utils::Package::package().name()).str());
	}
    }
}

void GVLE::onOpenGlobalVpz()
{
    if (not mModeling->isModified() or mModeling->getFileName().empty() or
	(mModeling->isModified() and
	 gvle::Question(_("Do you really want load a new Model ?\nCurrent "
			 "model will be destroy and not save")))) {
        Gtk::FileChooserDialog file("VPZ file", Gtk::FILE_CHOOSER_ACTION_OPEN);
        file.set_transient_for(*this);
        file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
        file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
        Gtk::FileFilter filter;
        filter.set_name(_("Vle Project gZipped"));
        filter.add_pattern("*.vpz");
        file.add_filter(filter);
	if (mGlobalVpzPrevDirPath != "") {
	    file.set_current_folder(mGlobalVpzPrevDirPath);
        }

        if (file.run() == Gtk::RESPONSE_OK) {
	    mGlobalVpzPrevDirPath = file.get_current_folder();
	    mEditor->closeAllTab();
	    utils::Package::package().select("");
            mPluginFactory.update();
	    mPackage = "";
            mModeling->parseXML(file.get_filename());
            if (mModeling->getTopModel()) {
                redrawModelTreeBox();
                redrawModelClassBox();
                mMenuAndToolbar->onOpenVpz();
                mMenuAndToolbar->hideCloseProject();
                mFileTreeView->clear();
            }
        }
    }
}

void GVLE::onRefresh()
{
    mFileTreeView->refresh();
}

void GVLE::onSave()
{
    int page = mEditor->get_current_page();

    if (page != -1) {
	if (dynamic_cast < Document* >(mEditor->get_nth_page(page))
	    ->isDrawingArea()) {
	    saveVpz();
	} else {
	    DocumentText* doc = dynamic_cast < DocumentText* >(
		mEditor->get_nth_page(page));

	    if (not doc->isNew()) {
		doc->save();
	    } else {
		Gtk::FileChooserDialog file(_("Text file"),
					    Gtk::FILE_CHOOSER_ACTION_SAVE);

		file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
		file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
		file.set_current_folder(utils::Path::path().getPackageDir());
		if (file.run() == Gtk::RESPONSE_OK) {
		    std::string filename(file.get_filename());

		    doc->saveAs(filename);
		    mModeling->setFileName(filename);
		}
	    }
	    refreshPackageHierarchy();
	}
    }
}

void GVLE::onSaveAs()
{
    int page = mEditor->get_current_page();

    Glib::ustring title;

    bool isVPZ = dynamic_cast < Document* >(mEditor->get_nth_page(page))
	->isDrawingArea();

    if (page != -1) {
	if (isVPZ) {
	    title = _("VPZ file");
	} else {
	    title = _("Text file");
	}

	Gtk::FileChooserDialog file(title,
				    Gtk::FILE_CHOOSER_ACTION_SAVE);

	file.set_transient_for(*this);
	file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

	if (isVPZ) {
	    Gtk::FileFilter filter;
	    filter.set_name(_("Vle Project gZipped"));
	    filter.add_pattern("*.vpz");
	    file.add_filter(filter);
	}

	// to provide a default filename
	// but also a default location
	file.set_filename(dynamic_cast < Document* >(mEditor->get_nth_page(page))
			  ->filepath());

	if (file.run() == Gtk::RESPONSE_OK) {
	    std::string filename(file.get_filename());

	    if (isVPZ) {
		vpz::Vpz::fixExtension(filename);

		Editor::Documents::const_iterator it =
		    mEditor->getDocuments().find(mModeling->getFileName());

		mModeling->saveXML(filename);

		if (it != mEditor->getDocuments().end()) {
		    it->second->setTitle(filename,
					 mModeling->getTopModel(), false);
		}
	    } else {
		DocumentText* doc = dynamic_cast < DocumentText* >(
		    mEditor->get_nth_page(page));

		doc->saveAs(filename);
	    }
	    refreshPackageHierarchy();
	}
    }
}

void GVLE::fixSave()
{
    if (not mEditor->getDocuments().empty()) {
        int page = mEditor->get_current_page();
        Gtk::Widget* tab = mEditor->get_nth_page(page);
        bool found = false;
        Editor::Documents::const_iterator it =
            mEditor->getDocuments().begin();

        while (not found and it != mEditor->getDocuments().end()) {
            if (it->second == tab) {
                found = true;
            } else {
                ++it;
            }
        }
        if (it->second->isModified()) {
            mMenuAndToolbar->showSave();
        } else {
            mMenuAndToolbar->hideSave();
        }
    }
}

bool GVLE::closeTab(const std::string& filepath)
{
    bool vpz = false;
    bool close = false;
    Editor::Documents::const_iterator it =
        mEditor->getDocuments().find(filepath);

    if (it != mEditor->getDocuments().end()) {
        if (not it->second->isModified() or
            gvle::Question(_("The current tab is not saved\n"
                             "Do you really want to close this file ?"))) {
            if (it->second->isDrawingArea()) {
                mModeling->clearModeling();
                clearModelTreeBox();
                clearModelClassBox();
                vpz = true;
                close = true;
            }
            mEditor->closeTab(it->first);
            mMenuAndToolbar->onCloseTab(vpz, mEditor->getDocuments().empty());
            fixSave();
            updateTitle();
        }
    }
    return close;
}

void GVLE::onCloseTab()
{
    bool vpz = false;
    int page = mEditor->get_current_page();
    Gtk::Widget* tab = mEditor->get_nth_page(page);
    bool found = false;
    Editor::Documents::const_iterator it = mEditor->getDocuments().begin();

    while (not found and it != mEditor->getDocuments().end()) {
        if (it->second == tab) {
            found = true;
        } else {
            ++it;
        }
    }
    if (found) {
        if (not it->second->isModified() or
            gvle::Question(_("The current tab is not saved\n"
                             "Do you really want to close this file ?"))) {
            if (it->second->isDrawingArea()) {
                mModeling->clearModeling();
                clearModelTreeBox();
                clearModelClassBox();
                vpz = true;
            }
            mEditor->closeTab(it->first);
            mMenuAndToolbar->onCloseTab(vpz, mEditor->getDocuments().empty());
            fixSave();
            updateTitle();
        }
    }
}

void GVLE::onCloseProject()
{
    mEditor->closeAllTab();
    mModeling->clearModeling();
    clearModelTreeBox();
    clearModelClassBox();
    utils::Package::package().select("");
    mPluginFactory.update();
    buildPackageHierarchy();
    mMenuAndToolbar->showMinimalMenu();
    setTitle("");
}

void GVLE::onQuit()
{
    mQuitBox->show();
}

void GVLE::onPreferences()
{
    if (mPreferencesBox->run() == Gtk::RESPONSE_OK) {
        mModeling->refreshViews();
        mEditor->refreshViews();
    }
}

void GVLE::onSimulationBox()
{
    if (mModeling->isSaved()) {
        LaunchSimulationBox box(mRefXML, ((const Modeling*)mModeling)->vpz());
        box.run();
    } else {
        gvle::Error(_("Save or load a project before simulation"));
    }
}

void GVLE::onParameterExecutionBox()
{
    ParameterExecutionBox box(mModeling);
    box.run();
}

void GVLE::onExperimentsBox()
{
    ExperimentBox box(mRefXML, mModeling);
    box.run();
}

void GVLE::onConditionsBox()
{
    const Modeling* modeling = (const Modeling*)mModeling;

    if (runConditionsBox(modeling->conditions()) == 1) {
	applyConditionsBox(mModeling->conditions());

	{
	    vpz::AtomicModelList& list =
		mModeling->vpz().project().model().atomicModels();
	    vpz::AtomicModelList::iterator it = list.begin();

	    while (it != list.end()) {
		vpz::Strings mdlConditions = it->second.conditions();
		vpz::Strings::const_iterator its = mdlConditions.begin();

		while (its != mdlConditions.end()) {
		    if (not mModeling->conditions().exist(*its)) {
			it->second.delCondition(*its);
		    }
		    ++its;
		}
		++it;
	    }
	}
    }
}

int GVLE::runConditionsBox(const vpz::Conditions& conditions)
{
    return mConditionsBox->run(conditions);
}

void GVLE::applyConditionsBox(vpz::Conditions& conditions)
{
    mConditionsBox->apply(conditions);
}

void GVLE::onHostsBox()
{
    HostsBox box(mRefXML);
    box.run();
}

void GVLE::onHelpBox()
{
}

void GVLE::onViewOutputBox()
{
    const Modeling* modeling((const Modeling*)mModeling);
    vpz::Views views(modeling->views());
    ViewOutputBox box(*mModeling, mRefXML, views);
    box.run();
}

void GVLE::onShowAbout()
{
    About box(mRefXML);
    box.run();
}

void GVLE::saveVpz()
{
    std::vector<std::string> vec;

    mModeling->vpz_is_correct(vec);
    if (vec.size() != 0) {
        std::string error = _("Vpz incorrect :\n");
        std::vector<std::string>::const_iterator it = vec.begin();

        while (it != vec.end()) {
            error += *it + "\n";
            ++it;
        }
        Error(error);
        return;
    }

    if (mModeling->isSaved()) {
        Editor::Documents::const_iterator it =
            mEditor->getDocuments().find(mModeling->getFileName());

        mModeling->saveXML(mModeling->getFileName());
        if (it != mEditor->getDocuments().end()) {
            it->second->setTitle(mModeling->getFileName(),
                                 mModeling->getTopModel(), false);
        }
    } else {
        saveFirstVpz();
        refreshPackageHierarchy();
    }
}

void GVLE::saveFirstVpz()
{
    if (not utils::Package::package().name().empty()) {
	mSaveVpzBox->show();
    } else {
	Gtk::FileChooserDialog file(_("VPZ file"),
				    Gtk::FILE_CHOOSER_ACTION_SAVE);
	file.set_transient_for(*this);
	file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
	Gtk::FileFilter filter;
	filter.set_name(_("Vle Project gZipped"));
	filter.add_pattern("*.vpz");
	file.add_filter(filter);

	if (file.run() == Gtk::RESPONSE_OK) {
	    std::string filename(file.get_filename());
	    vpz::Vpz::fixExtension(filename);
	    Editor::Documents::const_iterator it =
		mEditor->getDocuments().find(mModeling->getFileName());
	    mModeling->saveXML(filename);
	    if (it != mEditor->getDocuments().end()) {
		it->second->setTitle(filename,
				     mModeling->getTopModel(), false);
	    }
	}
    }
}

void GVLE::setTitle(const Glib::ustring& name)
{
    Glib::ustring title(WINDOW_TITLE);

    if (utils::Package::package().selected()) {
	title += " - " + utils::Package::package().name();
    }

    if (not name.empty()) {
        title += " - " + Glib::path_get_basename(name);
    }
    set_title(title);
}

void GVLE::updateTitle()
{
    if (not mEditor->getDocuments().empty()) {
        int page = mEditor->get_current_page();
        Gtk::Widget* tab = mEditor->get_nth_page(page);
        bool found = false;
        Editor::Documents::const_iterator it =
            mEditor->getDocuments().begin();

        while (not found and it != mEditor->getDocuments().end()) {
            if (it->second == tab) { found = true; } else { ++it; }
        }
        if (found) {
            setTitle(utils::Path::basename(it->second->filename()) +
                     utils::Path::extension(it->second->filename()));
        }
    }
}

std::string valuetype_to_string(value::Value::type type)
{
    switch (type) {
    case(value::Value::BOOLEAN):
        return "boolean";
        break;
    case(value::Value::INTEGER):
        return "integer";
        break;
    case(value::Value::DOUBLE):
        return "double";
        break;
    case(value::Value::STRING):
        return "string";
        break;
    case(value::Value::SET):
        return "set";
        break;
    case(value::Value::MAP):
        return "map";
        break;
    case(value::Value::TUPLE):
        return "tuple";
        break;
    case(value::Value::TABLE):
        return "table";
        break;
    case(value::Value::XMLTYPE):
        return "xml";
        break;
    case(value::Value::NIL):
        return "null";
        break;
    case(value::Value::MATRIX):
        return "matrix";
        break;
    default:
        return "(no value)";
        break;
    }
}

bool GVLE::packageTimer()
{
    std::string o, e;
    utils::Package::package().getOutputAndClear(o);
    utils::Package::package().getErrorAndClear(e);

    if (not o.empty()) {
        mLog->get_buffer()->insert(mLog->get_buffer()->end(), o);
    }

    if (not e.empty()) {
        mLog->get_buffer()->insert(mLog->get_buffer()->end(), e);
    }

    Gtk::TextBuffer::iterator iter = mLog->get_buffer()->end();
    mLog->get_buffer()->place_cursor(iter);
    mLog->scroll_to(iter, 0.0, 0.0, 1.0);

    if (utils::Package::package().isFinish()) {
        getMenu()->showProjectMenu();
        return false;
    } else {
        return true;
    }
}

bool GVLE::packageBuildTimer()
{
    std::string o, e;
    utils::Package::package().getOutputAndClear(o);
    utils::Package::package().getErrorAndClear(e);

    if (not o.empty()) {
        mLog->get_buffer()->insert(mLog->get_buffer()->end(), o);
    }

    if (not e.empty()) {
        mLog->get_buffer()->insert(mLog->get_buffer()->end(), e);
    }

    Gtk::TextBuffer::iterator iter = mLog->get_buffer()->end();
    mLog->get_buffer()->place_cursor(iter);
    mLog->scroll_to(iter, 0.0, 0.0, 1.0);

    if (utils::Package::package().isFinish()) {
        if (utils::Package::package().isSuccess()) {
            installProject();
        } else {
            getMenu()->showProjectMenu();
        }
        return false;
    } else {
        return true;
    }
}

void GVLE::configureProject()
{
    mLog->get_buffer()->insert(mLog->get_buffer()->end(),
                               "configure package\n");
    getMenu()->hideProjectMenu();
    try {
        utils::Package::package().configure();
    } catch (const std::exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    } catch (const Glib::Exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    }
    Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &GVLE::packageTimer), 250);
}

void GVLE::buildProject()
{
    mLog->get_buffer()->insert(mLog->get_buffer()->end(),
                               "build package\n");
    getMenu()->hideProjectMenu();
    try {
        utils::Package::package().build();
    } catch (const std::exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    } catch (const Glib::Exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    }
    Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &GVLE::packageBuildTimer), 250);
}

void GVLE::installProject()
{
    mLog->get_buffer()->insert(mLog->get_buffer()->end(),
                               "build package\n");
    getMenu()->hideProjectMenu();
    try {
        utils::Package::package().install();
    } catch (const std::exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    } catch (const Glib::Exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    }
    Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &GVLE::packageTimer), 250);
}

void GVLE::cleanProject()
{
    mLog->get_buffer()->insert(mLog->get_buffer()->end(),
                               "clean package\n");
    getMenu()->hideProjectMenu();
    try {
        utils::Package::package().clean();
    } catch (const std::exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    } catch (const Glib::Exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    }
    Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &GVLE::packageTimer), 250);
}

void GVLE::packageProject()
{
    mLog->get_buffer()->insert(mLog->get_buffer()->end(),
                               "make tarball\n");
    getMenu()->hideProjectMenu();
    try {
        utils::Package::package().pack();
    } catch (const std::exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    } catch (const Glib::Exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    }
    Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &GVLE::packageTimer), 250);
}

void GVLE::onCutModel()
{
    if (mCurrentTab >= 0) {
	if (dynamic_cast<Document*>(mEditor->get_nth_page(mCurrentTab))
	    ->isDrawingArea()) {
	    View* currentView = dynamic_cast<DocumentDrawingArea*>(
		mEditor->get_nth_page(mCurrentTab))->getView();

	    if (currentView) {
		currentView->onCutModel();
		mMenuAndToolbar->showPaste();
	    }
	} else {
	    DocumentText* doc = dynamic_cast<DocumentText*>(
		mEditor->get_nth_page(mCurrentTab));

	    if (doc) {
		doc->cut();
	    }
	    mMenuAndToolbar->showPaste();
	}
    }
}

void GVLE::onCopyModel()
{
    if (mCurrentTab >= 0) {
	if (dynamic_cast<Document*>(mEditor->get_nth_page(mCurrentTab))
	    ->isDrawingArea()) {
	    View* currentView = dynamic_cast<DocumentDrawingArea*>(
		mEditor->get_nth_page(mCurrentTab))->getView();

	    if (currentView) {
		currentView->onCopyModel();
		mMenuAndToolbar->showPaste();
	    }
	} else {
	    DocumentText* doc = dynamic_cast<DocumentText*>(
		mEditor->get_nth_page(mCurrentTab));

	    if (doc) {
		doc->copy();
	    }
	    mMenuAndToolbar->showPaste();
	}
    }
}

void GVLE::onPasteModel()
{
    if (mCurrentTab >= 0) {
	if (dynamic_cast<Document*>(mEditor->get_nth_page(mCurrentTab))
	    ->isDrawingArea()) {
	    View* currentView = dynamic_cast<DocumentDrawingArea*>(
		mEditor->get_nth_page(mCurrentTab))->getView();

	    if (currentView) {
		currentView->onPasteModel();
	    }
	} else {
	    DocumentText* doc = dynamic_cast<DocumentText*>(
		mEditor->get_nth_page(mCurrentTab));

	    if (doc) {
		doc->paste();
	    }
	}
    }
}

void GVLE::clearCurrentModel()
{
    if (mCurrentTab >= 0) {
	View* currentView = dynamic_cast<DocumentDrawingArea*>(
	    mEditor->get_nth_page(mCurrentTab))->getView();

	if (currentView) {
	    currentView->clearCurrentModel();
	}
    }
}

void GVLE::importModel()
{
    View* currentView = dynamic_cast<DocumentDrawingArea*>(
	mEditor->get_nth_page(mCurrentTab))->getView();
    currentView->importModel();
}

void GVLE::exportCurrentModel()
{
    View* currentView = dynamic_cast<DocumentDrawingArea*>(
	mEditor->get_nth_page(mCurrentTab))->getView();
    currentView->exportCurrentModel();
}

void GVLE::exportGraphic()
{
    ViewDrawingArea* tab = dynamic_cast<DocumentDrawingArea*>(
	mEditor->get_nth_page(mCurrentTab))->getDrawingArea();

    const vpz::Experiment& experiment = ((const Modeling*)mModeling)
	->vpz().project().experiment();
    if (experiment.name().empty() || experiment.duration() == 0) {
	Error(_("Fix a Value to the name and the duration "	\
		"of the experiment before exportation."));
        return;
    }

    Gtk::FileChooserDialog file(_("Image file"), Gtk::FILE_CHOOSER_ACTION_SAVE);
    file.set_transient_for(*this);
    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    Gtk::FileFilter filterAuto;
    Gtk::FileFilter filterPng;
    Gtk::FileFilter filterPdf;
    Gtk::FileFilter filterSvg;
    filterAuto.set_name(_("Guess type from file name"));
    filterAuto.add_pattern("*");
    filterPng.set_name(_("Portable Newtork Graphics (.png)"));
    filterPng.add_pattern("*.png");
    filterPdf.set_name(_("Portable Format Document (.pdf)"));
    filterPdf.add_pattern("*.pdf");
    filterSvg.set_name(_("Scalable Vector Graphics (.svg)"));
    filterSvg.add_pattern("*.svg");
    file.add_filter(filterAuto);
    file.add_filter(filterPng);
    file.add_filter(filterPdf);
    file.add_filter(filterSvg);


    if (file.run() == Gtk::RESPONSE_OK) {
        std::string filename(file.get_filename());
	std::string extension(file.get_filter()->get_name());

	if (extension == _("Guess type from file name")) {
	    size_t ext_pos = filename.find_last_of('.');
	    if (ext_pos != std::string::npos) {
		std::string type(filename, ext_pos+1);
		filename.resize(ext_pos);
		if (type == "png")
		    tab->exportPng(filename);
		else if (type == "pdf")
		    tab->exportPdf(filename);
		else if (type == "svg")
		    tab->exportSvg(filename);
		else
		    Error(_("Unsupported file format"));
	    }
	}
	else if (extension == _("Portable Newtork Graphics (.png)"))
	    tab->exportPng(filename);
	else if (extension == _("Portable Format Document (.pdf)"))
	    tab->exportPdf(filename);
	else if (extension == _("Scalable Vector Graphics (.svg)"))
	    tab->exportSvg(filename);
    }
}

void GVLE::addCoefZoom()
{
    ViewDrawingArea* tab = dynamic_cast<DocumentDrawingArea*>(
	mEditor->get_nth_page(mCurrentTab))->getDrawingArea();
    tab->addCoefZoom();
}

void GVLE::delCoefZoom()
{
    ViewDrawingArea* tab = dynamic_cast<DocumentDrawingArea*>(
	mEditor->get_nth_page(mCurrentTab))->getDrawingArea();
    tab->delCoefZoom();
}

void GVLE::setCoefZoom(double coef)
{
    ViewDrawingArea* tab = dynamic_cast<DocumentDrawingArea*>(
	mEditor->get_nth_page(mCurrentTab))->getDrawingArea();
    tab->setCoefZoom(coef);
}

void  GVLE::updateAdjustment(double h, double v)
{
    DocumentDrawingArea* tab = dynamic_cast<DocumentDrawingArea*>(
	mEditor->get_nth_page(mCurrentTab));
    tab->setHadjustment(h);
    tab->setVadjustment(v);
}

void GVLE::onRandomOrder()
{
    ViewDrawingArea* tab = dynamic_cast<DocumentDrawingArea*>(
	mEditor->get_nth_page(mCurrentTab))->getDrawingArea();
    tab->onRandomOrder();
    mModeling->setModified(true);
}

}} // namespace vle gvle
