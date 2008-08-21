/**
 * @file vle/gvle/TreeViewPort.hpp
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


#ifndef GUI_TREEVIEW_PORT_HPP
#define GUI_TREEVIEW_PORT_HPP

#include <gtkmm.h>
#include <vle/vpz/Condition.hpp>

namespace vle
{
namespace gvle {

class TreeViewPort : public Gtk::TreeView
{
public:
class ModelColumns : public Gtk::TreeModel::ColumnRecord
    {
    public:
        ModelColumns() {
            add(m_col_port);
        }

        Gtk::TreeModelColumn<Glib::ustring> m_col_port;
    };

    ModelColumns m_Columns;

    TreeViewPort(vpz::Condition* cond);
    void refresh();

protected:
    //The Tree model:
    Glib::RefPtr<Gtk::ListStore> m_refTreeModel;
    Gtk::Menu mMenu;
    vpz::Condition* mCond;

    virtual bool on_button_press_event(GdkEventButton* e);
    void on_menu_insert();
    void on_menu_remove();
};

}
} // namespace vle gvle
#endif
