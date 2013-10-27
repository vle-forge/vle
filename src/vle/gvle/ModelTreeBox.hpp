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


#ifndef GUI_MODELTREEBOX_HPP
#define GUI_MODELTREEBOX_HPP

#include <gtkmm/builder.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>
#include <gtkmm/uimanager.h>

namespace vle { namespace vpz {

class BaseModel;
class CoupledModel;

} } // namespace vle graph

namespace vle { namespace gvle {

class Modeling;
class GVLE;

/**
 * @brief A Gtk::Window to show the vpz::Model hierarchy into a window.
 */
class ModelTreeBox : public Gtk::TreeView
{
public:
    /**
     * make a new treeview
     *
     */
    ModelTreeBox(BaseObjectType* cobject,
		 const Glib::RefPtr < Gtk::Builder >& /*refGlade*/);

    /**
     * Show in the TextView the model tree with head 'top'
     *
     * @param top the head of tree to view
     */
    void parseModel(vpz::BaseModel* top);

    /**
     * active a row for a particular model
     *
     * @param mdl model to activate
     */
    void showRow(const vpz::BaseModel* mdl);

    /**
     * Get the treview row as an iterator from the selected model
     *
     * @param mdl selected model
     * @param current model child
     */
    Gtk::TreeModel::iterator getModelRow(const vpz::BaseModel* mdl, Gtk::TreeModel::Children child);

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
    inline void setModelingGVLE(Modeling* modeling, GVLE* gvle)
	{ m_modeling = modeling; m_gvle = gvle; }

    /**
     * @brief Clear the current Treeview
     */
    void clear();

    /**
     * @brief Clear the selection
     */
    void selectNone();

    bool onQueryTooltip(int wx,int wy, bool keyboard_tooltip,
                        const Glib::RefPtr<Gtk::Tooltip>& tooltip);

protected:
    /**
     * a recursive function to complete tree
     *
     * @param row
     * @param top
     */
    void parseModel(Gtk::TreeModel::Row row,
                    const vpz::CoupledModel* top);

    /**
     * Add a new row to top of the TreeView with model name
     *
     * @param name model's name
     * @return A Row to the top of TreeView
     */
    Gtk::TreeModel::Row addModel(vpz::BaseModel* model);

    /**
     * Add a new row to the child of row tree with model name and
     *
     * @param tree parent's row to add model
     * @param name model's name
     *
     * @return A Row to the child of Row tree
     */
    Gtk::TreeModel::Row addSubModel(Gtk::TreeModel::Row tree,
                                    vpz::BaseModel* model);


    /**
     * on activated a row, Modeling is all to show a new View centered on
     * vpz::CoupledModel or AtomicModelBox if model is a
     * vpz::AtomicModel.
     *
     * @param path
     * @param column
     */
    void row_activated(const Gtk::TreeModel::Path& path,
                       Gtk::TreeViewColumn* column);

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
        Gtk::TreeModelColumn <vpz::BaseModel*> mModel;
    };

    ModelTreeColumn              m_columns;
    Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;
    Gtk::Menu*                   m_menu;
    Glib::RefPtr <Gtk::UIManager> mUIManager;
    Glib::RefPtr <Gtk::ActionGroup> mActionGroup;
    
    Modeling*                    m_modeling;
    GVLE*                        m_gvle;
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
    bool onSelect(
        const Glib::RefPtr<Gtk::TreeModel>& model,
        const Gtk::TreeModel::Path& path, bool);
    bool onButtonRealeaseModels(GdkEventButton* event);

    struct ModelsColumnRecord : public Gtk::TreeModel::ColumnRecord
    {
        Gtk::TreeModelColumn < std::string > name;
        ModelsColumnRecord() { add(name); }
    } m_modelsColumnRecord;
};

} } // namespace vle gvle

#endif
