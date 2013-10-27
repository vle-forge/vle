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


#include <vle/gvle/FileTreeView.hpp>
#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/GVLEMenuAndToolbar.hpp>
#include <vle/gvle/Editor.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/ModelTreeBox.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/gvle/ModelClassBox.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Spawn.hpp>
#include <gtkmm/treerowreference.h>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>

namespace vle { namespace gvle {

FileTreeView::FileTreeView(BaseObjectType* cobject,
                           const Glib::RefPtr < Gtk::Builder >& /*ref*/)
    : Gtk::TreeView(cobject)
{
    mTreeModel = Gtk::TreeStore::create(mColumns);
    set_model(mTreeModel);
    mTreeSelection = get_selection();
    mTreeSelection->set_mode(Gtk::SELECTION_MULTIPLE);
    mColumnName = append_column(_("Project"), mColumns.mColname);

    mCellrenderer = dynamic_cast<Gtk::CellRendererText*>(
	get_column_cell_renderer(mColumnName - 1));

    mIgnoredFilesList.push_front("build");

    {
	mPopupActionGroup = Gtk::ActionGroup::create("FileTreeView");
	mPopupActionGroup->add(Gtk::Action::create("FileTV_ContextMenu", _("Context Menu")));

	mPopupActionGroup->add(Gtk::Action::create("FileTV_ContextOpen", _("_Open with...")),
	  sigc::mem_fun(*this, &FileTreeView::onOpen));

	mPopupActionGroup->add(Gtk::Action::create("FileTV_ContextDir", _("New _Directory")),
	  sigc::mem_fun(*this, &FileTreeView::onNewDirectory));

	mPopupActionGroup->add(Gtk::Action::create("FileTV_ContextFile", _("New _File")),
	  sigc::mem_fun(*this, &FileTreeView::onNewFile));

	mPopupActionGroup->add(Gtk::Action::create("FileTV_ContextCopy", _("_Copy")),
	  sigc::mem_fun(*this, &FileTreeView::onCopy));
        
        mPopupActionGroup->add(Gtk::Action::create("FileTV_ContextPaste", _("_Paste")),
	  sigc::mem_fun(*this, &FileTreeView::onPaste));

	mPopupActionGroup->add(Gtk::Action::create("FileTV_ContextRemove", _("_Remove")),
	  sigc::mem_fun(*this, &FileTreeView::onRemove));

	mPopupActionGroup->add(Gtk::Action::create("FileTV_ContextRename", _("_Rename")),
	  sigc::mem_fun(*this, &FileTreeView::onRename));

	mPopupUIManager = Gtk::UIManager::create();
	mPopupUIManager->insert_action_group(mPopupActionGroup);

	Glib::ustring ui_info =
	"<ui>"
	"  <popup name='FileTV_Popup'>"
	"      <menuitem action='FileTV_ContextOpen'/>"
	"      <menuitem action='FileTV_ContextDir'/>"
	"      <menuitem action='FileTV_ContextFile'/>"
	"      <menuitem action='FileTV_ContextCopy'/>"
	"      <menuitem action='FileTV_ContextPaste'/>"
	"      <menuitem action='FileTV_ContextRemove'/>"
	"      <menuitem action='FileTV_ContextRename'/>"
        "  </popup>"
	"</ui>";

	try {
		mPopupUIManager->add_ui_from_string(ui_info);
		mMenuPopup = (Gtk::Menu *) (
	    	mPopupUIManager->get_widget("/FileTV_Popup"));
	} catch(const Glib::Error& ex) {
		std::cerr << "building menus failed: FileTV_Popup " <<  ex.what();
	}
        
        if (!mMenuPopup)
            std::cerr << "menu not found FileTV_Popup\n";
	
    }
    
    signal_event().connect(sigc::mem_fun(*this, &FileTreeView::onEvent));

    Glib::RefPtr<Gtk::TreeSelection> selection = Gtk::TreeView::get_selection();

    selection->set_select_function(
        sigc::mem_fun(*this, &FileTreeView::onSelect) );
}

FileTreeView::~FileTreeView()
{
}

void FileTreeView::buildHierarchyDirectory(
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
			*(mTreeModel->append(parent->children()));
		    row[mColumns.mColname] = *it;
		    buildHierarchy(&(*row), nextpath);
		} else {
		    Gtk::TreeModel::Row row =
			*(mTreeModel->append());
		    row[mColumns.mColname] = *it;
		    buildHierarchy(&(*row), nextpath);
		}
	    }
	}
    }
}

