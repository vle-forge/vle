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


#include <vle/gvle/ModelTreeBox.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/gvle/Editor.hpp>
#include <vle/gvle/View.hpp>
#include <vle/vpz/BaseModel.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace vle { namespace gvle {

ModelTreeBox::ModelTreeBox(BaseObjectType* cobject,
			   const Glib::RefPtr < Gtk::Builder >&) :
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

    Glib::RefPtr<Gtk::TreeSelection> selection = Gtk::TreeView::get_selection();

    selection->set_select_function(
        sigc::mem_fun(*this, &ModelTreeBox::onSelect) );

    expand_all();
    set_rules_hint(true);
    initMenuPopupModels();

    set_has_tooltip();
    signal_query_tooltip().connect(
	sigc::mem_fun(*this, &ModelTreeBox::onQueryTooltip));
}

bool ModelTreeBox::onSelect(
    const Glib::RefPtr<Gtk::TreeModel>& model,
    const Gtk::TreeModel::Path& path, bool info)
{
    const Gtk::TreeModel::iterator iter = model->get_iter(path);

    View* view =
        m_gvle->getEditor()->getDocumentDrawingArea()->getView();

    vpz::BaseModel* mdl;

    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        mdl = (vpz::BaseModel*)row[m_columns.mModel];
        if (mdl) {
            if (info) {
                view->removeFromSelectedModel(mdl);
            } else {
                m_gvle->getModelClassBox()->selectNone();
                if (not view->existInSelectedModels(mdl)) {
                        if(mdl->getParent()) {
                            m_gvle->addView(mdl->getParent());
                        } else {
                            m_gvle->addView(mdl);
                        }
                        view = m_gvle->getEditor()->getDocumentDrawingArea()->getView();
                }
                view->clearSelectedModels();
                view->addModelToSelectedModels(mdl);
            }
            view->redraw();
        }
    }
    return true;
}

void ModelTreeBox::selectNone()
{
    Glib::RefPtr<Gtk::TreeSelection> selection = get_selection();
    selection->unselect_all();
}

void ModelTreeBox::initMenuPopupModels()
{
    mActionGroup = Gtk::ActionGroup::create("initMenuPopupModels");
    mActionGroup->add(Gtk::Action::create("ModelTB_ContextMenu", _("Context Menu")));
    
    mActionGroup->add(Gtk::Action::create("ModelTB_ContextRename", _("_Rename")),
        sigc::mem_fun(*this, &ModelTreeBox::onRenameModels));
    
    mUIManager = Gtk::UIManager::create();
    mUIManager->insert_action_group(mActionGroup);

    Glib::ustring ui_info =
        "<ui>"
        "  <popup name='ModelTB_Popup'>"
        "      <menuitem action='ModelTB_ContextRename'/>"
        "  </popup>"
        "</ui>";

    try {
        mUIManager->add_ui_from_string(ui_info);
        m_menu = (Gtk::Menu *) (
            mUIManager->get_widget("/ModelTB_Popup"));
    } catch(const Glib::Error& ex) {
        std::cerr << "building menus failed: ModelTB_Popup \n" <<  ex.what();
    }

    if (!m_menu)
        std::cerr << "menu not found : ModelTB_Popupu\n";
}

bool ModelTreeBox::onButtonRealeaseModels(GdkEventButton* event)
{
    if (event->button == 3) {
        m_menu->popup(event->button, event->time);
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
		    vpz::BaseModel::rename(row[m_columns.mModel], newname);
		    m_gvle->refreshViews();
                    m_gvle->setModified(true);
                } catch(utils::DevsGraphError dge) {
		    row[m_modelsColumnRecord.name] = oldname;
                }
            }
        }
    }
}

void ModelTreeBox::parseModel(vpz::BaseModel* top)
{
    assert(top);

    m_refTreeModel->clear();

    Gtk::TreeModel::Row row = addModel(top);
    const vpz::CoupledModel* coupled = (vpz::CoupledModel*)(top);

    const vpz::ModelList& list = coupled->getModelList();
    vpz::ModelList::const_iterator it = list.begin();
    while (it != list.end()) {
        Gtk::TreeModel::Row row2 = addSubModel(row, it->second);
        if (it->second->isCoupled()) {
           parseModel(row2, (vpz::CoupledModel*)it->second);
        }
        it++;
    }
    expand_all();
}

Gtk::TreeModel::iterator ModelTreeBox::getModelRow(const vpz::BaseModel* mdl,
                                                   Gtk::TreeModel::Children child)
{
    Gtk::TreeModel::iterator iter = child.begin();

    while (iter != child.end()) {
        if ((*iter).get_value(m_columns.mModel) == mdl) {
            return iter;
        }
        if (!((*iter).children().empty())) {
            Gtk::TreeModel::iterator iterin = getModelRow(mdl, (*iter).children());
            if (iterin != (*iter).children().end()) {
                return iterin;
            }
        }
        iter++;
    }
    return iter;
}

void ModelTreeBox::showRow(const vpz::BaseModel* mdl)
{
    if (mdl->getParent()) {
        Gtk::TreeModel::iterator iter = getModelRow(mdl->getParent(),
                                                    m_refTreeModel->children());
        expand_to_path(Gtk::TreePath(iter));
    }

    Gtk::TreeModel::iterator iter = getModelRow(mdl, m_refTreeModel->children());
    set_cursor(Gtk::TreePath(iter));
}

void ModelTreeBox::clear()
{
    m_refTreeModel->clear();
}

bool ModelTreeBox::onQueryTooltip(int wx,int wy, bool keyboard_tooltip,
                                  const Glib::RefPtr<Gtk::Tooltip>& tooltip)
{
    Gtk::TreeModel::iterator iter;
    Glib::ustring card;

    if (get_tooltip_context_iter(wx, wy, keyboard_tooltip, iter)) {
        vpz::BaseModel* mModel = (*iter).get_value(m_columns.mModel);
        card = m_modeling->getIdCard(mModel);
        tooltip->set_markup(card);
        set_tooltip_row(tooltip, Gtk::TreePath(iter));
        return true;
    } else {
        return false;
    }
}

Gtk::TreeModel::Row
ModelTreeBox::addModel(vpz::BaseModel* model)
{
    Gtk::TreeModel::Row row = *(m_refTreeModel->append());
    row[m_columns.mName] = model->getName();
    row[m_columns.mModel] = model;
    return row;
}

Gtk::TreeModel::Row
ModelTreeBox::addSubModel(Gtk::TreeModel::Row tree, vpz::BaseModel* model)
{
    Gtk::TreeModel::Row row = *(m_refTreeModel->append(tree.children()));
    row[m_columns.mName] = model->getName();
    row[m_columns.mModel] = model;
    return row;
}

void ModelTreeBox::parseModel(Gtk::TreeModel::Row row,
                              const vpz::CoupledModel* top)
{
    const vpz::ModelList& list = top->getModelList();
    vpz::ModelList::const_iterator it = list.begin();

    while (it != list.end()) {
        Gtk::TreeModel::Row row2 = addSubModel(row, it->second);
        if (it->second->isCoupled() == true) {
            parseModel(row2, (vpz::CoupledModel*)it->second);
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
        m_gvle->addView(row.get_value(m_columns.mModel));
    }
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
		    vpz::BaseModel::rename(row[m_columns.mModel], newName);
		    m_gvle->refreshViews();
                    m_gvle->setModified(true);
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
