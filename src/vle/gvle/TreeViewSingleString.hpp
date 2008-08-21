/**
 * @file vle/gvle/TreeViewSingleString.hpp
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


#ifndef GVLE_WIDGETS_TREEVIEWSINGLESTRING_HPP
#define GVLE_WIDGETS_TREEVIEWSINGLESTRING_HPP

#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>
#include <gtkmm/scrolledwindow.h>
#include <glibmm/ustring.h>

namespace vle
{
namespace gvle {

/**
 * @brief A subclass of Gtk::ScrolledWindow with a Gtk::TreeView for a list
 * of Glib::ustring has children.
 *
 */
class TreeViewSingleString : public Gtk::ScrolledWindow
{
public:
    typedef sigc::signal<void, std::string > type_signal_row_activated;

    /**
     * Create a new TreeViewSingleString with an empty list in Gtk::TreeView
     * class.
     *
     * @param column_name std::string has column name in Gtk::TreeView.
     */
    TreeViewSingleString(const std::string& column_name);

    /**
     * Nothing to delete.
     *
     */
    virtual ~TreeViewSingleString() {}

    /**
     * get current selected string. If no selection, return empty string
     *
     * @return current selected string or empty string if no selected
     */
    std::string get_selected();

    /**
     * add a string into TreeView.
     *
     * @param name std::string to add into TreeView.
     */
    void add(const std::string& name);

    /**
     * delete string from TreeView if exist.
     *
     * @param name to delete from TreeView.
     */
    void del(const std::string& name);

    /** delete all string from TreeView. */
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
            add(mString);
        }
        Gtk::TreeModelColumn < Glib::ustring > mString;
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
