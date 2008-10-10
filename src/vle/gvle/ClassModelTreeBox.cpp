/**
 * @file vle/gvle/ClassModelTreeBox.cpp
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


#include <vle/gvle/ClassModelTreeBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/DialogString.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/utils/Debug.hpp>

using namespace vle;

namespace vle
{
namespace gvle {

ClassModelTreeBox::ClassModelTreeBox(Modeling* modeling) :
        mButtonNew("new"),
        mButtonView("view"),
        mButtonDel("del"),
        mModeling(modeling)
{
    assert(modeling);

    mRefTreeModel = Gtk::ListStore::create(mColumns);
    mTreeView.set_model(mRefTreeModel);
    mTreeView.append_column("Name", mColumns.mName);
    mTreeView.expand_all();
    mTreeView.set_rules_hint(true);
    mScrolledWindow.add(mTreeView);
    mScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    mHBoxButton.pack_start(mButtonNew, true, true);
    mHBoxButton.pack_start(mButtonView, true, true);
    mHBoxButton.pack_start(mButtonDel, true, true);
    mVBox.pack_start(mScrolledWindow, true, true);
    mVBox.pack_start(mHBoxButton, false, true);
    add(mVBox);

    mHBoxButton.set_border_width(3);
    set_title("Models class");
    set_default_size(200, 350);
    mVBox.set_border_width(3);
    set_border_width(5);

    parseModel();
    connect();
}

void ClassModelTreeBox::parseModel()
{
    mRefTreeModel->clear();

    //const std::map < std::string, vpz::Class >& lst =
    //mModeling->vpz().project().classes().classes();

    //std::map < std::string, vpz::Class >::const_iterator it = lst.begin();
    //while (it != lst.end()) {
    //Gtk::TreeModel::Row row = *(mRefTreeModel->append());
    //row[mColumns.mName] = (*it).first;
    //++it;
    //}
}

void ClassModelTreeBox::showRow(const std::string& model)
{
    mSearch.assign(model);
    mRefTreeModel->foreach(sigc::mem_fun(*this, &ClassModelTreeBox::onForeach));
}

void ClassModelTreeBox::connect()
{
    mTreeView.signal_row_activated().connect(
        sigc::mem_fun(*this, &ClassModelTreeBox::onRowActivated));
    mButtonNew.signal_clicked().connect(
        sigc::mem_fun(*this, &ClassModelTreeBox::onClickOnButtonAdd));
    mButtonView.signal_clicked().connect(
        sigc::mem_fun(*this, &ClassModelTreeBox::onClickOnButtonView));
    mButtonDel.signal_clicked().connect(
        sigc::mem_fun(*this, &ClassModelTreeBox::onClickOnButtonDel));
}

bool ClassModelTreeBox::on_key_release_event(GdkEventKey* event)
{
    if (((event->state & GDK_CONTROL_MASK) and event->keyval == GDK_w) or
            (event->keyval == GDK_Escape)) {
        mModeling->hideClassModelTreeBox();
    }
    return true;
}

void ClassModelTreeBox::onRowActivated(const Gtk::TreeModel::Path& /*path*/,
                                       Gtk::TreeViewColumn* /*column*/)
{
    //if (column) {
    //    Gtk::TreeIter iter = mRefTreeModel->get_iter(path);
    //    Gtk::TreeRow row = (*iter);
    //    mModeling->addView(row.get_value(mColumns.mName));
    //}
}

void ClassModelTreeBox::onClickOnButtonAdd()
{
    //std::string new_model_name;
    //if (mModeling->getNewName(new_model_name)) {
    //mModeling->addClassModel(new_model_name);
    //parseModel();
    //}
}

void ClassModelTreeBox::onClickOnButtonView()
{
    //std::string selected_model;
    //if (getSelectedName(selected_model)) {
    //    mModeling->addView(selected_model);
    //    parseModel();
    //}
}

void ClassModelTreeBox::onClickOnButtonDel()
{
    //std::string selected_model;
    //if (getSelectedName(selected_model)) {
    //if (widgets::Question((boost::format(
    //"Really delete class model '%1%' ?") % selected_model).str())) {
    //mModeling->delClassModel(selected_model);
    //parseModel();
    //}
    //}
}

bool ClassModelTreeBox::getSelectedName(std::string& name)
{
    Glib::RefPtr < Gtk::TreeSelection > selection = mTreeView.get_selection();
    Gtk::TreeModel::iterator it = selection->get_selected();
    if (it) {
        Gtk::TreeModel::Row row = *it;
        Glib::ustring tmpName(row[mColumns.mName]);
        name.assign(tmpName);
        return true;
    }
    return false;
}

bool ClassModelTreeBox::onForeach(const Gtk::TreeModel::Path&,
                                  const Gtk::TreeModel::iterator& iter)
{
    if ((*iter).get_value(mColumns.mName) == mSearch) {
        mTreeView.set_cursor(mRefTreeModel->get_path(iter));
        return true;
    }
    return false;
}

}
} // namespace vle gvle
