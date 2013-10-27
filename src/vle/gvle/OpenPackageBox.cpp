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


#include <vle/gvle/OpenPackageBox.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/utils/Path.hpp>

namespace vle { namespace gvle {

OpenPackageBox::OpenPackageBox(const Glib::RefPtr < Gtk::Builder >& xml,
        vle::utils::Package& curr_pack)
    : mXml(xml), mCurrentPackage(curr_pack)
{
    xml->get_widget("DialogPackage", mDialog);
    xml->get_widget("TreeViewPackage", mTreeView);

    mTreeModel = Gtk::TreeStore::create(mColumns);
    mTreeView->append_column(_("Name"), mColumns.mName);
    mTreeView->set_model(mTreeModel);

    mTreeSelection = mTreeView->get_selection();
    mTreeSelection->set_mode(Gtk::SELECTION_MULTIPLE);

    mPopupActionGroup = Gtk::ActionGroup::create("OpenPackageBox");
    mPopupActionGroup->add(Gtk::Action::create("OpenPB_ContextMenu", _("Context Menu")));
    
    mPopupActionGroup->add(Gtk::Action::create("OpenPB_ContextRemove", _("_Remove")),
        sigc::mem_fun(*this, &OpenPackageBox::onRemove));
    
    mPopupUIManager = Gtk::UIManager::create();
    mPopupUIManager->insert_action_group(mPopupActionGroup);

    Glib::ustring ui_info =
        "<ui>"
        "  <popup name='OpenPB_Popup'>"
        "      <menuitem action='OpenPB_ContextRemove'/>"
        "  </popup>"
        "</ui>";

    try {
        mPopupUIManager->add_ui_from_string(ui_info);
        mMenuPopup = (Gtk::Menu *) (
            mPopupUIManager->get_widget("/OpenPB_Popup"));
    } catch(const Glib::Error& ex) {
        std::cerr << "building menus failed: OpenPB_Popup " <<  ex.what();
    }

    if (!mMenuPopup)
        std::cerr << "menu not found : OpenPB_Popup\n";
    
    mConnections.push_back(
        mTreeView->signal_row_activated().connect(
            sigc::mem_fun(*this, &OpenPackageBox::onRowActivated)));

    mConnections.push_back(
        mTreeView->signal_button_press_event().connect_notify(
            sigc::mem_fun(*this, &OpenPackageBox::onButtonPressEvent)));

    mConnections.push_back(
        mTreeSelection->signal_changed().connect(
            sigc::mem_fun(*this, &OpenPackageBox::onSelectionChanged)));
}

OpenPackageBox::~OpenPackageBox()
{
    if (mTreeView) {
        mTreeView->remove_all_columns();
    }

    for (std::list < sigc::connection >::iterator it = mConnections.begin();
         it != mConnections.end(); ++it) {
        it->disconnect();
    }
}

bool OpenPackageBox::run()
{
    build();

    mDialog->set_title("Open Project");
    mDialog->show();

    int result = mDialog->run();
    mDialog->hide();

    return result == Gtk::RESPONSE_OK and not mName.empty();
}

void OpenPackageBox::build()
{
    mTreeModel->clear();
    mName.clear();

    utils::PathList list = utils::Path::path().getBinaryPackages();

    std::sort(list.begin(), list.end());
    for (utils::PathList::const_iterator it = list.begin();
         it != list.end(); ++it) {
        Gtk::TreeModel::Row row = *(mTreeModel->append());
        row[mColumns.mName] = utils::Path::filename(*it);
    }
}

void OpenPackageBox::onRowActivated(const Gtk::TreeModel::Path& path,
                                    Gtk::TreeViewColumn* /* column */)
{
    Gtk::TreeModel::Path p(path);
    Gtk::TreeModel::iterator it = mTreeModel->get_iter(p);
    Gtk::TreeModel::Row row = *it;

    if (row) {
        mDialog->activate_default();
    }
}

void OpenPackageBox::onRemove()
{
    typedef std::list < Gtk::TreeModel::Path > paths_t;

    std::vector<Gtk::TreePath> v1 = mTreeSelection->get_selected_rows();
    std::list<Gtk::TreePath> p (v1.begin(), v1.end());
    //paths_t p = mTreeSelection->get_selected_rows();

    if (not p.empty()) {
        std::string result;
        paths_t::iterator it = p.begin();
        while (it != p.end()) {
            Gtk::TreeModel::iterator jt = mTreeModel->get_iter(*it);
            result.append("   - ");
            result.append((*jt).get_value(mColumns.mName));
            ++it;
            if (it != p.end()) {
                result.append("\n");
            }
        }

        if (gvle::Question(fmt(_("Do you really want to remove:\n%1% ?")) %
                           result)) {
            mTreeSelection->selected_foreach_iter(
                sigc::mem_fun(*this, &OpenPackageBox::onRemoveCallBack));
            mTreeSelection->unselect_all();
            build();
        }
    }
}

void OpenPackageBox::onRemoveCallBack(const Gtk::TreeModel::iterator& it)
{
    const Gtk::TreeModel::Row row = *it;

    if (row) {
        Glib::ustring package = row[mColumns.mName];
        mCurrentPackage.remove(package.raw(), vle::utils::PKG_SOURCE);
    }
}

void OpenPackageBox::onButtonPressEvent(GdkEventButton* event)
{
    if (event->type == GDK_BUTTON_PRESS && event->button== 3) {
        mMenuPopup->popup(event->button, event->time);
    }
}

void OpenPackageBox::onSelectionChanged()
{
    typedef std::list < Gtk::TreeModel::Path > paths_t;

    std::vector<Gtk::TreePath> v1 = mTreeSelection->get_selected_rows();
    std::list<Gtk::TreePath> p (v1.begin(), v1.end());
    //paths_t p = mTreeSelection->get_selected_rows();

    if (not p.empty()) {
        paths_t::reverse_iterator it = p.rbegin();
        Gtk::TreeModel::iterator jt = mTreeModel->get_iter(*it);
        mName = (*jt).get_value(mColumns.mName);
    }
}

}} // namespace vle gvle
