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


#ifndef GUI_MODELCLASSBOX_HPP
#define GUI_MODELCLASSBOX_HPP

#include <vle/gvle/NewModelClassBox.hpp>
#include <vle/vpz/Classes.hpp>
#include <vle/vpz/BaseModel.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <gtkmm/builder.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>
#include <gtkmm/uimanager.h>

namespace vle { namespace gvle {

class Modeling;
class GVLE;

/**
 * @brief A Gtk::Window to show the vpz::Class hierarchy into a window.
 */
class ModelClassBox : public Gtk::TreeView
{
public:
    /**
     * make a new Window show complete class and model tree read from Modeling
     *
     */
    ModelClassBox(BaseObjectType* cobject,
		  const Glib::RefPtr < Gtk::Builder >& xml);

    ~ModelClassBox();

    /**
     * Show in the TextView the class Tree
     */
    void parseClass();

    /**
     * active a row for a particular model
     *
     * @param mdl model to activate
     */
    void showRow(const vpz::BaseModel* mdl);

    Gtk::TreeModel::iterator getModelRow(const vpz::BaseModel* mdl,
                                         Gtk::TreeModel::Children child);


    Gtk::TreeModel::Row addClass(const vpz::Class& classe);

    virtual void onAdd();
    virtual void onRemove();
    virtual void onRename();
    virtual void onRenameClass(const std::string& newName);
    virtual void onExportVpz();
    virtual void onImportModelAsClass();
    virtual void onImportClassesFromVpz();

    // Signal handler for text area
    virtual void onEditionStarted(
	Gtk::CellEditable* cellEditatble,
	const Glib::ustring& path);
    virtual void onEdition(
	const Glib::ustring& pathString,
	const Glib::ustring& newName);

    /**
     * @brief init the NewModelClassBox
     *
     */
    void createNewModelBox(Modeling* m, GVLE* GVLE);

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
     * When user clicks, an editable text area appears if it is not a
     * double clic
     *
     * @param event to select mouse button
     */
    bool onExposeEvent(GdkEvent* event);

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


    class ModelColumns : public Gtk::TreeModelColumnRecord
    {
    public:
        ModelColumns() {
            add(mName);
	    add(mModel);
        }

        Gtk::TreeModelColumn<Glib::ustring> mName;
	Gtk::TreeModelColumn <vpz::BaseModel*> mModel;
    };

    ModelColumns mColumns;

private:

    void initMenuPopupModels();
    bool onSelect(
        const Glib::RefPtr<Gtk::TreeModel>& model,
        const Gtk::TreeModel::Path& path, bool);
    bool onButtonRealeaseModels(GdkEventButton *event);
    void onCursorChanged();

    /**
     * @brief the Class name of a model
     *
     * @param the path of the row where the model is stored
     */
    std::string getClassName(const Gtk::TreeModel::Path&);


    Glib::RefPtr < Gtk::Builder >        mXml;
    Modeling*                            mModeling;
    GVLE*                                mGVLE;
    NewModelClassBox*                    mNewModelBox;
    Gtk::Menu*                           mMenuPopup;
    Glib::RefPtr <Gtk::UIManager> mPopupUIManager;
    Glib::RefPtr <Gtk::ActionGroup> mPopupActionGroup;
    
    //Backup
    vpz::ClassList*                      mClasses_backup;
    Glib::RefPtr<Gtk::TreeStore>         mRefTreeModel;
    std::string                          mSearch;

    //Cell
    int                                  mColumnName;
    Gtk::CellRendererText*               mCellRenderer;
    std::string                          mOldName;
    bool                                 mValidateRetry;
    Glib::ustring                        mInvalidTextForRetry;
    guint32                              mDelayTime;
};

} } // namespace vle gvle

#endif
