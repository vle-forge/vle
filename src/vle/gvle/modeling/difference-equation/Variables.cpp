/**
 * @file vle/gvle/modeling/difference-equation/Variables.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
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


#include <vle/gvle/modeling/difference-equation/Variables.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace vle { namespace gvle { namespace modeling {

Variables::ValuesTreeView::ValuesTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr<Gnome::Glade::Xml>&) :
    Gtk::TreeView(cobject)
{
    m_refTreeValues = Gtk::ListStore::create(m_columnsValues);
    set_model(m_refTreeValues);
    buildMenu();
}

Variables::ValuesTreeView::~ValuesTreeView()
{
}

void Variables::ValuesTreeView::buildMenu()
{
    Gtk::Menu::MenuList& menulist = mMenu.items();
    menulist.push_back(
	Gtk::Menu_Helpers::MenuElem(
	    _("_Add"),
	    sigc::mem_fun(
		*this,
		&Variables::ValuesTreeView::onAdd)));
    menulist.push_back(
	Gtk::Menu_Helpers::MenuElem(
	    _("_Remove"),
	    sigc::mem_fun(
		*this,
		&Variables::ValuesTreeView::onRemove)));

    mMenu.accelerate(*this);
}

void Variables::ValuesTreeView::makeTreeView()
{
    m_refTreeValues->clear();
    clear();

    append_column("Value", m_columnsValues.m_col_value);

    const Values& vector = m_parent->getVectorValues(m_variable);
    for (Values::const_iterator iter = vector.begin();
         iter != vector.end (); ++iter) {
	Gtk::TreeModel::Row row = *(m_refTreeValues->append());
	row[m_columnsValues.m_col_value] = *iter;
    }
}

void Variables::ValuesTreeView::clear()
{
    remove_all_columns();
}

bool Variables::ValuesTreeView::on_button_press_event(GdkEventButton* event)
{
    if (not m_variable.empty()and
	event->type ==GDK_BUTTON_PRESS and event->button == 3) {
        mMenu.popup(event->button, event->time);
    }
    return TreeView::on_button_press_event(event);
}

void Variables::ValuesTreeView::onAdd()
{
    SimpleTypeBox box(_("Value"), "");

    try {
	double value = boost::lexical_cast< double >(
	    boost::trim_copy(box.run()));

	if (box.valid()) {
	    Gtk::TreeModel::Row row = *(m_refTreeValues->append());
	    row[m_columnsValues.m_col_value] = value;
	    Values& vector = m_parent->getVectorValues(m_variable);
	    vector.push_back(value);
	}
    } catch(const boost::bad_lexical_cast& e ) {
    }
}

void Variables::ValuesTreeView::onRemove()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();

    if (refSelection) {
	Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
	    Values& vector = m_parent->getVectorValues(m_variable);
	    int pos = boost::lexical_cast< int >(get_model()->get_string(iter));
	    Values::iterator iterVector = vector.begin();

	    for (int current = 0; current < pos; current++) {
                ++iterVector;
            }
	    vector.erase(iterVector);
            m_refTreeValues->erase(iter);
	}
    }
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

Variables::VariablesTreeView::VariablesTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr<Gnome::Glade::Xml>& xml):
    Gtk::TreeView(cobject)
{
    xml->get_widget("DialogVariableBox", m_dialog);
    xml->get_widget("VariableNameEntry", m_name);
    xml->get_widget("VariableValueEntry", m_value);

    m_refTreeVariable = Gtk::ListStore::create(m_columnsVariable);
    set_model(m_refTreeVariable);
    int name = append_column(_("Name"), m_columnsVariable.m_col_name);
    append_column(_("Value"), m_columnsVariable.m_col_value);

    Gtk::TreeViewColumn* columnName = get_column(name - 1);
    columnName->set_min_width(110);
    columnName->set_fixed_width(110);

    m_refTreeSelection = get_selection();
    m_refTreeSelection->signal_changed().connect(
        sigc::mem_fun(*this,
	&Variables::VariablesTreeView::onSelect));

    {
	Gtk::Menu::MenuList& menulist = m_menuPopup.items();

	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Add"),
		sigc::mem_fun(
		    *this,
		    &Variables::VariablesTreeView::onAdd)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Remove"),
		sigc::mem_fun(
		    *this,
		    &Variables::VariablesTreeView::onRemove)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Edit"),
		sigc::mem_fun(
		    *this,
		    &Variables::VariablesTreeView::onEdit)));
    }
    m_menuPopup.accelerate(*this);
}

Variables::VariablesTreeView::~VariablesTreeView()
{
}

Variables::Variables_t Variables::VariablesTreeView::getVariables() const
{
    Variables_t variables;
    Gtk::TreeNodeChildren child = get_model()->children();
    Gtk::TreeModel::iterator it = child.begin();

    while (it != child.end()) {
        std::string name(it->get_value(m_columnsVariable.m_col_name));

	variables.push_back(name);
	++it;
    }
    return variables;
}

void Variables::VariablesTreeView::init(const vpz::Condition& condition)
{
    m_parent->m_listValues.clear();
    m_parent->m_valuesTreeView->clear();
    value::VectorValue vector = condition.firstValue("variables").
	toSet().value();
    value::VectorValue::iterator iter = vector.begin();
    while (iter != vector.end()) {
	value::Value* set = *iter;

	if (set->isSet()) {
	    value::VectorValue vectorInside = set->toSet().value();
	    Gtk::TreeModel::Row row = *(m_refTreeVariable->append());

	    if (vectorInside.size() >= 1 and vectorInside[0]->isString()) {
		row[m_columnsVariable.m_col_name] = vectorInside[0]
		    ->toString().value();
		if (vectorInside.size() >= 2) {
		    if (vectorInside[1]->isDouble()) {
			row[m_columnsVariable.m_col_value] =
			    boost::lexical_cast <std::string> (
				vectorInside[1]->toDouble().value());
			if (vectorInside.size() >= 3
			    and vectorInside[2]->isSet())
			    initList(vectorInside[0]->toString().value(),
				     vectorInside[2]);
		    } else if (vectorInside[1]->isSet()) {
			initList(vectorInside[0]->toString().value(),
				 vectorInside[1]);
		    }
		}
	    }
	    ++iter;
	}
    }
}

void Variables::VariablesTreeView::initList(const std::string& name,
					  value::Value* value)
{
    value::VectorValue vectorValues = value->toSet().value();
    value::VectorValue::iterator iterValues = vectorValues.begin();
    while (iterValues != vectorValues.end()) {
	if ((*iterValues)->isDouble()) {
	    m_parent->getVectorValues(name).push_back(
		    (*iterValues)->toDouble().value());
	}
	++iterValues;
    }
}


bool Variables::VariablesTreeView::on_button_press_event(GdkEventButton* event)
{
    bool return_value = Gtk::TreeView::on_button_press_event(event);
    if ( (event->type == GDK_BUTTON_PRESS) and (event->button == 3) ) {
	m_menuPopup.popup(event->button, event->time);
    }

    return return_value;
}

void Variables::VariablesTreeView::onSelect()
{
    Gtk::TreeModel::iterator it = m_refTreeSelection->get_selected();

    if (it) {
	Gtk::TreeModel::Row row = *it;
	std::string name = row.get_value(m_columnsVariable.m_col_name);
	m_parent->buildTreeValues(name);
    }
}

void Variables::VariablesTreeView::onAdd()
{
    m_name->set_text("");
    m_value->set_text("");
    m_dialog->set_focus(*m_name);
    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
	if (not m_name->get_text().empty()) {
	    try {
		std::string name = m_name->get_text();
		if (not exist(m_name->get_text())) {
		    Gtk::TreeModel::Row row = *(
			m_refTreeVariable->append());
		    row[m_columnsVariable.m_col_name] = name;
		    if (not m_value->get_text().empty()) {
			double value = boost::lexical_cast< double >(
			  m_value->get_text());
			row[m_columnsVariable.m_col_value] =
			    boost::lexical_cast <std::string> (value);
		    }
		}
	    } catch(const boost::bad_lexical_cast& e ) { }
	}
    }
    m_dialog->hide();
}

void Variables::VariablesTreeView::onRemove()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();

    if (refSelection) {
	Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
	    Gtk::TreeRow row = *iter;
	    Glib::ustring name = row.get_value(m_columnsVariable.m_col_name);
            m_refTreeVariable->erase(iter);
	    m_parent->getVectorValues(name).clear();
	}
    }
}

void Variables::VariablesTreeView::onEdit()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();

    if (refSelection) {
	Gtk::TreeModel::iterator iter = refSelection->get_selected();
	if (iter) {
	    Gtk::TreeRow row = *iter;
	    Glib::ustring oldName = row.get_value(
		m_columnsVariable.m_col_name);
	    Glib::ustring oldValue = row.get_value(
		m_columnsVariable.m_col_value);

	    m_name->set_text(oldName);
	    m_value->set_text(oldValue);
            m_dialog->set_focus(*m_name);
	    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
		try {
		    std::string name = m_name->get_text();

		    if (not exist(m_name->get_text())) {
			row[m_columnsVariable.m_col_name] = name;
                        m_parent->changeName(oldName, name);
		    }
		    if (not m_value->get_text().empty()) {
			double value = boost::lexical_cast< double >(
			    m_value->get_text());

			row[m_columnsVariable.m_col_value] =
			    boost::lexical_cast <std::string> (value);
		    } else
			row[m_columnsVariable.m_col_value] = "";
		} catch(const boost::bad_lexical_cast& e ) {
		}
	    }

	}
    }
    m_dialog->hide();
}

bool Variables::VariablesTreeView::exist(const std::string& name)
{
    Gtk::TreeNodeChildren child = get_model()->children();
    Gtk::TreeModel::iterator iter = child.begin();
    while (iter != child.end()) {
	if (iter->get_value(m_columnsVariable.m_col_name) == name)
	  return true;
	++iter;
    }
    return false;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

Gtk::Widget& Variables::build(Glib::RefPtr<Gnome::Glade::Xml> ref)
{
    ref->get_widget("VariablesFrame", m_frame);
    ref->get_widget_derived("VariablesTreeView", m_variablesTreeView);
    m_variablesTreeView->setParent(this);
    ref->get_widget_derived("ValuesTreeView", m_valuesTreeView);
    m_valuesTreeView->setParent(this);
    m_valuesTreeView->clear();
    return *m_frame;
}

void Variables::changeName(const std::string& oldName,
                           const std::string& newName)
{
    ValuesMap::const_iterator valueIt = m_listValues.find(oldName);

    if (valueIt != m_listValues.end()) {
        Values::const_iterator it = valueIt->second.begin();

        m_listValues[newName] = Values();
        while (it != valueIt->second.end()) {
            m_listValues[newName].push_back(*it++);
        }
        m_listValues.erase(oldName);
        buildTreeValues(newName);
    }
}

void Variables::fillFields(const vpz::Condition& condition)
{
    if (validPort(condition, "variables")) {
	m_variablesTreeView->init(condition);
    }
}

void Variables::assign(vpz::Condition& condition)
{
    value::Set* set = 0;

    Glib::RefPtr<Gtk::TreeModel> refModel = m_variablesTreeView->get_model();
    Gtk::TreeRow row;
    if (refModel->children().size() != 0) {
	set = new value::Set();
	Gtk::TreeModel::iterator iter = refModel->children().begin();
	std::string name;
	double value;
	while (iter != refModel->children().end()) {
	    value::Set* setVariables = new value::Set();
	    row = *iter;
	    name = row.get_value(
		m_variablesTreeView->getColumns()->m_col_name);
	    setVariables->addString(name);
	    if (not row.get_value(
		    m_variablesTreeView->getColumns()->m_col_value).empty()) {
		value = boost::lexical_cast<double>(
		    row.get_value(
			m_variablesTreeView->getColumns()->m_col_value));
		setVariables->addDouble(value);
	    }
	    if (m_listValues[name].size() != 0) {
		value::Set* setValues = new value::Set();
		Values::iterator iterVector =
		    m_listValues[name].begin();
		while (iterVector != m_listValues[name].end()) {
		    setValues->addDouble(*iterVector);
		    ++iterVector;
		}
		setVariables->add(setValues);
	    }
	    set->add(setVariables);
	    ++iter;
	}
    }

    if (set != 0)
	condition.addValueToPort("variables", set);
}

void Variables::buildTreeValues(const std::string& name)
{
    m_valuesTreeView->setVariable(name);
    m_valuesTreeView->makeTreeView();
}

void Variables::deletePorts(vpz::Condition& condition)
{
    std::list < std::string> list;

    condition.portnames(list);
    if (std::find(list.begin(), list.end(), "variables") != list.end()) {
	condition.del("variables");
    }
}

bool Variables::validPort(const vpz::Condition& condition,
                          const std::string& name)
{
    std::list < std::string> list;

    condition.portnames(list);
    if (std::find(list.begin(), list.end(), name) != list.end()) {
	if (name == "variables") {
	    if (condition.getSetValues(name).size() != 0
		and condition.firstValue(name).isSet())
		return true;
	}
    }
    return false;
}

}}} // namespace vle gvle modeling
