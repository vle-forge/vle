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


#include <vle/gvle/AtomicModelBox.hpp>
#include <vle/gvle/GVLE.hpp>
#include <vle/utils/Package.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/gvle/OpenModelingPluginBox.hpp>
#include <vle/gvle/NewDynamicsBox.hpp>
#include <vle/gvle/InteractiveTypeBox.hpp>
#include <vle/gvle/ViewDrawingArea.hpp>
#include <vle/gvle/Editor.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Template.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <fstream>
#include <glibmm.h>

namespace vle { namespace gvle {

// InputPortTreeView

AtomicModelBox::InputPortTreeView::InputPortTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr < Gtk::Builder >& /*refGlade*/) :
    Gtk::TreeView(cobject),
    mMenuPopup(0),
    mValidateRetry(false)
{
    mRefTreeModelInputPort = Gtk::ListStore::create(mColumnsInputPort);
    set_model(mRefTreeModelInputPort);

    mColumnName = append_column_editable(_("Name"),
                                         mColumnsInputPort.m_col_name);
    Gtk::TreeViewColumn* nameCol = get_column(mColumnName - 1);
    nameCol->set_sort_column(mColumnsInputPort.m_col_name);

    mCellrendererValidated = dynamic_cast<Gtk::CellRendererText*>(
        get_column_cell_renderer(mColumnName - 1));
    mCellrendererValidated->property_editable() = true;
    mCellrendererValidated->signal_editing_started().connect(
        sigc::mem_fun(*this,
                      &AtomicModelBox::InputPortTreeView::
                      onEditionStarted) );

    mCellrendererValidated->signal_edited().connect(
        sigc::mem_fun(*this,
                      &AtomicModelBox::InputPortTreeView::
                      onEdition) );

    mPopupActionGroup = Gtk::ActionGroup::create("InputPortTreeView");
    mPopupActionGroup->add(Gtk::Action::create("IPTV_ContextMenu", _("Context Menu")));
    
    mPopupActionGroup->add(Gtk::Action::create("IPTV_ContextAdd", _("_Add")),
        sigc::mem_fun(*this, &AtomicModelBox::InputPortTreeView::onAdd));
    
    mPopupActionGroup->add(Gtk::Action::create("IPTV_ContextRemove", _("_Remove")),
        sigc::mem_fun(*this, &AtomicModelBox::InputPortTreeView::onRemove));
    
    mPopupActionGroup->add(Gtk::Action::create("IPTV_ContextRename", _("_Rename")),
        sigc::mem_fun(*this, &AtomicModelBox::InputPortTreeView::onRename));

    mPopupUIManager = Gtk::UIManager::create();
    mPopupUIManager->insert_action_group(mPopupActionGroup);

    Glib::ustring ui_info =
        "<ui>"
        "  <popup name='IPTView_Popup'>"
        "      <menuitem action='IPTV_ContextAdd'/>"
        "      <menuitem action='IPTV_ContextRemove'/>"
        "      <menuitem action='IPTV_ContextRename'/>"
        "  </popup>"
        "</ui>";

    try {
        mPopupUIManager->add_ui_from_string(ui_info);
        mMenuPopup = (Gtk::Menu *) (
            mPopupUIManager->get_widget("/IPTView_Popup"));
    } catch(const Glib::Error& ex) {
        std::cerr << "building menus failed: IPTView_Popup " <<  ex.what();
    }

    if (!mMenuPopup)
        std::cerr << "menu not found : IPTView_Popup\n";
}

AtomicModelBox::InputPortTreeView::~InputPortTreeView()
{
}

bool AtomicModelBox::InputPortTreeView::on_button_press_event(
    GdkEventButton* event)
{
  //Call base class, to allow normal handling,
  //such as allowing the row to be selected by the right-click:
  bool return_value = TreeView::on_button_press_event(event);

  //Then do our custom stuff:
  if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3)) {
      if (mMenuPopup)
          mMenuPopup->popup(event->button, event->time);
  }

  return return_value;
}

void AtomicModelBox::InputPortTreeView::applyRenaming(vpz::AtomicModel* model)
{
    renameList::const_iterator it = mRenameList.begin();

    while (it != mRenameList.end()) {
        if (model->existInputPort(it->first)) {
            model->renameInputPort(it->first, it->second);
        }
	++it;
    }
    mRenameList.clear();
}

void AtomicModelBox::InputPortTreeView::build()
{
    assert(mModel);

    mRefTreeModelInputPort->clear();

    vpz::ConnectionList list = mModel->getInputPortList();
    vpz::ConnectionList::const_iterator it = list.begin();
    while (it != list.end()) {
        Gtk::TreeModel::Row row = *(mRefTreeModelInputPort->append());
        row[mColumnsInputPort.m_col_name] = it->first;

        ++it;
    }
}

void AtomicModelBox::InputPortTreeView::onAdd()
{
    SimpleTypeBox box(_("Name of the Input port ?"), "");
    vpz::ConnectionList& list = mModel->getInputPortList();
    std::string name = boost::trim_copy(box.run());

    if (box.valid() and not name.empty() and list.find(name) == list.end()) {
        mModel->addInputPort(name);
	build();
    }
}

void AtomicModelBox::InputPortTreeView::onRemove()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();

    if (refSelection) {
	Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
            Gtk::TreeModel::Row row = *iter;
	    std::string name = row.get_value(mColumnsInputPort.m_col_name);

            if (mModel->existInputPort(name)) {
                mModel->delInputPort(name);
            }
            build();
        }
    }
}

void AtomicModelBox::InputPortTreeView::onRename()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();

    if (refSelection) {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
            Gtk::TreeModel::Row row = *iter;
	    std::string old_name = row.get_value(mColumnsInputPort.m_col_name);
	    SimpleTypeBox box(_("New name of the input port ?"), old_name);
	    vpz::ConnectionList& list = mModel->getInputPortList();
	    Glib::ustring new_name = boost::trim_copy(box.run());

	    if (box.valid() and not new_name.empty()
		and list.find(new_name) == list.end()) {

		row.set_value(mColumnsInputPort.m_col_name, new_name);

		if (mModel->existInputPort(old_name)) {
		    mModel->renameInputPort(old_name, new_name);
		    mRenameList.push_back(std::make_pair(old_name, new_name));
		    build();
		}
	    }
	}
    }
}

void AtomicModelBox::InputPortTreeView::onEditionStarted(
    Gtk::CellEditable* cell_editable, const Glib::ustring& /* path */)
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Gtk::TreeModel::iterator iter = refSelection->get_selected();

    if (iter) {
	Gtk::TreeModel::Row row = *iter;
	mOldName = row.get_value(mColumnsInputPort.m_col_name);
    }

    if(mValidateRetry) {
	Gtk::CellEditable* celleditable_validated = cell_editable;
	Gtk::Entry* pEntry = dynamic_cast<Gtk::Entry*>(celleditable_validated);
	if(pEntry) {
	    pEntry->set_text(mInvalidTextForRetry);
	    mValidateRetry = false;
	    mInvalidTextForRetry.clear();
	}
    }
}

void AtomicModelBox::InputPortTreeView::onEdition(
        const Glib::ustring& pathString,
        const Glib::ustring& newName)
{
    Gtk::TreePath path(pathString);

    vpz::ConnectionList& list = mModel->getInputPortList();

    if (list.find(newName) == list.end() and newName != "") {
	Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
	Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
	    Gtk::TreeModel::Row row = *iter;
	    row.set_value(mColumnsInputPort.m_col_name, newName);

	    if (mModel->existInputPort(mOldName)) {
		mModel->renameInputPort(mOldName, newName);
		mRenameList.push_back(std::make_pair(mOldName, newName));
		build();
	    }
	    mValidateRetry = true;
	}
    }
}

// OutputPortTreeView

