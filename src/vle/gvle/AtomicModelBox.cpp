/**
 * @file vle/gvle/AtomicModelBox.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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


#include <vle/gvle/AtomicModelBox.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/ObservableBox.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/gvle/InteractiveTypeBox.hpp>
#include <vle/gvle/ViewDrawingArea.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Template.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace vle { namespace gvle {

// InputPortTreeView

AtomicModelBox::InputPortTreeView::InputPortTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/) :
    Gtk::TreeView(cobject),
    mValidateRetry(false)
{
    mRefTreeModelInputPort = Gtk::ListStore::create(mColumnsInputPort);
    set_model(mRefTreeModelInputPort);
    //append_column(_("Name"), mColumnsInputPort.m_col_name);
    mColumnName = append_column_editable(_("Name"),
					 mColumnsInputPort.m_col_name);
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

    //Fill popup menu:
    {
	Gtk::Menu::MenuList& menulist = mMenuPopup.items();

	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Add"),
		sigc::mem_fun(
		    *this,
		    &AtomicModelBox::InputPortTreeView::onAdd)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Remove"),
		sigc::mem_fun(
		    *this,
		    &AtomicModelBox::InputPortTreeView::onRemove)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Rename"),
		sigc::mem_fun(
		    *this,
		    &AtomicModelBox::InputPortTreeView::onRename)));
    }
    mMenuPopup.accelerate(*this);
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
  if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) )
  {
      mMenuPopup.popup(event->button, event->time);
  }

  return return_value;
}

void AtomicModelBox::InputPortTreeView::applyRenaming(graph::AtomicModel* model)
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
    using namespace graph;

    mRefTreeModelInputPort->clear();

    ConnectionList list = mModel->getInputPortList();
    ConnectionList::const_iterator it = list.begin();
    while (it != list.end()) {
        Gtk::TreeModel::Row row = *(mRefTreeModelInputPort->append());
        row[mColumnsInputPort.m_col_name] = it->first;

        ++it;
    }
}

void AtomicModelBox::InputPortTreeView::onAdd()
{
    SimpleTypeBox box(_("Name of the Input port ?"), "");
    graph::ConnectionList& list = mModel->getInputPortList();
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
	    graph::ConnectionList& list = mModel->getInputPortList();
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

    graph::ConnectionList& list = mModel->getInputPortList();

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
    const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/) :
    Gtk::TreeView(cobject),
    mValidateRetry(false)
{
    mRefTreeModelOutputPort = Gtk::ListStore::create(mColumnsOutputPort);
    set_model(mRefTreeModelOutputPort);

    mColumnName = append_column_editable(_("Name"),
				     mColumnsOutputPort.m_col_name);
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

    //Fill popup menu:
    {
	Gtk::Menu::MenuList& menulist = mMenuPopup.items();

	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Add"),
		sigc::mem_fun(
		    *this,
		    &AtomicModelBox::OutputPortTreeView::onAdd)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Remove"),
		sigc::mem_fun(
		    *this,
		    &AtomicModelBox::OutputPortTreeView::onRemove)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Rename"),
		sigc::mem_fun(
		    *this,
		    &AtomicModelBox::OutputPortTreeView::onRename)));
    }
    mMenuPopup.accelerate(*this);
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
  if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) )
  {
    mMenuPopup.popup(event->button, event->time);
  }

  return return_value;
}

void AtomicModelBox::OutputPortTreeView::applyRenaming(
    graph::AtomicModel* model)
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
    using namespace graph;

    mRefTreeModelOutputPort->clear();

    ConnectionList list = mModel->getOutputPortList();
    ConnectionList::const_iterator it = list.begin();
    while (it != list.end()) {
        Gtk::TreeModel::Row row = *(mRefTreeModelOutputPort->append());
        row[mColumnsOutputPort.m_col_name] = it->first;

        ++it;
    }
}

void AtomicModelBox::OutputPortTreeView::onAdd()
{
    SimpleTypeBox box(_("Name of the output port ?"), "");
    graph::ConnectionList& list = mModel->getOutputPortList();
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
	    graph::ConnectionList& list = mModel->getOutputPortList();
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
    Gtk::CellEditable* cell_editable, const Glib::ustring& /* path */)
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

    graph::ConnectionList& list = mModel->getOutputPortList();

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
    const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/) :
    Gtk::TreeView(cobject)
{
    mRefTreeModel = Gtk::ListStore::create(mColumns);
    set_model(mRefTreeModel);
    append_column_editable(_("In"), mColumns.m_col_activ);
    mColumnName = append_column_editable(_("Name"), mColumns.m_col_name);

    mCellrendererValidated = dynamic_cast<Gtk::CellRendererText*>(
	get_column_cell_renderer(mColumnName - 1));
    mCellrendererValidated->property_editable() = true;
    mCellrendererValidated->signal_editing_started().connect(
	sigc::mem_fun(*this,
		      &AtomicModelBox::ConditionTreeView::
		      onEditionStarted) );

    mCellrendererValidated->signal_edited().connect(
	sigc::mem_fun(*this,
		      &AtomicModelBox::ConditionTreeView::
		      onEdition) );

    //Fill popup menu
    {
	Gtk::Menu::MenuList& menulist = mMenuPopup.items();

	menulist.push_back(Gtk::Menu_Helpers::MenuElem(
		_("_Rename"),
		sigc::mem_fun(
		    *this,
		    &AtomicModelBox::ConditionTreeView::onRename)));
    }
    mMenuPopup.accelerate(*this);

}

