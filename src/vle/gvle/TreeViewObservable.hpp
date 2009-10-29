/**
 * @file vle/gvle/TreeViewObservable.hpp
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


#ifndef GUI_TREEVIEW_OBSERVABLE_HPP
#define GUI_TREEVIEW_OBSERVABLE_HPP

#include <gtkmm.h>
#include <vle/vpz/Observable.hpp>
#include <vle/vpz/Views.hpp>

namespace vle
{
namespace gvle {

class TreeViewObservable : public Gtk::TreeView
{
public:
    //Tree model columns:
class ModelColumns : public Gtk::TreeModel::ColumnRecord
    {
    public:
        ModelColumns() {
            add(m_col_name);
            add(m_col_parent); /*add(m_col_value);*/
        }

        Gtk::TreeModelColumn<Glib::ustring> m_col_name;
        Gtk::TreeModelColumn<vpz::ObservablePort*> m_col_parent;
        //Gtk::TreeModelColumn<vpz::Base*> m_col_value;
    };

    ModelColumns m_Columns;

    TreeViewObservable();

    void makeTreeView(vpz::Observable& obs);

    void refresh();

protected:
    virtual void on_row_activated(const Gtk::TreeModel::Path& path,
                                  Gtk::TreeViewColumn* column);
    //virtual bool on_button_press_event(GdkEventButton* event);
private:
    //The Tree model:
    Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;

    //Gtk::Menu mMenu_port;
    //Gtk::Menu mMenu_no_port;
    vpz::Observable* mObservable;
    //vpz::Views* mViews;

    //void on_menu_insert_port();
    //void on_menu_insert_view();
    //void on_menu_remove();
};

}
} // namespace vle gvle
#endif