AtomicModelBox::OutputPortTreeView::OutputPortTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr < Gtk::Builder >& /*refGlade*/) :
    Gtk::TreeView(cobject),
    mMenuPopup(0),
    mValidateRetry(false)
{
    mRefTreeModelOutputPort = Gtk::ListStore::create(mColumnsOutputPort);
    set_model(mRefTreeModelOutputPort);

    mColumnName = append_column_editable(_("Name"),
				     mColumnsOutputPort.m_col_name);
    Gtk::TreeViewColumn* nameCol = get_column(mColumnName - 1);
    nameCol->set_sort_column(mColumnsOutputPort.m_col_name);

    mCellrendererValidated = dynamic_cast<Gtk::CellRendererText*>(
	get_column_cell_renderer(mColumnName - 1));
    mCellrendererValidated->property_editable() = true;
    mCellrendererValidated->signal_editing_started().connect(
	sigc::mem_fun(*this,
		      &AtomicModelBox::OutputPortTreeView::
		      onEditionStarted) );

    mCellrendererValidated->signal_edited().connect(
	sigc::mem_fun(*this,
		      &AtomicModelBox::OutputPortTreeView::
		      onEdition) );

    mPopupActionGroup = Gtk::ActionGroup::create("OutputPortTreeView");
    mPopupActionGroup->add(Gtk::Action::create("OPTV_ContextMenu", _("Context Menu")));
    mPopupActionGroup->add(
        Gtk::Action::create("OPTV_ContextAdd", _("Add")),
        sigc::mem_fun(*this, &AtomicModelBox::OutputPortTreeView::onAdd));
    mPopupActionGroup->add(
        Gtk::Action::create("OPTV_ContextRemove", _("Remove")),
        sigc::mem_fun(*this, &AtomicModelBox::OutputPortTreeView::onRemove));
    mPopupActionGroup->add(
        Gtk::Action::create("OPTV_ContextRename", _("Rename")),
        sigc::mem_fun(*this, &AtomicModelBox::OutputPortTreeView::onRename));

    mPopupUIManager = Gtk::UIManager::create();
    mPopupUIManager->insert_action_group(mPopupActionGroup);

    Glib::ustring ui_info =
        "<ui>"
        "  <popup name='OPTView_Popup'>"
        "      <menuitem action='OPTV_ContextAdd'/>"
        "      <menuitem action='OPTV_ContextRemove'/>"
        "      <menuitem action='OPTV_ContextRename'/>"
        "  </popup>"
        "</ui>";

    try {
        mPopupUIManager->add_ui_from_string(ui_info);
        mMenuPopup = (Gtk::Menu *) (
            mPopupUIManager->get_widget("/OPTView_Popup"));
    } catch(const Glib::Error& ex) {
        std::cerr << "building menus failed: OPTView_Popup " <<  ex.what();
    }

    if (!mMenuPopup)
        std::cerr << "menu not found : OPTView_Popup\n";
}

AtomicModelBox::OutputPortTreeView::~OutputPortTreeView()
{
}

bool AtomicModelBox::OutputPortTreeView::on_button_press_event(
    GdkEventButton* event)
{
  //Call base class, to allow normal handling,
  //such as allowing the row to be selected by the right-click:
  bool return_value = TreeView::on_button_press_event(event);

  //Then do our custom stuff:
  if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3)) {
      if (mMenuPopup)
          mMenuPopup->popup(event->button, event->time);
  }

  return return_value;
}

void AtomicModelBox::OutputPortTreeView::applyRenaming(
    vpz::AtomicModel* model)
{
    renameList::const_iterator it = mRenameList.begin();

    while (it != mRenameList.end()) {
        if (model->existOutputPort(it->first)) {
            model->renameOutputPort(it->first, it->second);
        }
	++it;
    }
    mRenameList.clear();
}

void AtomicModelBox::OutputPortTreeView::build()
{
    assert(mModel);

    mRefTreeModelOutputPort->clear();

    vpz::ConnectionList list = mModel->getOutputPortList();
    vpz::ConnectionList::const_iterator it = list.begin();
    while (it != list.end()) {
        Gtk::TreeModel::Row row = *(mRefTreeModelOutputPort->append());
        row[mColumnsOutputPort.m_col_name] = it->first;

        ++it;
    }
}

void AtomicModelBox::OutputPortTreeView::onAdd()
{
    SimpleTypeBox box(_("Name of the output port ?"), "");
    vpz::ConnectionList& list = mModel->getOutputPortList();
    std::string name = boost::trim_copy(box.run());

    if (box.valid() and not name.empty() and list.find(name) == list.end()) {
        mModel->addOutputPort(name);
        build();
    }
}

void AtomicModelBox::OutputPortTreeView::onRemove()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();

    if (refSelection) {
	Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
            Gtk::TreeModel::Row row = *iter;
	    std::string name = row.get_value(mColumnsOutputPort.m_col_name);

            if (mModel->existOutputPort(name)) {
                mModel->delOutputPort(name);
            }
            build();
        }
    }
}

void AtomicModelBox::OutputPortTreeView::onRename()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();

    if (refSelection) {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
            Gtk::TreeModel::Row row = *iter;
	    std::string old_name = row.get_value(mColumnsOutputPort.m_col_name);
	    SimpleTypeBox box(_("New name of the output port ?"), old_name);
	    vpz::ConnectionList& list = mModel->getOutputPortList();
	    Glib::ustring new_name = boost::trim_copy(box.run());

	    if (box.valid() and not new_name.empty()
		and list.find(new_name) == list.end()) {
		row.set_value(mColumnsOutputPort.m_col_name, new_name);

		if (mModel->existOutputPort(old_name)) {
		    mModel->renameOutputPort(old_name, new_name);
		    mRenameList.push_back(std::make_pair(old_name, new_name));
		}
		build();
	    }
	}
    }
}

void AtomicModelBox::OutputPortTreeView::onEditionStarted(
    Gtk::CellEditable* cell_editable, const Glib::ustring& /* path */ )
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Gtk::TreeModel::iterator iter = refSelection->get_selected();

    if (iter) {
	Gtk::TreeModel::Row row = *iter;
	mOldName = row.get_value(mColumnsOutputPort.m_col_name);
    }

    if(mValidateRetry) {
	Gtk::CellEditable* celleditable_validated = cell_editable;
	Gtk::Entry* pEntry = dynamic_cast<Gtk::Entry*>(celleditable_validated);
	if(pEntry) {
	    pEntry->set_text(mInvalidTextForRetry);
	    mValidateRetry = false;
	    mInvalidTextForRetry.clear();
	}
    }
}

void AtomicModelBox::OutputPortTreeView::onEdition(
        const Glib::ustring& pathString,
        const Glib::ustring& newName)
{
    Gtk::TreePath path(pathString);

    vpz::ConnectionList& list = mModel->getOutputPortList();

    if (list.find(newName) == list.end() and newName != "") {
	Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
	Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
	    Gtk::TreeModel::Row row = *iter;
	    row.set_value(mColumnsOutputPort.m_col_name, newName);

	    if (mModel->existOutputPort(mOldName)) {
		mModel->renameOutputPort(mOldName, newName);
		mRenameList.push_back(std::make_pair(mOldName, newName));
		build();
	    }
	    mValidateRetry = true;
	}
    }
}


// ConditionTreeView

