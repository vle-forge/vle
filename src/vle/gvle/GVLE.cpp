/*
 * @file vle/gvle/GVLE.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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
#include <vle/gvle/DynamicsBox.hpp>
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
#include <sstream>
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
    Gtk::TreeView(cobject)
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
		_("_Copy"),
		sigc::mem_fun(
		    *this,
		    &GVLE::FileTreeView::onCopy)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Paste"),
		sigc::mem_fun(
		    *this,
		    &GVLE::FileTreeView::onPaste)));
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

    signal_event().connect(sigc::mem_fun(*this, &GVLE::FileTreeView::onEvent));
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
        mColumnName = append_column_editable(utils::Path::filename(mPackage),
                                    mColumns.m_col_name);
	mCellrenderer = dynamic_cast<Gtk::CellRendererText*>(
	    get_column_cell_renderer(mColumnName - 1));
	buildHierarchy(0, mPackage);
    } else {
	mColumnName = append_column_editable("Project", mColumns.m_col_name);
	mCellrenderer = dynamic_cast<Gtk::CellRendererText*>(
	    get_column_cell_renderer(mColumnName - 1));
    }

    mCellrenderer->signal_edited().connect(
             sigc::mem_fun(*this, &GVLE::FileTreeView::onEdition));
    mCellrenderer->signal_editing_started().connect(
             sigc::mem_fun(*this, &GVLE::FileTreeView::onEditionStarted));
}

bool GVLE::FileTreeView::isDirectory(const std::string& dirname)
{
    return Glib::file_test(dirname, Glib::FILE_TEST_IS_DIR);
}


bool GVLE::FileTreeView::onEvent(GdkEvent* event)
{
    if (event->type == GDK_2BUTTON_PRESS) {
        mDelayTime = event->button.time;
        Gtk::TreeModel::Path path;
        Gtk::TreeViewColumn* column;
        get_cursor(path, column);
        on_row_activated(path, column);
        return true;
    }

    if (event->type == GDK_BUTTON_PRESS) {
        if (mDelayTime + 250 < event->button.time and
            mDelayTime + 1000 > event->button.time) {
            mDelayTime = event->button.time;
            mCellrenderer->property_editable() = true;
        } else {
            mDelayTime = event->button.time;
            mCellrenderer->property_editable() = false;
        }

    }
    return false;
}

void GVLE::FileTreeView::onEditionStarted(Gtk::CellEditable* /* cellEditable */,
                                          const Glib::ustring& /* path */)
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    if (refSelection) {
	    Gtk::TreeModel::const_iterator it = refSelection->get_selected();
	    if (it) {
            const Gtk::TreeModel::Row row = *it;
            mOldName = row.get_value(mColumns.m_col_name);
        }
    }
}

void GVLE::FileTreeView::onEdition(const Glib::ustring& pathString,
                                   const Glib::ustring& newFileName)
{
    std::string oldName = mOldName;
    std::string newName = newFileName;

    Gtk::TreeModel::const_iterator it = mRefTreeModel->get_iter(pathString);

    if (it) {
        const Gtk::TreeModel::Row row = *it;

        std::list < std::string > lstpath;
        projectFilePath(row, lstpath);

        if (utils::Path::extension(mOldName) != utils::Path::extension(newFileName)) {
            Error(_("When renaming a file, changing his extension is not allowed!"));
            Glib::ustring oldName = mOldName;
            row.set_value(mColumns.m_col_name, oldName);
            return;
        }

        std::string newFilePath = Glib::build_filename(lstpath);
        std::string oldFilePath =
            Glib::path_get_dirname(newFilePath) + "/" + oldName;

        if (not utils::Path::exist(Glib::build_filename(
            utils::Path::path().getPackageDir(), newFilePath))) {
                utils::Package::package().renameFile(oldFilePath, newName);
                mParent->refreshEditor(oldFilePath, newFilePath);
        } else {
            Glib::ustring oldName = mOldName;
            row.set_value(mColumns.m_col_name, oldName);
        }

        mParent->refreshPackageHierarchy();
    }
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
                    if (mParent->mModeling->getTopModel()) {
                        mParent->redrawModelTreeBox();
                        mParent->redrawModelClassBox();
                        mParent->getMenu()->onOpenVpz();
                        mParent->mModelTreeBox->set_sensitive(true);
                        mParent->mModelClassBox->set_sensitive(true);
                    }
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

        if(name.find(".vpz") != std::string::npos) {
            mParent->onNewNamedVpz(filepath, name);
            mParent->getModeling()->setSaved(true);
        } else {
            mParent->onNewFile(filepath, name);
        }
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

void GVLE::FileTreeView::onCopy()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    if (refSelection) {
        Gtk::TreeModel::const_iterator it = refSelection->get_selected();
        if (it) {
            const Gtk::TreeModel::Row row = *it;

            std::list < std::string > lstpath;
            projectFilePath(row, lstpath);

            mFileName = row.get_value(mColumns.m_col_name);

            mFilePath = Glib::build_filename(lstpath);

            mAbsolutePath =
                Glib::build_filename(utils::Path::path().getPackageDir(),
                                     mFilePath);
        }
    }
}

