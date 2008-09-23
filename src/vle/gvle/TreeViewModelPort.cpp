/**
 * @file vle/gvle/TreeViewModelPort.cpp
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


#include <boost/algorithm/string/trim.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/gvle/TreeViewModelPort.hpp>

using namespace vle;

namespace vle
{
namespace gvle {

TreeViewModelPort::TreeViewModelPort(graph::ConnectionList& c):
        mList(&c)
{
    //Create the Tree model:
    m_refTreeModel = Gtk::ListStore::create(m_Columns);
    set_model(m_refTreeModel);
    //Add the TreeView's view columns:
    append_column("Name", m_Columns.m_col_name);

    {
        using namespace Gtk::Menu_Helpers;
        Gtk::Menu::MenuList* items = &mMenu.items();
        items->push_back(Gtk::Menu_Helpers::MenuElem("Insert",
                         sigc::mem_fun(*this, &TreeViewModelPort::on_menu_insert)));
        items->push_back(Gtk::Menu_Helpers::MenuElem("Remove",
                         sigc::mem_fun(*this, &TreeViewModelPort::on_menu_remove)));
    }

    makeTreeView();
}

bool TreeViewModelPort::on_button_press_event(GdkEventButton* event)
{
    if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3)) {
        mMenu.popup(event->button, event->time);
    }
    return TreeView::on_button_press_event(event);
}

void TreeViewModelPort::makeTreeView()
{
    using namespace graph;

    m_refTreeModel->clear();
    ConnectionList::const_iterator it = mList->begin();
    while (it != mList->end()) {
        Gtk::TreeModel::Row row = *(m_refTreeModel->append());
        row[m_Columns.m_col_name] = it->first;

        ++it;
    }
}

void TreeViewModelPort::on_menu_insert()
{
    SimpleTypeBox box("name ?");
    std::string name;
    do {
        name = box.run();
        boost::trim(name);
    } while (name == ""  || (mList->find(name) != mList->end()));
    mList->insert(std::make_pair< std::string, graph::ModelPortList >(
                      name, graph::ModelPortList()));
    makeTreeView();
}

void TreeViewModelPort::on_menu_remove()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    if (refSelection) {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            graph::ConnectionList::iterator it = mList->find(row.get_value(m_Columns.m_col_name));
            if (it != mList->end()) {
                mList->erase(it);
                makeTreeView();
            }
        }
    }
}

}
} // namespace vle gvle
