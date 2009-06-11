/**
 * @file vle/gvle/PackageBrowserWindow.cpp
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

#include <vle/gvle/PackageBrowserWindow.hpp>
#include <vle/gvle/PackageBrowserMenu.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Package.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>

namespace vle { namespace gvle {

Document::Document(const std::string& filepath, bool newfile) :
    Gtk::ScrolledWindow(),
    mFilePath(filepath),
    mModified(false),
    mNew(newfile)
{
    mFileName = boost::filesystem::basename(filepath);
    init();
}

Document::~Document()
{
}

void Document::save()
{
    try {
	std::ofstream file(mFilePath.c_str());
	file << mView.get_buffer()->get_text();
	file.close();
	mNew = mModified = false;
    } catch(std::exception& e) {
	throw _("Error while saving file.");
    }
}

void Document::saveAs(const std::string& filename)
{
    mFilePath = filename;
    save();
}

void Document::init()
{
    if (not mNew) {
	std::ifstream file(mFilePath.c_str());
	if (file) {
	    std::stringstream filecontent;
	    filecontent << file.rdbuf();
	    file.close();
	    mView.get_buffer()->insert(mView.get_buffer()->end(), filecontent.str());
	    add(mView);
	} else {
	    throw std::string("cannot open: " + mFileName);
	}
    } else {
	add(mView);
    }
}

PackageBrowserWindow::FileTreeView::FileTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/) :
    Gtk::TreeView(cobject)
{
    mRefTreeModel = Gtk::TreeStore::create(mColumns);
    set_model(mRefTreeModel);
    append_column(_("Files"), mColumns.m_col_name);
    mRefTreeSelection = get_selection();
}

PackageBrowserWindow::FileTreeView::~FileTreeView()
{
}

void PackageBrowserWindow::FileTreeView::buildHierarchy(
    const Gtk::TreeModel::Row& parent, const std::string& dirname)
{
    Glib::Dir dir(dirname);
    Glib::Dir::iterator it;
    for (it = dir.begin(); it != dir.end(); ++it) {
	if ((*it)[0] != '.') { //Don't show hidden files
	    Gtk::TreeModel::Row row = *(mRefTreeModel->append(parent.children()));
	    row[mColumns.m_col_name] = *it;
	    std::string nextpath = Glib::build_filename(dirname, *it);
	    if (isDirectory(nextpath)) {
		buildHierarchy(*row, nextpath);
	    }
	}
    }
}

void PackageBrowserWindow::FileTreeView::clear()
{
    mRefTreeModel->clear();
}

void PackageBrowserWindow::FileTreeView::build()
{
    if (not mPackage.empty()) {
	Gtk::TreeIter iter = mRefTreeModel->append();
	Gtk::TreeModel::Row row = *iter;
	row[mColumns.m_col_name] = boost::filesystem::basename(mPackage);
	buildHierarchy(*row, mPackage);
	expand_row(Gtk::TreePath(iter), false);
    }
}

bool PackageBrowserWindow::FileTreeView::isDirectory(const std::string& dirname)
{
    return Glib::file_test(dirname, Glib::FILE_TEST_IS_DIR);
}

void PackageBrowserWindow::FileTreeView::on_row_activated(
    const Gtk::TreeModel::Path&,
    Gtk::TreeViewColumn*)
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection	= get_selection();
    Gtk::TreeModel::const_iterator it = refSelection->get_selected();
    const Gtk::TreeModel::Row row = *it;
    const std::list<std::string>* lstpath = projectFilePath(row);

    std::string absolute_path =
	Glib::build_filename(mPackage, Glib::build_filename(*lstpath));
    if (not isDirectory(absolute_path))
	mParent->openTab(absolute_path);
    else {
	if (not row_expanded(Gtk::TreePath(it)))
	    expand_row(Gtk::TreePath(it), false);
	else
	    collapse_row(Gtk::TreePath(it));
    }
}

std::list<std::string>* PackageBrowserWindow::FileTreeView::projectFilePath(
    const Gtk::TreeRow& row)
{
    if (row.parent()) {
	std::list<std::string>* lst =
	    projectFilePath(*row.parent());
	lst->push_back(std::string(row.get_value(mColumns.m_col_name)));
	return lst;
    } else {
	return new std::list<std::string>();
    }
}

/* PackageBrowserWindow */
PackageBrowserWindow::PackageBrowserWindow(Glib::RefPtr<Gnome::Glade::Xml> xml,
			       Modeling* modeling) :
    mXml(xml),
    mModeling(modeling)
{
    xml->get_widget("WindowPackageBrowser", mWindow);
    xml->get_widget("StatusBarPackageBrowser", mStatusbar);
    xml->get_widget("TextViewLogPackageBrowser", mLog);
    xml->get_widget_derived("FileTreeViewPackageBrowser", mFileTreeView);
    mFileTreeView->setParent(this);
    xml->get_widget("NotebookPackageBrowser", mNotebook);
    xml->get_widget_derived("MenuBarPackageBrowser", mMenuBar);
    mMenuBar->setParent(this);
    mMenuBar->makeMenus();
}