void GVLE::FileTreeView::onPaste()
{
    if (!(utils::Path::exist(mAbsolutePath))) {
        return;
    }

    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    if (refSelection) {
        Gtk::TreeModel::const_iterator it = refSelection->get_selected();
        if (it) {
            const Gtk::TreeModel::Row row = *it;

            std::list < std::string > lstpath;
            projectFilePath(row, lstpath);
            std::string newAbsolutePath;
            std::string newFileName;
            std::string newFilePath;

            int copyNumber = 1;
            std::string suffixe;
            do {
                suffixe = "_" + boost::lexical_cast < std::string >(copyNumber);

                newFileName = mFileName;
                newFileName.insert(newFileName.find_last_of("."),suffixe);

                if (isDirectory(Glib::build_filename(utils::Path::path().getPackageDir(),Glib::build_filename(lstpath)))) {
                    newFilePath = Glib::build_filename(lstpath) + "/" + newFileName;
                } else {
                    newFilePath = Glib::path_get_dirname(Glib::build_filename(lstpath)) + "/" + newFileName;
                }

                newAbsolutePath =
                    Glib::build_filename(utils::Path::path().getPackageDir(),
                                         newFilePath);
                copyNumber++;

            } while (utils::Path::exist(newAbsolutePath));

            if (utils::Path::extension(newFileName) == "") {
                newFileName += utils::Path::extension(mAbsolutePath);
            }

            if (not isDirectory(mAbsolutePath)){
                utils::Package::package().copyFile(mAbsolutePath, newAbsolutePath);
            }

            mParent->refreshPackageHierarchy();

            m_search = newFilePath;
            mRefTreeModel->foreach(sigc::mem_fun(*this, &GVLE::FileTreeView::on_foreach));
        }
    }
}

