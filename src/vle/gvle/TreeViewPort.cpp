/**
 * @file vle/gvle/TreeViewPort.cpp
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


#include <vle/gvle/TreeViewPort.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <boost/algorithm/string/trim.hpp>

using namespace vle;

namespace vle
{
namespace gvle {

TreeViewPort::TreeViewPort(vpz::Condition* cond):
        mCond(cond)
{
    m_refTreeModel = Gtk::ListStore::create(m_Columns);
    set_model(m_refTreeModel);
    append_column("Port", m_Columns.m_col_port);

    {
        using namespace Gtk::Menu_Helpers;
        using namespace value;
        Gtk::Menu::MenuList& items = mMenu.items();

        items.push_back(Gtk::Menu_Helpers::MenuElem("Insert",
                        sigc::mem_fun(*this, &TreeViewPort::on_menu_insert)));
        items.push_back(Gtk::Menu_Helpers::MenuElem("Remove",
                        sigc::mem_fun(*this, &TreeViewPort::on_menu_remove)));
    }
    refresh();
}

void TreeViewPort::refresh()
{
    m_refTreeModel->clear();

    std::list< std::string > list;
    mCond->portnames(list);
    std::list< std::string >::iterator it_port = list.begin();

    while (it_port != list.end()) {
        Gtk::TreeModel::Row row = *(m_refTreeModel->append());
        row[m_Columns.m_col_port] = *it_port;
        it_port++;
    }
}

bool TreeViewPort::on_button_press_event(GdkEventButton* event)
{
    if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3)) {
        mMenu.popup(event->button, event->time);
    }
    return Gtk::TreeView::on_button_press_event(event);
}

void TreeViewPort::on_menu_insert()
{
    std::list< std::string > list;
    mCond->portnames(list);

    SimpleTypeBox box("name ?");
    std::string name = boost::trim_copy(box.run());

    if (box.valid()  and not name.empty() and
        std::find(list.begin(), list.end(), name) == list.end()) {
        mCond->add(name);
        refresh();
    }
}

void TreeViewPort::on_menu_remove()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    if (refSelection) {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            mCond->del(row.get_value(m_Columns.m_col_port));
            refresh();
        }
    }
}

}
} // namespace vle gvle
