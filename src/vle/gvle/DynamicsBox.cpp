/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2012 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
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


#include <vle/gvle/DynamicsBox.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/utils/Path.hpp>
#include <gtkmm/treemodel.h>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

namespace vle { namespace gvle {

DynamicsBox::DynamicsBox(Modeling& modeling,
                         Glib::RefPtr < Gtk::Builder > ref,
                         vpz::Dynamics& dynamics) :
    mModeling(modeling), mDynsCopy(dynamics), mXml(ref),
    mValidateRetry(false)
{
    mXml->get_widget("DialogDynamics", mDialog);
    mXml->get_widget("treeviewDialogDynamics", mDynamics);
    mXml->get_widget("comboboxPackage", mPackage);
    mXml->get_widget("comboboxLibrary", mLibrary);

    initDynamics();
    fillDynamics();

    initPackage();
    fillPackage();

    initLibrary();

    initMenuPopupDynamics();

    mList.push_back(mDynamics->signal_button_release_event().connect(
                        sigc::mem_fun(*this,
                                      &DynamicsBox::onButtonRealeaseDynamics)));
    mList.push_back(mDynamics->signal_cursor_changed().connect(
                        sigc::mem_fun(*this,
                                      &DynamicsBox::onCursorChangedDynamics)));

    mList.push_back(mPackage->signal_changed().connect(
                        sigc::mem_fun(*this, &DynamicsBox::onChangedPackage)));

    mList.push_back(mLibrary->signal_changed().connect(
                        sigc::mem_fun(*this, &DynamicsBox::onChangedLibrary)));
}

DynamicsBox::~DynamicsBox()
{
    if (mDynamics) {
        mDynamics->remove_all_columns();
    }

    for (std::list < sigc::connection >::iterator it = mList.begin();
         it != mList.end(); ++it) {
        it->disconnect();
    }
}

void DynamicsBox::run()
{
    if (mDialog->run() == Gtk::RESPONSE_OK) {
        storePrevious();

        mModeling.dynamics() = mDynsCopy;
        mModeling.setModified(true);

        applyRenaming();

    }
    mDialog->hide();
}

void DynamicsBox::initDynamics()
{
    mDynamicsListStore = Gtk::ListStore::create(mDynamicsColumns);
    mDynamics->set_model(mDynamicsListStore);

    mColumnName = mDynamics->append_column_editable(_("Name"),
                                                    mDynamicsColumns.mName);
    Gtk::TreeViewColumn* nameCol = mDynamics->get_column(mColumnName - 1);
    nameCol->set_clickable(true);

    mList.push_back(nameCol->signal_clicked().connect(
                        sigc::bind(sigc::mem_fun(*this,
                                                 &DynamicsBox::onClickColumn),
                                   mColumnName)));

    mCellRenderer = dynamic_cast<Gtk::CellRendererText*>(
        mDynamics->get_column_cell_renderer(mColumnName - 1));
    mCellRenderer->property_editable() = true;

    mList.push_back(mCellRenderer->signal_editing_started().connect(
                        sigc::mem_fun(*this, &DynamicsBox::onEditionStarted)));
    mList.push_back(mCellRenderer->signal_edited().connect(
                        sigc::mem_fun(*this, &DynamicsBox::onEdition)));

    mColumnPackage = mDynamics->append_column(_("Package"),
                                              mDynamicsColumns.mPackage);
    Gtk::TreeViewColumn* packageCol = mDynamics->get_column(mColumnPackage - 1);
    packageCol->set_clickable(true);
    mList.push_back(packageCol->signal_clicked().connect(
                        sigc::bind(sigc::mem_fun(*this,
                                                 &DynamicsBox::onClickColumn),
                                   mColumnPackage)));

    mColumnLibrary = mDynamics->append_column(_("Library"),
                                              mDynamicsColumns.mLibrary);
    Gtk::TreeViewColumn* dynCol = mDynamics->get_column(mColumnLibrary - 1);
    dynCol->set_clickable(true);

    mList.push_back(dynCol->signal_clicked().connect(
                        sigc::bind(sigc::mem_fun(*this,
                                                 &DynamicsBox::onClickColumn),
                                   mColumnLibrary)));

    mDynamicsListStore->set_sort_column(0,Gtk::SORT_ASCENDING);
}

void DynamicsBox::fillDynamics()
{
    for (vpz::Dynamics::const_iterator it = mDynsCopy.begin();
         it != mDynsCopy.end(); ++it) {
        Gtk::TreeIter iter = mDynamicsListStore->append();
        if (iter) {
            Gtk::ListStore::Row row = *iter;
                row[mDynamicsColumns.mName] = it->first;
                row[mDynamicsColumns.mPackage] = it->second.package();
                row[mDynamicsColumns.mLibrary] = it->second.library();
        }
    }

    sensitive(false);

    mIter = mDynamicsListStore->children().end();
}

void DynamicsBox::initMenuPopupDynamics()
{
    Gtk::Menu::MenuList& menulist(mMenu.items());

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(
            _("_Add"), sigc::mem_fun(
                *this, &DynamicsBox::onAddDynamic)));

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(
            _("_Copy"), sigc::mem_fun(
                *this, &DynamicsBox::onCopyDynamic)));

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(
            _("_Remove"), sigc::mem_fun(
                *this, &DynamicsBox::onRemoveDynamic)));
    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(
            _("_Rename"), sigc::mem_fun(
                *this, &DynamicsBox::onRenameDynamic)));

    mMenu.accelerate(*mDynamics);
}