AtomicModelBox::ConditionTreeView::~ConditionTreeView()
{
}

void AtomicModelBox::ConditionTreeView::applyRenaming()
{
    renameList::const_iterator it = mRenameList.begin();

    while (it != mRenameList.end()) {
	vpz::AtomicModelList& atomlist(
	    mModeling->vpz().project().model().atomicModels());
	atomlist.updateCondition(it->first, it->second);
	++it;
    }
    mRenameList.clear();
}

void AtomicModelBox::ConditionTreeView::build()
{
    mRefTreeModel->clear();

    const vpz::Strings& cond = mModel->conditions();
    vpz::Strings::const_iterator f;
    const vpz::ConditionList& list = mConditions->conditionlist();
    vpz::ConditionList::const_iterator it = list.begin();
    std::string selections("");

    while (it != list.end()) {
        Gtk::TreeModel::Row row = *(mRefTreeModel->append());

        row[mColumns.m_col_name] = it->first;
        f = std::find(cond.begin(), cond.end(), it->first);
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

vpz::Strings AtomicModelBox::ConditionTreeView::getConditions()
{
    vpz::Strings vec;
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
    if (mModeling->runConditionsBox(*mConditions) == 1) {
	mModeling->applyConditionsBox(*mConditions);
	build();
    }
}

bool AtomicModelBox::ConditionTreeView::on_button_press_event(
    GdkEventButton* event)
{
    bool return_value = TreeView::on_button_press_event(event);
    if (event->type == GDK_BUTTON_PRESS and event->button == 3) {
	mMenuPopup.popup(event->button, event->time);
    }
    if (event->type == GDK_2BUTTON_PRESS && event->button == 1) {
	if (mModeling->runConditionsBox(*mConditions) == 1) {
	    mModeling->applyConditionsBox(*mConditions);
	    build();
	}
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
	    mModel->delCondition(oldname);
	    mModel->addCondition(newname);
	    mRenameList.push_back(std::make_pair(oldname, newname));
	    build();
	}
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
        const Glib::ustring& pathString,
        const Glib::ustring& newName)
{
    Gtk::TreePath path(pathString);
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Gtk::TreeModel::iterator it = refSelection->get_selected();

    if (it) {
	const vpz::ConditionList& list = mConditions->conditionlist();

	if (not newName.empty() and list.find(newName) == list.end()) {
	    mConditions->rename(mOldName, newName);
	    mModel->setConditions(getConditions());
	    mRenameList.push_back(std::make_pair(mOldName, newName));
	    build();
	}
    }
}

//DynamicTreeView

AtomicModelBox::DynamicTreeView::DynamicTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr < Gnome::Glade::Xml >& refGlade)
    : Gtk::TreeView(cobject), mXml(refGlade)
{
    mRefListDyn = Gtk::ListStore::create(mColumnsDyn);
    mRefTreeModelDyn = Gtk::TreeModelSort::create(mRefListDyn);
    set_model(mRefTreeModelDyn);
    mColumnName = append_column_editable(_("Name"), mColumnsDyn.m_col_name);
    Gtk::TreeViewColumn* nameCol = get_column(mColumnName - 1);
    nameCol->set_clickable(true);
    nameCol->signal_clicked().connect(
	sigc::bind(sigc::mem_fun(*this,
		&AtomicModelBox::DynamicTreeView::onClickColumn), mColumnName));
    nameCol->set_sort_order(Gtk::SORT_DESCENDING);

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
    packageCol->set_clickable(true);
    packageCol->signal_clicked().connect(
        sigc::bind(sigc::mem_fun(*this,
                                 &AtomicModelBox::DynamicTreeView::onClickColumn),
                   mColumnPackage));
    packageCol->set_sort_order(Gtk::SORT_DESCENDING);

    mColumnDyn = append_column(_("Library"), mColumnsDyn.m_dyn);
    Gtk::TreeViewColumn* dynCol = get_column(mColumnDyn - 1);
    dynCol->set_clickable(true);
    dynCol->signal_clicked().connect(
	sigc::bind(sigc::mem_fun(*this,
		&AtomicModelBox::DynamicTreeView::onClickColumn), mColumnDyn));
    dynCol->set_sort_order(Gtk::SORT_DESCENDING);

    mColumnModel = append_column(_("Model"), mColumnsDyn.m_model);
    Gtk::TreeViewColumn* modelCol = get_column(mColumnModel - 1);
    modelCol->set_clickable(true);
    modelCol->signal_clicked().connect(
	sigc::bind(sigc::mem_fun(*this,
		&AtomicModelBox::DynamicTreeView::onClickColumn),
		   mColumnModel));
    modelCol->set_sort_order(Gtk::SORT_DESCENDING);

    {
	Gtk::Menu::MenuList& menulist = mMenuPopup.items();

	menulist.push_back(
            Gtk::Menu_Helpers::MenuElem(
                _("_Add"), sigc::mem_fun(
                    *this, &AtomicModelBox::DynamicTreeView::onAdd)));
	menulist.push_back(
            Gtk::Menu_Helpers::MenuElem(
                _("_Edit"), sigc::mem_fun(
                    *this, &AtomicModelBox::DynamicTreeView::onEdit)));
	menulist.push_back(
            Gtk::Menu_Helpers::MenuElem(
                _("_Remove"), sigc::mem_fun(
                    *this, &AtomicModelBox::DynamicTreeView::onRemove)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
                _("_Rename"), sigc::mem_fun(
                    *this, &AtomicModelBox::DynamicTreeView::onRename)));
    }
    mMenuPopup.accelerate(*this);
}

AtomicModelBox::DynamicTreeView::~DynamicTreeView()
{

}

void AtomicModelBox::DynamicTreeView::applyRenaming()
{
    renameList::const_iterator it = mRenameList.begin();

    while (it != mRenameList.end()) {
	vpz::AtomicModelList& atomlist(
	    mModeling->vpz().project().model().atomicModels());
	atomlist.updateDynamics(it->first, it->second);
	++it;
    }
    mRenameList.clear();
}

void AtomicModelBox::DynamicTreeView::build()
{
    assert(mModeling);
    using namespace vpz;

    mRefListDyn->clear();

    const DynamicList& list = mDynamics->dynamiclist();
    DynamicList::const_iterator it = list.begin();
    while (it != list.end()) {
        Gtk::TreeModel::Row row = *(mRefListDyn->append());
        row[mColumnsDyn.m_col_name] = it->first;
        row[mColumnsDyn.m_package] = it->second.package();
        row[mColumnsDyn.m_dyn] = it->second.library();
	row[mColumnsDyn.m_model] = it->second.model();

        ++it;
    }

    const Gtk::TreeModel::Children& child = mRefTreeModelDyn->children();
    Gtk::TreeModel::Children::const_iterator it_child = child.begin();
    while (it_child != child.end()) {
        if (it_child->get_value(mColumnsDyn.m_col_name) == mModel->dynamics()) {
	    Gtk::TreeModel::Path path = mRefTreeModelDyn->get_path(it_child);
	    set_cursor(path);
	    mLabel->set_label(_("Selected Dynamic: ") + mModel->dynamics());
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
      mMenuPopup.popup(event->button, event->time);
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

void AtomicModelBox::DynamicTreeView::onClickColumn(int numColumn)
{
    if (get_column(numColumn - 1)->get_sort_order() == Gtk::SORT_ASCENDING) {
        mRefTreeModelDyn->set_sort_column(numColumn - 1, Gtk::SORT_DESCENDING);
	get_column(numColumn - 1)->set_sort_order(Gtk::SORT_DESCENDING);
    } else {
        mRefTreeModelDyn->set_sort_column_id(numColumn - 1,
                                             Gtk::SORT_ASCENDING);
	get_column(numColumn - 1)->set_sort_order(Gtk::SORT_ASCENDING);
    }

    if (numColumn == 1) {
	get_column(mColumnPackage - 1)->set_sort_order(Gtk::SORT_DESCENDING);
	get_column(mColumnDyn - 1)->set_sort_order(Gtk::SORT_DESCENDING);
	get_column(mColumnModel - 1)->set_sort_order(Gtk::SORT_DESCENDING);
    } else if (numColumn == 2) {
	get_column(mColumnName - 1)->set_sort_order(Gtk::SORT_DESCENDING);
	get_column(mColumnDyn - 1)->set_sort_order(Gtk::SORT_DESCENDING);
	get_column(mColumnModel - 1)->set_sort_order(Gtk::SORT_DESCENDING);
    } else if (numColumn == 3) {
	get_column(mColumnName - 1)->set_sort_order(Gtk::SORT_DESCENDING);
	get_column(mColumnPackage - 1)->set_sort_order(Gtk::SORT_DESCENDING);
	get_column(mColumnModel - 1)->set_sort_order(Gtk::SORT_DESCENDING);
    } else {
	get_column(mColumnName - 1)->set_sort_order(Gtk::SORT_DESCENDING);
	get_column(mColumnPackage - 1)->set_sort_order(Gtk::SORT_DESCENDING);
	get_column(mColumnDyn - 1)->set_sort_order(Gtk::SORT_DESCENDING);
    }
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
		  utils::Path::path().getPackageSrcDir(), (*it));
	    }
	}
    }
    return "";
}

