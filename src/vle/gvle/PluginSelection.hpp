/**
 * @file vle/gvle/PluginSelection.hpp
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


#ifndef GUI_PLUGIN_SELECTION_H
#define GUI_PLUGIN_SELECTION_H

#include <gtkmm.h>
#include <vector>

namespace vle
{
namespace gvle {

class TreeModelPlugin : public Gtk::ListStore
{
protected:
    TreeModelPlugin();

public:

class ModelColumnsPlugin : public Gtk::TreeModel::ColumnRecord
    {
    public:

        ModelColumnsPlugin() {
            add(m_col_lib);
            add(m_col_output);
        }

        Gtk::TreeModelColumn<Glib::ustring> m_col_lib;
        Gtk::TreeModelColumn<Glib::ustring> m_col_output;
    };

    ModelColumnsPlugin m_Columns;

    static Glib::RefPtr<TreeModelPlugin> create();
};


class PluginSelection : public Gtk::Dialog
{
public:
    /**
     * Constructor
     * @param parent the parent window
     * @param plugin_list the plugin list
     */
    PluginSelection(Gtk::Window &parent, std::vector < std::pair < std::string , std::string > > plugin_list);

    /**
     * Destructor
     */
    virtual ~PluginSelection();

    /**
     * Return the lib name of the selected row
     * @return the lib name of the selected row
     */
    std::string getSelectedLib();

    /**
     * Return the output name of the selected row
     * @return the output name of the selected row
     */
    std::string getSelectedOutput();

    /**
     * Selection a specific row
     * @lib name of the lib
     * @output name of the output
     */
    void setSelectedData(std::string lib, std::string output);

    /**
     * Add a new entry
     * @lib name of the lib
     * @output name of the output
     */
    void addEntry(std::string lib, std::string output);

    /**
     * Call the run method
     * @return the response ID corresponding to the button the user clicked
     */
    int run();

    /**
     * Erase an entry of the treeview.
     * But the data aren't delete in the plugin list.
     * @param lib the lib plugin
     * @param output the output plugin
     */
    void eraseEntry(std::string lib, std::string output);

    /**
     * Set the plugin list
     * @param plugin_list the plugin list
     */
    void setListPlugin(std::vector < std::pair < std::string , std::string > > plugin_list);

protected :
    std::vector < std::pair < std::string , std::string > > mpluginList;
    std::string mSelectedLib, mSelectedOutput;
    Gtk::Button *mOkButton;
    Gtk::ScrolledWindow mScrolledWindow;
    Gtk::TreeView mTreeViewPlugin;
    Glib::RefPtr<TreeModelPlugin> mRefTreeModelPlugin;
    Glib::RefPtr<Gtk::TreeSelection> mRefTreeSelection;

    /**
     * Initialize the treeview, append the columns
     */
    void initTreeview();

    /**
     * Fill the treeview with the data contained in the vector
     */
    void fillTreeview();

    /**
     * Add a row to the treeview
     * @lib name of the lib
     * @output name of the output
     */
    void addRow(std::string lib, std::string output);

    /**
     * Return the row contained the specific lib name and output name
     * or return a new row
     * @lib name of the lib
     * @output name of the output
     * @return the row
     */
    Gtk::TreeModel::Row getRow(std::string const &lib, std::string const &output);

    /**
     * Check if a row exist
     * @lib name of the lib
     * @output name of the output
     * @ret_iter iter which point of the row
     * @return true if the row exist, else false
     */
    bool existRow(std::string lib, std::string output, Gtk::TreeModel::iterator &ret_iter);

    /**
     * Set the value of mSelectedLib and mSelectedOutput with
     * the lib and output of the selected row
     */
    void updateSelected();

    /**
     * Erase the selected row
     */
    void eraseSelectedRow();

    /**
     * Call updateSelected() and eraseSelectedRow()
     */
    void on_close();

    /**
     * Call hide()
     */
    void on_cancel();

    /**
     * Calcul the row number
     * @return the row number
     */
    int getRowNumber();

    /**
     * Method call when the selected row change
     * change the ok_button sensitive
     */
    void on_selection_changed();
};

}
} // namespace vle gvle

#endif
