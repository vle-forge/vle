/**
 * @file vle/gvle/conditions/DifferenceEquation/Multiple.cpp
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


#include <vle/gvle/conditions/DifferenceEquation/Multiple.hpp>
#include <vle/utils/Path.hpp>

namespace vle { namespace gvle { namespace conditions {

Multiple::Multiple(const std::string& name) : ConditionPlugin(name), m_dialog(0)
{ }

void Multiple::build()
{
    Gtk::VBox* vbox;
    std::string glade = utils::Path::path().
        getConditionGladeFile("DifferenceEquation.glade");
    Glib::RefPtr<Gnome::Glade::Xml> ref = Gnome::Glade::Xml::create(glade);

    ref->get_widget("DialogPluginMultipleBox", m_dialog);
    m_dialog->set_title("DifferenceEquation - Multiple");
    ref->get_widget("MultiplePluginVBox", vbox);

    vbox->pack_start(Variables::build(ref));
    vbox->pack_start(TimeStep::build(ref));
    vbox->pack_start(Mapping::build(ref));
}

void Multiple::fillFields(vpz::Condition& condition)
{
    Mapping::fillFields(condition);
    TimeStep::fillFields(condition);
    Variables::fillFields(condition);
}

bool Multiple::start(vpz::Condition& condition)
{
    build();
    Multiple::fillFields(condition);
    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
	Multiple::assign(condition);
    }
    m_dialog->hide();
    return true;
}

void Multiple::assign(vpz::Condition& condition)
{
    Mapping::deletePorts(condition);
    TimeStep::deletePorts(condition);
    Variables::deletePorts(condition);

    Mapping::assign(condition);
    TimeStep::assign(condition);
    Variables::assign(condition);
}

}}}

DECLARE_GVLE_CONDITIONPLUGIN(vle::gvle::conditions::Multiple)
