/**
 * @file vle/gvle/conditions/DifferenceEquation/Simple.cpp
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


#include <vle/gvle/conditions/DifferenceEquation/Simple.hpp>
#include <vle/utils/Path.hpp>

namespace vle { namespace gvle { namespace conditions {

Simple::Simple(const std::string& name) : ConditionPlugin(name), m_dialog(0)
{ }

Simple::~Simple()
{ }

void Simple::build()
{
    Gtk::VBox* vbox;
    std::string glade = utils::Path::path().
        getConditionGladeFile("DifferenceEquation.glade");
    Glib::RefPtr<Gnome::Glade::Xml> ref = Gnome::Glade::Xml::create(glade);

    ref->get_widget("DialogPluginSimpleBox", m_dialog);
    m_dialog->set_title("DifferenceEquation - Simple");
    ref->get_widget("SimplePluginVBox", vbox);

    vbox->pack_start(NameValue::build(ref));
    vbox->pack_start(TimeStep::build(ref));
    vbox->pack_start(Mapping::build(ref));
}

void Simple::fillFields(vpz::Condition& condition)
{
    NameValue::fillFields(condition);
    Mapping::fillFields(condition);
    TimeStep::fillFields(condition);
}

bool Simple::start(vpz::Condition& condition)
{
    build();
    fillFields(condition);

    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
	assign(condition);
    }
    m_dialog->hide();
    return true;
}

void Simple::assign(vpz::Condition& condition)
{
    NameValue::deletePorts(condition);
    Mapping::deletePorts(condition);
    TimeStep::deletePorts(condition);

    NameValue::assign(condition);
    Mapping::assign(condition);
    TimeStep::assign(condition);
}

}}} // namespace vle gvle conditions

DECLARE_GVLE_CONDITIONPLUGIN(vle::gvle::conditions::Simple)

