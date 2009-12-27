/**
 * @file vle/gvle/conditions/DifferenceEquation/Mapping.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
 * Copyright (C) 2003-2009 ULCO http://www.univ-littoral.fr
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


#include <vle/gvle/conditions/DifferenceEquation/Mapping.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/String.hpp>

namespace vle { namespace gvle { namespace conditions {

Mapping::MappingTreeView::MappingTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr<Gnome::Glade::Xml>& xml):
    Gtk::TreeView(cobject)
{
    xml->get_widget("DialogMappingBox", m_dialog);
    xml->get_widget("entryPort", m_port);
    xml->get_widget("entryId", m_id);


    m_refTreeMapping = Gtk::ListStore::create(m_columnsMapping);
    set_model(m_refTreeMapping);
    m_columnPort = append_column(_("Port"), m_columnsMapping.m_col_port);
    m_columnId = append_column(_("Identifer"), m_columnsMapping.m_col_id);

    {
	Gtk::Menu::MenuList& menulist = m_menuPopup.items();

	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Add"),
		sigc::mem_fun(
		    *this,
		    &Mapping::MappingTreeView::onAdd)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Remove"),
		sigc::mem_fun(
		    *this,
		    &Mapping::MappingTreeView::onRemove)));

	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Edit"),
		sigc::mem_fun(
		    *this,
		    &Mapping::MappingTreeView::onEdit)));
    }
    m_menuPopup.accelerate(*this);
}

void Mapping::MappingTreeView::init(const vpz::Condition& condition)
{
    value::MapValue vector = condition.firstValue("mapping").toMap().value();
    value::MapValue::iterator iter = vector.begin();
    while (iter != vector.end()) {
	if (not iter->first.empty() and iter->second->isString()) {
	    Gtk::TreeModel::Row row = *(m_refTreeMapping->append());
	    row[m_columnsMapping.m_col_port] = iter->first;
	    row[m_columnsMapping.m_col_id] = iter->second->toString().value();
	}
	++iter;
    }
}


bool Mapping::MappingTreeView::on_button_press_event(GdkEventButton* event)
{
    bool return_value = Gtk::TreeView::on_button_press_event(event);
    if ( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) ) {
	m_menuPopup.popup(event->button, event->time);
    }

    return return_value;
}


void Mapping::MappingTreeView::onAdd()
{
    m_port->set_text("");
    m_id->set_text("");
    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
	if (not m_port->get_text().empty() and
	    not m_id->get_text().empty()) {
	    std::string port = m_port->get_text();
	    std::string id = m_id->get_text();
	    Gtk::TreeModel::Row row = *(m_refTreeMapping->append());
	    row[m_columnsMapping.m_col_port] = port;
	    row[m_columnsMapping.m_col_id] = id;
	}
    }
    m_dialog->hide();
}

void Mapping::MappingTreeView::onRemove()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();

    if (refSelection) {
	Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
            m_refTreeMapping->erase(iter);
	}
    }
}

void Mapping::MappingTreeView::onEdit()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();

    if (refSelection) {
	Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
	   Gtk::TreeRow row = *iter;
	   Glib::ustring oldPort = row.get_value(m_columnsMapping.m_col_port);
	   Glib::ustring oldId = row.get_value(m_columnsMapping.m_col_id);
	    m_port->set_text(oldPort);
	    m_id->set_text(oldId);
	    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
		if (not m_port->get_text().empty() and
		    not m_id->get_text().empty()) {
		    row[m_columnsMapping.m_col_port] = m_port->get_text();
		    row[m_columnsMapping.m_col_id] = m_id->get_text();
		}
	    }
	}
    }
    m_dialog->hide();
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

Mapping::~Mapping()
{
    delete m_RadioButtonName;
    delete m_RadioButtonPort;
    delete m_RadioButtonMapping;
}

void Mapping::assign(vpz::Condition& condition)
{
    condition.addValueToPort("mode", value::String::create(getMode()));
    createMap(condition);
}

Gtk::Widget& Mapping::build(Glib::RefPtr<Gnome::Glade::Xml> ref)
{
    ref->get_widget("ModeExpander", m_expander);
    ref->get_widget("ModeHBox", m_hboxMode);
    ref->get_widget("NameRadioButton", m_RadioButtonName);
    ref->get_widget("PortRadioButton", m_RadioButtonPort);
    ref->get_widget("MappingRadioButton", m_RadioButtonMapping);

    ref->get_widget_derived("MappingTreeView", m_mappingTreeView);
    m_mappingTreeView->set_sensitive(false);

    Gtk::RadioButton::Group group = m_RadioButtonName->get_group();
    m_RadioButtonPort->set_group(group);
    m_RadioButtonMapping->set_group(group);
    m_RadioButtonMapping->signal_clicked().connect(
        sigc::mem_fun( *this, &Mapping::onClickRadioButton));
    return *m_expander;
}

void Mapping::createMap(vpz::Condition& condition)
{
    value::Map* map = 0;
    Glib::RefPtr<Gtk::TreeModel> refModel = m_mappingTreeView->get_model();
    Gtk::TreeRow row;

    if (m_RadioButtonMapping->get_active()) {
	refModel = m_mappingTreeView->get_model();
	if (refModel->children().size() != 0) {
	    Gtk::TreeModel::iterator iter = refModel->children().begin();
	    std::string port, id;

	    map = new value::Map();
	    while (iter != refModel->children().end()) {
		row = *iter;
		port = row.get_value(m_mappingTreeView->getColumns()->
				     m_col_port);
		id = row.get_value(m_mappingTreeView->getColumns()->m_col_id);
		map->addString(port, id);
		++iter;
	    }
	}
    }
    if (map != 0) {
        condition.addValueToPort("mapping", map);
    }
}

void Mapping::deletePorts(vpz::Condition& condition)
{
    std::list < std::string> list;

    condition.portnames(list);
    if (std::find(list.begin(), list.end(), "mode") != list.end()) {
        condition.del("mode");
    }
    if (std::find(list.begin(), list.end(), "mapping") != list.end()) {
	condition.del("mapping");
    }
}

void Mapping::fillFields(const vpz::Condition& condition)
{
    if (validPort(condition, "mode")) {
	std::string mode = condition.firstValue("mode").toString().value();

	if (mode == "name")
	    m_RadioButtonName->set_active();
	else if (mode == "port")
	    m_RadioButtonPort->set_active();
	else if (mode == "mapping") {
	    m_RadioButtonMapping->set_active();
	    m_mappingTreeView->set_sensitive(true);
	}
    } else {
        m_RadioButtonName->set_active();
    }

    if (validPort(condition, "mapping")) {
        m_mappingTreeView->init(condition);
    }
}

std::string Mapping::getMode() const
{
    if (m_RadioButtonName->get_active()) {
	return "name";
    } else if (m_RadioButtonPort->get_active()) {
        return "port";
    } else {
        return "mapping";
    }
}

void Mapping::onClickRadioButton()
{
    m_mappingTreeView->set_sensitive(m_RadioButtonMapping->get_active());
}

bool Mapping::validPort(const vpz::Condition& condition,
                        const std::string& name)
{
    std::list < std::string> list;

    condition.portnames(list);
    if (std::find(list.begin(), list.end(), name) != list.end()) {
	if (name == "mode") {
	    if (condition.getSetValues("mode").size() != 0
		and condition.firstValue("mode").isString())
		return true;
	}
	if (name == "mapping") {
	    if (condition.getSetValues(name).size() != 0
		and condition.firstValue(name).isMap())
		return true;
	}
    }
    return false;
}

}}}
