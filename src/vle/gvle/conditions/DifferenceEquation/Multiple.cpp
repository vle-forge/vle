/**
 * @file vle/gvle/conditions/DifferenceEquation/Multiple.cpp
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

#include <vle/gvle/conditions/DifferenceEquation/Multiple.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Double.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/String.hpp>
#include <vle/utils/Path.hpp>
#include <gtkmm/filechooserdialog.h>
#include <gdkmm/cursor.h>
#include <gtkmm/stock.h>
#include <libglademm/xml.h>
#include <cassert>
#include <boost/assign/list_inserter.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>

using namespace std;
using namespace boost::assign;
using namespace vle;

namespace vle { namespace gvle { namespace conditions {

Multiple::ValuesTreeView::ValuesTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr<Gnome::Glade::Xml>&) :
    Gtk::TreeView(cobject)
{
    m_refTreeValues = Gtk::ListStore::create(m_columnsValues);
    set_model(m_refTreeValues);
    buildMenu();
}

Multiple::ValuesTreeView::~ValuesTreeView()
{
}

void Multiple::ValuesTreeView::buildMenu()
{
    Gtk::Menu::MenuList& menulist = mMenu.items();
    menulist.push_back(
	Gtk::Menu_Helpers::MenuElem(
	    _("_Add"),
	    sigc::mem_fun(
		*this,
		&Multiple::ValuesTreeView::onAdd)));
    menulist.push_back(
	Gtk::Menu_Helpers::MenuElem(
	    _("_Remove"),
	    sigc::mem_fun(
		*this,
		&Multiple::ValuesTreeView::onRemove)));

    mMenu.accelerate(*this);
}

void Multiple::ValuesTreeView::makeTreeView()
{
    m_refTreeValues->clear();
    clear();

    append_column("Value", m_columnsValues.m_col_value);

    std::vector < double > vector = m_parent->getVectorValues(m_variable);
    for (std::vector<double>::iterator iter = vector.begin(); iter != vector.end (); ++iter) {
	Gtk::TreeModel::Row row = *(m_refTreeValues->append());
	row[m_columnsValues.m_col_value] = *iter;
    }
}

void Multiple::ValuesTreeView::clear()
{
    remove_all_columns();
}

bool Multiple::ValuesTreeView::on_button_press_event(GdkEventButton* event)
{
    if (not m_variable.empty()and
	event->type ==GDK_BUTTON_PRESS and event->button == 3) {
        mMenu.popup(event->button, event->time);
    }
    return TreeView::on_button_press_event(event);
}

void Multiple::ValuesTreeView::onAdd()
{

    SimpleTypeBox box(_("Value"));
    try {
	double value = boost::lexical_cast< double >(
	    boost::trim_copy(box.run()));

	if (box.valid()) {
	    Gtk::TreeModel::Row row = *(m_refTreeValues->append());
	    row[m_columnsValues.m_col_value] = value;
	    std::vector<double>& vector = m_parent->getVectorValues(m_variable);
	    vector.push_back(value);
	}
    } catch(const boost::bad_lexical_cast& e ) {
    }
}

void Multiple::ValuesTreeView::onRemove()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();

    if (refSelection) {
	Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
	    std::vector<double>& vector = m_parent->getVectorValues(m_variable);
	    int pos = boost::lexical_cast< int >(get_model()->get_string(iter));
	    std::vector<double>::iterator iterVector = vector.begin();
	    for (int current = 0; current < pos; current++)
		++iterVector;
	    vector.erase(iterVector);

            m_refTreeValues->erase(iter);
	}
    }
}

Multiple::VariableTreeView::VariableTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr<Gnome::Glade::Xml>& xml):
    Gtk::TreeView(cobject)
{
    xml->get_widget("DialogVariableBox", m_dialog);
    xml->get_widget("entryVariableName", m_name);
    xml->get_widget("entryVariableValue", m_value);

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
	&Multiple::VariableTreeView::onSelect));

    {
	Gtk::Menu::MenuList& menulist = m_menuPopup.items();

	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Add"),
		sigc::mem_fun(
		    *this,
		    &Multiple::VariableTreeView::onAdd)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Remove"),
		sigc::mem_fun(
		    *this,
		    &Multiple::VariableTreeView::onRemove)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Edit"),
		sigc::mem_fun(
		    *this,
		    &Multiple::VariableTreeView::onEdit)));
    }
    m_menuPopup.accelerate(*this);
}

Multiple::VariableTreeView::~VariableTreeView()
{
}

void Multiple::VariableTreeView::init(vpz::Condition& condition)
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

void Multiple::VariableTreeView::initList(const std::string& name,
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


bool Multiple::VariableTreeView::on_button_press_event(GdkEventButton* event)
{
    bool return_value = Gtk::TreeView::on_button_press_event(event);
    if ( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) ) {
	m_menuPopup.popup(event->button, event->time);
    }

    return return_value;
}

void Multiple::VariableTreeView::onSelect()
{
    Gtk::TreeModel::iterator it = m_refTreeSelection->get_selected();

    if (it) {
	Gtk::TreeModel::Row row = *it;
	std::string name = row.get_value(m_columnsVariable.m_col_name);
	m_parent->buildTreeValues(name);
    }
}

void Multiple::VariableTreeView::onAdd()
{
    m_name->set_text("");
    m_value->set_text("");
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
	    } catch(const boost::bad_lexical_cast& e ) {

	    }
	}
    }
    m_dialog->hide();
}

void Multiple::VariableTreeView::onRemove()
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

void Multiple::VariableTreeView::onEdit()
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
	    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
		try {

		    std::string name = m_name->get_text();
		    if (not exist(m_name->get_text())) {
			row[m_columnsVariable.m_col_name] = name;
			m_parent->getVectorValues(name) =
			    m_parent->getVectorValues(oldName);
			m_parent->getVectorValues(oldName).clear();
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

bool Multiple::VariableTreeView::exist(const std::string& name)
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

Multiple::Multiple(const std::string& name) :
    ConditionPlugin(name), Mapping(), m_dialog(0)
{
}

Multiple::~Multiple()
{
}

void Multiple::build()
{
    std::string glade = utils::Path::path().
	getConditionGladeFile("DifferenceEquation.glade");
    Glib::RefPtr<Gnome::Glade::Xml> ref = Gnome::Glade::Xml::create(glade);

    ref->get_widget("DialogPluginMultipleBox", m_dialog);
    m_dialog->set_title("DifferenceEquation - Multiple");
    ref->get_widget("SpinButtonTimeMultiple", m_spinTime);
    ref->get_widget("TimestepGlobalButtonMultiple", m_checkGlobal);
    m_checkGlobal->signal_clicked().connect(
	sigc::mem_fun( *this, &Multiple::onClickCheckButton));
    ref->get_widget("HBoxTimestepMultiple", m_hboxTimestep);

    ref->get_widget("HBoxModeMultiple", m_hboxMode);
    ref->get_widget_derived("variableTreeViewMultiple", m_variableTreeView);
    m_variableTreeView->setParent(this);
    ref->get_widget_derived("mappingTreeViewMultiple", m_mappingTreeView);
    m_mappingTreeView->setSizeColumn(237);
    ref->get_widget_derived("valuesTreeViewMultiple", m_valuesTreeView);
    m_valuesTreeView->setParent(this);
    m_valuesTreeView->clear();
    m_mappingTreeView->set_sensitive(false);

    assert(m_dialog);
    assert(m_spinTime);
    assert(m_checkGlobal);
    assert(m_hboxTimestep);
    assert(m_hboxMode);
    assert(m_variableTreeView);
    assert(m_mappingTreeView);
    assert(m_valuesTreeView);

    m_spinTime->set_range(
	0.01,
	std::numeric_limits < double >::max());
    m_spinTime->set_value(1.00);

    m_comboUnit = new Gtk::ComboBoxText();
    m_comboUnit->append_text("");
    m_comboUnit->append_text("day");
    m_comboUnit->append_text("week");
    m_comboUnit->append_text("month");
    m_comboUnit->append_text("year");
    m_hboxTimestep->pack_start(*m_comboUnit, true, true, 5);
    m_comboUnit->show_all();

    buildButtonMapping();
}

void Multiple::fillFields(vpz::Condition& condition)
{

    if (validPort(condition, "time-step")) {
	if (condition.firstValue("time-step").isMap()){
	    value::Map map = condition.firstValue("time-step").toMap();
	    if (map.value().find("value") != map.end()
		and map.value().find("unit") != map.end()) {
		m_spinTime->set_value(map["value"].toDouble().value());
		m_comboUnit->set_active_text(map["unit"].toString().value());
	    }
	} else {
	    m_spinTime->set_value(condition.firstValue("time-step").
				  toDouble().value());
	}
    } else {
	m_checkGlobal->set_active(true);
	m_spinTime->set_sensitive(false);
	m_comboUnit->set_sensitive(false);
    }

    if (validPort(condition, "variables"))
	m_variableTreeView->init(condition);

    fillFieldsMapping(condition);
}

bool Multiple::start(vpz::Condition& condition)
{
    build();
    fillFields(condition);

    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
	assign(condition);
    }

    m_dialog->hide();

    return true;
}

void Multiple::createSetVariable(vpz::Condition& condition)
{
    value::Set* set = 0;

    Glib::RefPtr<Gtk::TreeModel> refModel = m_variableTreeView->get_model();
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
		m_variableTreeView->getColumns()->m_col_name);
	    setVariables->addString(name);
	    if (not row.get_value(
		    m_variableTreeView->getColumns()->m_col_value).empty()) {
		value = boost::lexical_cast<double>(
		    row.get_value(
			m_variableTreeView->getColumns()->m_col_value));
		setVariables->addDouble(value);
	    }
	    if (m_listValues[name].size() != 0) {
		value::Set* setValues = new value::Set();
		std::vector<double>::iterator iterVector =
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

void Multiple::assign(vpz::Condition& condition)
{
    deletePorts(condition);
    deletePortsMapping(condition);
    if (not m_checkGlobal->get_active()) {
	if (m_comboUnit->get_active_text() == "")
	    condition.addValueToPort("time-step",
				     value::Double::create(
					 m_spinTime->get_value()));
	else {
	    value::Map* map = new value::Map();
	    map->addDouble("value", m_spinTime->get_value() );
	    map->addString("unit", m_comboUnit->get_active_text());
	    condition.addValueToPort("time-step", map);
	}
    }

    createSetVariable(condition);

    assignMode(condition);
}

void Multiple::onClickCheckButton()
{
    if (m_checkGlobal->get_active()) {
	m_spinTime->set_sensitive(false);
	m_comboUnit->set_sensitive(false);
    } else {
	m_spinTime->set_sensitive(true);
	m_comboUnit->set_sensitive(true);
    }
}

bool Multiple::existPort(vpz::Condition& condition, const std::string& name)
{
    std::list < std::string> list;
    condition.portnames(list);
    if (std::find(list.begin(), list.end(), name) != list.end())
	return true;
    else
	return false;
}

bool Multiple::validPort(vpz::Condition& condition, const std::string& name)
{
    if (existPort(condition, name)) {
	if (name == "time-step") {
	    if (condition.getSetValues(name).size() != 0 and
		(condition.firstValue(name).isDouble()
		 or condition.firstValue(name).isMap()))
		return true;
	}

	if (name == "variables") {
	    if (condition.getSetValues(name).size() != 0
		and condition.firstValue(name).isSet())
		return true;
	}
    }
    return false;
}

void Multiple::deletePorts(vpz::Condition& condition)
{
    if (existPort(condition, "time-step"))
	condition.del("time-step");

    if (existPort(condition, "variables"))
	condition.del("variables");
}

void Multiple::buildTreeValues(const std::string& name)
{
    m_valuesTreeView->setVariable(name);
    m_valuesTreeView->makeTreeView();
}

}}}

DECLARE_GVLE_CONDITIONPLUGIN(vle::gvle::conditions::Multiple)