AtomicModelBox::ConditionTreeView::ConditionTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr < Gtk::Builder >& /*refGlade*/) :
    Gtk::TreeView(cobject),
    mMenuPopup(0)
{
    mRefTreeModel = Gtk::ListStore::create(mColumns);
    set_model(mRefTreeModel);
    append_column_editable(_("In"), mColumns.m_col_activ);

    mColumnName = append_column_editable(_("Name"), mColumns.m_col_name);
    Gtk::TreeViewColumn* nameCol = get_column(mColumnName - 1);
    nameCol->set_sort_column(mColumns.m_col_name);

    mCellrendererValidated = dynamic_cast<Gtk::CellRendererText*>(
	get_column_cell_renderer(mColumnName - 1));
    mCellrendererValidated->property_editable() = true;
    mCellrendererValidated->signal_editing_started().connect(
	sigc::mem_fun(*this,
		      &AtomicModelBox::ConditionTreeView::
		      onEditionStarted) );

    set_has_tooltip();
    signal_query_tooltip().connect(
        sigc::mem_fun(*this, &AtomicModelBox::ConditionTreeView::onQueryTooltip));

    mCellrendererValidated->signal_edited().connect(
	sigc::mem_fun(*this,
		      &AtomicModelBox::ConditionTreeView::
		      onEdition) );

    mPopupActionGroup = Gtk::ActionGroup::create("ConditionTreeView");
    mPopupActionGroup->add(Gtk::Action::create("CondTV_ContextMenu", _("Context Menu")));
    mPopupActionGroup->add(
        Gtk::Action::create("CondTV_ContextRename", _("_Rename")),
        sigc::mem_fun(*this, &AtomicModelBox::ConditionTreeView::onRename));
    mPopupActionGroup->add(
        Gtk::Action::create("CondTV_ContextEditor", _("Conditions Editor")),
        sigc::mem_fun(*this,
                      &AtomicModelBox::ConditionTreeView::onConditionsEditor));

    mPopupUIManager = Gtk::UIManager::create();
    mPopupUIManager->insert_action_group(mPopupActionGroup);

    Glib::ustring ui_info =
        "<ui>"
        "  <popup name='CondTV_Popup'>"
        "      <menuitem action='CondTV_ContextRename'/>"
        "      <menuitem action='CondTV_ContextEditor'/>"
        "  </popup>"
        "</ui>";

    try {
        mPopupUIManager->add_ui_from_string(ui_info);
        mMenuPopup = (Gtk::Menu *) (
            mPopupUIManager->get_widget("/CondTV_Popup"));
    } catch(const Glib::Error& ex) {
        std::cerr << "building menus failed: CondTV_Popup " <<  ex.what();
    }

    if (!mMenuPopup)
        std::cerr << "menu not found : CondTV_Popup\n";
}

AtomicModelBox::ConditionTreeView::~ConditionTreeView()
{
}

void AtomicModelBox::ConditionTreeView::applyRenaming()
{
    renameList::const_iterator it = mRenameList.begin();

    while (it != mRenameList.end()) {
        mModeling->vpz().project().model().updateConditions(it->first,
                                                            it->second);
        mModeling->vpz().project().classes().updateConditions(it->first,
                                                              it->second);
        ++it;
    }

    mRenameList.clear();
}

void AtomicModelBox::ConditionTreeView::build()
{
    mRefTreeModel->clear();

    const std::vector < std::string >& cond = mAtom->conditions();
    const vpz::ConditionList& list = mConditions->conditionlist();
    vpz::ConditionList::const_iterator it = list.begin();
    std::string selections("");

    while (it != list.end()) {
        Gtk::TreeModel::Row row = *(mRefTreeModel->append());

        row[mColumns.m_col_name] = it->first;
        std::vector < std::string >::const_iterator f =
            std::find(cond.begin(), cond.end(), it->first);
        if (f != cond.end()) {
            row[mColumns.m_col_activ] = true;
            selections += (it->first + " ");
        } else {
            row[mColumns.m_col_activ] = false;
        }
        it++;
    }
    mLabel->set_label(_("Selected conditions: ") + selections);
}

std::vector < std::string >
AtomicModelBox::ConditionTreeView::getConditions()
{
    std::vector < std::string > vec;

    typedef Gtk::TreeModel::Children type_children;
    type_children children = mRefTreeModel->children();

    for (type_children::iterator iter = children.begin();
	 iter != children.end(); ++iter) {
	Gtk::TreeModel::Row row = *iter;

	if (row.get_value(mColumns.m_col_activ)) {
	    vec.push_back(row.get_value(mColumns.m_col_name));
	}
    }
    return vec;
}

void AtomicModelBox::ConditionTreeView::on_row_activated(
    const Gtk::TreeModel::Path&, Gtk::TreeViewColumn* )
{
    if (mGVLE->runConditionsBox(*mConditions) == 1) {
        renameList tmpRename = mGVLE->applyConditionsBox(*mConditions);

        std::vector < std::string > selected = getConditions();
        renameList::const_iterator it = tmpRename.begin();
        while (it != tmpRename.end()) {
            mRenameList.push_back(*it);
            std::vector < std::string >::const_iterator its =
                std::find(selected.begin(), selected.end(), it->first);

            if(its != selected.end()) {
                mAtom->delCondition(it->first);
                mAtom->addCondition(it->second);
            }
            ++it;
        }
	build();
    }
}



bool AtomicModelBox::ConditionTreeView::on_button_press_event(
    GdkEventButton* event)
{
    bool return_value = TreeView::on_button_press_event(event);
    if (event->type == GDK_BUTTON_PRESS and event->button == 3) {
        if (mMenuPopup)
            mMenuPopup->popup(event->button, event->time);
    }
    if (event->type == GDK_2BUTTON_PRESS && event->button == 1) {
        onConditionsEditor();
    }
    return return_value;
}

void AtomicModelBox::ConditionTreeView::onRename()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Gtk::TreeModel::iterator it = refSelection->get_selected();

    if (it) {
	Glib::ustring oldname = (*it)[mColumns.m_col_name];
	SimpleTypeBox box(_("New name of the condition ?"), oldname);
        std::string newname = boost::trim_copy(box.run());
	const vpz::ConditionList& list = mConditions->conditionlist();

	if (box.valid() and not newname.empty()
	   and list.find(newname) == list.end()) {
	    mConditions->rename(oldname, newname);
	    mAtom->delCondition(oldname);
	    mAtom->addCondition(newname);
	    mRenameList.push_back(std::make_pair(oldname, newname));
	    build();
	}
    }
}

void AtomicModelBox::ConditionTreeView::onConditionsEditor()
{
    if (mGVLE->runConditionsBox(*mConditions) == 1) {
        renameList tmpRename = mGVLE->applyConditionsBox(*mConditions);

        std::vector < std::string > selected = getConditions();

        renameList::const_iterator it = tmpRename.begin();
        while (it != tmpRename.end()) {
            mRenameList.push_back(*it);
            std::vector < std::string >::const_iterator its =
                std::find(selected.begin(), selected.end(), it->first);

            if(its != selected.end()) {
                mAtom->delCondition(it->first);
                mAtom->addCondition(it->second);
            }
            ++it;
        }
        build();
    }
}

void AtomicModelBox::ConditionTreeView::onEditionStarted(
    Gtk::CellEditable* cell_editable, const Glib::ustring& /* path */)
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Gtk::TreeModel::iterator iter = refSelection->get_selected();

    if (iter) {
	Gtk::TreeModel::Row row = *iter;
	mOldName = row.get_value(mColumns.m_col_name);
    }

    if(mValidateRetry) {
	Gtk::CellEditable* celleditable_validated = cell_editable;
	Gtk::Entry* pEntry = dynamic_cast<Gtk::Entry*>(celleditable_validated);
	if(pEntry) {
	    pEntry->set_text(mInvalidTextForRetry);
	    mValidateRetry = false;
	    mInvalidTextForRetry.clear();
	}
    }
}

void AtomicModelBox::ConditionTreeView::onEdition(
    const Glib::ustring& /*pathString*/,
    const Glib::ustring& newName)
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Gtk::TreeModel::iterator it = refSelection->get_selected();

    if (it) {
	const vpz::ConditionList& list = mConditions->conditionlist();

	if (not newName.empty() and list.find(newName) == list.end()) {
	    mConditions->rename(mOldName, newName);
	    mAtom->setConditions(getConditions());
	    mRenameList.push_back(std::make_pair(mOldName, newName));
	    build();
	}
    }
}

