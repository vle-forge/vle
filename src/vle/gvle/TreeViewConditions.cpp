/**
 * @file vle/gvle/TreeViewConditions.cpp
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


#include <vle/gvle/TreeViewConditions.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>

using namespace vle;

namespace vle
{
namespace gvle {

TreeViewConditions::TreeViewConditions(vpz::Conditions& cond, const vpz::AtomicModel& atom):
        mCond(&cond),
        mAtom(&atom)
{
    //Create the Tree model:
    m_refTreeModel = Gtk::ListStore::create(m_Columns);
    set_model(m_refTreeModel);

    //Add the TreeView's view columns:
    append_column(_("Name"), m_Columns.m_col_name);
    append_column_editable(_("In"), m_Columns.m_col_activ);
    {
        using namespace Gtk::Menu_Helpers;
        Gtk::Menu::MenuList* items = &mMenu.items();
        items->push_back(Gtk::Menu_Helpers::MenuElem(_("Insert"),
                         sigc::mem_fun(*this, &TreeViewConditions::on_menu_insert)));
        items->push_back(Gtk::Menu_Helpers::MenuElem(_("Remove"),
                         sigc::mem_fun(*this, &TreeViewConditions::on_menu_remove)));
    }
}

TreeViewConditions::~TreeViewConditions()
{
}

void TreeViewConditions::on_row_activated(
    const Gtk::TreeModel::Path& /* path */,
    Gtk::TreeViewColumn* /*column*/)
{
}

bool TreeViewConditions::on_button_press_event(GdkEventButton* event)
{
    if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3)) {
        mMenu.popup(event->button, event->time);
    }
    return TreeView::on_button_press_event(event);
}

std::vector<std::string>* TreeViewConditions::get_conditions_checked()
{
    std::vector<std::string>* ret = new std::vector<std::string>();
    typedef Gtk::TreeModel::Children type_children;
    type_children children = m_refTreeModel->children();
    for (type_children::iterator iter = children.begin();
            iter != children.end(); ++iter) {
        Gtk::TreeModel::Row row = *iter;
        if (row.get_value(m_Columns.m_col_activ) == true) {
            ret->push_back(row.get_value(m_Columns.m_col_name));
        }
    }
    return ret;
}

void TreeViewConditions::makeTreeView()
{
    m_refTreeModel->clear();
    const vpz::Strings& cond = mAtom->conditions();
    vpz::Strings::const_iterator f;
    vpz::ConditionList list = mCond->conditionlist();
    vpz::ConditionList::iterator it = list.begin();

    while (it != list.end()) {
        Gtk::TreeModel::Row row = *(m_refTreeModel->append());
        row[m_Columns.m_col_name] = it->first;
        f = std::find(cond.begin(), cond.end(), it->first);
        if (f != cond.end())
            row[m_Columns.m_col_activ] = true;
        else
            row[m_Columns.m_col_activ] = false;

        it++;
    }
}

void TreeViewConditions::on_menu_insert()
{
    SimpleTypeBox box(_("Name of the port ?"), "");
    std::string name = boost::trim_copy(box.run());

    if (box.valid() and not name.empty()) {
        mCond->add(vpz::Condition(name));
        makeTreeView();
    }
}

void TreeViewConditions::on_menu_remove()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    if (refSelection) {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            mCond->del(row.get_value(m_Columns.m_col_name));
            makeTreeView();
        }
    }
}

}
} // namespace vle gvle

