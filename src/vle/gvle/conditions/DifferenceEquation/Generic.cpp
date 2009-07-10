/**
 * @file vle/gvle/conditions/DifferenceEquation/Generic.cpp
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

#include <vle/gvle/conditions/DifferenceEquation/Generic.hpp>
#include <vle/utils/Path.hpp>
#include <libglademm/xml.h>
#include <boost/lexical_cast.hpp>
#include <boost/assign/list_inserter.hpp>

using namespace std;
using namespace boost::assign;
using namespace vle;

namespace vle { namespace gvle { namespace conditions {

Generic::Generic(const std::string& name):
    ConditionPlugin(name),
    GenericPlugin(),
    m_dialog(0)
{}

Generic::~Generic()
{}

void Generic::build()
{
    std::string glade = utils::Path::path().
	getConditionGladeFile("DifferenceEquation.glade");
    Glib::RefPtr<Gnome::Glade::Xml> ref = Gnome::Glade::Xml::create(glade);

    ref->get_widget("DialogPluginGenericBox", m_dialog);
    m_dialog->set_title("DifferenceEquation - Generic");
    ref->get_widget("SpinButtonTimeGeneric", m_spinTime);
    ref->get_widget("TimestepGlobalButtonGeneric", m_checkGlobal);
    ref->get_widget("HBoxTimestepGeneric", m_hboxTimestep);
    ref->get_widget("entryNameGeneric", m_entryName);
    ref->get_widget("entryValueGeneric", m_entryValue);
    ref->get_widget_derived("initTreeViewGeneric", m_initTreeView);

    assert(m_dialog);
    assert(m_spinTime);
    assert(m_checkGlobal);
    assert(m_hboxTimestep);
    assert(m_entryName);
    assert(m_entryValue);
    assert(m_initTreeView);

    buildGeneric();
}

void Generic::fillFields(vpz::Condition& condition)
{
    fillFieldsGeneric(condition);
}

bool Generic::start(vpz::Condition& condition)
{
    build();
    fillFields(condition);

    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
	assign(condition);
    }

    m_dialog->hide();

    return true;
}

void Generic::assign(vpz::Condition& condition)
{
    deletePortsGeneric(condition);
    assignGeneric(condition);
}

}}} // namespace vle gvle conditions

DECLARE_GVLE_CONDITIONPLUGIN(vle::gvle::conditions::Generic)
