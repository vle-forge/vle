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


#ifndef GUI_TREEVIEW_CONDITION_HPP
#define GUI_TREEVIEW_CONDITION_HPP

#include <gtkmm/treeview.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/liststore.h>
#include <gtkmm/uimanager.h>
#include <vle/value/Set.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/XML.hpp>
#include <iostream>

namespace vle
{
namespace gvle {

class TreeViewValue : public Gtk::TreeView
{
public:
    //Tree model columns:
class ModelColumns : public Gtk::TreeModel::ColumnRecord
    {
    public:
        ModelColumns() {
            add(m_col_name);
            add(m_col_type);
            add(m_col_view);
            add(m_col_value);
        }

        Gtk::TreeModelColumn<Glib::ustring> m_col_name;
        Gtk::TreeModelColumn<Glib::ustring> m_col_type;
        Gtk::TreeModelColumn<Glib::ustring> m_col_view;
        Gtk::TreeModelColumn<value::Value*> m_col_value;
    };

    TreeViewValue();
    virtual ~TreeViewValue();

    void makeTreeView(value::Set& set);
    void makeTreeView(value::Map& map);

    void refresh();

protected:
    virtual bool on_button_press_event(GdkEventButton* e);

private:
    ModelColumns m_Columns;
    //The Tree model:
    Glib::RefPtr<Gtk::ListStore> m_refTreeModel;

    Gtk::Menu* mMenu;
    Glib::RefPtr <Gtk::UIManager> mUIManager;
    Glib::RefPtr <Gtk::ActionGroup> mMenuActionGroup;
    
    value::Value* mValue;

    void on_row_activated(const Gtk::TreeModel::Path& path,
                          Gtk::TreeViewColumn*  column);
    void on_menu_insert(value::Value::type);
    void on_menu_remove();
    void on_menu_duplicate();

};

}
} // namespace vle gvle
#endif
