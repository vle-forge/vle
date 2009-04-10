/**
 * @file vle/gvle/ConditionsBox.cpp
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


#include <vle/gvle/ConditionsBox.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/vpz/AtomicModels.hpp>

namespace vle { namespace gvle {

////
//  ConditionsTreeView
///

ConditionsBox::ConditionsTreeView::ConditionsTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/) :
    Gtk::TreeView(cobject),
    mConditions(0)
{
    mRefTreeModel = Gtk::ListStore::create(mColumns);
    set_model(mRefTreeModel);
    append_column("Initial conditions", mColumns.m_col_name);
    mRefTreeSelection = get_selection();
    mRefTreeSelection->signal_changed().connect(
        sigc::mem_fun(*this,
                      &ConditionsBox::ConditionsTreeView::on_select));

    //Fill popup menu:
    {
	Gtk::Menu::MenuList& menulist = mMenuPopup.items();

	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		"_Add",
		sigc::mem_fun(
		    *this,
		    &ConditionsBox::ConditionsTreeView::on_add)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		"_Remove",
		sigc::mem_fun(
		    *this,
		    &ConditionsBox::ConditionsTreeView::on_remove)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		"_Rename",
		sigc::mem_fun(
		    *this,
		    &ConditionsBox::ConditionsTreeView::onRename)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		"_Copy",
		sigc::mem_fun(
		    *this,
		    &ConditionsBox::ConditionsTreeView::onCopy)));
    }
    mMenuPopup.accelerate(*this);
}

ConditionsBox::ConditionsTreeView::~ConditionsTreeView()
{
}

bool ConditionsBox::ConditionsTreeView::on_button_press_event(
    GdkEventButton* event)
{
    //Call base class, to allow normal handling,
    //such as allowing the row to be selected by the right-click:
    bool return_value = TreeView::on_button_press_event(event);

    //Then do our custom stuff:
    if (mConditions and event->type == GDK_BUTTON_PRESS and event->button == 3)
    {
	mMenuPopup.popup(event->button, event->time);
    }

    return return_value;
}

void ConditionsBox::ConditionsTreeView::build()
{
    assert(mConditions);

    mRefTreeModel->clear();

    vpz::ConditionList& conditionList = mConditions->conditionlist();
    vpz::ConditionList::const_iterator it = conditionList.begin();

    for(it = conditionList.begin(); it != conditionList.end(); ++it) {
	Gtk::TreeModel::Row row = *(mRefTreeModel->append());

	row[mColumns.m_col_name] = it->first;
    }
}

void ConditionsBox::ConditionsTreeView::on_select()
{
    Gtk::TreeModel::iterator it = mRefTreeSelection->get_selected();

    if (it) {
        Glib::ustring name = (*it)[mColumns.m_col_name];

	mParent->buildTreePorts(name);
    }
}

void ConditionsBox::ConditionsTreeView::on_add()
{
    SimpleTypeBox box("Name of the condition ?");
    vpz::ConditionList& list = mConditions->conditionlist();
    std::string name = boost::trim_copy(box.run());

    if (box.valid() and not name.empty() and list.find(name) == list.end()) {
        mConditions->add(vpz::Condition(name));
        mParent->buildTreeConditions();
    }
}

void ConditionsBox::ConditionsTreeView::on_remove()
{
    Gtk::TreeModel::iterator it = mRefTreeSelection->get_selected();

    if (it) {
        Glib::ustring name = (*it)[mColumns.m_col_name];

	mConditions->del(name);
	mParent->buildTreeConditions();
    }
}

void ConditionsBox::ConditionsTreeView::onRename()
{
    Gtk::TreeModel::iterator it = mRefTreeSelection->get_selected();

    if (it) {
        Glib::ustring oldname = (*it)[mColumns.m_col_name];
        SimpleTypeBox box("New name of the condition ?");
        std::string newname = boost::trim_copy(box.run());
	vpz::ConditionList& list = mConditions->conditionlist();

	if (box.valid() and not newname.empty()
	   and list.find(newname) == list.end()) {
	    mConditions->rename(oldname, newname);
	    mParent->buildTreeConditions();

	    vpz::AtomicModelList& atomlist(
		mParent->getModeling()->vpz().project().model().atomicModels());
	    atomlist.updateCondition(oldname, newname);
	}
    }
}

void ConditionsBox::ConditionsTreeView::onCopy()
{
    vpz::ConditionList& list = mConditions->conditionlist();
    Gtk::TreeModel::iterator it = mRefTreeSelection->get_selected();
    int number = 1;
    std::string copy;

    if (it) {
	Glib::ustring name = (*it)[mColumns.m_col_name];
	do {
	    copy = name + "_" + boost::lexical_cast < std::string >(number);
	    ++number;
	}while (list.find(copy) != list.end());
	mConditions->copy(name, copy);
        mParent->buildTreeConditions();
    }

}




////
//  PortsTreeView
////

ConditionsBox::PortsTreeView::PortsTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/) :
    Gtk::TreeView(cobject),
    mCondition(0)
{
    mRefTreeModel = Gtk::ListStore::create(mColumns);
    set_model(mRefTreeModel);
    append_column("Parameters", mColumns.m_col_name);
    mRefTreeSelection = get_selection();
    mRefTreeSelection->signal_changed().connect(
        sigc::mem_fun(*this,
                      &ConditionsBox::PortsTreeView::on_select));

    //Fill popup menu:
    {
	Gtk::Menu::MenuList& menulist = mMenuPopup.items();

	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		"_Add",
		sigc::mem_fun(
		    *this,
		    &ConditionsBox::PortsTreeView::on_add)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		"_Remove",
		sigc::mem_fun(
		    *this,
		    &ConditionsBox::PortsTreeView::on_remove)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		"_Rename",
		sigc::mem_fun(
		    *this,
		    &ConditionsBox::PortsTreeView::onRename)));
    }
    mMenuPopup.accelerate(*this);
}

ConditionsBox::PortsTreeView::~PortsTreeView()
{
}

bool ConditionsBox::PortsTreeView::on_button_press_event(
    GdkEventButton* event)
{
    //Call base class, to allow normal handling,
    //such as allowing the row to be selected by the right-click:
    bool return_value = TreeView::on_button_press_event(event);

    //Then do our custom stuff:
    if (mCondition and event->type == GDK_BUTTON_PRESS and event->button == 3)
    {
	mMenuPopup.popup(event->button, event->time);
    }

    return return_value;
}

void ConditionsBox::PortsTreeView::clear()
{
    mRefTreeModel->clear();
}

void ConditionsBox::PortsTreeView::build()
{
    if (mCondition) {
	mRefTreeModel->clear();

	const vpz::ConditionValues& conditionValues =
	    mCondition->conditionvalues();
	vpz::ConditionValues::const_iterator it = conditionValues.begin();

	for (it = conditionValues.begin(); it != conditionValues.end(); ++it) {
	    Gtk::TreeModel::Row row = *(mRefTreeModel->append());

	    row[mColumns.m_col_name] = it->first;
	}
    }
}

void ConditionsBox::PortsTreeView::on_select()
{
    Gtk::TreeModel::iterator it = mRefTreeSelection->get_selected();

    if (it) {
        Glib::ustring name = (*it)[mColumns.m_col_name];

	mParent->buildTreeValues(mCondition->name(), name);
    }
}

void ConditionsBox::PortsTreeView::on_add()
{
    SimpleTypeBox box("Name of the parameter ?");
    std::list < std::string > list;

    mCondition->portnames(list);

    std::string name = boost::trim_copy(box.run());
    std::list < std::string >::const_iterator it =
	std::find(list.begin(), list.end(), name);

    if (box.valid() and not name.empty() and it == list.end()) {
        mCondition->add(name);
        mParent->buildTreePorts(mCondition->name());
    }
}

void ConditionsBox::PortsTreeView::on_remove()
{
    Gtk::TreeModel::iterator it = mRefTreeSelection->get_selected();

    if (it) {
        Glib::ustring name = (*it)[mColumns.m_col_name];
        mCondition->del(name);
        mParent->buildTreePorts(mCondition->name());
    }
}

void ConditionsBox::PortsTreeView::onRename()
{
    Gtk::TreeModel::iterator it_select = mRefTreeSelection->get_selected();
    std::list < std::string > list;
    mCondition->portnames(list);
    if (it_select) {
        SimpleTypeBox box("Name of the condition ?");
        std::string newname = boost::trim_copy(box.run());
        std::list < std::string >::const_iterator it_find =
            std::find(list.begin(), list.end(), newname);
        if (it_find == list.end()){
            Glib::ustring oldname = (*it_select)[mColumns.m_col_name];
            mCondition->rename(oldname, newname);
            mParent->buildTreePorts(mCondition->name());
        }
    }
}

////
// ConditionsBox
////

ConditionsBox::ConditionsBox(Glib::RefPtr<Gnome::Glade::Xml> xml,
			     Modeling* modeling) :
    mModeling(modeling),
    mConditions(0),
    mBackup(0),
    mDialog(0),
    mButtonCancel(0),
    m_xml(xml)
{
    xml->get_widget("DialogConditions", mDialog);

    m_xml->get_widget_derived("simuTreeConditions", m_treeConditions);
    m_treeConditions->setParent(this);

    m_xml->get_widget_derived("simuTreePorts", m_treePorts);
    m_treePorts->setParent(this);

    m_xml->get_widget_derived("simuTreeValues", m_treeValues);

    xml->get_widget("ButtonConditionsCancel", mButtonCancel);
    if (mButtonCancel)
        mButtonCancel->signal_clicked().connect(
	    sigc::mem_fun
	    (*this, &ConditionsBox::on_cancel));

    xml->get_widget("ButtonConditionsApply", mButtonApply);
    if (mButtonApply)
        mButtonApply->signal_clicked().connect(
	    sigc::mem_fun
	    (*this, &ConditionsBox::on_apply));
}

ConditionsBox::~ConditionsBox()
{
    if (mBackup)
        delete mBackup;
}

void ConditionsBox::buildTreeConditions()
{
    m_treePorts->clear();
    m_treeValues->clear();
    m_treeConditions->setConditions(mConditions);
    m_treePorts->setCondition(0);
    m_treeValues->setCondition(0);
    m_treeConditions->build();
}

void ConditionsBox::buildTreePorts(const std::string& conditionName)
{
    m_treeValues->clear();
    m_treePorts->setCondition(&mConditions->get(conditionName));
    m_treeValues->setCondition(0);
    m_treePorts->build();
}

void ConditionsBox::buildTreeValues(const std::string& conditionName,
				    const std::string& portName)
{
    value::Set&
        values(mConditions->get(conditionName).getSetValues(portName));

    m_treeValues->setCondition(&mConditions->get(conditionName));
    m_treeValues->makeTreeView(values);
}

void ConditionsBox::show()
{
    mConditions = &mModeling->conditions();
    if (mBackup)
        delete mBackup;
    mBackup = new vpz::Conditions(*mConditions);

    buildTreeConditions();
    mDialog->show_all();
    mDialog->run();
}

void ConditionsBox::on_apply()
{
    if (mDialog)
        mDialog->hide();
}

void ConditionsBox::on_cancel()
{
    mConditions->clear();
    const vpz::ConditionList& conditions = mBackup->conditionlist();
    vpz::ConditionList::const_iterator it = conditions.begin();

    while (it != conditions.end()) {
        mConditions->add(it->second);
        ++it;
    }
    if (mDialog)
        mDialog->hide();
}

} } // namespace vle gvle
