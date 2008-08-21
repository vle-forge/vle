/**
 * @file vle/gvle/TreeViewSingleString.cpp
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


#include <vle/gvle/TreeViewSingleString.hpp>

using std::string;

namespace vle
{
namespace gvle {

TreeViewSingleString::TreeViewSingleString(const string& name)
{
    set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
    mListStore = Gtk::ListStore::create(mColumn);
    mTreeView.set_model(mListStore);
    mTreeView.append_column(name, mColumn.mString);
    ScrolledWindow::add(mTreeView);

    mTreeView.signal_row_activated().connect(sigc::mem_fun(
                *this, &TreeViewSingleString::on_row_activated));
}

string TreeViewSingleString::get_selected()
{
    string result;
    Glib::RefPtr < Gtk::TreeSelection > selection = mTreeView.get_selection();
    Gtk::TreeModel::iterator it = selection->get_selected();
    if (it) {
        Gtk::TreeModel::Row row = *it;
        Glib::ustring name(row[mColumn.mString]);
        result = name;
    }
    return result;
}

void TreeViewSingleString::add(const string& name)
{
    Gtk::TreeModel::Row row = *(mListStore->append());
    if (row) {
        row[mColumn.mString] = name;
    }
}

void TreeViewSingleString::del(const string& name)
{
    Gtk::TreeModel::Children children = mListStore->children();
    Gtk::TreeModel::iterator it = children.begin();
    while (it != children.end()) {
        Gtk::TreeModel::Row row = *it;
        if (row[mColumn.mString] == name) {
            mListStore->erase(it);
            break;
        }
        ++it;
    }
}

void TreeViewSingleString::clear()
{
    mListStore->clear();
}

void TreeViewSingleString::on_row_activated(const Gtk::TreeModel::Path&,
        Gtk::TreeViewColumn*)
{
    mSignalRowActivated.emit(get_selected());
}

}
} // namespace vle gvle
