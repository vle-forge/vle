/**
 * @file vle/gvle/ClassModelTreeBox.hpp
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


#ifndef GUI_CLASSMODELTREEBOX_HPP
#define GUI_CLASSMODELTREEBOX_HPP

#include <gtkmm/window.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>
#include <gtkmm/liststore.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>

namespace vle
{
namespace graph {

class CoupledModel;

}
} // namespace vle graph

namespace vle
{
namespace gvle {

class Modeling;

/**
 * This class define a window to show all deconnected models from modeling
 * class ; This window allow user to add, delete or view deconnected
 * models.
 */
class ClassModelTreeBox : public Gtk::Window
{
public:
    /**
     * make a new Window show deconnected model tree read from Modeling.
     *
     * @param modeling where to get model information.
     */
    ClassModelTreeBox(Modeling* modeling);

    /** Nothing to delete. */
    virtual ~ClassModelTreeBox() {}

    /** Parse modeling deconnected models and show it name in treeview. */
    void parseModel();

    /**
     * To select row model name into tree view.
     *
     * @param model name of CoupledModel to show.
     */
    void showRow(const std::string& model);

private:
    /** connect all event with functions. */
    void connect();

    /**
     * When user press ECHAP or CTRL-W, we close window.
     *
     * @param event to select key press.
     * @return true.
     */
    bool on_key_release_event(GdkEventKey* event);

    /**
     * when user active a row into tree view.
     *
     * @param path of activated row.
     * @param column of actived row.
     */
    void onRowActivated(const Gtk::TreeModel::Path& path,
                        Gtk::TreeViewColumn* column);

    /** when user click on button add. */
    void onClickOnButtonAdd();

    /** when user click on button view. */
    void onClickOnButtonView();

    /** when user click on button del. */
    void onClickOnButtonDel();

    /**
     * return selected model from TreeView widget.
     *
     * @param name return parameter to get name.
     * @return true if name is correct, false otherwise.
     */
    bool getSelectedName(std::string& name);

    /**
     * find a string equal to mSearch into treeview and show selected
     * iterator.
     *
     * @param path TreeModel::Path of iterator.
     * @param iter TreeModel::iterator of current row.
     * @return true if found, otherwise false.
     */
    bool onForeach(const Gtk::TreeModel::Path& path, const
                   Gtk::TreeModel::iterator& iter);

    /**
     * Describe ColumnRead of ModelTree with to string, name and
     * description.
     */
class ModelTreeColumn : public Gtk::TreeModel::ColumnRecord
    {
    public:
        ModelTreeColumn() {
            add(mName);
        }
        Gtk::TreeModelColumn <Glib::ustring> mName;
    };

    Gtk::VBox                       mVBox;
    Gtk::ScrolledWindow             mScrolledWindow;
    Gtk::TreeView                   mTreeView;
    Glib::RefPtr < Gtk::ListStore > mRefTreeModel;
    ModelTreeColumn                 mColumns;
    Gtk::HBox                       mHBoxButton;
    Gtk::Button                     mButtonNew;
    Gtk::Button                     mButtonView;
    Gtk::Button                     mButtonDel;
    Modeling*                       mModeling;
    std::string                     mSearch;
};

}
} // namespace vle gvle

#endif
