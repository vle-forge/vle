/**
 * @file vle/gvle/ModelClassBox.hpp
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


#ifndef GUI_MODELCLASSBOX_HPP
#define GUI_MODELCLASSBOX_HPP

#include <libglademm.h>
#include <gtkmm.h>
#include <vle/vpz/Classes.hpp>
#include <vle/gvle/NewModelClassBox.hpp>

namespace vle
{
namespace gvle {

class Modeling;

/**
 * @brief A Gtk::Window to show the vpz::Class hierarchy into a window.
 */
class ModelClassBox : public Gtk::Window
{
public:
    /**
     * make a new Window show complete class and model tree read from Modeling
     *
     * @param m where to get model information
     */
    ModelClassBox(Glib::RefPtr < Gnome::Glade::Xml > xml, Modeling* m);

    ~ModelClassBox();

    void hide();

    /**
     * Show in the TextView the class Tree
     */
    void parseClass();
    Gtk::TreeModel::Row addClass(vpz::Class& classe);

    virtual void onAdd();
    virtual void onRemove();
    virtual void onRename();
    virtual void onExportVpz();
    virtual void onImportModelAsClass();

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

    Glib::RefPtr<Gnome::Glade::Xml>      mXml;
    Modeling*                            mModeling;
    NewModelClassBox*                    mNewModelBox;
    Gtk::Menu                            mMenuPopup;

    //Backup
    vpz::ClassList*                      mClasses_backup;
    Gtk::TreeView                        mTreeView;
    Glib::RefPtr<Gtk::TreeStore>         mRefTreeModel;
    Gtk::ScrolledWindow                  mScrolledWindow;
    std::string                          mSearch;
};

}
} // namespace vle gvle

#endif