void DynamicsBox::onAddDynamic()
{
    storePrevious();
    sensitive(true);

    SimpleTypeBox box(_("Name of the Dynamic ?"), "");
    std::string name = boost::trim_copy(box.run());
    if (box.valid() and not name.empty() and not mDynsCopy.exist(name)) {
        Gtk::TreeIter iter = mDynamicsListStore->append();
        if (iter) {
            Gtk::ListStore::Row row = *iter;
            row[mDynamicsColumns.mName] = name;

            mDynsCopy.add(name);
            mIter = iter;
            updateDynamic(name);
            mDynamics->set_cursor(mDynamicsListStore->get_path(iter));
            mDeletedDynamics.erase(name);
        }
    }
}

void DynamicsBox::onRemoveDynamic()
{
    storePrevious();

    Glib::RefPtr < Gtk::TreeView::Selection > ref = mDynamics->get_selection();
    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            std::string name(row.get_value(mDynamicsColumns.mName));
            mDynsCopy.del(name);
            mDynamicsListStore->erase(iter);
            mDeletedDynamics.insert(name);

            Gtk::TreeModel::Children children = mDynamicsListStore->children();
            mIter = children.begin();
            if (mIter != children.end()) {
                row = *mIter;
                updateDynamic(row.get_value(mDynamicsColumns.mName));
                Gtk::TreeModel::Path path = mDynamicsListStore->get_path(mIter);
                mDynamics->set_cursor(path);
            } else {
                sensitive(false);
            }
        }
    }
}

void DynamicsBox::onRenameDynamic()
{
    storePrevious();

    Glib::RefPtr < Gtk::TreeView::Selection > ref = mDynamics->get_selection();
    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            std::string oldname(row.get_value(mDynamicsColumns.mName));
            SimpleTypeBox box(_("Name of the Dynamic ?"), oldname);
            std::string newname = boost::trim_copy(box.run());
            if (box.valid() and not newname.empty()
                and not mDynsCopy.exist(newname)) {
                row[mDynamicsColumns.mName] = newname;
                mDynsCopy.rename(oldname, newname);
                mRenameList.push_back(std::make_pair(oldname, newname));
            }
        }
    }
}


void DynamicsBox::onCopyDynamic()
{
    int number = 1;
    std::string copy;

    Glib::RefPtr < Gtk::TreeView::Selection > ref = mDynamics->get_selection();
    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            std::string name(row.get_value(mDynamicsColumns.mName));
            do {
                copy = name + "_" + boost::lexical_cast< std::string >(number);
                ++number;
            } while (mDynsCopy.exist(copy));
            iter = mDynamicsListStore->append();
            row = *iter;
            mDynsCopy.copy(name, copy);
            row[mDynamicsColumns.mName] = copy;
            row[mDynamicsColumns.mPackage] = mDynsCopy.get(name).package();
            updateDynamic(copy);
            mDynamics->set_cursor(mDynamicsListStore->get_path(iter));
            mDeletedDynamics.erase(copy);
        }
    }
}