void PackageBrowserWindow::show()
{
    mPackage = vle::utils::Path::path().getPackageDir();
    mWindow->set_title(std::string(_("Browsing package: ")).append(
			   boost::filesystem::basename(mPackage)));
    buildPackageHierarchy();
    mWindow->show_all();
}

void PackageBrowserWindow::hide()
{
    mWindow->hide_all();
}

void PackageBrowserWindow::focusTab(const std::string& filepath)
{
    int page = mNotebook->page_num(*mDocuments.find(filepath)->second);
    mNotebook->set_current_page(page);
}

void PackageBrowserWindow::openTab(const std::string& filepath)
{
    try {
	if (mDocuments.find(filepath) == mDocuments.end()) {
	    Document* doc = new Document(filepath);
	    mDocuments.insert(
		std::make_pair < std::string, Document* >(filepath, doc));
	    int page = mNotebook->append_page(*doc, doc->filename());
	    mNotebook->set_current_page(page);
	} else {
	    focusTab(filepath);
	}
   } catch (std::exception& e) {
	mLog->get_buffer()->insert(
	    mLog->get_buffer()->end(), e.what());
    }
    mWindow->show_all_children();
}

void PackageBrowserWindow::closeTab(const std::string& filepath)
{
    Documents::iterator it = mDocuments.find(filepath);
    if (it != mDocuments.end()) {
	int page = mNotebook->page_num(*it->second);
	mNotebook->remove_page(page);
	mDocuments.erase(filepath);

	mNotebook->set_current_page(--page);
    }
}

void PackageBrowserWindow::configureProject()
{
    std::string out, err;
    utils::CMakePackage::configure(out, err);
    mLog->get_buffer()->insert(mLog->get_buffer()->end(), out);
    if (not err.empty())
	mLog->get_buffer()->insert(mLog->get_buffer()->end(), err);
}

void PackageBrowserWindow::buildProject()
{
    std::string out, err;
    utils::CMakePackage::build(out, err);
    mLog->get_buffer()->insert(mLog->get_buffer()->end(), out);
    if (not err.empty())
	mLog->get_buffer()->insert(mLog->get_buffer()->end(), err);
}

void PackageBrowserWindow::cleanProject()
{
    std::string out, err;
    utils::CMakePackage::clean(out, err);
    mLog->get_buffer()->insert(mLog->get_buffer()->end(), out);
    if (not err.empty())
	mLog->get_buffer()->insert(mLog->get_buffer()->end(), err);
}

void PackageBrowserWindow::packageProject()
{
    std::string out, err;
    utils::CMakePackage::package(out, err);
    mLog->get_buffer()->insert(mLog->get_buffer()->end(), out);
    if (not err.empty())
	mLog->get_buffer()->insert(mLog->get_buffer()->end(), err);
}

void PackageBrowserWindow::newFile()
{
    try {
	Document* doc = new Document(_("untitled file"), true);
	mDocuments.insert(
	    std::make_pair < std::string, Document* >(_("untitled file"), doc));
	mNotebook->append_page(*doc, doc->filename());
    } catch (std::exception& e) {
	std::cout << e.what() << std::endl;
    }
    mWindow->show_all_children();
}

void PackageBrowserWindow::openFile()
{
    Gtk::FileChooserDialog file(_("Choose a file"), Gtk::FILE_CHOOSER_ACTION_OPEN);
    file.set_transient_for(*mWindow);
    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

    if (file.run() == Gtk::RESPONSE_OK) {
	std::string selected_file = file.get_filename();
	openTab(selected_file);
    }
}

void PackageBrowserWindow::saveFile()
{
    int page = mNotebook->get_current_page();
    if (page != -1) {
	Document* doc = dynamic_cast < Document* >(
	    mNotebook->get_nth_page(page));
	if (not doc->isNew()) {
	    doc->save();
	} else {
	    Gtk::FileChooserDialog file(_("VPZ file"), Gtk::FILE_CHOOSER_ACTION_SAVE);
	    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

	    if (file.run() == Gtk::RESPONSE_OK) {
		std::string filename(file.get_filename());
		doc->saveAs(filename);
	    }
	}
	buildPackageHierarchy();
    }
}

void PackageBrowserWindow::saveFileAs()
{
    int page = mNotebook->get_current_page();
    if (page != -1) {
	Document* doc = dynamic_cast < Document* >(
	    mNotebook->get_nth_page(page));
	Gtk::FileChooserDialog file(_("VPZ file"), Gtk::FILE_CHOOSER_ACTION_SAVE);
	file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

	if (file.run() == Gtk::RESPONSE_OK) {
	    std::string filename(file.get_filename());
	    doc->saveAs(filename);
	}
	buildPackageHierarchy();
    }
}

void PackageBrowserWindow::closeFile()
{
    int page = mNotebook->get_current_page();
    if (page != -1) {
	Document* doc = dynamic_cast < Document* >(
	    mNotebook->get_nth_page(page));
	closeTab(doc->filepath());
    }
}

void PackageBrowserWindow::buildPackageHierarchy()
{
    mFileTreeView->clear();
    mFileTreeView->setPackage(mPackage);
    mFileTreeView->build();
}

}}
