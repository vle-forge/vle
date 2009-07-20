/**
 * @file vle/gvle/outputs/cairolevel/CairoLevel.cpp
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

#include <vle/gvle/outputs/cairolevel/CairoLevel.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/String.hpp>
#include <vle/utils/Path.hpp>
#include <libglademm/xml.h>
#include <cassert>
#include <boost/lexical_cast.hpp>
#include <boost/assign/std/vector.hpp>

namespace vle { namespace gvle { namespace outputs {

CairoLevel::CurveSettings::CurveSettings(
    CairoLevel* parent, std::string& curve, Gdk::Color color,
    double min, double max)
    : Gtk::Frame(curve), m_parent(parent), m_delete(Gtk::Stock::DELETE),
      m_curve(curve), m_color(color)
{
    m_alignment.show();
    m_delete.show();
    m_vbox.show();
    m_color.show();
    m_min.show();
    m_max.show();

    m_vbox.set_spacing(5);
    m_delete.signal_clicked().connect(
	sigc::bind(sigc::mem_fun(*m_parent, &CairoLevel::removeCurve), m_curve));

    m_alignment.set_padding(5, 5, 10, 10);
    m_alignment.add(m_vbox);
    add(m_alignment);
    init();
    show();

    m_min.set_value(min);
    m_max.set_value(max);
}

void CairoLevel::CurveSettings::init()
{
    Gtk::Label *labelcurve, *labelmin, *labelmax;

    m_min.set_numeric(true);
    m_max.set_numeric(true);
    m_min.set_digits(3);
    m_max.set_digits(3);
    m_min.set_increments(0.1, 1);
    m_max.set_increments(0.1, 1);
    m_min.set_range(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    m_max.set_range(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());

    labelcurve = new Gtk::Label(m_curve);
    labelmin = new Gtk::Label(_("min"));
    labelmax = new Gtk::Label(_("max"));
    labelcurve->show(); labelmin->show(); labelmax->show();

    Gtk::HBox* hbox = new Gtk::HBox();
    hbox->show();
    hbox->set_spacing(5);
    hbox->pack_start(*labelcurve, Gtk::PACK_EXPAND_PADDING);
    hbox->pack_start(m_color, Gtk::PACK_EXPAND_PADDING);
    hbox->pack_start(m_delete, Gtk::PACK_EXPAND_PADDING);
    m_vbox.pack_start(*hbox, Gtk::PACK_SHRINK);

    hbox = new Gtk::HBox();
    hbox->show();
    hbox->set_spacing(5);
    hbox->pack_start(*labelmin, Gtk::PACK_EXPAND_PADDING);
    hbox->pack_start(m_min, Gtk::PACK_EXPAND_PADDING);
    hbox->pack_start(*labelmax, Gtk::PACK_EXPAND_PADDING);
    hbox->pack_start(m_max, Gtk::PACK_EXPAND_PADDING);
    m_vbox.pack_start(*hbox, Gtk::PACK_SHRINK);

    show_all_children();
}

CairoLevel::CairoLevel(const std::string& name)
    : OutputPlugin(name), m_dialog(0), m_counter(0)
{
    using namespace boost::assign;

    m_colors += "purple1", "red", "green", "blue", "black", "yellow",
	"pink", "orange", "chocolate", "navy"; /* /!\ it will start
						* with 'red'
						*/
}

CairoLevel::~CairoLevel()
{
}

bool CairoLevel::start(vpz::Output& output, vpz::View& /* view */)
{
    assert(output.plugin() == "cairolevel");

    std::string glade = utils::Path::path().getOutputGladeFile("cairolevel.glade");
    Glib::RefPtr<Gnome::Glade::Xml> ref = Gnome::Glade::Xml::create(glade);

    ref->get_widget("dialog", m_dialog);
    ref->get_widget("ButtonAddCurve", m_addCurve);
    ref->get_widget("spinbuttonMinX", m_minx);
    ref->get_widget("spinbuttonMaxX", m_maxx);
    ref->get_widget("spinbuttonMinY", m_miny);
    ref->get_widget("spinbuttonMaxY", m_maxy);
    ref->get_widget("vboxCurves", m_curvesVBox);

    m_addCurve->signal_clicked().connect(
	sigc::mem_fun(*this, &CairoLevel::onAddCurve));

    assert(m_dialog);
    assert(m_addCurve);
    assert(m_minx);
    assert(m_maxx);
    assert(m_miny);
    assert(m_maxy);
    assert(m_curvesVBox);

    init(output);

    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
	assign(output);
    }
    m_dialog->hide();
    return true;
}