void DynamicsBox::onEditionStarted(
    Gtk::CellEditable* cellEditable, const Glib::ustring& /* path */)
{
    storePrevious();

    Glib::RefPtr < Gtk::TreeView::Selection > ref = mDynamics->get_selection();
    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            mOldName = row.get_value(mDynamicsColumns.mName);
        }
    }

    if(mValidateRetry)
    {
        Gtk::CellEditable* celleditableValidated = cellEditable;
        Gtk::Entry* pEntry = dynamic_cast<Gtk::Entry*>(celleditableValidated);
        if(pEntry)
        {
            pEntry->set_text(mInvalidTextForRetry);
            mValidateRetry = false;
            mInvalidTextForRetry.clear();
        }
    }
}

void DynamicsBox::onEdition(
    const Glib::ustring& pathString,
    const Glib::ustring& newName)
{
    Gtk::TreePath path(pathString);

    if (not mDynsCopy.exist(newName) and newName != "") {
        Glib::RefPtr < Gtk::TreeView::Selection > ref =
            mDynamics->get_selection();
        if (ref) {
            Gtk::TreeModel::iterator iter = ref->get_selected();
            if (iter) {
                Gtk::TreeModel::Row row = *iter;
                row[mDynamicsColumns.mName] = newName;
                mDynsCopy.rename(mOldName, newName);
                mRenameList.push_back(std::make_pair(mOldName, newName));
                mValidateRetry = true;
            }
        }
    }
}

void DynamicsBox::sensitive(bool active)
{
    mPackage->set_sensitive(active);
    mLibrary->set_sensitive(active);
}

bool DynamicsBox::onButtonRealeaseDynamics(GdkEventButton* event)
{
    if(event->button == 3) {
        mMenu.popup(event->button, event->time);
    }
    return true;
}

void DynamicsBox::onCursorChangedDynamics()
{
    storePrevious();

    Glib::RefPtr < Gtk::TreeView::Selection > ref = mDynamics->get_selection();

    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();

        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            std::string name(row.get_value(mDynamicsColumns.mName));

            sensitive(true);

            mIter = iter;
            updateDynamic(name);
        }
    }
}

void DynamicsBox::onChangedPackage()
{
    fillLibrary();

    if (mIter != mDynamicsListStore->children().end()) {
        Gtk::TreeModel::Row row = *mIter;
        row[mDynamicsColumns.mPackage] = getPackageStr();
    }
}

void DynamicsBox::onChangedLibrary()
{
    if (mIter != mDynamicsListStore->children().end()) {
        Gtk::TreeModel::Row row = *mIter;
        row[mDynamicsColumns.mLibrary] = getLibraryStr();
    }
}

void DynamicsBox::storePrevious()
{
    if (mIter != mDynamicsListStore->children().end()) {
        Gtk::TreeModel::Row row = *mIter;
        std::string name(row.get_value(mDynamicsColumns.mName));

        assignDynamic(name);
    }
}

void DynamicsBox::assignDynamic(const std::string& name)
{
    vpz::Dynamic& dyn(mDynsCopy.get(name));

    dyn.setPackage(getPackageStr());

    dyn.setLibrary(getLibraryStr());
}

void DynamicsBox::updateDynamic(const std::string& name)
{
    mDialog->set_title((fmt(_("Dynamics: %1%")) % name).str());

    vpz::Dynamic& dyn(mDynsCopy.get(name));

    setPackageStr(dyn.package());

    fillLibrary();
    setLibraryStr(dyn.library());

}

void DynamicsBox::applyRenaming()
{
    renameList::const_iterator it = mRenameList.begin();

    while (it != mRenameList.end()) {
        mModeling.vpz().project().model().updateDynamics(it->first,
                                                          it->second);
        mModeling.vpz().project().classes().updateDynamics(it->first,
                                                            it->second);
        ++it;
    }

    mRenameList.clear();
}

void DynamicsBox::initPackage()
{
    mPackageListStore = Gtk::ListStore::create(mPackageColumns);
    mPackage->set_model(mPackageListStore);
}