void AtomicModelBox::DynamicTreeView::onRowActivated(
    const Gtk::TreeModel::Path& path,
    Gtk::TreeViewColumn* column)
{
    if (column
	and not vle::utils::Path::path().getPackageDir().empty()
        and utils::Path::exist(vle::utils::Path::path().getPackageSrcDir())) {
	Gtk::TreeRow row = (*mRefTreeModelDyn->get_iter(path));
	vpz::Dynamic& dynamic = mDynamics->get(
	    row.get_value(mColumnsDyn.m_col_name));

	std::string searchFile;
	if (not dynamic.model().empty()) {
            searchFile = dynamic.model();
        } else {
            searchFile = row.get_value(mColumnsDyn.m_dyn);
        }
	std::transform(searchFile.begin(), searchFile.end(),
		       searchFile.begin(), tolower);

	std::string newTab = pathFileSearch(
	    utils::Path::path().getPackageSrcDir(), searchFile);
        if (not newTab.empty()) {
            try {
                std::string pluginname, conf;
                utils::Template tpl;
                tpl.open(newTab);
                tpl.tag(pluginname, conf);
                PluginFactory& plf = mModeling->getGVLE()->pluginFactory();
                ModelingPlugin& plugin = plf.getModeling(pluginname);

                if (plugin.modify(*mAtom, *mModel, dynamic, *mConditions,
                                  *mObservables, conf, tpl.buffer())) {
                    const std::string& buffer = plugin.source();
                    std::string filename = utils::Path::path()
                        .getPackageSrcFile(dynamic.library());
                    filename += ".cpp";

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
                mModeling->getGVLE()->getEditor()->openTab(newTab);
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
	box.hide_all();
        name = boost::trim_copy(name);
        if (mDynamics->exist(name)) {
            Error((fmt(_("The Dynamics '%1%' already exists")) % name).str());
        } else {
            vpz::Dynamic dyn(name);
            DynamicBox mDynamicBox(mXml, mModeling->getGVLE(),
                                   *mAtom, *mModel, dyn, *mConditions,
                                   *mObservables);
            mDynamicBox.show(&dyn);
            if (mDynamicBox.valid()) {
                mDynamics->add(dyn);
                mParent->refresh();

		const Gtk::TreeModel::Children& child(mRefTreeModelDyn->children());
                Gtk::TreeModel::Children::const_iterator it = child.begin();
                while (it != child.end()) {
                    if (it->get_value(mColumnsDyn.m_col_name) == dyn.name()) {
			Gtk::TreeModel::Path path = mRefTreeModelDyn->get_path(it);
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
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection	= get_selection();
    Gtk::TreeModel::iterator it = refSelection->get_selected();
    Glib::ustring dyn = (*it)[mColumnsDyn.m_col_name];

    if (not dyn.empty()) {
        vpz::Dynamic& dynamic(mDynamics->get(dyn));
        DynamicBox mDynamicBox(mXml,
                               mModeling->getGVLE(),
                               *mAtom, *mModel, dynamic,
                               *mConditions, *mObservables);
        mDynamicBox.show(&dynamic);
        build();

	const Gtk::TreeModel::Children& child(mRefTreeModelDyn->children());
	Gtk::TreeModel::Children::const_iterator it = child.begin();
	while (it != child.end()) {
	    if (it->get_value(mColumnsDyn.m_col_name) == dyn) {
		Gtk::TreeModel::Path path = mRefTreeModelDyn->get_path(it);
		set_cursor(path);
		mLabel->set_label(_("Selected Dynamic: ") + mModel->dynamics());
		break;
	    }
	    ++it;
	}
    }
}

void AtomicModelBox::DynamicTreeView::onRemove()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection	= get_selection();
    Gtk::TreeModel::iterator it = refSelection->get_selected();
    Glib::ustring dyn = (*it)[mColumnsDyn.m_col_name];

    if (not dyn.empty()) {
        mDynamics->del(dyn);
	build();
    }
}

void AtomicModelBox::DynamicTreeView::onRename()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Gtk::TreeModel::iterator it = refSelection->get_selected();
    Glib::ustring oldName = (*it)[mColumnsDyn.m_col_name];

    if (not oldName.empty()) {
	SimpleTypeBox box(_("Name of the Dynamic ?"), oldName);

	std::string newName = box.run();
	if (box.valid() and not newName.empty()) {
	    box.hide_all();
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
		newDynamic->setModel(oldDynamic.model());
		newDynamic->setLanguage(oldDynamic.language());
		if (oldDynamic.location().empty()) {
		    newDynamic->setLocalDynamics();
		} else {
		    std::vector<std::string> vec;
		    boost::split(vec, oldDynamic.location(),
				 boost::is_any_of(":"));
		    std::string location =
			oldDynamic.location().substr(
			    0, oldDynamic.location().size()
			    - vec[vec.size() - 1].size() - 1);
		    int port = boost::lexical_cast<int>(vec[vec.size() - 1]);

		    newDynamic->setDistantDynamics(location, port);
		}
		mDynamics->add(*newDynamic);
		mDynamics->del(oldName);
		mModel->setDynamics(newName);
		mRenameList.push_back(std::make_pair(oldName, newName));
		build();
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
        const Glib::ustring& pathString,
        const Glib::ustring& newName)
{
    Gtk::TreePath path(pathString);

    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    if (refSelection and newName != mOldName) {
	Gtk::TreeModel::iterator it = refSelection->get_selected();
	if (not newName.empty() and not mDynamics->exist(newName)) {
	    vpz::Dynamic* newDynamic = new vpz::Dynamic(newName);
	    vpz::Dynamic oldDynamic = mDynamics->get(mOldName);
	    newDynamic->setLibrary(oldDynamic.library());
	    newDynamic->setModel(oldDynamic.model());
	    newDynamic->setLanguage(oldDynamic.language());
	    if (oldDynamic.location().empty()) {
		newDynamic->setLocalDynamics();
	    } else {
		std::vector<std::string> vec;
		boost::split(vec, oldDynamic.location(),
			     boost::is_any_of(":"));
		std::string location =
		    oldDynamic.location().substr(
			0, oldDynamic.location().size()
			- vec[vec.size() - 1].size() - 1);
		int port = boost::lexical_cast<int>(vec[vec.size() - 1]);

		newDynamic->setDistantDynamics(location, port);

	    }
	    mDynamics->add(*newDynamic);
	    mDynamics->del(mOldName);
	    mModel->setDynamics(newName);
	    mRenameList.push_back(std::make_pair(mOldName, newName));
	    build();
	}
    }
}

std::string AtomicModelBox::DynamicTreeView::getDynamic()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Gtk::TreeModel::iterator iter = refSelection->get_selected();
    Gtk::TreeModel::Row row = *iter;
    std::string dynamicname = row.get_value(mColumnsDyn.m_col_name);
    return dynamicname;
}

//ObservableTreeView

AtomicModelBox::ObservableTreeView::ObservableTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr<Gnome::Glade::Xml>& xml /*refGlade*/) :
    Gtk::TreeView(cobject),
    mObsAndViewBox(xml)
{
    mRefTreeModelObs = Gtk::ListStore::create(mColumnsObs);
    set_model(mRefTreeModelObs);
    mColumnName = append_column_editable(_("Name"),
					 mColumnsObs.m_col_name);
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


    //Fill popup menu:
    {
	Gtk::Menu::MenuList& menulist = mMenuPopup.items();

	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Add"),
		sigc::mem_fun(
		    *this,
		    &AtomicModelBox::ObservableTreeView::onAdd)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Edit"),
		sigc::mem_fun(
		    *this,
		    &AtomicModelBox::ObservableTreeView::onEdit)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Remove"),
		sigc::mem_fun(
		    *this,
		    &AtomicModelBox::ObservableTreeView::onRemove)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Rename"),
		sigc::mem_fun(
		    *this,
		    &AtomicModelBox::ObservableTreeView::onRename)));
    }
    mMenuPopup.accelerate(*this);
}

AtomicModelBox::ObservableTreeView::~ObservableTreeView()
{
}

void AtomicModelBox::ObservableTreeView::applyRenaming()
{
    renameList::const_iterator it = mRenameList.begin();

    while (it != mRenameList.end()) {
	vpz::AtomicModelList& atomlist(
	    mModeling->vpz().project().model().atomicModels());
	atomlist.updateObservable(it->first, it->second);
	++it;
    }
    mRenameList.clear();
}

void AtomicModelBox::ObservableTreeView::build()
{
    mRefTreeModelObs->clear();

    vpz::Observables::const_iterator it = mObservables->begin();
    Gtk::TreeModel::Row row = *(mRefTreeModelObs->append());

    row[mColumnsObs.m_col_name] = "";
    while (it != mObservables->end()) {
        Gtk::TreeModel::Row row = *(mRefTreeModelObs->append());

        row[mColumnsObs.m_col_name] = it->first;
        ++it;
    }

    const Gtk::TreeModel::Children& child = mRefTreeModelObs->children();
    Gtk::TreeModel::Children::const_iterator it_child = child.begin();
    while (it_child != child.end()) {
        if (it_child->get_value(mColumnsObs.m_col_name) ==
	    mModel->observables()) {
	    Gtk::TreeModel::Path path = mRefTreeModelObs->get_path(it_child);
	    set_cursor(path);
	    mLabel->set_label(_("Selected Observable: ") +
			      mModel->observables());
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
	mMenuPopup.popup(event->button, event->time);
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
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection	= get_selection();
    Gtk::TreeModel::iterator it = refSelection->get_selected();
    Glib::ustring obs = (*it)[mColumnsObs.m_col_name];

    if (not obs.empty()) {
        mObsAndViewBox.show(*mObservables, obs, mModeling->views());
    }
}

void AtomicModelBox::ObservableTreeView::onRemove()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection	= get_selection();
    Gtk::TreeModel::iterator it = refSelection->get_selected();
    Glib::ustring obs = (*it)[mColumnsObs.m_col_name];

    if (not obs.empty()) {
        mObservables->del(obs);
	build();
    }
}

void AtomicModelBox::ObservableTreeView::onRename()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection	= get_selection();
    Gtk::TreeModel::iterator it = refSelection->get_selected();
    Glib::ustring oldName = (*it)[mColumnsObs.m_col_name];

    if (not oldName.empty()) {
	InteractiveTypeBox box(_("New name for this observable ?"),
			       &(mModeling->observables()),
			       oldName);
	std::string newName = box.run();
	if (box.valid() and not newName.empty()) {
	    box.hide_all();
	    newName = boost::trim_copy(newName);
	    mObservables->rename(oldName, newName);
	    mModel->setObservables(newName);
	    mRenameList.push_back(std::make_pair(oldName, newName));
	    build();
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
    const Glib::ustring& pathString,
    const Glib::ustring& newName)
{
    Gtk::TreePath path(pathString);
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Gtk::TreeModel::iterator it = refSelection->get_selected();

    if (it) {
	if (not newName.empty() and (not mObservables->exist(newName))) {
	    mObservables->rename(mOldName, newName);
	    mRenameList.push_back(std::make_pair(mOldName, newName));
	}
    }
    build();
}

std::string AtomicModelBox::ObservableTreeView::getObservable()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Gtk::TreeModel::iterator iter = refSelection->get_selected();
    Gtk::TreeModel::Row row = *iter;
    std::string observablename = row.get_value(mColumnsObs.m_col_name);
    return observablename;
}

// AtomicModelBox

AtomicModelBox::AtomicModelBox(Glib::RefPtr<Gnome::Glade::Xml> xml,
			       Modeling* m):
    mXml(xml),
    mModeling(m),
    mModel(0),
    mGraphModel(0),
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
    mDialog->hide_all();
}

void AtomicModelBox::show(graph::AtomicModel* model,
			  const std::string& className)
{
    const Modeling& modeling = (*(const Modeling*)mModeling);
    mDialog->set_title("Atomic Model " + model->getName());

    mCurrentModel = &mModeling->get_model(model, className);
    mCurrentGraphModel = model;

    mModel = new vpz::AtomicModel(*mCurrentModel);
    mGraphModel = new graph::AtomicModel(*model);
    mCond = new vpz::Conditions(modeling.conditions());
    mDyn = new vpz::Dynamics(modeling.dynamics());
    mObs = new vpz::Observables(modeling.observables());

    mInputPorts->setModel(mGraphModel);
    mInputPorts->clearRenaming();
    mInputPorts->build();

    mOutputPorts->setModel(mGraphModel);
    mOutputPorts->clearRenaming();
    mOutputPorts->build();

    mConditions->setModel(mModel);
    mConditions->setModeling(mModeling);
    mConditions->setLabel(mLabelConditions);
    mConditions->setConditions(mCond);
    mConditions->clearRenaming();
    mConditions->build();

    mDynamics->setModel(mModel);
    mDynamics->setGraphModel(mGraphModel);
    mDynamics->setModeling(mModeling);
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
}

void AtomicModelBox::applyPorts()
{
    mInputPorts->applyRenaming(mCurrentGraphModel);
    mOutputPorts->applyRenaming(mCurrentGraphModel);

    // Apply input ports
    {
	graph::ConnectionList connec_in_list =
	    mCurrentGraphModel->getInputPortList();
	graph::ConnectionList::iterator it = connec_in_list.begin();

	while (it != connec_in_list.end()) {
	    if (not mGraphModel->existInputPort(it->first)) {
		mCurrentGraphModel->delInputPort(it->first);
	    }
	    ++it;
	}
    }
    {
     	graph::ConnectionList connec_in_list =
     	    mGraphModel->getInputPortList();
     	graph::ConnectionList::iterator it = connec_in_list.begin();

     	while (it != connec_in_list.end()) {
	    if (not mCurrentGraphModel->existInputPort(it->first)) {
		mCurrentGraphModel->addInputPort(it->first);
	    }
     	    ++it;
     	}
     }

    // Apply output ports
    {
	graph::ConnectionList connec_out_list =
	    mCurrentGraphModel->getOutputPortList();
	graph::ConnectionList::iterator it = connec_out_list.begin();

	while (it != connec_out_list.end()) {
	    if (not mGraphModel->existOutputPort(it->first)) {
		mCurrentGraphModel->delOutputPort(it->first);
	    }
	    ++it;
	}
    }
    {
     	graph::ConnectionList& connec_out_list =
     	    mGraphModel->getOutputPortList();
     	graph::ConnectionList::iterator it = connec_out_list.begin();

     	while (it != connec_out_list.end()) {
	    if (not mCurrentGraphModel->existOutputPort(it->first)) {
		mCurrentGraphModel->addOutputPort(it->first);
	    }
     	    ++it;
     	}
    }

    // Apply height
    {
        mCurrentGraphModel->setHeight(
            ViewDrawingArea::MODEL_HEIGHT +
            std::max(mCurrentGraphModel->getInputPortNumber(),
                     mCurrentGraphModel->getOutputPortNumber()) *
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

    {
	vpz::Conditions& conditions = mModeling->conditions();
	vpz::AtomicModelList& list =
	    mModeling->vpz().project().model().atomicModels();
	vpz::AtomicModelList::iterator it = list.begin();

	while (it != list.end()) {
	    vpz::Strings mdlConditions = it->second.conditions();
	    vpz::Strings::const_iterator its = mdlConditions.begin();

	    while (its != mdlConditions.end()) {
		if (not conditions.exist(*its)) {
		    it->second.delCondition(*its);
		}
		++its;
	    }
	    ++it;
	}
    }

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

    {
	vpz::AtomicModelList& list =
	    mModeling->vpz().project().model().atomicModels();
	vpz::AtomicModelList::iterator it = list.begin();

	while (it != list.end()) {
	    if (not mModeling->dynamics().exist(it->second.dynamics())) {
		it->second.setDynamics("");
	    }
	    ++it;
	}
    }

    {
	Glib::RefPtr<Gtk::TreeView::Selection> refSelection =
	    mDynamics->get_selection();
	if (refSelection->get_selected()) {
	    mCurrentModel->setDynamics(mDynamics->getDynamic());
	}
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

    {
	vpz::AtomicModelList& list =
	    mModeling->vpz().project().model().atomicModels();
	vpz::AtomicModelList::iterator it = list.begin();

	while (it != list.end()) {
	    if (not mModeling->observables().exist(it->second.observables())) {
		it->second.setObservables("");
	    }
	    ++it;
	}
    }

    {
	Glib::RefPtr<Gtk::TreeView::Selection> refSelection =
	    mObservables->get_selection();

	if (refSelection->get_selected()) {
	    mCurrentModel->setObservables(mObservables->getObservable());
	}
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
    delete mGraphModel;
    mGraphModel = 0;

    mDialog->hide_all();
}

void AtomicModelBox::on_cancel()
{
    delete mModel;
    mModel = 0;
    delete mGraphModel;
    mGraphModel = 0;

    delete mCond;
    mCond = 0;
    delete mDyn;
    mDyn = 0;
    delete mObs;
    mObs = 0;

    mDialog->hide_all();
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
