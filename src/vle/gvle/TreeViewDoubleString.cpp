/**
 * @file vle/gvle/TreeViewDoubleString.cpp
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


#include <vle/gvle/TreeViewDoubleString.hpp>

using std::string;

namespace vle
{
namespace gvle {

TreeViewDoubleString::TreeViewDoubleString(const string& column1, const string&
        column2)
{
    set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
    mListStore = Gtk::ListStore::create(mColumn);
    mTreeView.set_model(mListStore);
    mTreeView.append_column(column1, mColumn.mString1);
    mTreeView.append_column(column2, mColumn.mString2);
    ScrolledWindow::add(mTreeView);

    mTreeView.signal_row_activated().connect(sigc::mem_fun(
                *this, &TreeViewDoubleString::on_row_activated));
}

void TreeViewDoubleString::get_selected(std::string& s1, std::string& s2)
{
    string result;
    Glib::RefPtr < Gtk::TreeSelection > selection = mTreeView.get_selection();
    Gtk::TreeModel::iterator it = selection->get_selected();
    if (it) {
        Gtk::TreeModel::Row row = *it;
        Glib::ustring name;
        name = row[mColumn.mString1];
        s1 = name;
        name = row[mColumn.mString2];
        s2 = name;
    }
}

void TreeViewDoubleString::add(const std::string& name1, const std::string& name2)
{
    Gtk::TreeModel::Row row = *(mListStore->append());
    if (row) {
        row[mColumn.mString1] = name1;
        row[mColumn.mString2] = name2;
    }
}

void TreeViewDoubleString::del(const std::string& name1, const std::string& name2)
{
    Gtk::TreeModel::Children children = mListStore->children();
    Gtk::TreeModel::iterator it = children.begin();
    while (it != children.end()) {
        Gtk::TreeModel::Row row = *it;
        if (row[mColumn.mString1] == name1 and row[mColumn.mString2] == name2) {
            mListStore->erase(it);
            break;
        }
        ++it;
    }
}

void TreeViewDoubleString::clear()
{
    mListStore->clear();
}

void TreeViewDoubleString::on_row_activated(const Gtk::TreeModel::Path&,
        Gtk::TreeViewColumn*)
{
    std::string str1, str2;
    get_selected(str1, str2);
    mSignalRowActivated.emit(str1, str2);
}

}
} // namespace vle gvle