void CairoLevel::init(vpz::Output& output)
{
    const value::Value* init = output.data();
    m_curves.clear();
    m_counter = 0;

    if (init and init->isMap()) {
	const value::Map* map = value::toMapValue(init);
	if (map->existValue("size")) {
	    const value::Map& sizeMap = map->getMap("size");

	    if (sizeMap.existValue("minx")) {
		m_minx->set_value(value::toDouble(sizeMap.get("minx")));
	    } else {
		m_minx->set_value(0.0);
	    }
	    if (sizeMap.existValue("maxx")) {
		m_maxx->set_value(value::toDouble(sizeMap.get("maxx")));
	    } else {
		m_maxx->set_value(10.0);
	    }
	    if (sizeMap.existValue("miny")) {
		m_miny->set_value(value::toDouble(sizeMap.get("miny")));
	    } else {
		m_miny->set_value(0.0);
	    }
	    if (sizeMap.existValue("maxy")) {
		m_maxy->set_value(value::toDouble(sizeMap.get("maxy")));
	    } else {
		m_maxy->set_value(10.0);
	    }
	}
	if (map->existValue("curves")) {
	    const value::Set& curvesSet = map->getSet("curves");

	    for (value::Set::const_iterator it = curvesSet.begin();
		 it != curvesSet.end(); ++it) {
		value::Map* curveMap = value::toMapValue(*it);
		Gdk::Color color; double min; double max; std::string curve;

		if (curveMap->existValue("color")) {
		    color = Gdk::Color(curveMap->getString("color"));
		} else {
		    color = Gdk::Color(m_colors[++m_counter % m_colors.size()]);
		}
		if (curveMap->existValue("min")) {
		    min = curveMap->getDouble("min");
		} else {
		    min = -10.0;
		}
		if (curveMap->existValue("max")) {
		    max = curveMap->getDouble("max");
		} else {
		    max = 10.0;
		}
		std::ostringstream oss;
		oss << _("curve ") << ++m_counter;
		curve = oss.str();
		addCurve(curve, color, min, max);
	    }
	}
    }
}

void CairoLevel::assign(vpz::Output& output)
{
    value::Map* map = new value::Map();
    value::Set* setCurves = new value::Set();
    value::Map* mapSize = new value::Map();

    // add curves
    for(Curves::iterator it = m_curves.begin(); it != m_curves.end(); ++it) {
	value::Map* cMap = new value::Map();
	if (it->second->get_min() > it->second->get_max()) {
	    Error((fmt(_("Curve %1%: Min must be inferior to Max")) %
		   m_counter).str());
	} else {
	    cMap->addString("color", it->second->get_color().to_string());
	    cMap->addDouble("min", it->second->get_min());
	    cMap->addDouble("max", it->second->get_max());
	    setCurves->add(*cMap);
	}
    }

    // add size
    if (m_minx->get_value() > m_maxx->get_value()) {
	Error(_("MinX must be inferior to MaxX"));
    } else {
	mapSize->addDouble("minx", m_minx->get_value());
	mapSize->addDouble("maxx", m_maxx->get_value());
    }

    if (m_miny->get_value() > m_maxy->get_value()) {
	Error(_("MinY must be inferior to MaxY"));
    } else {
	mapSize->addDouble("miny", m_miny->get_value());
	mapSize->addDouble("maxy", m_maxy->get_value());
    }

    map->add("curves", setCurves);
    map->add("size", mapSize);

    output.setData(map);
}

void CairoLevel::onAddCurve()
{
    std::ostringstream oss;
    oss << _("curve ") << ++m_counter;
    std::string curve = oss.str();
    Gdk::Color color(m_colors[m_counter % m_colors.size()]);

    addCurve(curve, color, -10.0, 10.0);
}

void CairoLevel::addCurve(std::string& curve, Gdk::Color& color, double min, double max)
{
    CurveSettings* curvebox = new CurveSettings(this, curve, color, min, max);
    m_curves.insert(std::make_pair<std::string, CurveSettings*>(curve, curvebox));

    m_curvesVBox->pack_start(*curvebox);
    m_curvesVBox->show();
}

void CairoLevel::removeCurve(std::string& curve)
{
    Curves::iterator it = m_curves.find(curve);

    if (it != m_curves.end()) {
	m_curvesVBox->remove(*it->second);
	m_curves.erase(it);
    }
    m_curvesVBox->show();
}

}}} // namespace vle gvle outputs

DECLARE_GVLE_OUTPUTPLUGIN(vle::gvle::outputs::CairoLevel)