bool AtomicModelBox::ConditionTreeView::onQueryTooltip(
    int wx,int wy, bool keyboard_tooltip,
    const Glib::RefPtr<Gtk::Tooltip>& tooltip)
{
    Gtk::TreeModel::iterator iter;
    Glib::ustring card;

    if (get_tooltip_context_iter(wx, wy, keyboard_tooltip, iter)) {
        Glib::ustring cond = (*iter).get_value(mColumns.m_col_name);
        card = mModeling->getInfoCard(cond);
        tooltip->set_markup(card);
        set_tooltip_row(tooltip, Gtk::TreePath(iter));
        return true;
    } else {
        return false;
    }
}

//DynamicTreeView

AtomicModelBox::DynamicTreeView::DynamicTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr < Gtk::Builder >& refGlade)
    : Gtk::TreeView(cobject), mXml(refGlade)
{
    mRefListDyn = Gtk::ListStore::create(mColumnsDyn);
    set_model(mRefListDyn);

    mColumnSel = append_column_editable(_(" "), mColumnsDyn.m_sel);
    Gtk::CellRendererToggle* cellRenderer
        = dynamic_cast<Gtk::CellRendererToggle*>(
            get_column_cell_renderer(mColumnSel - 1));
    cellRenderer->set_radio();
    cellRenderer->signal_toggled().connect(
        sigc::mem_fun(*this,
                      &AtomicModelBox::DynamicTreeView::
                      onToggled));

    mColumnName = append_column_editable(_("Name"), mColumnsDyn.m_col_name);
    Gtk::TreeViewColumn* nameCol = get_column(mColumnName - 1);
    nameCol->set_sort_column(mColumnsDyn.m_col_name);

    mCellRenderer = dynamic_cast<Gtk::CellRendererText*>(
	get_column_cell_renderer(mColumnName - 1));
    mCellRenderer->property_editable() = true;
    mCellRenderer->signal_editing_started().connect(
	sigc::mem_fun(*this,
		      &AtomicModelBox::DynamicTreeView::onEditionStarted));
    mCellRenderer->signal_edited().connect(
	sigc::mem_fun(*this,
		      &AtomicModelBox::DynamicTreeView::onEdition));

    mColumnPackage = append_column(_("Package"), mColumnsDyn.m_package);
    Gtk::TreeViewColumn* packageCol = get_column(mColumnPackage - 1);
    packageCol->set_sort_column(mColumnsDyn.m_package);

    mColumnDyn = append_column(_("Library"), mColumnsDyn.m_dyn);
    Gtk::TreeViewColumn* dynCol = get_column(mColumnDyn - 1);
    dynCol->set_sort_column(mColumnsDyn.m_dyn);

    mPopupActionGroup = Gtk::ActionGroup::create();
    mPopupActionGroup->add(Gtk::Action::create("DynTV_ContextMenu", _("Context Menu")));
    mPopupActionGroup->add(
        Gtk::Action::create("DynTV_ContextAdd", _("_Add")),
        sigc::mem_fun(*this, &AtomicModelBox::DynamicTreeView::onAdd));
    mPopupActionGroup->add(
        Gtk::Action::create("DynTV_ContextEdit", _("_Edit")),
        sigc::mem_fun(
            *this, &AtomicModelBox::DynamicTreeView::onEdit));
    mPopupActionGroup->add(
        Gtk::Action::create("DynTV_ContextNewLibrary", _("_New")),
        sigc::mem_fun(
            *this, &AtomicModelBox::DynamicTreeView::onNewLibrary));
    mPopupActionGroup->add(
        Gtk::Action::create("DynTV_ContextRemove", _("_Remove")),
        sigc::mem_fun(
            *this, &AtomicModelBox::DynamicTreeView::onRemove));
    mPopupActionGroup->add(
        Gtk::Action::create("DynTV_ContextRename", _("_Rename")),
        sigc::mem_fun(
            *this, &AtomicModelBox::DynamicTreeView::onRename));

    mPopupUIManager = Gtk::UIManager::create();
    mPopupUIManager->insert_action_group(mPopupActionGroup);

    Glib::ustring ui_info =
        "<ui>"
        "  <popup name='DynTV_Popup'>"
        "      <menuitem action='DynTV_ContextAdd'/>"
        "      <menuitem action='DynTV_ContextEdit'/>"
        "      <menuitem action='DynTV_ContextNewLibrary'/>"
        "      <menuitem action='DynTV_ContextRemove'/>"
        "      <menuitem action='DynTV_ContextRename'/>"
        "  </popup>"
        "</ui>";

    try {
        mPopupUIManager->add_ui_from_string(ui_info);
        mMenuPopup = (Gtk::Menu *) (
            mPopupUIManager->get_widget("/DynTV_Popup"));
    } catch(const Glib::Error& ex) {
        std::cerr << "building menus failed: DynTV_Popup " <<  ex.what();
    }

    if (!mMenuPopup)
        std::cerr << "menu not found DynTV_Popup \n";
}

AtomicModelBox::DynamicTreeView::~DynamicTreeView()
{

}

void AtomicModelBox::DynamicTreeView::applyRenaming()
{
    renameList::const_iterator it = mRenameList.begin();

    while (it != mRenameList.end()) {
	mModeling->vpz().project().model().updateDynamics(it->first,
                                                          it->second);
        mModeling->vpz().project().classes().updateDynamics(it->first,
                                                            it->second);

        ++it;
    }

    mRenameList.clear();
}

void AtomicModelBox::DynamicTreeView::build()
{
    assert(mModeling);
    using namespace vpz;

    mRefListDyn->clear();
    mLabel->set_label(_("Selected Dynamic: "));

    const DynamicList& list = mDynamics->dynamiclist();
    DynamicList::const_iterator it = list.begin();
    while (it != list.end()) {
        Gtk::TreeModel::Row row = *(mRefListDyn->append());
        row[mColumnsDyn.m_col_name] = it->first;
        row[mColumnsDyn.m_package] = it->second.package();
        row[mColumnsDyn.m_dyn] = it->second.library();
        row[mColumnsDyn.m_sel] = false;
        ++it;
    }

    const Gtk::TreeModel::Children& child = mRefListDyn->children();
    Gtk::TreeModel::Children::const_iterator it_child = child.begin();
    while (it_child != child.end()) {
        if (it_child->get_value(mColumnsDyn.m_col_name) == mAtom->dynamics()) {
	    Gtk::TreeModel::Path path = mRefListDyn->get_path(it_child);
	    set_cursor(path);
            (*it_child)[mColumnsDyn.m_sel] = true;
	    mLabel->set_label(_("Selected Dynamic: ") + mAtom->dynamics());
        }
        ++it_child;
    }
}

bool AtomicModelBox::DynamicTreeView::on_button_press_event(
    GdkEventButton *event)
{
  //Call base class, to allow normal handling,
  //such as allowing the row to be selected by the right-click:
  bool return_value = TreeView::on_button_press_event(event);

  //Then do our custom stuff:
  if ((event->type == GDK_BUTTON_PRESS) and (event->button == 3)) {
      if (mMenuPopup)
          mMenuPopup->popup(event->button, event->time);
  }

  if (event->type == GDK_2BUTTON_PRESS) {
      mDelayTime = event->time;
      Gtk::TreeModel::Path path;
      Gtk::TreeViewColumn* column;
      get_cursor(path, column);
      onRowActivated(path, column);
  }
  if (event->type == GDK_BUTTON_PRESS) {
      if (mDelayTime + 250 < event->time) {
	  mDelayTime = event->time;
	  mCellRenderer->property_editable() = true;
      } else {
	  mDelayTime = event->time;
	  mCellRenderer->property_editable() = false;
      }
  }
  return return_value;
}


