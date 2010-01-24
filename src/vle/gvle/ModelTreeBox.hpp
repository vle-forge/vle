/**
 * @file vle/gvle/ModelTreeBox.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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

#include <vle/gvle/SimpleTypeBox.hpp>
#include <gtkmm/window.h>
#include <libglademm.h>
#include <gtkmm.h>

namespace vle { namespace graph {

class Model;
class CoupledModel;

} } // namespace vle graph

namespace vle { namespace gvle {

class Modeling;

/**
 * @brief A Gtk::Window to show the graph::Model hierarchy into a window.
 */
class ModelTreeBox : public Gtk::TreeView
{
public:
    /**
     * make a new treeview
     *
     */
    ModelTreeBox(BaseObjectType* cobject,
		 const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/);

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

    /**
     * @brief Rename a model with the specified name
     */
    void onRenameModels();

    // Signal handler for text area
    virtual void onEditionStarted(
	Gtk::CellEditable* cellEditatble,
	const Glib::ustring& path);
    virtual void onEdition(
	const Glib::ustring& pathString,
	const Glib::ustring& newName);

    /**
     * @brief Set a value to modeling
     *
     * @param modeling the new value
     *
     */
    inline void setModeling(Modeling* modeling)
	{ m_modeling = modeling; }

    /**
     * @brief Clear the current Treeview
     */
    void clear();

protected:
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
    Gtk::TreeModel::Row addModel(graph::Model* model);

    /**
     * Add a new row to the child of row tree with model name and
     *
     * @param tree parent's row to add model
     * @param name model's name
     *
     * @return A Row to the child of Row tree
     */
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
     * When user clicks, an editable text area appears if it is not
     * a double clic
     *
     * @param event to select mouse button
     */
    bool onExposeEvent(GdkEvent* event);

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
        Gtk::TreeModelColumn <graph::Model*> mModel;
    };

    ModelTreeColumn              m_columns;
    Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;
    Gtk::Menu                    m_menu;
    Modeling*                    m_modeling;
    std::string                  m_search;

    // Cell
    int                          m_columnName;
    Gtk::CellRendererText*       m_cellRenderer;
    std::string                  m_oldName;
    bool                         m_validateRetry;
    Glib::ustring                m_invalidTextForRetry;
    guint32                      m_delayTime;

private:
    void initMenuPopupModels();
    bool onButtonRealeaseModels(GdkEventButton* event);

    struct ModelsColumnRecord : public Gtk::TreeModel::ColumnRecord
    {
        Gtk::TreeModelColumn < std::string > name;
        ModelsColumnRecord() { add(name); }
    } m_modelsColumnRecord;
};

} } // namespace vle gvle

#endif