void FileTreeView::buildHierarchyFile(
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
			*(mTreeModel->append(parent->children()));
		    row[mColumns.mColname] = *it;
		} else {
		    Gtk::TreeModel::Row row =
			*(mTreeModel->append());
		    row[mColumns.mColname] = *it;
		}
	    }
	}
    }
}


void FileTreeView::buildHierarchy(
    const Gtk::TreeModel::Row* parent, const std::string& dirname)
{
    buildHierarchyDirectory(parent, dirname);
    buildHierarchyFile(parent, dirname);
}

void FileTreeView::clear()
{
    mTreeModel->clear();
}

void FileTreeView::build()
{
    remove_all_columns();
    if (not mPackage.empty()) {
        mColumnName = append_column_editable(utils::Path::filename(mPackage),
                                    mColumns.mColname);
	mCellrenderer = dynamic_cast<Gtk::CellRendererText*>(
	    get_column_cell_renderer(mColumnName - 1));
	buildHierarchy(0, mPackage);
    } else {
	mColumnName = append_column_editable("Project", mColumns.mColname);
	mCellrenderer = dynamic_cast<Gtk::CellRendererText*>(
	    get_column_cell_renderer(mColumnName - 1));
    }

    mCellrenderer->signal_edited().connect(
             sigc::mem_fun(*this, &FileTreeView::onEdition));
    mCellrenderer->signal_editing_started().connect(
             sigc::mem_fun(*this, &FileTreeView::onEditionStarted));
}

Gtk::TreeModel::iterator FileTreeView::getFileRow(
    std::vector <std::string> path,
    Gtk::TreeModel::Children child)
{
    typedef std::vector < std::string > PathList;

    PathList::const_iterator jt = path.begin();
    Gtk::TreeModel::iterator it;
    Gtk::TreeModel::Children children = child;

    while (jt != path.end()) {
        it = children.begin();
        while (it != children.end()) {
            if ((*it).get_value(mColumns.mColname) == *jt) {
                children = (*it).children();
                jt++;
                break;
            } else {
                it++;
            }
        }
        return child.end(); /* An element of the path was not found in a sub
                               model of TreeModel. We return the end iterator
                               of child parameter. */
    }
    return it; /* Current iterator it reference the correct row in the
                  TreeModel. */
}

void FileTreeView::showRow(std::string pathFile)
{
    std::string pathPackage = mParent->currentPackage().getParentDir(
            vle::utils::PKG_SOURCE);

    std::string path = pathPackage;
    path.append(pathFile);

    std::vector <std::string> lst;

    boost::algorithm::split(lst, path, boost::is_any_of("/"),
                            boost::algorithm::token_compress_on);

    Gtk::TreeModel::iterator it = getFileRow(lst, mTreeModel->children());
    if (it != mTreeModel->children().end()) {
        expand_to_path(Gtk::TreePath(it));

        Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = get_selection();
        if (refTreeSelection) {
            refTreeSelection->set_select_function(
                sigc::mem_fun(*this, &FileTreeView::onSelectHighlightOnly));

            refTreeSelection->unselect_all();
            refTreeSelection->select(it);

            refTreeSelection->set_select_function(
                sigc::mem_fun(*this, &FileTreeView::onSelect) );
        }
    }
}

bool FileTreeView::isDirectory(const std::string& dirname)
{
    return Glib::file_test(dirname, Glib::FILE_TEST_IS_DIR);
}