std::string AtomicModelBox::DynamicTreeView::pathFileSearch(
    const std::string& path, const std::string& filename)
{
    Glib::Dir dir(path);
    Glib::Dir::iterator it;
    for (it = dir.begin(); it != dir.end(); ++it) {
	if ((*it)[0] != '.') {
	    std::string name = Glib::build_filename(path, (*it));
	    std::transform(name.begin(), name.end(), name.begin(), tolower);
	    if (Glib::file_test((name), Glib::FILE_TEST_IS_DIR)) {
		pathFileSearch(Glib::build_filename(path, (*it)),
			       filename);
	    }
	    if (utils::Path::basename(name) == filename
		and utils::Path::extension(name) == ".cpp") {
		return Glib::build_filename(
		  mGVLE->getPackageSrcDir(), (*it));
	    }
	}
    }
    return "";
}

void AtomicModelBox::DynamicTreeView::onRowActivated(
    const Gtk::TreeModel::Path& path,
    Gtk::TreeViewColumn* column)
{
    if (column and
        utils::Path::exist(mGVLE->getPackageSrcDir())) {

        Gtk::TreeRow row = (*mRefListDyn->get_iter(path));
	vpz::Dynamic& dynamic = mDynamics->get(
	    row.get_value(mColumnsDyn.m_col_name));

	std::string searchFile;
        searchFile = row.get_value(mColumnsDyn.m_dyn);

	std::transform(searchFile.begin(), searchFile.end(),
		       searchFile.begin(), tolower);

	std::string newTab = pathFileSearch(
	    mGVLE->getPackageSrcDir(), searchFile);
        if (not newTab.empty()) {
            try {
                std::string pluginname, packagename, conf;
                utils::Template tpl;

                tpl.open(newTab);

                tpl.tag(pluginname, packagename, conf);
                ModelingPluginPtr plugin =
                    mGVLE->pluginFactory().getModelingPlugin(packagename,
                                      pluginname,
                                      mGVLE->currentPackage().name());

                if (plugin->modify(*mAtom, dynamic, *mConditions,
                                   *mObservables, conf, tpl.buffer())) {
                    const std::string& buffer = plugin->source();
                    std::string filename = mGVLE->getPackageSrcFile(dynamic.library() +
                        ".cpp");
                    try {
                        std::ofstream f(filename.c_str());
                        f.exceptions(std::ofstream::failbit |
                                     std::ofstream::badbit);
                        f << buffer;
                    } catch(const std::ios_base::failure& e) {
                        throw utils::ArgError(fmt(
                                _("Cannot store buffer in file '%1%'")) %
                            filename);
                    }
                    mParent->refresh();
                }
            } catch(...) {
                mParent->on_apply();
                mGVLE->getEditor()->openTab(newTab);
            }
	} else {
            Error((fmt(_("The source file '%1%'.cpp does not exist")) %
                   searchFile).str());
	}
    }
}

void AtomicModelBox::DynamicTreeView::onAdd()
{
    SimpleTypeBox box(_("Name of the Dynamic ?"), "");

    std::string name = box.run();
    if (box.valid()) {
	box.hide();
        name = boost::trim_copy(name);
        if (mDynamics->exist(name)) {
            Error((fmt(_("The Dynamics '%1%' already exists")) % name).str());
        } else {
            vpz::Dynamic dyn(name);
            DynamicBox mDynamicBox(mXml, mGVLE,
                                   *mAtom, dyn, *mConditions,
                                   *mObservables);
            mDynamicBox.show(&dyn);
            if (mDynamicBox.valid()) {
                mDynamics->add(dyn);
                mParent->refresh();

		const Gtk::TreeModel::Children& child(mRefListDyn->children());
                Gtk::TreeModel::Children::const_iterator it = child.begin();
                while (it != child.end()) {
                    if (it->get_value(mColumnsDyn.m_col_name) == dyn.name()) {
			Gtk::TreeModel::Path path = mRefListDyn->get_path(it);
			set_cursor(path);
                        break;
                    }
                    ++it;
                }
            }
        }
    }
}

void AtomicModelBox::DynamicTreeView::onEdit()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Gtk::TreeModel::iterator it = refSelection->get_selected();

    if (it) {
        Glib::ustring dyn = (*it)[mColumnsDyn.m_col_name];

        if (not dyn.empty()) {
            vpz::Dynamic& dynamic(mDynamics->get(dyn));
            DynamicBox mDynamicBox(mXml,
                                   mGVLE,
                                   *mAtom, dynamic,
                                   *mConditions, *mObservables);
            mDynamicBox.show(&dynamic);
            build();

            const Gtk::TreeModel::Children& child(mRefListDyn->children());
            Gtk::TreeModel::Children::const_iterator it = child.begin();
            while (it != child.end()) {
                if (it->get_value(mColumnsDyn.m_col_name) == dyn) {
                    Gtk::TreeModel::Path path = mRefListDyn->get_path(it);
                    set_cursor(path);
                    mLabel->set_label(_("Selected Dynamic: ") + mAtom->dynamics());
                    break;
                }
                ++it;
            }
        }
    }
}

void AtomicModelBox::DynamicTreeView::onNewLibrary()
{

    SimpleTypeBox box(_("Name of the Dynamic ?"), "");

    std::string name = box.run();
    if (box.valid()) {
	box.hide();
        name = boost::trim_copy(name);
        if (mDynamics->exist(name)) {
            Error((fmt(_("The Dynamics '%1%' already exists")) % name).str());
        } else {
            vpz::Dynamic dyn(name);

            OpenModelingPluginBox box1(mXml, mGVLE);

            if (box1.run() == Gtk::RESPONSE_OK) {
                NewDynamicsBox box2(mXml, mGVLE->currentPackage());

                if (box2.run() == Gtk::RESPONSE_OK and
                    not box2.getClassName().empty() and
                    not box2.getNamespace().empty()) {
                    if (execPlugin(
                            dyn,
                            box1.pluginName(),
                            box2.getClassName(),
                            box2.getNamespace()) == Gtk::RESPONSE_OK) {
                        dyn.setLibrary(box2.getClassName());
                        dyn.setPackage(mGVLE->currentPackage().name());
                        mDynamics->add(dyn);
                        build();

                        mParent->refresh();
                    }
                }
            }
        }
    }
}

int AtomicModelBox::DynamicTreeView::execPlugin(
    vpz::Dynamic& dynamic,
    const std::string& pluginname,
    const std::string& classname,
    const std::string& namespace_)
{
    ModelingPluginPtr plugin =
        mGVLE->pluginFactory().getModelingPlugin(pluginname,
                mGVLE->currentPackage().name());

    if (plugin->create(*mAtom, dynamic, *mConditions,
                       *mObservables, classname, namespace_)) {
        const std::string& buffer = plugin->source();
        std::string filename = mGVLE->getPackageSrcFile(classname + ".cpp");
        //filename += ".cpp";

        try {
            std::ofstream f(filename.c_str());
            f.exceptions(std::ofstream::failbit | std::ofstream::badbit);
            f << buffer;
        } catch(const std::ios_base::failure& e) {
            throw utils::ArgError(fmt(
                    _("Cannot store buffer in file '%1%'")) % filename);
        }

        return Gtk::RESPONSE_OK;
    } else {
        return Gtk::RESPONSE_CANCEL;
    }
}

void AtomicModelBox::DynamicTreeView::onRemove()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Gtk::TreeModel::iterator it = refSelection->get_selected();

    if (it) {
        Glib::ustring dyn = (*it)[mColumnsDyn.m_col_name];
        if (not dyn.empty()) {
            if ((*it)[mColumnsDyn.m_sel] == true) {
                mAtom->setDynamics("");
            }
            mDynamics->del(dyn);
            build();
        }
    }
}

