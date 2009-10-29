/**
 * @file vle/gvle/outputs/cairogauge/CairoGauge.cpp
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


#include <vle/gvle/outputs/cairogauge/CairoGauge.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Double.hpp>
#include <vle/utils/Path.hpp>
#include <gtkmm/filechooserdialog.h>
#include <libglademm/xml.h>
#include <cassert>

namespace vle { namespace gvle { namespace outputs {

CairoGauge::CairoGauge(const std::string& name)
    : OutputPlugin(name), m_dialog(0)
{
}

CairoGauge::~CairoGauge()
{
}

bool CairoGauge::start(vpz::Output& output, vpz::View& /* view */)
{
    assert(output.plugin() == "cairogauge");

    std::string glade =
	utils::Path::path().getOutputGladeFile("cairogauge.glade");
    Glib::RefPtr<Gnome::Glade::Xml> ref = Gnome::Glade::Xml::create(glade);

    /*
      recup widgets
    */
    ref->get_widget("dialog", m_dialog);
    ref->get_widget("spinbuttonMin", m_min);
    ref->get_widget("spinbuttonMax", m_max);

    /*
      assert
    */
    assert(m_dialog);
    assert(m_min);
    assert(m_max);

    init(output);

    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
        assign(output);
    }

    m_dialog->hide();
    return true;
}

void CairoGauge::init(vpz::Output& output)
{
    const value::Value* init = output.data();
    if (init and init->isMap()) {
	const value::Map* map = value::toMapValue(init);
	if (map->existValue("min")) {
            m_min->set_value(value::toDouble(map->get("min")));
        } else {
            m_min->set_value(1.0);
        }
	if (map->existValue("max")) {
            m_max->set_value(value::toDouble(map->get("max")));
        } else {
            m_max->set_value(10.0);
        }
    }
}

void CairoGauge::assign(vpz::Output& output)
{
    value::Map* map = new value::Map();
    if (m_min->get_value() < m_max->get_value()) {
	map->addDouble("min", m_min->get_value());
	map->addDouble("max", m_max->get_value());
	output.setData(map);
    } else {
	Error(_("Min must be inferior to Max."));
	delete map;
    }
}

}}} // namespace vle gvle outputs

DECLARE_GVLE_OUTPUTPLUGIN(vle::gvle::outputs::CairoGauge)