bool FileTreeView::onEvent(GdkEvent* event)
{
    if (event->type == GDK_BUTTON_PRESS) {
        if (event->button.button == 1) {
            if (mDelayTime + 250 < event->button.time and
                mDelayTime + 1000 > event->button.time) {
                mDelayTime = event->button.time;
                mCellrenderer->property_editable() = true;
            } else {
                mDelayTime = event->button.time;
                mCellrenderer->property_editable() = false;
            }
        }
    }

    return false;
}

bool FileTreeView::onSelectHighlightOnly(
    const Glib::RefPtr<Gtk::TreeModel>& /*model*/,
    const Gtk::TreeModel::Path& /*path*/, bool /*info*/)
{
    return true;
}

bool FileTreeView::onSelect(
    const Glib::RefPtr<Gtk::TreeModel>& /*model*/,
    const Gtk::TreeModel::Path& path, bool /*info*/)
{
    Gtk::TreeModel::Path selectedpath(path);
    Gtk::TreeModel::iterator it = mTreeModel->get_iter(selectedpath);
    Gtk::TreeModel::Row row = *it;

    std::list<std::string> lstpath;
    projectFilePath(row, lstpath);

    std::string absolute_path =
	Glib::build_filename(mPackage, Glib::build_filename(lstpath));

    if (not isDirectory(absolute_path)) {
	if (mParent->getEditor()->existTab(absolute_path)) {
	    mParent->getEditor()->focusAndSignal(absolute_path);
        }
    }

    return true;
}

void FileTreeView::onEditionStarted(Gtk::CellEditable* /*cell*/,
                                    const Glib::ustring& path)
{
    Gtk::TreeModel::Path selectedpath(path);
    Gtk::TreeModel::iterator it = mTreeModel->get_iter(selectedpath);

    Gtk::TreeModel::Row row = *it;
    if (row) {
        mOldName = row.get_value(mColumns.mColname);
    } else {
        mOldName.clear();
    }
}

void FileTreeView::onEdition(const Glib::ustring& path,
                             const Glib::ustring& newstring)
{
    std::string newFilename = newstring.raw();
    std::string oldFilename = mOldName.raw();

    Gtk::TreeModel::iterator it = mTreeModel->get_iter(path);

    if (*it and not newFilename.empty() and newFilename != oldFilename) {
        Gtk::TreeModel::Row row = *it;

        std::list < std::string > lstpath;
        projectFilePath(row, lstpath);

        if (utils::Path::extension(oldFilename) !=
            utils::Path::extension(newFilename)) {
            Error(_("When renaming a file, changing his extension is not"
                    " allowed!"));
            Glib::ustring oldname(oldFilename);
            row.set_value(mColumns.mColname, oldname);
            return;
        }

        std::string newPath(Glib::build_filename(lstpath));
        std::string oldFilePath(utils::Path::buildFilename(
                Glib::path_get_dirname(newPath), oldFilename));

        if (not utils::Path::exist(Glib::build_filename(
                    mParent->currentPackage().getDir(vle::utils::PKG_SOURCE),
                    newPath))) {
            mParent->currentPackage().rename(oldFilePath, newFilename,
                    vle::utils::PKG_SOURCE);
            mParent->refreshEditor(oldFilePath, newPath);
        } else {
            Glib::ustring oldName = oldFilename;
            row.set_value(mColumns.mColname, oldName);
        }

        mParent->refreshPackageHierarchy();
    }
}