void AtomicModelBox::DynamicTreeView::onRename()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Gtk::TreeModel::iterator it = refSelection->get_selected();

    if (it) {
        Glib::ustring oldName = (*it)[mColumnsDyn.m_col_name];

        if (not oldName.empty()) {
            SimpleTypeBox box(_("Name of the Dynamic ?"), oldName);

            std::string newName = box.run();
            if (box.valid() and not newName.empty()) {
                box.hide();
                newName = boost::trim_copy(newName);
                if (mDynamics->exist(newName)) {
                    Error(
                        boost::str(
                            fmt(_("The Dynamics '%1%' already exists")) % newName));
                } else {
                    vpz::Dynamic* newDynamic = new vpz::Dynamic(newName);
                    vpz::Dynamic oldDynamic = mDynamics->get(oldName);
                    newDynamic->setLibrary(oldDynamic.library());
                    newDynamic->setPackage(oldDynamic.package());
                    newDynamic->setLanguage(oldDynamic.language());
                    mDynamics->add(*newDynamic);
                    mDynamics->del(oldName);
                    if ((*it)[mColumnsDyn.m_sel] == true) {
                        mAtom->setDynamics(newName);
                    }
                    mRenameList.push_back(std::make_pair(oldName, newName));
                    build();
                }
            }
        }
    }
}

void AtomicModelBox::DynamicTreeView::onEditionStarted(
    Gtk::CellEditable* cell_editable, const Glib::ustring& /* path */)
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();

    if (refSelection) {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            mOldName = row.get_value(mColumnsDyn.m_col_name);
        }
    }

    if(mValidateRetry)
    {
	Gtk::CellEditable* celleditable_validated = cell_editable;
	Gtk::Entry* pEntry = dynamic_cast<Gtk::Entry*>(
	    celleditable_validated);
	if(pEntry)
	{
	    pEntry->set_text(mInvalidTextForRetry);
	    mValidateRetry = false;
	    mInvalidTextForRetry.clear();
	}
    }
}

void AtomicModelBox::DynamicTreeView::onEdition(
        const Glib::ustring& path,
        const Glib::ustring& newName)
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();

    if (refSelection and newName != mOldName) {
	if (not newName.empty() and not mDynamics->exist(newName)) {
	    vpz::Dynamic* newDynamic = new vpz::Dynamic(newName);
	    vpz::Dynamic oldDynamic = mDynamics->get(mOldName);
	    newDynamic->setLibrary(oldDynamic.library());
	    newDynamic->setLanguage(oldDynamic.language());
	    mDynamics->add(*newDynamic);
	    mDynamics->del(mOldName);

            const Gtk::TreeModel::iterator iter = mRefListDyn->get_iter(path);
            if ((*iter)[mColumnsDyn.m_sel] == true) {
                mAtom->setDynamics(newName);
            }

	    mRenameList.push_back(std::make_pair(mOldName, newName));
	    build();
	}
    }
}

void AtomicModelBox::DynamicTreeView::onToggled(const Glib::ustring&  path)
{
    const Gtk::TreeModel::iterator iterOnToggled = mRefListDyn->get_iter(path);

    Gtk::TreeModel::Children child = mRefListDyn->children();
    Gtk::TreeModel::iterator iter = child.begin();

    while (iter != child.end()) {
        if (iter != iterOnToggled) {
            (*iter)[mColumnsDyn.m_sel] = false;
        }
        iter++;
    }
    mAtom->setDynamics(getDynamic());
    mLabel->set_label(_("Selected Dynamic: ") + getDynamic());
}

Gtk::TreeModel::iterator AtomicModelBox::DynamicTreeView::getSelectedRadio()
{
    Gtk::TreeModel::Children child = mRefListDyn->children();
    Gtk::TreeModel::iterator it = child.begin();
    Gtk::TreeModel::iterator iter;
    while (it != child.end()) {
        if ((*it)[mColumnsDyn.m_sel] == true) {
            return it;
        }
        it++;
    }
    return iter;
}

std::string AtomicModelBox::DynamicTreeView::getDynamic()
{
    Gtk::TreeModel::iterator iter = getSelectedRadio();
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        std::string dynamicname = row.get_value(mColumnsDyn.m_col_name);
        return dynamicname;
    } else {
        return "";
    }
}

//ObservableTreeView

AtomicModelBox::ObservableTreeView::ObservableTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr < Gtk::Builder >& xml /*refGlade*/) :
    Gtk::TreeView(cobject),
    mMenuPopup(0),
    mObsAndViewBox(xml)
{
    mRefTreeModelObs = Gtk::ListStore::create(mColumnsObs);
    set_model(mRefTreeModelObs);

    mColumnSel = append_column_editable(_(" "), mColumnsObs.m_col_sel);
    Gtk::CellRendererToggle* cellRenderer = dynamic_cast<Gtk::CellRendererToggle*>(
        get_column_cell_renderer(mColumnSel - 1));
    cellRenderer->set_radio();
    // to change the behaviour
    cellRenderer->signal_toggled().connect(
    sigc::mem_fun(*this,
                  &AtomicModelBox::ObservableTreeView::
                  onToggled));

    mColumnName = append_column_editable(_("Name"),
					 mColumnsObs.m_col_name);
    Gtk::TreeViewColumn* nameCol = get_column(mColumnName - 1);
    nameCol->set_sort_column(mColumnsObs.m_col_name);

    mCellRenderer = dynamic_cast<Gtk::CellRendererText*>(
	get_column_cell_renderer(mColumnName - 1));
    mCellRenderer->property_editable() = true;
    mCellRenderer->signal_editing_started().connect(
	sigc::mem_fun(*this,
		      &AtomicModelBox::ObservableTreeView::
		      onEditionStarted) );

    mCellRenderer->signal_edited().connect(
	sigc::mem_fun(*this,
		      &AtomicModelBox::ObservableTreeView::
		      onEdition) );

    mPopupActionGroup = Gtk::ActionGroup::create();
    mPopupActionGroup->add(Gtk::Action::create("ObsTV_ContextMenu", _("Context Menu")));
    mPopupActionGroup->add(
        Gtk::Action::create("ObsTV_ContextAdd", _("_Add")),
        sigc::mem_fun(*this, &AtomicModelBox::ObservableTreeView::onAdd));
    mPopupActionGroup->add(
        Gtk::Action::create("ObsTV_ContextEdit", _("_Edit")),
        sigc::mem_fun(*this, &AtomicModelBox::ObservableTreeView::onEdit));
    mPopupActionGroup->add(
        Gtk::Action::create("ObsTV_ContextRemove", _("_Remove")),
        sigc::mem_fun(*this, &AtomicModelBox::ObservableTreeView::onRemove));
    mPopupActionGroup->add(
        Gtk::Action::create("ObsTV_ContextRename", _("_Rename")),
        sigc::mem_fun(*this, &AtomicModelBox::ObservableTreeView::onRename));

    mPopupUIManager = Gtk::UIManager::create();
    mPopupUIManager->insert_action_group(mPopupActionGroup);

    Glib::ustring ui_info =
        "<ui>"
        "  <popup name='ObsTV_Popup'>"
        "      <menuitem action='ObsTV_ContextAdd'/>"
        "      <menuitem action='ObsTV_ContextEdit'/>"
        "      <menuitem action='ObsTV_ContextRemove'/>"
        "      <menuitem action='ObsTV_ContextRename'/>"
        "  </popup>"
        "</ui>";

    try {
        mPopupUIManager->add_ui_from_string(ui_info);
        mMenuPopup = (Gtk::Menu *) (
            mPopupUIManager->get_widget("/ObsTV_Popup"));
    } catch(const Glib::Error& ex) {
        std::cerr << "building menus failed: ObsTV_Popup " <<  ex.what();
    }

    if (!mMenuPopup)
        std::cerr << "menu not found ObsTV_Popup \n";
}

