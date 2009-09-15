/**
 * @file vle/gvle/AtomicModelBox.cpp
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

#include <vle/gvle/AtomicModelBox.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/ObservableBox.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/gvle/InteractiveTypeBox.hpp>
#include <vle/utils/Path.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>

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
    SimpleTypeBox box(_("Name of the Input port ?"));
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
    SimpleTypeBox box(_("New name of the input port ?"));
    graph::ConnectionList& list = mModel->getInputPortList();
    Glib::ustring new_name = boost::trim_copy(box.run());

    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();

    if (refSelection and box.valid() and not new_name.empty()
	and list.find(new_name) == list.end()) {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
            Gtk::TreeModel::Row row = *iter;
	    std::string old_name = row.get_value(mColumnsInputPort.m_col_name);
	    row.set_value(mColumnsInputPort.m_col_name, new_name);

	    if (mModel->existInputPort(old_name)) {
	        mModel->renameInputPort(old_name, new_name);
	    }
	    build();
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
	    }
	    mValidateRetry = true;
	}
    }
    build();
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
    SimpleTypeBox box(_("Name of the output port ?"));
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
    SimpleTypeBox box(_("New name of the output port ?"));
    graph::ConnectionList& list = mModel->getOutputPortList();
    Glib::ustring new_name = boost::trim_copy(box.run());

    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();

    if (refSelection and box.valid() and not new_name.empty()
	and list.find(new_name) == list.end()) {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
            Gtk::TreeModel::Row row = *iter;
	    std::string old_name = row.get_value(mColumnsOutputPort.m_col_name);
	    row.set_value(mColumnsOutputPort.m_col_name, new_name);

	    if (mModel->existOutputPort(old_name)) {
	        mModel->renameOutputPort(old_name, new_name);
	    }
	    build();
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
	    }
	    mValidateRetry = true;
	}
    }
    build();
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

void AtomicModelBox::ConditionTreeView::build()
{
    mRefTreeModel->clear();

    const vpz::Strings& cond = mModel->conditions();
    vpz::Strings::const_iterator f;
    vpz::ConditionList list = mConditions->conditionlist();
    vpz::ConditionList::iterator it = list.begin();
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

	if (row.get_value(mColumns.m_col_activ) == true) {
	    vec.push_back(row.get_value(mColumns.m_col_name));
	}
    }
    return vec;
}

void AtomicModelBox::ConditionTreeView::on_row_activated(
    const Gtk::TreeModel::Path&, Gtk::TreeViewColumn* )
{
    mModeling->getGVLE()->getConditionsBox()->show();
    build();
}

bool AtomicModelBox::ConditionTreeView::on_button_press_event(
    GdkEventButton* event)
{
    bool return_value = TreeView::on_button_press_event(event);
    if (event->type == GDK_BUTTON_PRESS and event->button == 3) {
	mMenuPopup.popup(event->button, event->time);
    }
    if (event->type == GDK_2BUTTON_PRESS && event->button == 1) {
	mModeling->getGVLE()->getConditionsBox()->show();
    }
    return return_value;
}

void AtomicModelBox::ConditionTreeView::onRename()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Gtk::TreeModel::iterator it = refSelection->get_selected();

    if (it) {
	Glib::ustring oldname = (*it)[mColumns.m_col_name];
	SimpleTypeBox box(_("New name of the condition ?"));
        std::string newname = boost::trim_copy(box.run());
	vpz::ConditionList& list = mConditions->conditionlist();

	if (box.valid() and not newname.empty()
	   and list.find(newname) == list.end()) {
	    mConditions->rename(oldname, newname);

	    vpz::AtomicModelList& atomlist(
		mModeling->vpz().project().model().atomicModels());
	    atomlist.updateCondition(oldname, newname);
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
	vpz::ConditionList& list = mConditions->conditionlist();

	if (not newName.empty() and list.find(newName) == list.end()) {
	    mConditions->rename(mOldName, newName);

	    vpz::AtomicModelList& atomlist(
		mModeling->vpz().project().model().atomicModels());
	    atomlist.updateCondition(mOldName, newName);
	}
    }

    build();
}

//DynamicTreeView

AtomicModelBox::DynamicTreeView::DynamicTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr<Gnome::Glade::Xml>& xml /*refGlade*/) :
    Gtk::TreeView(cobject),
    mDynamicBox(xml)
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

