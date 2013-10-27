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


#include <vle/gvle/DynamicsBox.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/utils/Path.hpp>

#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

namespace vle { namespace gvle {

DynamicsBox::DynamicsBox(Modeling& modeling,
                         PluginFactory& pluginfactory,
                         Glib::RefPtr < Gtk::Builder > ref,
                         vpz::Dynamics& dynamics) :
    mModeling(modeling), mPluginFactory(pluginfactory),
    mDynsCopy(dynamics), mXml(ref)
{
    mXml->get_widget("DialogDynamics", mDialog);
    mXml->get_widget("treeviewDialogDynamics", mDynamics);

    fillLibrariesListStore();

    mDynamicsListStore = Gtk::ListStore::create(mDynamicsColumns);
    mDynamics->set_model(mDynamicsListStore);

    initDynamicsColumnName();
    initDynamicsColumnPackage();
    initDynamicsColumnLibrary();

    fillDynamics();

    initMenuPopupDynamics();
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

void DynamicsBox::onCellrendererChoiceEditedPackage(
    const Glib::ustring& pathstring, const Glib::ustring& newtext)
{
    Gtk::TreePath path(pathstring);

    Gtk::TreeModel::iterator iter = mDynamicsListStore->get_iter(path);
    if (iter) {
        Gtk::TreeRow row = *iter;
        row[mDynamicsColumns.mPackage] = newtext;
        row[mDynamicsColumns.mLibrary] = "";
        row[mDynamicsColumns.mLibraries] = mLibrariesListStore[newtext];

        std::string name(row.get_value(mDynamicsColumns.mName));
        vpz::Dynamic& dyn(mDynsCopy.get(name));
        dyn.setPackage(newtext.raw());

        dyn.setLibrary("");
    }
}

void DynamicsBox::onCellrendererChoiceEditedLibrary(
    const Glib::ustring& pathstring, const Glib::ustring& newtext)
{
    Gtk::TreePath path(pathstring);

    Gtk::TreeModel::iterator iter = mDynamicsListStore->get_iter(path);
    if (iter) {
        Gtk::TreeRow row = *iter;
        row[mDynamicsColumns.mLibrary] = newtext;

        std::string name(row.get_value(mDynamicsColumns.mName));
        vpz::Dynamic& dyn(mDynsCopy.get(name));
        dyn.setLibrary(newtext.raw());
    }
}

void DynamicsBox::run()
{
    if (mDialog->run() == Gtk::RESPONSE_OK) {

        mModeling.dynamics() = mDynsCopy;
        mModeling.setModified(true);

        applyRenaming();

    }
    mDialog->hide();
}

void DynamicsBox::initDynamicsColumnName()
{
    mColumnName = mDynamics->append_column_editable(_("Name"),
                                                    mDynamicsColumns.mName);
    Gtk::TreeViewColumn* nameCol = mDynamics->get_column(mColumnName - 1);

    nameCol->set_clickable(true);
    nameCol->set_resizable(true);

    mList.push_back(nameCol->signal_clicked().connect(
                        sigc::bind(sigc::mem_fun(*this,
                                                 &DynamicsBox::onClickColumn),
                                   mColumnName)));

    Gtk::CellRendererText* mCellRenderer = dynamic_cast<Gtk::CellRendererText*>(
        mDynamics->get_column_cell_renderer(mColumnName - 1));

    mCellRenderer->property_editable() = true;

    mList.push_back(mCellRenderer->signal_editing_started().connect(
                        sigc::mem_fun(*this, &DynamicsBox::onEditionStarted)));
    mList.push_back(mCellRenderer->signal_edited().connect(
                        sigc::mem_fun(*this, &DynamicsBox::onEdition)));
}

void DynamicsBox::initDynamicsColumnPackage()
{
    Gtk::TreeView::Column* pColumn =
        Gtk::manage(new Gtk::TreeView::Column(_("Package")));
    Gtk::CellRendererCombo* pRenderer =
        Gtk::manage(new Gtk::CellRendererCombo);
    pColumn->pack_start(*pRenderer);

    pColumn->set_resizable(true);

    mColumnPackage = mDynamics->append_column(*pColumn);

    pColumn->add_attribute(pRenderer->property_text(),
                           mDynamicsColumns.mPackage);

    pColumn->add_attribute(pRenderer->property_model(),
                           mDynamicsColumns.mPackages);

    pRenderer->property_text_column() = 0;
    pRenderer->property_editable() = true;
    pRenderer->property_has_entry() = false;

    mList.push_back(
        pRenderer->signal_edited().connect(
            sigc::mem_fun(*this,
                          &DynamicsBox::onCellrendererChoiceEditedPackage)));

    Gtk::TreeViewColumn* packageCol = mDynamics->get_column(mColumnPackage - 1);
    packageCol->set_clickable(true);

    mList.push_back(
        packageCol->signal_clicked().connect(
            sigc::bind(sigc::mem_fun(*this,
                                     &DynamicsBox::onClickColumn),
                       mColumnPackage)));
}

void DynamicsBox::initDynamicsColumnLibrary()
{
    Gtk::TreeView::Column* pColumnl =
        Gtk::manage(new Gtk::TreeView::Column(_("Library")));
    Gtk::CellRendererCombo* pRendererl =
        Gtk::manage(new Gtk::CellRendererCombo);
    pColumnl->pack_start(*pRendererl);

    pColumnl->set_resizable(true);

    mColumnLibrary = mDynamics->append_column(*pColumnl);

    pColumnl->add_attribute(pRendererl->property_text(),
                           mDynamicsColumns.mLibrary);

    pColumnl->add_attribute(pRendererl->property_model(),
                            mDynamicsColumns.mLibraries);

    pRendererl->property_text_column() = 0;
    pRendererl->property_editable() = true;
    pRendererl->property_has_entry() = false;

    mList.push_back(
        pRendererl->signal_edited().connect(
            sigc::mem_fun(*this,
                          &DynamicsBox::onCellrendererChoiceEditedLibrary)));

    Gtk::TreeViewColumn* dynCol = mDynamics->get_column(mColumnLibrary - 1);
    dynCol->set_clickable(true);

    mList.push_back(dynCol->signal_clicked().connect(
                        sigc::bind(sigc::mem_fun(*this,
                                                 &DynamicsBox::onClickColumn),
                                   mColumnLibrary)));

    mDynamicsListStore->set_sort_column(0,Gtk::SORT_ASCENDING);
}

void DynamicsBox::fillLibrariesListStore()
{
    utils::PathList paths = utils::Path::path().getBinaryPackages();
    std::sort(paths.begin(), paths.end());
    for (utils::PathList::const_iterator i = paths.begin(), e = paths.end();
         i != e; ++i) {

        Glib::RefPtr<Gtk::ListStore> mLibraryListStore =
            Gtk::ListStore::create(mLibraryColumns);

        utils::ModuleList lst;
        utils::ModuleList::iterator jt;
        mPluginFactory.getDynamicsPlugins(*i, &lst);

        Gtk::TreeModel::Row row;
        for (jt = lst.begin(); jt != lst.end(); ++jt) {
            row = *(mLibraryListStore->append());
            row[mLibraryColumns.mContent] = jt->library;
        }
        mLibraryListStore->set_sort_column(0, Gtk::SORT_ASCENDING);
        mLibrariesListStore[*i] =  mLibraryListStore;
    }
}

void DynamicsBox::fillDynamics()
{
    Gtk::TreeModel::Row row;

    mPackagesListStore = Gtk::ListStore::create(mPackageColumns);

    utils::PathList paths = utils::Path::path().getBinaryPackages();
    std::sort(paths.begin(), paths.end());
    for (utils::PathList::const_iterator i = paths.begin(), e = paths.end();
         i != e; ++i) {
        row = *(mPackagesListStore->append());
        row[mPackageColumns.mContent] = *i;
    }

    for (vpz::Dynamics::const_iterator it = mDynsCopy.begin();
         it != mDynsCopy.end(); ++it) {
        Gtk::TreeIter iter = mDynamicsListStore->append();
        if (iter) {
            Gtk::ListStore::Row row = *iter;
            row[mDynamicsColumns.mName] = it->first;
            row[mDynamicsColumns.mPackage] = it->second.package();
            row[mDynamicsColumns.mPackages] = mPackagesListStore;
            row[mDynamicsColumns.mLibrary] = it->second.library();
            row[mDynamicsColumns.mLibraries] =
                mLibrariesListStore[it->second.package()];
        }
    }

    mIter = mDynamicsListStore->children().end();
}

void DynamicsBox::initMenuPopupDynamics()
{
    mActionGroup = Gtk::ActionGroup::create("initMenuPopupDynamics");
    mActionGroup->add(Gtk::Action::create("DynBox_ContextMenu", _("Context Menu")));
    
    mActionGroup->add(Gtk::Action::create("DynBox_ContextNew", _("_New")),
	sigc::mem_fun(*this, &DynamicsBox::onAddDynamic));
    
    mActionGroup->add(Gtk::Action::create("DynBox_ContextDuplicate", _("_Duplicate")),
	sigc::mem_fun(*this, &DynamicsBox::onCopyDynamic));

    mActionGroup->add(Gtk::Action::create("DynBox_ContextRemove", _("_Remove")),
	sigc::mem_fun(*this, &DynamicsBox::onRemoveDynamic));

    mActionGroup->add(Gtk::Action::create("DynBox_ContextRename", _("_Rename")),
	sigc::mem_fun(*this, &DynamicsBox::onRenameDynamic));

    mUIManager = Gtk::UIManager::create();
    mUIManager->insert_action_group(mActionGroup);
    
    Glib::ustring ui_info =
	"<ui>"
	"  <popup name='DynBox_Popup'>"
        "      <menuitem action='DynBox_ContextNew'/>"
	"      <menuitem action='DynBox_ContextDuplicate'/>"
	"      <menuitem action='DynBox_ContextRemove'/>"
	"      <menuitem action='DynBox_ContextRename'/>"
	"  </popup>"
	"</ui>";

    try {
	mUIManager->add_ui_from_string(ui_info);
	mMenu = (Gtk::Menu *) (
	mUIManager->get_widget("/DynBox_Popup"));
    } catch(const Glib::Error& ex) {
	std::cerr << "building menus failed: DynBox_Popup " <<  ex.what();
    }
        
    if (!mMenu)
        std::cerr << "menu not found : DynBox_Popup \n";
    
    mList.push_back(mDynamics->signal_button_release_event().connect(
                        sigc::mem_fun(*this,
                                      &DynamicsBox::onButtonRealeaseDynamics)));

}

void DynamicsBox::onAddDynamic()
{
    Gtk::TreeIter iter = mDynamicsListStore->append();
    if (iter) {

        std::string newName = "newDynamic";
        int copyNumber = 1;
        std::string suffixe;
        while (mDynsCopy.exist(newName)) {
            suffixe = "_" + boost::lexical_cast < std::string >(copyNumber);
            newName = "newDynamic" + suffixe;

            copyNumber++;
        };

        mDynsCopy.add(newName);

        Gtk::ListStore::Row row = *iter;
        row[mDynamicsColumns.mName] = newName;
        row[mDynamicsColumns.mPackages] = mPackagesListStore;

        Gtk::TreeViewColumn* nameCol = mDynamics->get_column(mColumnName - 1);
        mDynamics->set_cursor(mDynamicsListStore->get_path(iter),*nameCol,true);
    }
}

void DynamicsBox::onRemoveDynamic()
{

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
                Gtk::TreeModel::Path path = mDynamicsListStore->get_path(mIter);
                mDynamics->set_cursor(path);
            }
        }
    }
}

