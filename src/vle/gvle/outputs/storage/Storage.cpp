/*
 * @file vle/gvle/outputs/storage/Storage.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#include <vle/gvle/outputs/storage/Storage.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Integer.hpp>
#include <vle/utils/Path.hpp>
#include <gtkmm/filechooserdialog.h>
#include <libglademm/xml.h>
#include <cassert>

namespace vle { namespace gvle { namespace outputs {

Storage::Storage(const std::string& name)
    : OutputPlugin(name), m_dialog(0), m_rows(0), m_columns(0), m_rzrows(0),
    m_rzcolumns(0)
{
}

Storage::~Storage()
{
}

bool Storage::start(vpz::Output& output, vpz::View& /* view */)
{
    assert(output.plugin() == "storage");

    std::string glade = utils::Path::path().getOutputGladeFile("storage.glade");
    Glib::RefPtr<Gnome::Glade::Xml> ref = Gnome::Glade::Xml::create(glade);

    ref->get_widget("dialog", m_dialog);
    ref->get_widget("spinbuttonRows", m_rows);
    ref->get_widget("spinbuttonColumns", m_columns);
    ref->get_widget("spinbuttonResizeRows", m_rzrows);
    ref->get_widget("spinbuttonResizeColumns", m_rzcolumns);

    assert(m_dialog);
    assert(m_rows);
    assert(m_columns);
    assert(m_rzrows);
    assert(m_rzcolumns);

    init(output);

    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
        assign(output);
    }
    m_dialog->hide();
    return true;
}

void Storage::init(vpz::Output& output)
{
    const value::Value* init = output.data();
    if (init and init->isMap()) {
        const value::Map* map = value::toMapValue(init);
        if (map->exist("columns")) {
            m_columns->set_value(value::toInteger(map->get("columns")));
        } else {
            m_columns->set_value(50.0);
        }

        if (map->exist("rows")) {
            m_rows->set_value(value::toInteger(map->get("rows")));
        } else {
            m_rows->set_value(100.0);
        }

        if (map->exist("inc_columns")) {
            m_rzcolumns->set_value(value::toInteger(map->get("inc_columns")));
        } else {
            m_rzcolumns->set_value(50.0);
        }

        if (map->exist("inc_rows")) {
            m_rzrows->set_value(value::toInteger(map->get("inc_rows")));
        } else {
            m_rzrows->set_value(1000.0);
        }
    }
}

void Storage::assign(vpz::Output& output)
{
    value::Map* map = new value::Map();
    map->addInt("columns", m_columns->get_value());
    map->addInt("rows", m_rows->get_value());
    map->addInt("inc_columns", m_rzcolumns->get_value());
    map->addInt("inc_rows", m_rzrows->get_value());
    output.setData(map);
}

}}} // namespace vle gvle outputs

DECLARE_GVLE_OUTPUTPLUGIN(vle::gvle::outputs::Storage)