void AtomicModelBox::DynamicTreeView::build()
{
    assert(mModeling);
    using namespace vpz;

    mRefListDyn->clear();

    const DynamicList& list = mModeling->dynamics().dynamiclist();
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

bool AtomicModelBox::DynamicTreeView::on_button_press_event(GdkEventButton *event)
{
  //Call base class, to allow normal handling,
  //such as allowing the row to be selected by the right-click:
  bool return_value = TreeView::on_button_press_event(event);

  //Then do our custom stuff:
  if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) )
  {
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
	    if (boost::filesystem::basename(name) == filename
		and boost::filesystem::extension(name) == ".cpp") {
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
	and boost::filesystem::exists(
	    vle::utils::Path::path().getPackageSrcDir())) {
	Gtk::TreeRow row = (*mRefTreeModelDyn->get_iter(path));
	vpz::Dynamic& dynamic = mModeling->dynamics().get(
	    row.get_value(mColumnsDyn.m_col_name));

	std::string searchFile;
	if (dynamic.model() != "")
	    searchFile = dynamic.model();
	else
	    searchFile = row.get_value(mColumnsDyn.m_dyn);
	std::transform(searchFile.begin(), searchFile.end(),
		       searchFile.begin(), tolower);

	std::string newTab = pathFileSearch(
	    utils::Path::path().getPackageSrcDir(), searchFile);
	if (not newTab.empty()) {
	    mParent->on_apply();
	    mModeling->getGVLE()->
		getEditor()->openTab(newTab);
	} else {
	    std::string message = _("The source file ")
		+ searchFile
		+ _(".cpp does not exist");
	    Error(message);
	}
    }
}

