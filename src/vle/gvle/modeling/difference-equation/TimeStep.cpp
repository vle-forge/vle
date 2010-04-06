/**
 * @file vle/gvle/modeling/difference-equation/TimeStep.cpp
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


#include <vle/gvle/modeling/difference-equation/TimeStep.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/String.hpp>

namespace vle { namespace gvle { namespace modeling { namespace de {

void TimeStep::assign(vpz::Condition& condition)
{
    if (not m_checkGlobal->get_active()) {
	if (m_comboUnit->get_active_text().empty()) {
	    condition.addValueToPort("time-step", value::Double::create(
					 m_spinTime->get_value()));
	} else {
	    value::Map* map = new value::Map();

	    map->addDouble("value", m_spinTime->get_value());
	    map->addString("unit", m_comboUnit->get_active_text());
	    condition.addValueToPort("time-step", map);
	}
    }
}

Gtk::Widget& TimeStep::build(Glib::RefPtr<Gnome::Glade::Xml> ref)
{
    ref->get_widget("TimeStepFrame", m_frame);
    ref->get_widget("TimeStepSpinButton", m_spinTime);
    ref->get_widget("TimeStepGlobalButton", m_checkGlobal);
    ref->get_widget_derived("UnitTimeStepComboBox", m_comboUnit);

    m_spinTime->set_range(0.01,
			  std::numeric_limits < double >::max());
    m_spinTime->set_value(1.00);

    m_comboUnit->append_text("");
    m_comboUnit->append_text("day");
    m_comboUnit->append_text("week");
    m_comboUnit->append_text("month");
    m_comboUnit->append_text("year");
    m_comboUnit->show_all();

    m_checkGlobal->signal_clicked().connect(
	sigc::mem_fun(*this, &TimeStep::onClickCheckButton));

    return *m_frame;
}

void TimeStep::deletePorts(vpz::Condition& condition)
{
    std::list < std::string> list;

    condition.portnames(list);
    if (std::find(list.begin(), list.end(), "time-step") != list.end()) {
	condition.del("time-step");
    }
    if (std::find(list.begin(), list.end(), "init") != list.end()) {
	condition.del("init");
    }
}

void TimeStep::fillFields(const vpz::Condition& condition)
{
    if (validPort(condition, "time-step")) {
       if (condition.firstValue("time-step").isMap()) {
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
}

bool TimeStep::validPort(const vpz::Condition& condition,
			 const std::string& name)
{
    std::list < std::string> list;

    condition.portnames(list);
    if (std::find(list.begin(), list.end(), name) != list.end()) {
	if (name == "time-step") {
	    if (condition.getSetValues(name).size() != 0 and
		(condition.firstValue(name).isDouble()
		 or condition.firstValue(name).isMap()))
		return true;
	}
    }
    return false;
}

void TimeStep::onClickCheckButton()
{
    if (m_checkGlobal->get_active()) {
	m_spinTime->set_sensitive(false);
	m_comboUnit->set_sensitive(false);
    } else {
	m_spinTime->set_sensitive(true);
	m_comboUnit->set_sensitive(true);
    }
}

}}}} // namespace vle gvle modeling de
