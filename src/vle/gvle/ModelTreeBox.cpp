/**
 * @file vle/gvle/ModelTreeBox.cpp
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


#include <vle/gvle/ModelTreeBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/graph/Model.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <queue>

namespace vle
{
namespace gvle {

ModelTreeBox::ModelTreeBox(Modeling* m) :
        m_modeling(m)
{
    assert(m);

    set_title("Models tree");
    set_border_width(5);
    set_default_size(200, 350);

    m_ScrolledWindow.add(m_TreeView);
    m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    m_refTreeModel = Gtk::TreeStore::create(m_Columns);
    m_TreeView.set_model(m_refTreeModel);

    m_TreeView.append_column("Name", m_Columns.mName);

    m_TreeView.signal_row_activated().connect(
        sigc::mem_fun(*this, &ModelTreeBox::row_activated));
    m_TreeView.signal_button_release_event().connect(
        sigc::mem_fun(*this, &ModelTreeBox::onButtonRealeaseModels));

    m_TreeView.expand_all();
    m_TreeView.set_rules_hint(true);
    add(m_ScrolledWindow);
    initMenuPopupModels();
    show_all();
}

void ModelTreeBox::initMenuPopupModels()
{
    Gtk::Menu::MenuList& menulist(m_menu.items());

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(
            "_Rename", sigc::mem_fun(
		*this, &ModelTreeBox::onRenameModels)));
    m_menu.accelerate(m_TreeView);
}

bool ModelTreeBox::onButtonRealeaseModels(GdkEventButton* event)
{
    if (event->button == 3) {
        m_menu.popup(event->button, event->time);
    }
    return true;
}

void ModelTreeBox::onRenameModels()
{
    Glib::RefPtr < Gtk::TreeView::Selection > ref = m_TreeView.get_selection();
    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            std::string oldname(row.get_value(m_modelscolumnrecord.name));
	    SimpleTypeBox box("New name of this model?");
	    std::string newname = boost::trim_copy(box.run());
	    if (box.valid() and not newname.empty()) {
		try {
		    row[m_modelscolumnrecord.name] = newname;
		    graph::Model::rename(row[m_Columns.mModel], newname);
		} catch(utils::DevsGraphError dge) {
		    row[m_modelscolumnrecord.name] = oldname;
		}
	    }
	}
    }
}

void ModelTreeBox::parseModel(graph::Model* top)
{
    assert(top);

    m_refTreeModel->clear();

    Gtk::TreeModel::Row row = addModel(top);
    const graph::CoupledModel* coupled = (graph::CoupledModel*)(top);

    const graph::ModelList& list = coupled->getModelList();
    graph::ModelList::const_iterator it = list.begin();
    while (it != list.end()) {
        Gtk::TreeModel::Row row2 = addSubModel(row, it->second);
        if (it->second->isCoupled()) {
            parseModel(row2, (graph::CoupledModel*)it->second);
        }
        it++;
    }
    m_TreeView.expand_all();
}

void ModelTreeBox::showRow(const std::string& model_name)
{
    m_search.assign(model_name);
    m_refTreeModel->foreach(sigc::mem_fun(*this, &ModelTreeBox::on_foreach));
}


bool ModelTreeBox::on_key_release_event(GdkEventKey* event)
{
    if (((event->state & GDK_CONTROL_MASK) and event->keyval == GDK_w) or
            (event->keyval == GDK_Escape)) {
        m_modeling->hideModelTreeBox();
    }
    return true;
}

Gtk::TreeModel::Row
ModelTreeBox::addModel(graph::Model* model)
{
    Gtk::TreeModel::Row row = *(m_refTreeModel->append());
    row[m_Columns.mName] = model->getName();
    row[m_Columns.mModel] = model;
    return row;
}

Gtk::TreeModel::Row
ModelTreeBox::addSubModel(Gtk::TreeModel::Row tree, graph::Model* model)
{
    Gtk::TreeModel::Row row = *(m_refTreeModel->append(tree.children()));
    row[m_Columns.mName] = model->getName();
    row[m_Columns.mModel] = model;
    return row;
}

void ModelTreeBox::parseModel(Gtk::TreeModel::Row row,
                              const graph::CoupledModel* top)
{
    const graph::ModelList& list = top->getModelList();
    graph::ModelList::const_iterator it = list.begin();

    while (it != list.end()) {
        Gtk::TreeModel::Row row2 = addSubModel(row, it->second);
        if (it->second->isCoupled() == true) {
            parseModel(row2, (graph::CoupledModel*)it->second);
        }
        ++it;
    }
}

void ModelTreeBox::row_activated(const Gtk::TreeModel::Path& path,
                                 Gtk::TreeViewColumn* column)
{
    if (column) {
        Gtk::TreeIter iter = m_refTreeModel->get_iter(path);
        Gtk::TreeRow row = (*iter);
        m_modeling->addView(row.get_value(m_Columns.mModel));
    }
}

bool ModelTreeBox::on_foreach(const Gtk::TreeModel::Path&,
                              const Gtk::TreeModel::iterator& iter)
{
    if ((*iter).get_value(m_Columns.mName) == m_search) {
        m_TreeView.set_cursor(m_refTreeModel->get_path(iter));
        return true;
    }
    return false;
}

}
} // namespace vle gvle