bool GVLE::FileTreeView::on_foreach(const Gtk::TreeModel::Path&,
                              const Gtk::TreeModel::iterator& iter)
{
    const Gtk::TreeModel::Row row = *iter;

    std::list < std::string > lstpath;
    projectFilePath(row, lstpath);

    std::string  fileName = row.get_value(mColumns.m_col_name);
    std::string  filePath = Glib::build_filename(lstpath);

    if (filePath == m_search) {
        mCellrenderer->property_editable() = true;
        Gtk::TreeViewColumn* nameCol = get_column(mColumnName - 1);
        set_cursor(mRefTreeModel->get_path(iter),*nameCol,true);
        return true;
    }
    return false;
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

        Gtk::TreeViewColumn* nameCol = get_column(mColumnName - 1);

        mCellrenderer->property_editable() =  true;

        set_cursor(mRefTreeModel->get_path(it),*nameCol,true);

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

void GVLE::FileTreeView::openTab(std::string path)
{
    if (utils::Path::extension(path) == ".vpz") {
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
            mParent->getEditor()->openTab(path);
            if (mParent->mModeling->getTopModel()) {
                mParent->redrawModelTreeBox();
                mParent->redrawModelClassBox();
                mParent->getMenu()->onOpenVpz();
                mParent->mModelTreeBox->set_sensitive(true);
                mParent->mModelClassBox->set_sensitive(true);
            }
        }
    } else {
        mParent->getEditor()->openTab(path);
        mParent->getMenu()->onOpenFile();
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
    mModelTreeBox->set_sensitive(false);
    mModelClassBox->set_sensitive(false);
    mFileTreeView->set_sensitive(false);

    mMenuAndToolbar = new GVLEMenuAndToolbar(this);
    mMenuAndToolbarVbox->pack_start(*mMenuAndToolbar->getMenuBar());
    mMenuAndToolbarVbox->pack_start(*mMenuAndToolbar->getToolbar());
    mMenuAndToolbar->getToolbar()->set_toolbar_style(Gtk::TOOLBAR_BOTH);

    mModeling->setModified(false);

    Glib::signal_timeout().connect( sigc::mem_fun(*this, &GVLE::on_timeout), 1000 );

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

bool GVLE::on_timeout()
{
    mConnectionTimeout.disconnect();

    mStatusbar->push("");
    return false;
}

void GVLE::show()
{
    buildPackageHierarchy();
    show_all();
}

void GVLE::showMessage(const std::string& message)
{
    mConnectionTimeout.disconnect();
    mConnectionTimeout = Glib::signal_timeout().
        connect(sigc::mem_fun(*this, &GVLE::on_timeout), 30000);

    mStatusbar->push(message);
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
        mEditor->closeTab(Glib::build_filename(mPackage, oldName));
    } else {
        std::string filePath = utils::Path::buildFilename(mPackage, oldName);
        std::string newFilePath = utils::Path::buildFilename(mPackage, newName);;
        mEditor->changeFile(filePath, newFilePath);
        mModeling->setFileName(newFilePath);
    }
}

void GVLE::setFileName(std::string name)
{
    if (not name.empty() and utils::Path::extension(name) == ".vpz") {
        mModeling->parseXML(name);
        if (mModeling->getTopModel()) {
            mMenuAndToolbar->showEditMenu();
            mMenuAndToolbar->showSimulationMenu();
            redrawModelTreeBox();
            redrawModelClassBox();
        }
    }
    mModeling->setModified(false);
}

void GVLE::insertLog(const std::string& text)
{
    if (not text.empty()) {
        Glib::RefPtr < Gtk::TextBuffer > ref = mLog->get_buffer();
        if (ref) {
            ref->insert(ref->end(), text);
        }
    }
}

void GVLE::scrollLogToLastLine()
{
    Glib::RefPtr < Gtk::TextBuffer > ref = mLog->get_buffer();
    if (ref) {
        Glib::RefPtr < Gtk::TextMark > mark = ref->get_mark("end");
        if (mark) {
            ref->move_mark(mark, ref->end());
        } else {
            mark = ref->create_mark("end", ref->end());
        }

        mLog->scroll_to(mark, 0.0, 0.0, 1.0);
    }
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
    mEditor->getDocumentDrawingArea()->updateCursor();
   showMessage(_("Selection"));
}

void GVLE::onAddModels()
{
    mCurrentButton = ADDMODEL;
    mEditor->getDocumentDrawingArea()->updateCursor();
   showMessage(_("Add models"));
}

void GVLE::onAddLinks()
{
    mCurrentButton = ADDLINK;
    mEditor->getDocumentDrawingArea()->updateCursor();
   showMessage(_("Add links"));
}

void GVLE::onDelete()
{
    mCurrentButton = DELETE;
    mEditor->getDocumentDrawingArea()->updateCursor();
   showMessage(_("Delete object"));
}

void GVLE::onAddCoupled()
{
    mCurrentButton = ADDCOUPLED;
    mEditor->getDocumentDrawingArea()->updateCursor();
   showMessage(_("Coupled Model"));
}

void GVLE::onZoom()
{
    mCurrentButton = ZOOM;
    mEditor->getDocumentDrawingArea()->updateCursor();
   showMessage(_("Zoom"));
}

void GVLE::onQuestion()
{
    mCurrentButton = QUESTION;
    mEditor->getDocumentDrawingArea()->updateCursor();
   showMessage(_("Question"));
}

void GVLE::onNewFile()
{
    mEditor->createBlankNewFile();
    mMenuAndToolbar->onOpenFile();
    mMenuAndToolbar->showSave();
}

void GVLE::onNewFile(const std::string& path, const std::string& fileName)
{
    mEditor->createBlankNewFile(path, fileName);
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
        mEditor->getDocumentDrawingArea()->updateCursor();
        mModelTreeBox->set_sensitive(true);
        mModelClassBox->set_sensitive(true);
        if (mCurrentButton == POINTER){
           showMessage(_("Selection"));
        }
    }
    onExperimentsBox();
}