void FileTreeView::on_row_activated(const Gtk::TreeModel::Path& path,
					  Gtk::TreeViewColumn*)
{
    Gtk::TreeModel::Path selectedpath(path);
    Gtk::TreeModel::iterator it = mTreeModel->get_iter(selectedpath);
    Gtk::TreeModel::Row row = *it;

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
                    if (mParent->getModeling()->getTopModel()) {
                        mParent->redrawModelTreeBox();
                        mParent->redrawModelClassBox();
                        mParent->getMenu()->onOpenVpz();
                        mParent->getModelTreeBox()->set_sensitive(true);
                        mParent->getModelClassBox()->set_sensitive(true);
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

void FileTreeView::projectFilePath(const Gtk::TreeRow& row,
                                   std::list<std::string>& lst)
{
    if (row.parent()) {
	projectFilePath(*row.parent(), lst);
    }
    lst.push_back(std::string(row.get_value(mColumns.mColname)));
}

bool FileTreeView::on_button_press_event(GdkEventButton* event)
{
    if (event->type == GDK_BUTTON_PRESS and event->button == 3
	and not mParent->currentPackage().getDir(vle::utils::PKG_SOURCE).empty()) {

        Gtk::TreeModel::Path path;
        Gtk::TreeViewColumn* column;
        get_cursor(path, column);
        mRecentSelectedPath = path;

	mMenuPopup->popup(event->button, event->time);
        return true;
    } else {
        return TreeView::on_button_press_event(event);
    }
}

void FileTreeView::onOpen()
{
    Gtk::TreeModel::iterator it = mTreeModel->get_iter(mRecentSelectedPath);
    if (*it) {
        SimpleTypeBox box(_("Path of the program ?"), "");
        std::string prg = Glib::find_program_in_path(
            boost::trim_copy(box.run()));

        if (box.valid() and not prg.empty()) {
            Gtk::TreeModel::Row row = *it;
            std::vector< std::string > argv;
            std::list< std::string > lstpath;
            projectFilePath(row, lstpath);
            std::string filepath = Glib::build_filename(
                mPackage, Glib::build_filename(lstpath));
            argv.push_back(filepath);

            try {
                vle::utils::Spawn& spawn = mParent->spawnPool().addSpawn();

                spawn.start(prg,
                            utils::Path::path().getParentPath(filepath),
                            argv);

            } catch(const std::exception& e) {
                Error(_("The program can not be launched"));
            }
        }
    }
}

void FileTreeView::onNewFile()
{
    SimpleTypeBox box(_("Name of the File ?"), "");
    std::string name = boost::trim_copy(box.run());
    std::string filepath;

    if (box.valid() and not name.empty()) {
        Gtk::TreeModel::iterator it = mTreeModel->get_iter(mRecentSelectedPath);
        if (*it) {
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

void FileTreeView::onNewDirectory()
{
    std::vector < Gtk::TreeModel::Path > v = mTreeSelection->get_selected_rows();
    std::list < Gtk::TreeModel::Path > lst(v.begin(), v.end());

    SimpleTypeBox box(_("Name of the Directory ?"), "");
    std::string name(boost::trim_copy(box.run()));

    if (box.valid() and not name.empty()) {
        std::string path;
        std::list < std::string > lstpath;
        if (not lst.empty()) {
            Gtk::TreeModel::iterator it = mTreeModel->get_iter(lst.front());
            if (*it) {
                do {
                    Gtk::TreeModel::Row row = *it;
                    lstpath.push_front(row.get_value(mColumns.mColname));
                    it = row.parent();
                } while (*it);

                path = Glib::build_filename(lstpath);
            }
        }
        if (mParent->currentPackage().existsFile(path,
                vle::utils::PKG_SOURCE)) {
            lstpath.pop_back();
            path.assign(Glib::build_filename(lstpath));
        }
        mParent->currentPackage().addDirectory(path, name,
                vle::utils::PKG_SOURCE);
        mParent->refreshPackageHierarchy();
    }
}

void FileTreeView::onCopy()
{
    Gtk::TreeModel::iterator it = mTreeModel->get_iter(mRecentSelectedPath);
    if (*it) {
        const Gtk::TreeModel::Row row = *it;

        std::list < std::string > lstpath;
        projectFilePath(row, lstpath);

        mFileName = row.get_value(mColumns.mColname);

        mFilePath = Glib::build_filename(lstpath);

        mAbsolutePath = Glib::build_filename(
                    mParent->currentPackage().getDir(vle::utils::PKG_SOURCE),
                    mFilePath);
    }
}

void FileTreeView::onPaste()
{
    if (!(utils::Path::exist(mAbsolutePath))) {
        return;
    }

    Gtk::TreeModel::iterator it = mTreeModel->get_iter(mRecentSelectedPath);
    if (*it) {
        const Gtk::TreeModel::Row row = *it;

        std::list < std::string > lstpath;
        projectFilePath(row, lstpath);
        std::string newAbsolutePath;
        std::string newFileName;
        std::string newFilePath;

        int copyNumber = 1;
        std::string suffixe;
        if (mFileName.find_last_of(".") == std::string::npos) {
            return;
        }

        do {
            suffixe = "_" + boost::lexical_cast < std::string >(copyNumber);

            newFileName = mFileName;
            newFileName.insert(newFileName.find_last_of("."),suffixe);

            if (isDirectory(Glib::build_filename(
                    mParent->currentPackage().getDir(vle::utils::PKG_SOURCE),
                    Glib::build_filename(lstpath)))) {
                newFilePath = Glib::build_filename(lstpath) + "/" + newFileName;
            } else {
                newFilePath = Glib::path_get_dirname(
                        Glib::build_filename(lstpath)) + "/" + newFileName;
            }

            newAbsolutePath = Glib::build_filename(
                        mParent->currentPackage().getDir(vle::utils::PKG_SOURCE),
                        newFilePath);
            copyNumber++;

        } while (utils::Path::exist(newAbsolutePath));

        if (utils::Path::extension(newFileName) == "") {
            newFileName += utils::Path::extension(mAbsolutePath);
        }
        if (not isDirectory(mAbsolutePath)){
            utils::Package pack;
            pack.copy(mAbsolutePath, newAbsolutePath);
        }

        mParent->refreshPackageHierarchy();

        m_search = newFilePath;
        mTreeModel->foreach(sigc::mem_fun(*this, &FileTreeView::on_foreach));
    }
}

bool FileTreeView::on_foreach(const Gtk::TreeModel::Path&,
                              const Gtk::TreeModel::iterator& it)
{
    const Gtk::TreeModel::Row row = *it;

    std::list < std::string > lstpath;
    projectFilePath(row, lstpath);

    std::string  fileName = row.get_value(mColumns.mColname);
    std::string  filePath = Glib::build_filename(lstpath);

    if (filePath == m_search) {
        mCellrenderer->property_editable() = true;
        Gtk::TreeViewColumn* nameCol = get_column(mColumnName - 1);
        set_cursor(mTreeModel->get_path(it),*nameCol,true);
        return true;
    }
    return false;
}

void FileTreeView::onRemove()
{
    std::vector < Gtk::TreeModel::Path > v = mTreeSelection->get_selected_rows();
    std::list < Gtk::TreeModel::Path > lst(v.begin(), v.end());

    std::list < Gtk::TreeModel::Path >::const_iterator it;
    std::string files;

    it = lst.begin();
    while (it != lst.end()) {
        Gtk::TreeModel::iterator jt = mTreeModel->get_iter(*it);
        files.append("   - ");
        files.append((*jt).get_value(mColumns.mColname));
        ++it;
        if (it != lst.end()) {
            files.append("\n");
        }
    }

    if (gvle::Question(fmt(_("Do yo really want to remove:\n%1% ?")) %
                       files)) {
        mTreeSelection->selected_foreach_iter(
            sigc::mem_fun(*this, &FileTreeView::onRemoveCallBack));
    }

    mParent->refreshPackageHierarchy();
}

void FileTreeView::onRemoveCallBack(const Gtk::TreeModel::iterator& it)
{
    const Gtk::TreeModel::Row row = *it;

    if (row) {
        std::list < std::string > lstpath;
        std::string oldname;

        projectFilePath(row, lstpath);
        oldname = Glib::build_filename(lstpath);
        mParent->currentPackage().remove(oldname, vle::utils::PKG_SOURCE);
        mParent->refreshEditor(oldname, "");
        if (mParent->getEditor()->getDocuments().empty()) {
            mParent->setTitle();
        }
    }
}

void FileTreeView::onRename()
{
    Gtk::TreeModel::iterator it = mTreeModel->get_iter(mRecentSelectedPath);
    if (*it) {
        Gtk::TreeViewColumn* nameCol = get_column(mColumnName - 1);

        mCellrenderer->property_editable() =  true;

        set_cursor(mTreeModel->get_path(it),*nameCol,true);

        mParent->refreshPackageHierarchy();
    }
}

void FileTreeView::refresh()
{
    refreshHierarchy(0, mPackage);
}

void FileTreeView::refreshHierarchy(
    const Gtk::TreeModel::Row* parent,
    const std::string& dirname)
{
    refreshHierarchyDirectory(parent, dirname);
    refreshHierarchyFile(parent, dirname);
}

void FileTreeView::refreshHierarchyDirectory(
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
                           and (*itt)[mColumns.mColname] != *it) {
                        ++itt;
                    }
                    if (itt == parent->children().end()) {
                        Gtk::TreeModel::Row row =
                            *(mTreeModel->append(parent->children()));
                        row[mColumns.mColname] = *it;
                        refreshHierarchy(&(*row), nextpath);
                    } else {
                        refreshHierarchy(&(*itt), nextpath);
                    }
		} else {
                    Gtk::TreeModel::const_iterator itt =
                        mTreeModel->children().begin();

                    while (itt != mTreeModel->children().end()
                           and (*itt)[mColumns.mColname] != *it) {
                        ++itt;
                    }
                    if (itt == mTreeModel->children().end()) {
                        Gtk::TreeModel::Row row =
                            *(mTreeModel->append());
                        row[mColumns.mColname] = *it;
                        refreshHierarchy(&(*row), nextpath);
                    } else {
                        refreshHierarchy(&(*itt), nextpath);
                    }
		}
	    }
	}
    }
}

