/*
 * @file vle/gvle/TreeViewObservable.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/gvle/TreeViewObservable.hpp>
#include <boost/algorithm/string/trim.hpp>

using namespace vle;

namespace vle
{
namespace gvle {

TreeViewObservable::TreeViewObservable()
{
    m_refTreeModel = Gtk::TreeStore::create(m_Columns);
    set_model(m_refTreeModel);

    remove_all_columns();
    append_column(_("name"), m_Columns.m_col_name);
    show_all_children();
}

void TreeViewObservable::makeTreeView(vpz::Observable& obs)
{
    mObservable = &obs;

    m_refTreeModel->clear();

    const vpz::ObservablePortList& list = mObservable->observableportlist();
    vpz::ObservablePortList::const_iterator it_port = list.begin();
    while (it_port != list.end()) {
        Gtk::TreeModel::Row row = *(m_refTreeModel->append());
        row[m_Columns.m_col_name] = it_port->first;
        row[m_Columns.m_col_parent] = &(mObservable->get(it_port->first));

        const vpz::ViewNameList& viewlist = it_port->second.viewnamelist();
        vpz::ViewNameList::const_iterator it_view = viewlist.begin();
        while (it_view != viewlist.end()) {
            Gtk::TreeModel::Row childrow = *(m_refTreeModel->append(row.children()));
            childrow[m_Columns.m_col_name] = *it_view;
            childrow[m_Columns.m_col_parent] = &(mObservable->get(it_port->first));

            ++it_view;
        }

        ++it_port;
    }
    expand_all();
}

}} // namespace vle gvle
