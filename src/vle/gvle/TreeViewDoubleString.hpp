/**
 * @file vle/gvle/TreeViewDoubleString.hpp
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


#ifndef GVLE_WIDGETS_TREEVIEWDOUBLESTRING_HPP
#define GVLE_WIDGETS_TREEVIEWDOUBLESTRING_HPP

#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>
#include <gtkmm/scrolledwindow.h>
#include <glibmm/ustring.h>

namespace vle
{
namespace gvle {

/**
 * A subclass of Gtk::ScrolledWindow with a Gtk::TreeView for a
 * list of Glib::ustring pair has children.
 *
 */
class TreeViewDoubleString : public Gtk::ScrolledWindow
{
public:
    typedef sigc::signal<void, std::string, std::string >
    type_signal_row_activated;

    /**
     * Create a new TreeViewDoubleString with an empty list in Gtk::TreeView
     * class.
     *
     * @param column1 name of first column.
     * @param column2 name of second column.
     */
    TreeViewDoubleString(const std::string& column1,
                         const std::string& column2);

    /**
     * Nothing to delete.
     *
     */
    virtual ~TreeViewDoubleString() {}

    /**
     * get current selected string. If no selection, return empty string
     *
     * @param s1 get current selected string from column 1 or empty string
     * if no selected.
     * @param s2 get current selected string from column 2 or empty string
     * if no selected.
     */
    void get_selected(std::string& s1, std::string& s2);

    /**
     * add a row into TreeView.
     *
     * @param name1 to add into TreeView
     * @param name2 to add into TreeView
     */
    void add(const std::string& name1, const std::string& name2);

    /**
     * delete row from TreeView with first column equal name1 and second
     * column equal name2.
     *
     * @param name1 to delete from TreeView
     * @param name2 to delete from TreeView
     */
    void del(const std::string& name1, const std::string& name2);

    /** delete all string from TreeView */
    void clear();

    /** when a row is activated in TreeView. */
    type_signal_row_activated signal_row_activated() {
        return mSignalRowActivated;
    }

protected:
class Column : public Gtk::TreeModel::ColumnRecord
    {
    public:
        Column() {
            add(mString1);
            add(mString2);
        }
        Gtk::TreeModelColumn < Glib::ustring > mString1;
        Gtk::TreeModelColumn < Glib::ustring > mString2;
    };

    /** When a row is activated. */
    virtual void on_row_activated(const Gtk::TreeModel::Path& path,
                                  Gtk::TreeViewColumn* column);

    Gtk::TreeView                   mTreeView;
    Column                          mColumn;
    Glib::RefPtr < Gtk::ListStore > mListStore;
    type_signal_row_activated       mSignalRowActivated;
};

}
} // namespace vle gvle

#endif
