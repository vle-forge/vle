/**
 * @file vle/gvle/ModelTreeBox.hpp
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


#ifndef GUI_MODELTREEBOX_HPP
#define GUI_MODELTREEBOX_HPP

#include <gtkmm/window.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>

namespace vle
{
namespace graph {

class Model;
class CoupledModel;

}
} // namespace vle graph

namespace vle
{
namespace gvle {

class Modeling;

/**
 * @brief A Gtk::Window to show the graph::Model hierarchy into a window.
 */
class ModelTreeBox : public Gtk::Window
{
public:
    /**
     * make a new Window show complete model tree read from Modeling
     *
     * @param m where to get model information
     */
    ModelTreeBox(Modeling* m);

    /**
     * Show in the TextView the model tree with head 'top'
     *
     * @param top the head of tree to view
     */
    void parseModel(graph::Model* top);

    /**
     * active a row for a particular string, all activated row are hide
     *
     * @param model_name model name to activate
     */
    void showRow(const std::string& model_name);

protected:
    /**
     * When user press ECHAP or CTRL-W, we close window
     *
     * @param event to select key press
     * @return true
     */
    bool on_key_release_event(GdkEventKey* event);

    /**
     * a recursive function to complete tree
     *
     * @param row
     * @param top
     */
    void parseModel(Gtk::TreeModel::Row row,
                    const graph::CoupledModel* top);

    /**
     * Add a new row to top of the TreeView with model name
     *
     * @param name model's name
     * @return A Row to the top of TreeView
     */
    //Gtk::TreeModel::Row addModel(const Glib::ustring& name);
    Gtk::TreeModel::Row addModel(graph::Model* model);

    /**
     * Add a new row to the child of row tree with model name and
     *
     * @param tree parent's row to add model
     * @param name model's name
     *
     * @return A Row to the child of Row tree
     */
    //Gtk::TreeModel::Row addSubModel(Gtk::TreeModel::Row tree,
    //                               const Glib::ustring& name);
    Gtk::TreeModel::Row addSubModel(Gtk::TreeModel::Row tree,
                                    graph::Model* model);


    /**
     * on activated a row, Modeling is all to show a new View centered on
     * graph::CoupledModel or AtomicModelBox if model is a
     * graph::AtomicModel.
     *
     * @param path
     * @param column
     */
    void row_activated(const Gtk::TreeModel::Path& path,
                       Gtk::TreeViewColumn* column);


    /**
     * slot call function to call set cursor on a row
     *
     * @param iter iterator to test
     * @return true if found, otherwise false
     */
    bool on_foreach(const Gtk::TreeModel::Path&,
                    const Gtk::TreeModel::iterator& iter);

    /**
     * Describe ColumnRead of ModelTree with to string, name and
     * description.
     *
     */
class ModelTreeColumn : public Gtk::TreeModel::ColumnRecord
    {
    public:
        ModelTreeColumn() {
            add(mName);
            add(mModel);
        }

        Gtk::TreeModelColumn <Glib::ustring> mName;
        Gtk::TreeModelColumn <graph::Model*>   mModel;
    };

    ModelTreeColumn              m_Columns;
    Gtk::ScrolledWindow          m_ScrolledWindow;
    Gtk::TreeView                m_TreeView;
    Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;
    Modeling*                    m_modeling;
    std::string                  m_search;
};

}
} // namespace vle gvle

#endif