AtomicModelBox::ObservableTreeView::~ObservableTreeView()
{
}

void AtomicModelBox::ObservableTreeView::applyRenaming()
{
    renameList::const_iterator it = mRenameList.begin();

    while (it != mRenameList.end()) {
        mModeling->vpz().project().model().updateObservable(it->first,
                                                            it->second);
        mModeling->vpz().project().classes().updateObservable(it->first,
                                                              it->second);

	++it;
    }

    mRenameList.clear();
}

void AtomicModelBox::ObservableTreeView::build()
{
    mRefTreeModelObs->clear();
    mLabel->set_label(_("Selected Observable: "));

    vpz::Observables::const_iterator it = mObservables->begin();

    while (it != mObservables->end()) {
        Gtk::TreeModel::Row row = *(mRefTreeModelObs->append());

        row[mColumnsObs.m_col_name] = it->first;
        row[mColumnsObs.m_col_sel] = false;
        ++it;
    }

    const Gtk::TreeModel::Children& child = mRefTreeModelObs->children();
    Gtk::TreeModel::Children::const_iterator it_child = child.begin();

    while (it_child != child.end()) {
        if (it_child->get_value(mColumnsObs.m_col_name) ==
	    mAtom->observables()) {
	    Gtk::TreeModel::Path path = mRefTreeModelObs->get_path(it_child);
	    set_cursor(path);
            (*it_child)[mColumnsObs.m_col_sel] = true;
	    mLabel->set_label(_("Selected Observable: ") +
			      mAtom->observables());
	}
        ++it_child;
    }
}

bool AtomicModelBox::ObservableTreeView::on_button_press_event(
    GdkEventButton *event)
{
    //Call base class, to allow normal handling,
    //such as allowing the row to be selected by the right-click:
    bool return_value = TreeView::on_button_press_event(event);

    //Then do our custom stuff:
    if ((event->type == GDK_BUTTON_PRESS) and (event->button == 3)) {
        if (mMenuPopup)
            mMenuPopup->popup(event->button, event->time);
    }
    return return_value;
}

void AtomicModelBox::ObservableTreeView::onAdd()
{
    SimpleTypeBox box(_("Name of the Observable ?"), "");

    std::string name = box.run();
    if (box.valid()) {
        name = boost::trim_copy(name);
        if (mObservables->exist(name)) {
            Error(boost::str(fmt(
                        _("The observable '%1%' already exists")) % name));
        } else {
            mObservables->add(vpz::Observable(name));
	    build();

            const Gtk::TreeModel::Children& child =
		mRefTreeModelObs->children();
            Gtk::TreeModel::Children::const_iterator it_child = child.begin();
            while (it_child != child.end()) {
                if (it_child->get_value(mColumnsObs.m_col_name) == name) {
		    Gtk::TreeModel::Path path =
			mRefTreeModelObs->get_path(it_child);
		    set_cursor(path);
                    break;
                }
                ++it_child;
            }
        }
    }
}

void AtomicModelBox::ObservableTreeView::onEdit()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Gtk::TreeModel::iterator it = refSelection->get_selected();

    if (it) {
        Glib::ustring obs = (*it)[mColumnsObs.m_col_name];

        if (not obs.empty()) {
            mObsAndViewBox.show(*mObservables, obs, mModeling->views());
        }
    }
}

void AtomicModelBox::ObservableTreeView::onRemove()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Gtk::TreeModel::iterator it = refSelection->get_selected();

    if (it) {
        Glib::ustring obs = (*it)[mColumnsObs.m_col_name];
        if (not obs.empty()) {
            if ((*it)[mColumnsObs.m_col_sel] == true) {
                mAtom->setObservables("");
            }
            mObservables->del(obs);
            build();
        }
    }
}

void AtomicModelBox::ObservableTreeView::onRename()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Gtk::TreeModel::iterator it = refSelection->get_selected();

    if (it) {
        Glib::ustring oldName = (*it)[mColumnsObs.m_col_name];

        if (not oldName.empty()) {
            InteractiveTypeBox box(_("New name for this observable ?"),
                                   &(mModeling->observables()),
                                   oldName);
            std::string newName = box.run();
            if (box.valid() and not newName.empty()) {
                box.hide();
                newName = boost::trim_copy(newName);
                if ((*it)[mColumnsObs.m_col_sel] == true) {
                    mAtom->setObservables(newName);
                }
                mObservables->rename(oldName, newName);
                mRenameList.push_back(std::make_pair(oldName, newName));
                build();
            }
        }
    }
}

void AtomicModelBox::ObservableTreeView::onEditionStarted(
    Gtk::CellEditable* cellEditable,
    const Glib::ustring& /* path */)
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Gtk::TreeModel::iterator iter = refSelection->get_selected();

    if (iter) {
	Gtk::TreeModel::Row row = *iter;
	mOldName = row.get_value(mColumnsObs.m_col_name);
    }

    if(mValidateRetry) {
	Gtk::CellEditable* celleditable_validated = cellEditable;
	Gtk::Entry* pEntry = dynamic_cast<Gtk::Entry*>(celleditable_validated);
	if(pEntry) {
	    pEntry->set_text(mInvalidTextForRetry);
	    mValidateRetry = false;
	    mInvalidTextForRetry.clear();
	}
    }
}

void AtomicModelBox::ObservableTreeView::onEdition(
    const Glib::ustring& path,
    const Glib::ustring& newName)
{
    if (not newName.empty() and (not mObservables->exist(newName))) {
        const Gtk::TreeModel::iterator iter =
            mRefTreeModelObs->get_iter(path);
        if ((*iter)[mColumnsObs.m_col_sel] == true) {
            mAtom->setObservables(newName);
        }
        mObservables->rename(mOldName, newName);
        mRenameList.push_back(std::make_pair(mOldName, newName));
    }
    build();
}

void AtomicModelBox::ObservableTreeView::onToggled(const Glib::ustring&  path)
{
    const Gtk::TreeModel::iterator iterOnToggled =
        mRefTreeModelObs->get_iter(path);
    Gtk::TreeModel::Children child = mRefTreeModelObs->children();
    Gtk::TreeModel::iterator iter = child.begin();

    while (iter != child.end()) {
        if (iter != iterOnToggled) {
            (*iter)[mColumnsObs.m_col_sel] = false;
        }
        iter++;
    }
    mAtom->setObservables(getObservable());
    mLabel->set_label(_("Selected Observable: ") + getObservable());
}


Gtk::TreeModel::iterator AtomicModelBox::ObservableTreeView::getSelectedRadio()
{
    Gtk::TreeModel::Children child = mRefTreeModelObs->children();
    Gtk::TreeModel::iterator it = child.begin();
    Gtk::TreeModel::iterator iter;
    while (it != child.end()) {
        if ((*it)[mColumnsObs.m_col_sel] == true) {
            return it;
        }
        it++;
    }
    return iter;
}

std::string AtomicModelBox::ObservableTreeView::getObservable()
{
    Gtk::TreeModel::iterator iter = getSelectedRadio();
    if (iter) {
        return (*iter).get_value(mColumnsObs.m_col_name);
    } else {
        return "";
    }
}

// AtomicModelBox