void DynamicsBox::onRenameDynamic()
{
    Glib::RefPtr < Gtk::TreeView::Selection > ref = mDynamics->get_selection();
    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
            Gtk::TreeViewColumn* nameCol = mDynamics->get_column(mColumnName - 1);
            mDynamics->set_cursor(mDynamicsListStore->get_path(iter),
                                  *nameCol,true);
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
            row[mDynamicsColumns.mPackages] = mPackagesListStore;
            row[mDynamicsColumns.mLibrary] = mDynsCopy.get(name).library();
            row[mDynamicsColumns.mLibraries] =
                mLibrariesListStore[mDynsCopy.get(name).package()];
            mDynamics->set_cursor(mDynamicsListStore->get_path(iter));
            mDeletedDynamics.erase(copy);
        }
    }
}

void DynamicsBox::onEditionStarted(
    Gtk::CellEditable* /*celleditable*/,
    const Glib::ustring& path)
{
    Gtk::TreeModel::Path selectedpath(path);
    Gtk::TreeModel::iterator it = mDynamicsListStore->get_iter(selectedpath);

    Gtk::TreeModel::Row row = *it;
    if (row) {
        mOldName = row.get_value(mDynamicsColumns.mName);
    }
}

void DynamicsBox::onEdition(
    const Glib::ustring& /*pathstring*/,
    const Glib::ustring& newstring)
{
    std::string newName = newstring.raw();
    boost::trim(newName);
    std::string oldName = mOldName.raw();

    Glib::RefPtr < Gtk::TreeView::Selection > ref = mDynamics->get_selection();
    if (ref) {
        Gtk::TreeModel::iterator it = ref->get_selected();
        if (*it and not newName.empty() and newName != oldName) {
            Gtk::TreeModel::Row row = *it;
            if (not mDynsCopy.exist(newName) and isValidName(newName)) {
                mDynsCopy.rename(mOldName, newName);
                mRenameList.push_back(std::make_pair(oldName, newName));
                row[mDynamicsColumns.mName] = newstring;

            } else {
                row[mDynamicsColumns.mName] = mOldName;
            }
        }
    }
}

bool DynamicsBox::onButtonRealeaseDynamics(GdkEventButton* event)
{
    if(event->button == 3) {
        mMenu->popup(event->button, event->time);
    }
    return true;
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

void DynamicsBox::onClickColumn(int numColumn)
{
    int current_sort_column_id;
    Gtk::SortType current_order;

    if (mDynamicsListStore->get_sort_column_id (current_sort_column_id,
                                                current_order)) {
        if (current_sort_column_id == numColumn - 1) {
            if (current_order == Gtk::SORT_ASCENDING) {
                mDynamicsListStore->set_sort_column(numColumn - 1,
                                                    Gtk::SORT_DESCENDING);
            } else {
                mDynamicsListStore->set_sort_column(numColumn - 1,
                                                    Gtk::SORT_ASCENDING);
            }
        } else {
            mDynamicsListStore->set_sort_column(numColumn - 1,
                                                Gtk::SORT_ASCENDING);
        }
    }
}

}} // namespace vle gvle