void GVLE::onNewNamedVpz(const std::string& path, const std::string& filename)
{
    if (not mModeling->isModified() or mModeling->getFileName().empty() or
	(mModeling->isModified() and
	 gvle::Question(_("Do you really want load a new Model ?\nCurrent "
                          "model will be destroy and not save")))) {
        mModeling->start(path.c_str(), filename.c_str());
        mMenuAndToolbar->onOpenVpz();
        mMenuAndToolbar->showSave();
        mEditor->getDocumentDrawingArea()->updateCursor();
        mModelTreeBox->set_sensitive(true);
        mModelClassBox->set_sensitive(true);
        if (mCurrentButton == POINTER){
           showMessage(_("Selection"));
        }
    }
    onExperimentsBox();
}

void GVLE::onNewProject()
{
    mNewProjectBox->show();
    mMenuAndToolbar->onOpenProject();
    clearModelTreeBox();
    clearModelClassBox();
    mFileTreeView->set_sensitive(true);
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
        mFileTreeView->set_sensitive(true);
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
                mModelTreeBox->set_sensitive(true);
                mModelClassBox->set_sensitive(true);
                mEditor->getDocumentDrawingArea()->updateCursor();
                if (mCurrentButton == POINTER){
                   showMessage(_("Selection"));
                }
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
                mModelTreeBox->set_sensitive(true);
                mModelClassBox->set_sensitive(true);
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

void GVLE::onShowCompleteView()
{
    DocumentDrawingArea* tab = dynamic_cast<DocumentDrawingArea*>(
	mEditor->get_nth_page(mCurrentTab));
    graph::CoupledModel* currentModel;
    if (tab-> isComplete()) {
	currentModel = tab->getCompleteDrawingArea()->getModel();
    } else {
	currentModel = tab->getSimpleDrawingArea()->getModel();
    }
    mEditor->showCompleteView(mModeling->getFileName(),currentModel);
}

void GVLE::onShowSimpleView()
{
    DocumentDrawingArea* tab = dynamic_cast<DocumentDrawingArea*>(
	mEditor->get_nth_page(mCurrentTab));
    graph::CoupledModel* currentModel;
    if (tab-> isComplete()) {
	currentModel = tab->getCompleteDrawingArea()->getModel();
    } else {
	currentModel = tab->getSimpleDrawingArea()->getModel();
    }
    mEditor->showSimpleView(mModeling->getFileName(), currentModel);
}

bool GVLE::checkVpz()
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
        return false;
    }
    return true;
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

	    if (not doc->isNew() || doc->hasFullName()) {
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
        if (!checkVpz()) {
            return;
        }
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
                mModelTreeBox->set_sensitive(false);
                mModelClassBox->set_sensitive(false);
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
                mModelTreeBox->set_sensitive(false);
                mModelClassBox->set_sensitive(false);
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
    mModelTreeBox->set_sensitive(false);
    mModelClassBox->set_sensitive(false);
    utils::Package::package().select("");
    mPluginFactory.update();
    buildPackageHierarchy();
    mMenuAndToolbar->showMinimalMenu();
    setTitle("");
    mFileTreeView->set_sensitive(false);

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
                std::vector < std::string > mdlConditions =
                    it->second.conditions();
                std::vector < std::string >::const_iterator its =
                    mdlConditions.begin();

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

void GVLE::onDynamicsBox()
{
    const Modeling* modeling((const Modeling*)mModeling);
    vpz::Dynamics dynamics(modeling->dynamics());
    DynamicsBox box(*mModeling, mRefXML, dynamics);
    box.run();
}

void GVLE::onShowAbout()
{
    About box(mRefXML);
    box.run();
}

void GVLE::saveVpz()
{
    if (!checkVpz()){
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

bool GVLE::packageAllTimer()
{
    std::string o, e;
    utils::Package::package().getOutputAndClear(o);
    utils::Package::package().getErrorAndClear(e);
    Glib::RefPtr < Gtk::TextBuffer > ref = mLog->get_buffer();

    insertLog(o);
    insertLog(e);
    scrollLogToLastLine();

    if (utils::Package::package().isFinish()) {
        ++itDependencies;
        if (itDependencies != mDependencies.end()) {
            utils::Package::package().select(*itDependencies);
            buildAllProject();
        } else {
            utils::Package::package().select(utils::Path::filename(mPackage));
            insertLog("package " +
                      utils::Package::package().name() +
                      " & first level dependencies built\n");
            getMenu()->showProjectMenu();
        }
        scrollLogToLastLine();
        return false;
    } else {
        return true;
    }
}

bool GVLE::packageTimer()
{
    std::string o, e;
    utils::Package::package().getOutputAndClear(o);
    utils::Package::package().getErrorAndClear(e);
    Glib::RefPtr < Gtk::TextBuffer > ref = mLog->get_buffer();

    insertLog(o);
    insertLog(e);
    scrollLogToLastLine();

    if (utils::Package::package().isFinish()) {
        getMenu()->showProjectMenu();
        return false;
    } else {
        return true;
    }
}

bool GVLE::packageConfigureAllTimer()
{
    std::string o, e;
    utils::Package::package().getOutputAndClear(o);
    utils::Package::package().getErrorAndClear(e);

    insertLog(o);
    insertLog(e);
    scrollLogToLastLine();

    if (utils::Package::package().isFinish()) {
        if (utils::Package::package().isSuccess()) {
            buildAllProject();
        } else {
            getMenu()->showProjectMenu();
        }
        scrollLogToLastLine();
        return false;
    } else {
        return true;
    }
}

bool GVLE::packageBuildAllTimer()
{
    std::string o, e;
    utils::Package::package().getOutputAndClear(o);
    utils::Package::package().getErrorAndClear(e);

    insertLog(o);
    insertLog(e);
    scrollLogToLastLine();

    if (utils::Package::package().isFinish()) {
        if (utils::Package::package().isSuccess()) {
            installAllProject();
        } else {
            getMenu()->showProjectMenu();
        }
        scrollLogToLastLine();
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

    insertLog(o);
    insertLog(e);
    scrollLogToLastLine();

    if (utils::Package::package().isFinish()) {
        if (utils::Package::package().isSuccess()) {
            installProject();
        } else {
            getMenu()->showProjectMenu();
        }
        scrollLogToLastLine();
        return false;
    } else {
        return true;
    }
}

void GVLE::configureAllProject()
{
    insertLog("configure package " + utils::Package::package().name() + "\n");
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
        sigc::mem_fun(*this, &GVLE::packageConfigureAllTimer), 250);
}

void GVLE::configureProject()
{
    insertLog("configure package\n");
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

void GVLE::buildAllProject()
{
    insertLog("build package " + utils::Package::package().name() + "\n");
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
        sigc::mem_fun(*this, &GVLE::packageBuildAllTimer), 250);
}

void GVLE::buildProject()
{
    insertLog("build package\n");
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

std::map < std::string, Depends > GVLE::depends()
{
    std::map < std::string, Depends > result;

    utils::PathList vpz(utils::Path::path().getInstalledExperiments());
    std::sort(vpz.begin(), vpz.end());

    for (utils::PathList::iterator it = vpz.begin(); it != vpz.end(); ++it) {
        std::set < std::string > depends;
        try {
            vpz::Vpz vpz(*it);
            depends = vpz.depends();
        } catch (const std::exception& /*e*/) {
        }
        result[*it] = depends;
    }

    return result;
}

void GVLE::makeAllProject()
{
    AllDepends deps = depends();

    insertLog("\nbuild package "  +
              utils::Package::package().name() +
              " & first level of dependencies\n");

    getMenu()->hideProjectMenu();

    mDependencies.clear();

    for (AllDepends::const_iterator it = deps.begin(); it != deps.end(); ++it) {
        for (Depends::const_iterator jt = it->second.begin(); jt !=
                 it->second.end(); ++jt) {
            mDependencies.insert(*jt);
        }
    }

    mDependencies.insert(utils::Package::package().name());

    using utils::Path;
    using utils::Package;

    itDependencies = mDependencies.begin();

    if (itDependencies != mDependencies.end()) {

        Package::package().select(*itDependencies);

        insertLog("configure package " + utils::Package::package().name() + "\n");

        Package::package().configure();

        Glib::signal_timeout().connect(
            sigc::mem_fun(*this, &GVLE::packageConfigureAllTimer), 250);
    }
}

void GVLE::displayDependencies()
{
    std::string dependsbuffer;

    AllDepends deps = depends();

    for (AllDepends::const_iterator it = deps.begin(); it != deps.end(); ++it) {
        if (it->second.empty()) {
            dependsbuffer += "<b>" + utils::Path::basename(it->first) + "</b> : -\n";
        } else {
            dependsbuffer += "<b>" + utils::Path::basename(it->first) + "</b> : ";

            Depends::const_iterator jt = it->second.begin();
            while (jt != it->second.end()) {
                Depends::const_iterator kt = jt++;
                dependsbuffer += *kt;
                if (jt != it->second.end()) {
                    dependsbuffer += ", ";
                } else {
                    dependsbuffer += '\n';
                }
            }
        }
    }

    const std::string title =
        utils::Path::filename(mPackage) +
        _(" - Package Dependencies");

    Gtk::MessageDialog* box;

    box = new Gtk::MessageDialog(dependsbuffer, true, Gtk::MESSAGE_INFO,
                                 Gtk::BUTTONS_OK, true);
    box->set_title(title);

    box->run();
    delete box;
}

void GVLE::testProject()
{
    insertLog("test package\n");
    getMenu()->hideProjectMenu();
    try {
        utils::Package::package().test();
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


void GVLE::installAllProject()
{
    insertLog("install package : " +
              utils::Package::package().name() +
              "\n");
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
        sigc::mem_fun(*this, &GVLE::packageAllTimer), 250);
}


void GVLE::installProject()
{
    insertLog("install package\n");
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
    insertLog("clean package\n");
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
    insertLog("make source and binary packages\n");
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
    ViewDrawingArea* tab;
    if ( dynamic_cast<DocumentDrawingArea*>(
	     mEditor->get_nth_page(mCurrentTab))->isComplete()) {
	tab = dynamic_cast<DocumentDrawingArea*>(
	    mEditor->get_nth_page(mCurrentTab))->getCompleteDrawingArea();
    } else {
	tab = dynamic_cast<DocumentDrawingArea*>(
	    mEditor->get_nth_page(mCurrentTab))->getSimpleDrawingArea();
    }


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
    ViewDrawingArea* tab;
    if ( dynamic_cast<DocumentDrawingArea*>(
	     mEditor->get_nth_page(mCurrentTab))->isComplete()) {
	tab = dynamic_cast<DocumentDrawingArea*>(
	    mEditor->get_nth_page(mCurrentTab))->getCompleteDrawingArea();
    } else {
	tab = dynamic_cast<DocumentDrawingArea*>(
	    mEditor->get_nth_page(mCurrentTab))->getSimpleDrawingArea();
    }
    tab->addCoefZoom();
}

void GVLE::delCoefZoom()
{
    ViewDrawingArea* tab;
    if ( dynamic_cast<DocumentDrawingArea*>(
	     mEditor->get_nth_page(mCurrentTab))->isComplete()) {
	tab = dynamic_cast<DocumentDrawingArea*>(
	    mEditor->get_nth_page(mCurrentTab))->getCompleteDrawingArea();
    } else {
	tab = dynamic_cast<DocumentDrawingArea*>(
	    mEditor->get_nth_page(mCurrentTab))->getSimpleDrawingArea();
    }
    tab->delCoefZoom();
}

void GVLE::setCoefZoom(double coef)
{
    ViewDrawingArea* tab;
    if ( dynamic_cast<DocumentDrawingArea*>(
	     mEditor->get_nth_page(mCurrentTab))->isComplete()) {
	tab = dynamic_cast<DocumentDrawingArea*>(
	    mEditor->get_nth_page(mCurrentTab))->getCompleteDrawingArea();
    } else {
	tab = dynamic_cast<DocumentDrawingArea*>(
	    mEditor->get_nth_page(mCurrentTab))->getSimpleDrawingArea();
    }
    tab->setCoefZoom(coef);
}

void  GVLE::updateAdjustment(double h, double v)
{
    DocumentDrawingArea* tab = dynamic_cast<DocumentDrawingArea*>(
	mEditor->get_nth_page(mCurrentTab));
    tab->setHadjustment(h);
    tab->setVadjustment(v);
}

void GVLE::onOrder()
{
    ViewDrawingArea* tab;
    if ( dynamic_cast<DocumentDrawingArea*>(
	     mEditor->get_nth_page(mCurrentTab))->isComplete()) {
	tab = dynamic_cast<DocumentDrawingArea*>(
	    mEditor->get_nth_page(mCurrentTab))->getCompleteDrawingArea();
    } else {
	tab = dynamic_cast<DocumentDrawingArea*>(
	    mEditor->get_nth_page(mCurrentTab))->getSimpleDrawingArea();
    }
    tab->onOrder();
    mModeling->setModified(true);
}

}} // namespace vle gvle