AtomicModelBox::AtomicModelBox(const Glib::RefPtr < Gtk::Builder >& xml,
			       Modeling* m, GVLE* gvle):
    mXml(xml),
    mModeling(m),
    mGVLE(gvle),
    mModel(0),
    mCond(0),
    mDyn(0),
    mObs(0)
{
    xml->get_widget("DialogAtomicModel", mDialog);

    //Input Ports
    xml->get_widget_derived("TreeViewInputPorts", mInputPorts);

    //Output Ports
    xml->get_widget_derived("TreeViewOutputPorts", mOutputPorts);

    //Dynamic
    xml->get_widget_derived("TreeViewDynamics", mDynamics);
    xml->get_widget("labelDynamicList", mLabelDynamics);

    //Observable
    xml->get_widget_derived("TreeViewObservables", mObservables);
    xml->get_widget("labelObservableList", mLabelObservables);

    //Conditions
    xml->get_widget_derived("TreeViewConditions", mConditions);
    xml->get_widget("labelConditionList", mLabelConditions);

    //Buttons
    xml->get_widget("ButtonAtomicApply", mButtonApply);
    mButtonApply->signal_clicked().connect(
        sigc::mem_fun(*this, &AtomicModelBox::on_apply));

    xml->get_widget("ButtonAtomicCancel", mButtonCancel);
    mButtonCancel->signal_clicked().connect(
        sigc::mem_fun(*this, &AtomicModelBox::on_cancel));
}

AtomicModelBox::~AtomicModelBox()
{
    mDialog->hide();
}

void AtomicModelBox::show(vpz::AtomicModel* model)
{
    const Modeling& modeling = (*(const Modeling*)mModeling);
    mDialog->set_title("Atomic Model " + model->getName());

    mCurrentModel = model;

    mModel = new vpz::AtomicModel(*model);
    mCond = new vpz::Conditions(modeling.conditions());
    mDyn = new vpz::Dynamics(modeling.dynamics());
    mObs = new vpz::Observables(modeling.observables());

    mInputPorts->setModel(mModel);
    mInputPorts->clearRenaming();
    mInputPorts->build();

    mOutputPorts->setModel(mModel);
    mOutputPorts->clearRenaming();
    mOutputPorts->build();

    mConditions->setModel(mModel);
    mConditions->setModeling(mModeling);
    mConditions->setGVLE(mGVLE);
    mConditions->setLabel(mLabelConditions);
    mConditions->setConditions(mCond);
    mConditions->clearRenaming();
    mConditions->build();

    mDynamics->setModel(mModel);
    mDynamics->setModeling(mModeling);
    mDynamics->setGVLE(mGVLE);
    mDynamics->setLabel(mLabelDynamics);
    mDynamics->setDynamics(mDyn);
    mDynamics->setConditions(mCond);
    mDynamics->setObservables(mObs);
    mDynamics->setParent(this);
    mDynamics->clearRenaming();
    mDynamics->build();

    mObservables->setModel(mModel);
    mObservables->setModeling(mModeling);
    mObservables->setLabel(mLabelObservables);
    mObservables->setObservables(mObs);
    mObservables->clearRenaming();
    mObservables->build();

    mDialog->show_all();
    mDialog->run();
    mDialog->hide();
}

void AtomicModelBox::applyPorts()
{
    mInputPorts->applyRenaming(mCurrentModel);
    mOutputPorts->applyRenaming(mCurrentModel);

    // Apply input ports
    {
	vpz::ConnectionList connec_in_list =
	    mCurrentModel->getInputPortList();
	vpz::ConnectionList::iterator it = connec_in_list.begin();

	while (it != connec_in_list.end()) {
	    if (not mModel->existInputPort(it->first)) {
		mCurrentModel->delInputPort(it->first);
	    }
	    ++it;
	}
    }
    {
	vpz::ConnectionList connec_in_list =
     	    mModel->getInputPortList();
	vpz::ConnectionList::iterator it = connec_in_list.begin();

     	while (it != connec_in_list.end()) {
	    if (not mCurrentModel->existInputPort(it->first)) {
		mCurrentModel->addInputPort(it->first);
	    }
     	    ++it;
     	}
     }

    // Apply output ports
    {
	vpz::ConnectionList connec_out_list =
	    mCurrentModel->getOutputPortList();
	vpz::ConnectionList::iterator it = connec_out_list.begin();

	while (it != connec_out_list.end()) {
	    if (not mModel->existOutputPort(it->first)) {
		mCurrentModel->delOutputPort(it->first);
	    }
	    ++it;
	}
    }
    {
	vpz::ConnectionList& connec_out_list =
     	    mModel->getOutputPortList();
	vpz::ConnectionList::iterator it = connec_out_list.begin();

     	while (it != connec_out_list.end()) {
	    if (not mCurrentModel->existOutputPort(it->first)) {
		mCurrentModel->addOutputPort(it->first);
	    }
     	    ++it;
     	}
    }

    // Apply height
    {
        mCurrentModel->setHeight(
            ViewDrawingArea::MODEL_HEIGHT +
            std::max(mCurrentModel->getInputPortNumber(),
                     mCurrentModel->getOutputPortNumber()) *
            (ViewDrawingArea::MODEL_SPACING_PORT +
             ViewDrawingArea::MODEL_PORT));
    }
}

void AtomicModelBox::applyConditions()
{
    mModeling->conditions().clear();

    const vpz::ConditionList& list = mCond->conditionlist();
    vpz::ConditionList::const_iterator it = list.begin();

    while (it != list.end()) {
	mModeling->conditions().add(it->second);
	++it;
    }

    mConditions->applyRenaming();

    std::set < std::string > conditions = mModeling->conditions().getKeys();

    mModeling->vpz().project().model().purgeConditions(conditions);
    mModeling->vpz().project().classes().purgeConditions(conditions);

    mCurrentModel->setConditions(mConditions->getConditions());

    delete mCond;
    mCond = 0;
}

void AtomicModelBox::applyDynamics()
{
    mModeling->dynamics().clear();

    const vpz::DynamicList& list = mDyn->dynamiclist();
    vpz::DynamicList::const_iterator it = list.begin();

    while (it != list.end()) {
	mModeling->dynamics().add(it->second);
	++it;
    }

    mDynamics->applyRenaming();

    std::set < std::string > dynamics = mModeling->dynamics().getKeys();

    mModeling->vpz().project().model().purgeDynamics(dynamics);
    mModeling->vpz().project().classes().purgeDynamics(dynamics);

    Glib::RefPtr<Gtk::TreeView::Selection> refSelection =
        mDynamics->get_selection();

    if (refSelection->get_selected()) {
        mCurrentModel->setDynamics(mDynamics->getDynamic());
    }

    delete mDyn;
    mDyn = 0;
}

void AtomicModelBox::applyObservables()
{
    mModeling->observables().clear();

    const vpz::ObservableList& list = mObs->observablelist();
    vpz::ObservableList::const_iterator it = list.begin();

    while (it != list.end()) {
	mModeling->observables().add(it->second);
	++it;
    }

    mObservables->applyRenaming();

    std::set < std::string > observables = mModeling->observables().getKeys();

    mModeling->vpz().project().model().purgeObservable(observables);
    mModeling->vpz().project().classes().purgeObservable(observables);

    Glib::RefPtr<Gtk::TreeView::Selection> refSelection =
        mObservables->get_selection();

    if (refSelection->get_selected()) {
        mCurrentModel->setObservables(mObservables->getObservable());
    }

    delete mObs;
    mObs = 0;
}

void AtomicModelBox::on_apply()
{
    applyPorts();
    applyConditions();
    applyDynamics();
    applyObservables();

    delete mModel;
    mModel = 0;
}

void AtomicModelBox::on_cancel()
{
    delete mModel;
    mModel = 0;
    delete mCond;
    mCond = 0;
    delete mDyn;
    mDyn = 0;
    delete mObs;
    mObs = 0;
}

void AtomicModelBox::refresh()
{
    mInputPorts->build();
    mOutputPorts->build();
    mConditions->build();
    mDynamics->build();
    mObservables->build();
}

} } // namespace vle gvle
