/**
 * @file vle/gvle/ModelTreeBox.cpp
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


#include <vle/gvle/ModelTreeBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/graph/Model.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <queue>

namespace vle { namespace gvle {

ModelTreeBox::ModelTreeBox(BaseObjectType* cobject,
			   const Glib::RefPtr<Gnome::Glade::Xml>&) :
    Gtk::TreeView(cobject), m_delayTime(0)
{
    m_refTreeModel = Gtk::TreeStore::create(m_columns);
    set_model(m_refTreeModel);

    m_columnName = append_column_editable(_("Model"), m_columns.mName);

    m_cellRenderer = dynamic_cast<Gtk::CellRendererText*>(
	get_column_cell_renderer(m_columnName - 1));
    m_cellRenderer->property_editable() = true;
    m_cellRenderer->signal_editing_started().connect(
	sigc::mem_fun(*this,
		      &ModelTreeBox::onEditionStarted));
    m_cellRenderer->signal_edited().connect(
	sigc::mem_fun(*this, &ModelTreeBox::onEdition));

    signal_event().connect(sigc::mem_fun(*this, &ModelTreeBox::onExposeEvent));
    signal_button_release_event().connect(
	sigc::mem_fun(*this, &ModelTreeBox::onButtonRealeaseModels));

    expand_all();
    set_rules_hint(true);
    initMenuPopupModels();
}

void ModelTreeBox::initMenuPopupModels()
{
    Gtk::Menu::MenuList& menuList(m_menu.items());

    menuList.push_back(
        Gtk::Menu_Helpers::MenuElem(
            _("_Rename"), sigc::mem_fun(
		*this, &ModelTreeBox::onRenameModels)));
    m_menu.accelerate(*this);
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
    Glib::RefPtr < Gtk::TreeView::Selection > ref = get_selection();
    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            std::string oldname(row.get_value(m_modelsColumnRecord.name));
	    SimpleTypeBox box(_("New name of this model?"), oldname);
	    std::string newname = boost::trim_copy(box.run());
	    if (box.valid() and not newname.empty() and newname != m_oldName) {
		try {
		    row[m_modelsColumnRecord.name] = newname;
		    graph::Model::rename(row[m_columns.mModel], newname);
		    m_modeling->refreshViews();
		    m_modeling->setModified(true);
		} catch(utils::DevsGraphError dge) {
		    row[m_modelsColumnRecord.name] = oldname;
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
    expand_all();
}

void ModelTreeBox::showRow(const std::string& model_name)
{
    m_search.assign(model_name);
    m_refTreeModel->foreach(sigc::mem_fun(*this, &ModelTreeBox::on_foreach));
}

void ModelTreeBox::clear()
{
    m_refTreeModel->clear();
}

Gtk::TreeModel::Row
ModelTreeBox::addModel(graph::Model* model)
{
    Gtk::TreeModel::Row row = *(m_refTreeModel->append());
    row[m_columns.mName] = model->getName();
    row[m_columns.mModel] = model;
    return row;
}

Gtk::TreeModel::Row
ModelTreeBox::addSubModel(Gtk::TreeModel::Row tree, graph::Model* model)
{
    Gtk::TreeModel::Row row = *(m_refTreeModel->append(tree.children()));
    row[m_columns.mName] = model->getName();
    row[m_columns.mModel] = model;
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

bool ModelTreeBox::onExposeEvent(GdkEvent* event)
{
    if (event->type == GDK_2BUTTON_PRESS) {
	m_delayTime = event->button.time;
	Gtk::TreeModel::Path path;
	Gtk::TreeViewColumn* column;
	get_cursor(path, column);
	row_activated(path, column);
	return true;
    }
    if (event->type == GDK_BUTTON_PRESS) {
	if (m_delayTime + 250 < event->button.time and
	    m_delayTime + 1000 > event->button.time) {
	    m_delayTime = event->button.time;
	    m_cellRenderer->property_editable() = true;
	} else {
	    m_delayTime = event->button.time;
	    m_cellRenderer->property_editable() = false;
	}
    }
    return false;

}

void ModelTreeBox::row_activated(const Gtk::TreeModel::Path& path,
                                 Gtk::TreeViewColumn* column)
{
    if (column) {
        Gtk::TreeIter iter = m_refTreeModel->get_iter(path);
        Gtk::TreeRow row = (*iter);
        m_modeling->addView(row.get_value(m_columns.mModel));
    }
}

bool ModelTreeBox::on_foreach(const Gtk::TreeModel::Path&,
                              const Gtk::TreeModel::iterator& iter)
{
    if ((*iter).get_value(m_columns.mName) == m_search) {
        set_cursor(m_refTreeModel->get_path(iter));
        return true;
    }
    return false;
}

void ModelTreeBox::onEditionStarted(
    Gtk::CellEditable* cellEditable, const Glib::ustring& /* path */)
{
    Glib::RefPtr < Gtk::TreeView::Selection > ref = get_selection();
    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
	    m_oldName = (*iter)[m_modelsColumnRecord.name];
	}
    }

    if (m_validateRetry)
    {
	Gtk::CellEditable* celleditable_validated = cellEditable;
	Gtk::Entry* pEntry = dynamic_cast<Gtk::Entry*>(celleditable_validated);
	if(pEntry)
	{
	    pEntry->set_text(m_invalidTextForRetry);
	    m_validateRetry = false;
	    m_invalidTextForRetry.clear();
	}
    }
}

void ModelTreeBox::onEdition(
    const Glib::ustring& pathString,
    const Glib::ustring& newName)
{
    Gtk::TreePath path(pathString);
    Glib::RefPtr < Gtk::TreeView::Selection > ref = get_selection();
    if (ref) {
	Gtk::TreeModel::iterator iter = ref->get_selected();
	if (iter) {
	    Gtk::TreeModel::Row row = *iter;
	    if (not newName.empty() and newName != m_oldName) {
		try {
		    row[m_modelsColumnRecord.name] = newName;
		    graph::Model::rename(row[m_columns.mModel], newName);
		    m_modeling->refreshViews();
		    m_modeling->setModified(true);
		} catch(utils::DevsGraphError dge) {
		    row[m_modelsColumnRecord.name] = m_oldName;
		}
	    } else {
		row[m_modelsColumnRecord.name] = m_oldName;
	    }
	}
    }
}

} } // namespace vle gvle
