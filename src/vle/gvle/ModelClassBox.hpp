/**
 * @file vle/gvle/ModelClassBox.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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


#ifndef GUI_MODELCLASSBOX_HPP
#define GUI_MODELCLASSBOX_HPP

#include <libglademm.h>
#include <gtkmm.h>
#include <vle/vpz/Classes.hpp>
#include <vle/gvle/NewModelClassBox.hpp>

namespace vle { namespace gvle {

class Modeling;

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
		  Glib::RefPtr < Gnome::Glade::Xml > xml);

    ~ModelClassBox();

    void hide();

    /**
     * Show in the TextView the class Tree
     */
    void parseClass();
    Gtk::TreeModel::Row addClass(const vpz::Class& classe);

    virtual void onAdd();
    virtual void onRemove();
    virtual void onRename();
    virtual void onRenameClass(const std::string& newName);
    virtual void onExportVpz();
    virtual void onImportModelAsClass();
    virtual void onImportClassesFromVpz();
    void showRow(const std::string& model_name);

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
    void createNewModelBox(Modeling* m);

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
     * slot call function to call set cursor on a row
     *
     * @param iter iterator to test
     * @return true if found, otherwise false
     */
    bool on_foreach(const Gtk::TreeModel::Path&,
                    const Gtk::TreeModel::iterator& iter);

    /**
     * When user clicks, an editable text area appears if it is not a
     * double clic
     *
     * @param event to select mouse button
     */
    bool onExposeEvent(GdkEvent* event);

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


    class ModelColumns : public Gtk::TreeModelColumnRecord
    {
    public:
        ModelColumns() {
            add(mName);
	    add(mModel);
        }

        Gtk::TreeModelColumn<Glib::ustring> mName;
	Gtk::TreeModelColumn <graph::Model*> mModel;
    };

    ModelColumns mColumns;

private:

    void initMenuPopupModels();
    bool onButtonRealeaseModels(GdkEventButton *event);
    void on_cursor_changed();

    Glib::RefPtr<Gnome::Glade::Xml>      mXml;
    Modeling*                            mModeling;
    NewModelClassBox*                    mNewModelBox;
    Gtk::Menu                            mMenuPopup;

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