void AtomicModelBox::DynamicTreeView::onAdd()
{
    SimpleTypeBox box(_("Name of the Dynamic ?"));

    std::string name = box.run();
    if (box.valid()) {
	box.hide_all();
        name = boost::trim_copy(name);
        if (mModeling->dynamics().exist(name)) {
            Error(boost::str(fmt(
                        _("The Dynamics '%1%' already exists")) % name));
        } else {
            vpz::Dynamic* dyn = new vpz::Dynamic(name);
            mDynamicBox.show(dyn);
            if (mDynamicBox.valid()) {
                mModeling->dynamics().add(*dyn);
                build();

		const Gtk::TreeModel::Children& child(mRefTreeModelDyn->children());
                Gtk::TreeModel::Children::const_iterator it = child.begin();
                while (it != child.end()) {
                    if (it->get_value(mColumnsDyn.m_col_name) == dyn->name()) {
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

    if (!dyn.empty()) {
        mDynamicBox.show(&(mModeling->dynamics().get(dyn)));
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

    if (dyn != "") {
        mModeling->dynamics().del(dyn);
	build();
    }
}

void AtomicModelBox::DynamicTreeView::onRename()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Gtk::TreeModel::iterator it = refSelection->get_selected();
    Glib::ustring oldName = (*it)[mColumnsDyn.m_col_name];

    if (oldName != "") {
	SimpleTypeBox box(_("Name of the Dynamic ?"));

	std::string newName = box.run();
	if (box.valid() and not newName.empty()) {
	    box.hide_all();
	    newName = boost::trim_copy(newName);
	    if (mModeling->dynamics().exist(newName)) {
		Error(
		    boost::str(
			fmt(_("The Dynamics '%1%' already exists")) % newName));
	    } else {
		vpz::Dynamic* newDynamic = new vpz::Dynamic(newName);
		vpz::Dynamic oldDynamic = mModeling->dynamics().get(oldName);
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
		mModeling->dynamics().add(*newDynamic);
		mModeling->dynamics().del(oldName);
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
	if (not newName.empty() and not mModeling->dynamics().exist(newName)) {
	    vpz::Dynamic* newDynamic = new vpz::Dynamic(newName);
	    vpz::Dynamic oldDynamic = mModeling->dynamics().get(mOldName);
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
	    mModeling->dynamics().add(*newDynamic);
	    mModeling->dynamics().del(mOldName);
	}
    }
    build();

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

void AtomicModelBox::ObservableTreeView::build()
{
    assert(mModeling);
    using namespace vpz;

    mRefTreeModelObs->clear();

    Observables observables = mModeling->observables();
    Observables::const_iterator it = observables.begin();

    Gtk::TreeModel::Row row = *(mRefTreeModelObs->append());
    row[mColumnsObs.m_col_name] = "";

    while (it != observables.end()) {
        Gtk::TreeModel::Row row = *(mRefTreeModelObs->append());
        row[mColumnsObs.m_col_name] = it->first;

        ++it;
    }

    const Gtk::TreeModel::Children& child = mRefTreeModelObs->children();
    Gtk::TreeModel::Children::const_iterator it_child = child.begin();
    while (it_child != child.end()) {
        if (it_child->get_value(mColumnsObs.m_col_name) == mModel->observables()) {
	    Gtk::TreeModel::Path path = mRefTreeModelObs->get_path(it_child);
	    set_cursor(path);
	    mLabel->set_label(_("Selected Observable: ") + mModel->observables());
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
  if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) )
  {
      mMenuPopup.popup(event->button, event->time);
  }

  return return_value;
}

void AtomicModelBox::ObservableTreeView::onAdd()
{
    SimpleTypeBox box(_("Name of the Observable ?"));

    std::string name = box.run();
    if (box.valid()) {
        name = boost::trim_copy(name);
        if (mModeling->observables().exist(name)) {
            Error(boost::str(fmt(
                        _("The observable '%1%' already exists")) % name));
        } else {
            mModeling->observables().add(vpz::Observable(name));
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

    if (obs != "") {
        mObsAndViewBox.show(mModeling->observables(),
			    std::string(obs),
			    mModeling->views());
    }
}

void AtomicModelBox::ObservableTreeView::onRemove()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection	= get_selection();
    Gtk::TreeModel::iterator it = refSelection->get_selected();
    Glib::ustring obs = (*it)[mColumnsObs.m_col_name];

    if (obs != "") {
        mModeling->observables().del(obs);
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

	    mModeling->observables().rename(oldName, newName);
	    vpz::AtomicModelList& atomlist(
		mModeling->vpz().project().model().atomicModels());
	    atomlist.updateObservable(oldName, newName);
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
	vpz::Observables& listobs = mModeling->observables();

	if (not newName.empty() and (not listobs.exist(newName))) {
	    listobs.rename(mOldName, newName);

	    vpz::AtomicModelList& atomlist(
		mModeling->vpz().project().model().atomicModels());
	    atomlist.updateObservable(mOldName, newName);
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

AtomicModelBox::AtomicModelBox(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* m):
    mXml(xml),
    mModeling(m),
    mAtom(0),
    mDyn(0),
    mObs(0),
    mViews(0),
    mCond(0),
    mAtom_backup(0),
    mDyn_backup(0),
    mObs_backup(0),
    mViews_backup(0),
    mCond_backup(0),
    mOutputs_backup(0),
    mConnection_in_backup(0),
    mConnection_out_backup(0)
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
    delete mAtom_backup;
    delete mDyn_backup;
    delete mObs_backup;
    delete mViews_backup;
    delete mCond_backup;
}

void AtomicModelBox::show(vpz::AtomicModel& atom,  graph::AtomicModel& model)
{
    mDialog->set_title("Atomic Model " + model.getName());

    //Data
    mAtom = &atom;
    mGraph_atom = &model;
    mDyn = &mModeling->dynamics();
    mObs = &mModeling->observables();
    mViews = &mModeling->views();
    mCond = &mModeling->conditions();
    mOutputs = &mModeling->outputs();

    //Backup
    delete mAtom_backup;
    mAtom_backup = new vpz::AtomicModel(atom);

    delete mObs_backup;
    mObs_backup = new vpz::Observables(*mObs);

    delete mDyn_backup;
    mDyn_backup = new vpz::Dynamics(*mDyn);

    delete mCond_backup;
    mCond_backup = new vpz::Conditions(*mCond);

    delete mViews_backup;
    mViews_backup = new vpz::Views(*mViews);

    delete mOutputs_backup;
    mOutputs_backup = new vpz::Outputs(*mOutputs);

    delete mConnection_in_backup;
    mConnection_in_backup = new graph::ConnectionList(model.getInputPortList());

    delete mConnection_out_backup;
    mConnection_out_backup = new graph::ConnectionList(model.getOutputPortList());

    mInputPorts->setModel(&model);
    mInputPorts->build();

    mOutputPorts->setModel(&model);
    mOutputPorts->build();

    mConditions->setModel(&atom);
    mConditions->setConditions(&mModeling->conditions());
    mConditions->setModeling(mModeling);
    mConditions->setLabel(mLabelConditions);
    mConditions->build();

    mDynamics->setModel(&atom);
    mDynamics->setModeling(mModeling);
    mDynamics->setLabel(mLabelDynamics);
    mDynamics->setParent(this);
    mDynamics->build();

    mObservables->setModel(&atom);
    mObservables->setModeling(mModeling);
    mObservables->setLabel(mLabelObservables);
    mObservables->build();

    mDialog->show_all();
    mDialog->run();
}

void AtomicModelBox::on_apply()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection
	= mDynamics->get_selection();
    if (not refSelection->get_selected()) {
        Error("You Have to assign a dynamic.");
    } else {
	refSelection = mObservables->get_selection();
	if (refSelection->get_selected()) {
	    mAtom->setObservables(mObservables->getObservable());
	}
	mAtom->setDynamics(mDynamics->getDynamic());
	mAtom->setConditions(mConditions->getConditions());

	mDialog->hide_all();
    }
}

void AtomicModelBox::on_cancel()
{
    using namespace vpz;

    mAtom->setConditions(mAtom_backup->conditions());
    mAtom->setDynamics(mAtom_backup->dynamics());
    mAtom->setObservables(mAtom_backup->observables());

    //Dynamics
    mDyn->clear();
    const DynamicList& list_dyn = mDyn_backup->dynamiclist();
    DynamicList::const_iterator it_dyn = list_dyn.begin();
    while (it_dyn != list_dyn.end()) {
        mDyn->add(it_dyn->second);
        ++it_dyn;
    }

    //Conditions
    mCond->clear();
    const ConditionList& list_cond = mCond_backup->conditionlist();
    ConditionList::const_iterator it_cond = list_cond.begin();
    while (it_cond != list_cond.end()) {
        mCond->add(it_cond->second);
        ++it_cond;
    }

    //Views
    mViews->clear();
    const ViewList& list_view = mViews_backup->viewlist();
    ViewList::const_iterator it_view = list_view.begin();
    while (it_view != list_view.end()) {
        mViews->add(it_view->second);
        ++it_view;
    }

    //Outputs
    mOutputs->clear();
    const OutputList& list_output = mOutputs_backup->outputlist();
    OutputList::const_iterator it_out = list_output.begin();
    while (it_out  != list_output.end()) {
        mOutputs->add(it_out->second);

        ++it_out;
    }

    //Observable
    mObs->clear();
    const ObservableList& list_obs = mObs_backup->observablelist();
    ObservableList::const_iterator it_obs = list_obs.begin();
    while (it_obs != list_obs.end()) {
        mObs->add(it_obs->second);
        ++it_obs;
    }

    //Ports
    graph::CoupledModel* parent = mGraph_atom->getParent();

    graph::ConnectionList& connec_in_list = mGraph_atom->getInputPortList();
    {
        graph::ConnectionList::iterator it = connec_in_list.begin();
        while (it != connec_in_list.end()) {
            mGraph_atom->delInputPort(it->first);
            it = connec_in_list.begin();
        }
    }

    for (graph::ConnectionList::iterator it_in_connec
         =mConnection_in_backup->begin(); it_in_connec !=
         mConnection_in_backup->end(); ++it_in_connec) {
        mGraph_atom->addInputPort(it_in_connec->first);
    }

    graph::ConnectionList& connec_out_list = mGraph_atom->getOutputPortList();
    {
        graph::ConnectionList::iterator it = connec_out_list.begin();
        while (it != connec_out_list.end()) {
            mGraph_atom->delOutputPort(it->first);
            it = connec_out_list.begin();
        }
    }
    for (graph::ConnectionList::iterator it_out_connec =
         mConnection_out_backup->begin(); it_out_connec !=
         mConnection_out_backup->end(); ++it_out_connec) {
        mGraph_atom->addOutputPort(it_out_connec->first);
    }

    //Input Ports
    for (graph::ConnectionList::iterator it_in_connec = mConnection_in_backup->begin();
            it_in_connec != mConnection_in_backup->end();
            ++it_in_connec) {
        graph::ModelPortList& in_port_list = it_in_connec->second;
        graph::ModelPortList::iterator it_port_list = in_port_list.begin();
        while (it_port_list != in_port_list.end()) {
            if (it_port_list->first == parent) {
                parent->addInputConnection(it_port_list->second, mGraph_atom,
					   it_in_connec->first);
            } else {
		parent->addInternalConnection(it_port_list->first,
					      it_port_list->second,
					      mGraph_atom,
					      it_in_connec->first);
            }

            ++it_port_list;
        }
    }

    //Output Ports
    for (graph::ConnectionList::iterator it_out_connec = mConnection_out_backup->begin();
            it_out_connec != mConnection_out_backup->end();
            ++it_out_connec) {
        graph::ModelPortList& out_port_list = it_out_connec->second;
        graph::ModelPortList::iterator it_port_list = out_port_list.begin();
        while (it_port_list != out_port_list.end()) {
            if (it_port_list->first == parent) {
                parent->addOutputConnection(mGraph_atom, it_out_connec->first,
					    it_port_list->second);
            } else {
		if(not parent->existInternalConnection(mGraph_atom->getName(),
						       it_out_connec->first,
						       it_port_list->first->getName(),
						       it_port_list->second)) {
		    parent->addInternalConnection(mGraph_atom, it_out_connec->first,
						  it_port_list->first,
						  it_port_list->second);
		}
            }

            ++it_port_list;
        }
    }

    mDialog->hide_all();
    mGraph_atom = 0;
}

} } // namespace vle gvle
