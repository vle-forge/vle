/**
 * @file vle/gvle/ExpConditions.hpp
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


#ifndef GUI_EXPCONDITIONS_HPP
#define GUI_EXPCONDITIONS_HPP

#include <gtkmm/box.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/frame.h>
#include <gtkmm/button.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>
#include <gtkmm/radiobutton.h>

namespace vle
{
namespace graph {

class CoupledModel;
class Model;
class AtomicModel;

}
} // namespace vle graph

namespace vle
{
namespace gvle {

class Modeling;

/**
 * @brief ExpCondition inherits of Gtk::VBox to be inserted into
 * gui::ExperimentBox. It defines the condition of the Experiment project.
 */
class ExpConditions : public Gtk::VBox
{
public:
    /** Vector of string */
    typedef std::vector < std::string > StringList_t;

    /**
     * Construct frame with all condition from complete Tree. If current
     * exist, it is select into treeview
     *
     * @param top model at top of tree model
     * @param current model to show in start
     */
    ExpConditions(Modeling* mod, graph::Model* current);

    /**
     * Delete frame :(
     */
    virtual ~ExpConditions() {}

private:
    /**
     * Affect treeview with model information. Row specifie parent treeview
     * row to affect treeview
     *
     * @param row treeview parent row
     * @param model model to affect new row
     */
    void addInitPortModel(Gtk::TreeModel::Row row,
                          graph::AtomicModel* model);

    /**
     * Set to treeview current model model only
     *
     * @param model to show data
     */
    void setTreeCurrent(graph::Model* model);

    /**
     * Set to treeview current model and it's brother (same parent)
     *
     * @param model to show data
     */
    void setTreeBrother(graph::Model* model);

    /**
     * Set to treeview all model under model (recursive)
     *
     * @param tree treeview parent row
     * @param model parent model
     */
    void setTreeAll(Gtk::TreeModel::Row tree,
                    graph::CoupledModel* model);

    /**
     * Set to treeview all model under model (recursive)
     *
     * @param model graph::CoupledModel to show childre
     */
    void setTreeAll(graph::CoupledModel* model);

    /** When user click current button */
    void on_current_button_clicked();

    /** When user click brother button */
    void on_brother_button_clicked();

    /** When user click all button */
    void on_all_button_clicked();

    /** When user click init button */
    void on_add_init_button_clicked();

    /**
     * A class to define treeview column in init treeview
     *
     */
class TreeColumnInits : public Gtk::TreeModel::ColumnRecord
    {
    public:
        TreeColumnInits() {
            add(mName);
            add(mPortName);
            add(mValue);
        }

        Gtk::TreeModelColumn < Glib::ustring > mName;
        Gtk::TreeModelColumn < Glib::ustring > mPortName;
        Gtk::TreeModelColumn < Glib::ustring > mValue;
    };

    Modeling*                       mModeling;
    graph::Model*              mCurrent;

    Gtk::VBox                       mVBoxTreeView;
    Gtk::Button                     mAddInitButton;
    Gtk::Frame                      mFrameTreeView;
    Gtk::ScrolledWindow             mScrolledWindow;
    Gtk::TreeView                   mTreeViewInits;
    Glib::RefPtr < Gtk::TreeStore > mTreeStoreInits;
    TreeColumnInits                 mColumnsInits;

    Gtk::Frame                      mFrameButtons;
    Gtk::HBox                       mFrameHBox;
    Gtk::RadioButtonGroup           mRadioButtonGroup;
    Gtk::RadioButton                mCurrentButton;
    Gtk::RadioButton                mBrotherButton;
    Gtk::RadioButton                mAllButton;
};

}
} // namespace vle gvle

#endif