void DynamicsBox::fillPackage()
{
    mPackage->clear();
    mPackageListStore->clear();

    utils::PathList paths = utils::Path::path().getInstalledPackages();
    std::sort(paths.begin(), paths.end());
    for (utils::PathList::const_iterator i = paths.begin(), e = paths.end();
         i != e; ++i) {
        mRowPackage = *(mPackageListStore->append());
        mRowPackage[mPackageColumns.mContent] = *i;
    }

    mPackage->pack_start(mPackageColumns.mContent);
}

void DynamicsBox::setPackageStr(Glib::ustring str)
{
    const Gtk::TreeModel::Children& child = mPackageListStore->children();
    Gtk::TreeModel::Children::const_iterator it = child.begin();
    while (it != child.end()) {
        if (it and (*it)[mPackageColumns.mContent] == str) {
            mPackage->set_active(it);
        }
        ++it;
    }
}

Glib::ustring DynamicsBox::getPackageStr()
{
    Gtk::TreeModel::iterator iter = mPackage->get_active();
    if(iter)
    {
        Gtk::TreeModel::Row row = *iter;
        if(row)
            return row[mPackageColumns.mContent];
    }
    return "";
}

void DynamicsBox::initLibrary()
{
    mLibraryListStore = Gtk::ListStore::create(mLibraryColumns);
    mLibrary->set_model(mLibraryListStore);
}

void DynamicsBox::fillLibrary()
{
    mLibrary->clear();
    mLibraryListStore->clear();

    utils::PathList paths;
    if (getPackageStr().empty()) {
        paths = utils::Path::path().getSimulatorDirs();
    } else {
        paths.push_back(
            utils::Path::path().getExternalPackagePluginSimulatorDir(
                getPackageStr()));
    }

    utils::PathList::iterator it = paths.begin();
    while (it != paths.end()) {
        if (Glib::file_test(*it, Glib::FILE_TEST_EXISTS)) {
            Glib::Dir rep(*it);
            Glib::DirIterator in = rep.begin();
            while (in != rep.end()) {
#ifdef G_OS_WIN32
                if (((*in).find("lib") == 0) &&
                    ((*in).rfind(".dll") == (*in).size() - 4)) {
                    mRowLibrary = *(mLibraryListStore->append());
                    mRowLibrary[mLibraryColumns.mContent] =
                        (*in).substr(3, (*in).size() - 7);
                }
#else
                if (((*in).find("lib") == 0) &&
                    ((*in).rfind(".so") == (*in).size() - 3)) {
                    mRowLibrary = *(mLibraryListStore->append());
                    mRowLibrary[mLibraryColumns.mContent] =
                        (*in).substr(3, (*in).size() - 6);
                }
#endif
                in++;
            }
        }
        it++;
    }

    mLibraryListStore->set_sort_column(0, Gtk::SORT_ASCENDING);

    mLibrary->pack_start(mLibraryColumns.mContent);
}

void DynamicsBox::setLibraryStr(Glib::ustring str)
{
    const Gtk::TreeModel::Children& child = mLibraryListStore->children();
    Gtk::TreeModel::Children::const_iterator it = child.begin();
    while (it != child.end()) {
        if (it and (*it)[mLibraryColumns.mContent] == str) {
            mLibrary->set_active(it);
        }
        ++it;
    }
}

Glib::ustring DynamicsBox::getLibraryStr()
{
    Gtk::TreeModel::iterator iter = mLibrary->get_active();
    if(iter)
    {
        Gtk::TreeModel::Row row = *iter;
        if(row)
            return row[mLibraryColumns.mContent];
    }
    return "";
}

void DynamicsBox::onClickColumn(int numColumn)
{
    int current_sort_column_id;
    Gtk::SortType current_order;

    if (mDynamicsListStore->get_sort_column_id (current_sort_column_id, current_order)) {
        if (current_sort_column_id == numColumn - 1) {
            if (current_order == Gtk::SORT_ASCENDING) {
                mDynamicsListStore->set_sort_column(numColumn - 1, Gtk::SORT_DESCENDING);
            } else {
                mDynamicsListStore->set_sort_column(numColumn - 1, Gtk::SORT_ASCENDING);
            }
        } else {
            mDynamicsListStore->set_sort_column(numColumn - 1, Gtk::SORT_ASCENDING);
        }
    }
}

}} // namespace vle gvle
