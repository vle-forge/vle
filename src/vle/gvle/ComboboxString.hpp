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


#ifndef GVLE_WIDGETS_COMBOBOXSTRING_HPP
#define GVLE_WIDGETS_COMBOBOXSTRING_HPP

#include <gtkmm/combobox.h>
#include <gtkmm/liststore.h>
#include <glibmm/ustring.h>

namespace vle
{
namespace gvle {

/**
 * A class to replace Gtk Widget Gtk::ComboBoxText because this class have
 * no function remove.
 */
class ComboBoxString : public Gtk::ComboBox
{
public:
    /**
     * Create new ComboBoxString with an empty list.
     *
     */
    ComboBoxString() {
        mListStore = Gtk::ListStore::create(mColumn);
        set_model(mListStore);
        pack_start(mColumn.mString);
    }

    /**
     * Nothing to delete.
     *
     */
    virtual ~ComboBoxString() {}

    /**
     * Append a string into ComboBoxString and active it.
     *
     * @param str string to append into ComboBoxString.
     */
    void append_string(const Glib::ustring& str) {
        Gtk::TreeModel::iterator it = mListStore->append();
        if (it) {
            Gtk::TreeModel::Row row = *it;
            if (row) {
                row[mColumn.mString] = str;
                set_active(it);
            }
        }
    }

    /**
     * Delete a string from ComboBoxString if exist.
     *
     * @param str string to delete from ComboBoxString.
     */
    void delete_string(const Glib::ustring& str) {
        Gtk::TreeModel::Children children = mListStore->children();
        Gtk::TreeModel::Children::iterator it = children.begin();
        while (it != children.end()) {
            Gtk::TreeModel::Row row = (*it);
            if (row and row[mColumn.mString] == str) {
                mListStore->erase(it);
            }
            ++it;
        }
    }

    /**
     * Select (highlight it) string from ComboBoxString if exist.
     *
     * @param str string to select from ComboBoxString.
     */
    void select_string(const Glib::ustring& str) {
        Gtk::TreeModel::Children children = mListStore->children();
        Gtk::TreeModel::Children::iterator it = children.begin();
        while (it != children.end()) {
            Gtk::TreeModel::Row row = (*it);
            if (row and row[mColumn.mString] == str) {
                set_active(it);
            }
            ++it;
        }
    }

    /**
     * Delete all string from ComboBoxString
     *
     */
    void clear() {
        mListStore->clear();
    }

    /**
     * Get active string selected by user.
     *
     * @return Glib::ustring selected by user it ComboBox.
     */
    Glib::ustring get_active_string() const {
        Glib::ustring result;
        Gtk::TreeModel::iterator it = get_active();
        if (it and *it) {
            result.assign((*it)[mColumn.mString]);
        }
        return result;
    }

private:
class Column : public Gtk::TreeModel::ColumnRecord
    {
    public:
        Column() {
            add(mString);
        }

        Gtk::TreeModelColumn < Glib::ustring > mString;
    };

    Column                          mColumn;
    Glib::RefPtr < Gtk::ListStore > mListStore;
};

}
} // namespace vle gvle

#endif
