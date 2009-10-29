/**
 * @file vle/gvle/conditions/DifferenceEquation/GenericPlugin.cpp
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


#include <vle/gvle/conditions/DifferenceEquation/GenericPlugin.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/String.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>

using namespace std;
using namespace vle;

namespace vle { namespace gvle { namespace conditions {

GenericPlugin::InitTreeView::InitTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr<Gnome::Glade::Xml>&):
    Gtk::TreeView(cobject)
{
    m_refTreeInit = Gtk::ListStore::create(m_columnsInit);
    set_model(m_refTreeInit);
    append_column(_("Value"), m_columnsInit.m_col_value);

    {
	Gtk::Menu::MenuList& menulist = m_menuPopup.items();

	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Add"),
		sigc::mem_fun(
		    *this,
		    &GenericPlugin::InitTreeView::onAdd)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Remove"),
		sigc::mem_fun(
		    *this,
		    &GenericPlugin::InitTreeView::onRemove)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Up"),
		sigc::mem_fun(
		    *this,
		    &GenericPlugin::InitTreeView::onUp)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Down"),
		sigc::mem_fun(
		    *this,
		    &GenericPlugin::InitTreeView::onDown)));
    }
    m_menuPopup.accelerate(*this);
}

GenericPlugin::InitTreeView::~InitTreeView()
{
}

void GenericPlugin::InitTreeView::init(vpz::Condition& condition)
{
    value::VectorValue vector = condition.firstValue("init").toSet().value();
    value::VectorValue::iterator iter = vector.begin();
    while (iter != vector.end()) {
	value::Value* val = *iter;
	if (val->isDouble()) {
	    Gtk::TreeModel::Row row = *(m_refTreeInit->append());
	    row[m_columnsInit.m_col_value] = val->toDouble().value();
	}
	++iter;
    }
}

bool GenericPlugin::InitTreeView::on_button_press_event(GdkEventButton* event)
{
    bool return_value = Gtk::TreeView::on_button_press_event(event);
    if ( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) ) {
	m_menuPopup.popup(event->button, event->time);
    }

    return return_value;
}

void GenericPlugin::InitTreeView::onAdd()
{
    SimpleTypeBox box(_("Value of the init"), "");
    try {
	double value = boost::lexical_cast< double >(
	    boost::trim_copy(box.run()));

	if (box.valid()) {
	    Gtk::TreeModel::Row row = *(m_refTreeInit->append());
	    row[m_columnsInit.m_col_value] = value;
	}
    } catch(const boost::bad_lexical_cast& e ) {
	Error(_("It is not a real"));
    }
}

void GenericPlugin::InitTreeView::onRemove()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();

    if (refSelection) {
	Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
            m_refTreeInit->erase(iter);
	}
    }
}

void GenericPlugin::InitTreeView::onUp()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Glib::RefPtr<Gtk::TreeModel> refModel = get_model();

    if (refSelection) {
	Gtk::TreeModel::iterator iter = refSelection->get_selected();
	Gtk::TreeModel::iterator iterBefore = iter;
	if (iter and iter != refModel->children().begin()) {
	    --iterBefore;
	    m_refTreeInit->iter_swap(iter, iterBefore);
	}
    }
}

void GenericPlugin::InitTreeView::onDown()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Glib::RefPtr<Gtk::TreeModel> refModel = get_model();

    if (refSelection) {
	Gtk::TreeModel::iterator iter = refSelection->get_selected();
	Gtk::TreeModel::iterator iterBefore = iter;
	if (iter and (bool)(++iterBefore)) {
	    m_refTreeInit->iter_swap(iter, iterBefore);
	}
    }
}

GenericPlugin::GenericPlugin()
{
}

GenericPlugin::~GenericPlugin()
{
}

void GenericPlugin::buildGeneric()
{
    m_spinTime->set_range(
	0.01,
	std::numeric_limits < double >::max());
    m_spinTime->set_value(1.00);

    m_checkGlobal->signal_clicked().connect(
	sigc::mem_fun( *this, &GenericPlugin::onClickCheckButton));

    m_comboUnit = new Gtk::ComboBoxText();
    m_comboUnit->append_text("");
    m_comboUnit->append_text("day");
    m_comboUnit->append_text("week");
    m_comboUnit->append_text("month");
    m_comboUnit->append_text("year");
    m_hboxTimestep->pack_start(*m_comboUnit, true, true, 5);
    m_comboUnit->show_all();
}

void GenericPlugin::fillFieldsGeneric(vpz::Condition& condition)
{
    if (validPortGeneric(condition, "time-step")) {
       if (condition.firstValue("time-step").isMap()){
	    value::Map map = condition.firstValue("time-step").toMap();
	    if (map.value().find("value") != map.end()
		and map.value().find("unit") != map.end()) {
		m_spinTime->set_value(map["value"].toDouble().value());
		m_comboUnit->set_active_text(map["unit"].toString().value());
	    }
       } else  {
	   m_spinTime->set_value(condition.firstValue("time-step").
				 toDouble().value());
       }
    } else {
	m_checkGlobal->set_active(true);
	m_spinTime->set_sensitive(false);
	m_comboUnit->set_sensitive(false);
    }

    if (validPortGeneric(condition, "name"))
	m_entryName->set_text(condition.firstValue("name").
			 toString().value());
    if (validPortGeneric(condition, "value"))
	m_entryValue->set_text(boost::lexical_cast<std::string>(
			      condition.firstValue("value").
			      toDouble().value()));

    if (validPortGeneric(condition, "init"))
	m_initTreeView->init(condition);
}

void GenericPlugin::createTimeStep(vpz::Condition& condition)
{
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
}

void GenericPlugin::createSetInit(vpz::Condition& condition)
{
    value::Set* set = 0;
    Glib::RefPtr<Gtk::TreeModel> refModel = m_initTreeView->get_model();
    Gtk::TreeRow row;
    if (refModel->children().size() != 0) {
	set = new value::Set();
	Gtk::TreeModel::iterator iter = refModel->children().begin();
	double initValue;
	while (iter != refModel->children().end()) {
	    row = *iter;
	    initValue = row.get_value(
		m_initTreeView->getColumns()->m_col_value);
	    set->addDouble(initValue);
	    ++iter;
	}
    }

    if (set != 0)
	condition.addValueToPort("init",set);
}

void GenericPlugin::assignGeneric(vpz::Condition& condition)
{
    createTimeStep(condition);

    if (not m_entryName->get_text().empty())
	condition.addValueToPort("name",
				 value::String::create(
				     m_entryName->get_text()));
    try {
	if (not m_entryValue->get_text().empty())
	    condition.addValueToPort("value",
				     value::Double::create(
					 boost::lexical_cast<double>(
					     m_entryValue->get_text())));

    } catch(const boost::bad_lexical_cast& e ) {
    }

    createSetInit(condition);
}

bool GenericPlugin::existPort(vpz::Condition& condition, std::string name)
{
    std::list < std::string> list;
    condition.portnames(list);
    if (std::find(list.begin(), list.end(), name) != list.end())
	return true;
    else
	return false;
}

void GenericPlugin::deletePortsGeneric(vpz::Condition& condition)
{
    if (existPort(condition, "name"))
	condition.del("name");

    if (existPort(condition, "value"))
	condition.del("value");

    if (existPort(condition, "time-step"))
	condition.del("time-step");

    if (existPort(condition, "init"))
	condition.del("init");
}

bool GenericPlugin::validPortGeneric(vpz::Condition& condition,
				     const std::string& name)
{
    if (existPort(condition, name)) {
	if (name == "name") {
	    if (condition.getSetValues(name).size() != 0
		and condition.firstValue(name).isString())
		return true;
	}

	if (name == "value") {
	    if (condition.getSetValues(name).size() != 0
		and condition.firstValue(name).isDouble())
		return true;
	}

	if (name == "time-step") {
	    if (condition.getSetValues(name).size() != 0 and
		(condition.firstValue(name).isDouble()
		 or condition.firstValue(name).isMap()))
		return true;
	}

	if (name == "init") {
	    if (condition.getSetValues(name).size() != 0
		and condition.firstValue(name).isSet())
		return true;
	}
    }
    return false;
}

void GenericPlugin::onClickCheckButton()
{
    if (m_checkGlobal->get_active()) {
	m_spinTime->set_sensitive(false);
	m_comboUnit->set_sensitive(false);
    } else {
	m_spinTime->set_sensitive(true);
	m_comboUnit->set_sensitive(true);
    }
}


}}} // namespace vle gvle conditions