void FileTreeView::refreshHierarchyFile(
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
                           and (*itt)[mColumns.mColname] != *it) {
                        ++itt;
                    }
                    if (itt == parent->children().end()) {
                        Gtk::TreeModel::Row row =
                            *(mTreeModel->append(parent->children()));
                        row[mColumns.mColname] = *it;
                    }
		} else {
                    Gtk::TreeModel::const_iterator itt =
                        mTreeModel->children().begin();

                    while (itt != mTreeModel->children().end()
                           and (*itt)[mColumns.mColname] != *it) {
                        ++itt;
                    }
                    if (itt == mTreeModel->children().end()) {
                        Gtk::TreeModel::Row row =
                            *(mTreeModel->append());
                        row[mColumns.mColname] = *it;
                    }
		}
	    }
	}
    }
}

void FileTreeView::removeFiles(const Gtk::TreeModel::Row* parent,
                                     const std::list < std::string >& entries)
{
    std::list < Gtk::TreeModel::RowReference > rows;
    Gtk::TreeModel::const_iterator itt = parent ? parent->children().begin() :
        mTreeModel->children().begin();

    while (itt != (parent ? parent->children().end() :
                   mTreeModel->children().end())) {
        if (std::find(entries.begin(), entries.end(),
                      (*itt)[mColumns.mColname]) == entries.end()) {
            rows.push_back(Gtk::TreeRowReference(
                               mTreeModel,
                               mTreeModel->get_path(itt)));
        }
        ++itt;
    }

    for (std::list < Gtk::TreeModel::RowReference >::iterator it =
             rows.begin(); it != rows.end(); ++it) {
        Gtk::TreeModel::iterator treeit =
            mTreeModel->get_iter(it->get_path());

        mTreeModel->erase(treeit);
    }
}

}} // namespace vle gvle
