/**
 * @file vle/gvle/outputs/storage/CairoPlot.cpp
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

#include <vle/gvle/outputs/cairoplot/CairoPlot.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/utils/Path.hpp>
#include <libglademm/xml.h>
#include <cassert>
#include <boost/lexical_cast.hpp>
#include <boost/assign/std/vector.hpp>

namespace vle { namespace gvle { namespace outputs {

CairoPlot::CurveSettings::CurveSettings(CairoPlot* parent, std::string& curve,
					Gdk::Color color, std::string& type,
					std::string& name) :
    Gtk::Frame(curve), m_parent(parent), m_delete(Gtk::Stock::DELETE),
    m_color(color)

{
    m_name.set_text(name);
    m_type.append_text("integer");
    m_type.append_text("real");
    m_type.set_active_text(type);
    m_delete.signal_clicked().connect(
	sigc::bind(sigc::mem_fun(*m_parent, &CairoPlot::removeCurve), curve));

    init();
    show();
}

void CairoPlot::CurveSettings::init()
{
    Gtk::HBox *hbox = new Gtk::HBox();
    hbox->show();
    hbox->pack_start(*new Gtk::Label(_("color: ")), false, false, 0);
    hbox->pack_start(m_color, false, false, 10);
    hbox->pack_end(m_delete, false, false, 0);
    hbox->show_all_children();
    m_vbox.pack_start(*hbox, false, false, 0);

    hbox = new Gtk::HBox();
    hbox->show();
    hbox->pack_start(*new Gtk::Label(_("name:")), false, false, 0);
    hbox->pack_start(m_name, false, false, 10);
    hbox->show_all_children();
    m_vbox.pack_start(*hbox, false, false, 0);

    hbox = new Gtk::HBox();
    hbox->show();
    hbox->pack_start(*new Gtk::Label(_("type:")), false, false, 0);
    hbox->pack_start(m_type, false, false, 18);
    hbox->show_all_children();
    m_vbox.pack_start(*hbox, false, false, 0);

    m_vbox.show();
    add(m_vbox);
}

CairoPlot::LimitSettings::LimitSettings(CairoPlot* parent, std::string& limit,
					Gdk::Color color, double value) :
    Gtk::Frame(limit), m_parent(parent), m_delete(Gtk::Stock::DELETE),
    m_color(color)
{
    m_delete.signal_clicked().connect(
	sigc::bind(sigc::mem_fun(*m_parent, &CairoPlot::removeLimit), limit));

    m_value.set_range(std::numeric_limits<int>::min(),
		      std::numeric_limits<int>::max());
    m_value.set_increments(0.1, 1);
    m_value.set_digits(2);
    m_value.set_value(value);
    init();
    show();
}

void CairoPlot::LimitSettings::init()
{
    Gtk::HBox* hbox = new Gtk::HBox();
    hbox->show();
    hbox->pack_start(*new Gtk::Label(_("color: ")), false, false, 0);
    hbox->pack_start(m_color, false, false, 10);
    hbox->pack_end(m_delete, false, false, 5);
    hbox->show_all_children();
    m_vbox.pack_start(*hbox, false, false, 0);

    hbox = new Gtk::HBox();
    hbox->show();
    hbox->pack_start(*new Gtk::Label(_("value: ")), false, false, 0);
    hbox->pack_start(m_value, false, false, 10);
    hbox->show_all_children();
    m_vbox.pack_start(*hbox, false, false, 0);

    m_vbox.show();
    add(m_vbox);
}

CairoPlot::CairoPlot(const std::string& name)
    : OutputPlugin(name), m_dialog(0), m_counter_curves(0), m_counter_limits(0)
{
    using namespace boost::assign;

    m_colors += "purple1", "red", "green", "blue", "black", "yellow",
	"pink", "orange", "chocolate", "navy"; /* /!\ it will start
						* with 'red'
						*/
}

CairoPlot::~CairoPlot()
{
}

bool CairoPlot::start(vpz::Output& output, vpz::View& /* view */)
{
    assert(output.plugin() == "cairoplot");

    std::string glade =
	utils::Path::path().getOutputGladeFile("cairoplot.glade");
    Glib::RefPtr<Gnome::Glade::Xml> ref = Gnome::Glade::Xml::create(glade);

    /*
      recup widgets
    */
    ref->get_widget("dialog", m_dialog);
    ref->get_widget("ButtonAddCurve", m_addCurve);
    ref->get_widget("ButtonAddLimit", m_addLimit);
    ref->get_widget("vboxCurves", m_vboxCurves);
    ref->get_widget("vboxLimits", m_vboxLimits);
    ref->get_widget("useWindow", m_useWindow);
    ref->get_widget("useValue", m_useValue);
    ref->get_widget("useScrolling", m_useScrolling);
    ref->get_widget("useLimits", m_useLimits);
    ref->get_widget("checkScrolling", m_checkScrolling);
    ref->get_widget("spinWindow", m_spinWindow);
    ref->get_widget("spinValueMin", m_spinValueMin);
    ref->get_widget("spinValueMax", m_spinValueMax);
    ref->get_widget("expandOptions", m_expandOptions);
    ref->get_widget("expandLimits", m_expandLimits);
    /*
      assert
    */
    assert(m_dialog);
    assert(m_addCurve);
    assert(m_addLimit);
    assert(m_vboxCurves);
    assert(m_vboxLimits);
    assert(m_useWindow);
    assert(m_useValue);
    assert(m_useScrolling);
    assert(m_useLimits);
    assert(m_checkScrolling);
    assert(m_spinWindow);
    assert(m_spinValueMin);
    assert(m_spinValueMax);
    assert(m_expandOptions);
    assert(m_expandLimits);

   /*
     signals
   */
    m_addCurve->signal_clicked().connect(
	sigc::mem_fun(*this, &CairoPlot::onAddCurve));
    m_addLimit->signal_clicked().connect(
	sigc::mem_fun(*this, &CairoPlot::onAddLimit));
    m_useWindow->signal_toggled().connect(
	sigc::bind(sigc::mem_fun(*this, &CairoPlot::sensitiveArea), WINDOW));
    m_useValue->signal_toggled().connect(
	sigc::bind(sigc::mem_fun(*this, &CairoPlot::sensitiveArea), VALUE));
    m_useScrolling->signal_toggled().connect(
	sigc::bind(sigc::mem_fun(*this, &CairoPlot::sensitiveArea), SCROLLING));
    m_useLimits->signal_toggled().connect(
	sigc::bind(sigc::mem_fun(*this, &CairoPlot::sensitiveArea), LIMITS));

    init(output);

    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
        assign(output);
    }

    m_dialog->hide();
    return true;
}

void CairoPlot::init(vpz::Output& output)
{
    m_counter_limits = 0; m_limits.clear();
    m_counter_curves = 0; m_curves.clear();

    const value::Value* init = output.data();
    if (init and init->isMap()) {
	const value::Map* map = value::toMapValue(init);

	if (map->existValue("curves")) {
	    const value::Set& curvesSet = map->getSet("curves");

	    for (value::Set::const_iterator it = curvesSet.begin();
		 it != curvesSet.end(); ++it) {
		value::Map* curveMap = value::toMapValue(*it);
		int r, g, b; std::string name, type, curve;

		m_counter_curves++;
		if (curveMap->existValue("color_red")
		    and curveMap->existValue("color_green")
		    and curveMap->existValue("color_blue"))
		{
		    r = curveMap->getInt("color_red");
		    g = curveMap->getInt("color_green");
		    b = curveMap->getInt("color_blue");
		} else {
		    Gdk::Color color = Gdk::Color(
			m_colors[m_counter_curves % m_colors.size()]);
		    r = color.get_red();
		    g = color.get_green();
		    b = color.get_blue();
		}
		if (curveMap->existValue("name")) {
		    name = curveMap->getString("name");
		} else {
		    name = "curve ";
		}
		if (curveMap->existValue("type")) {
		    type = curveMap->getString("type");
		} else {
		    type = "real";
		}

		std::ostringstream oss;
		oss << _("curve ") << m_counter_curves;
		curve = oss.str();
		Gdk::Color color;
		color.set_rgb(r, g, b);
		addCurve(curve, color, type, name);
	    }
	}
	if (map->existValue("limits")) {
	    m_useLimits->set_active(true);
	    sensitiveArea(LIMITS);
	    const value::Set& limitsSet = map->getSet("limits");

	    for (value::Set::const_iterator it = limitsSet.begin();
		 it != limitsSet.end(); ++it) {
		value::Map* limitMap = value::toMapValue(*it);
		int r, g, b; double value; std::string limit;

		m_counter_limits++;
		if (limitMap->existValue("color_red")
		    and limitMap->existValue("color_green")
		    and limitMap->existValue("color_blue"))
		{
		    r = limitMap->getInt("color_red");
		    g = limitMap->getInt("color_green");
		    b = limitMap->getInt("color_blue");
		} else {
		    Gdk::Color color = Gdk::Color(
			m_colors[m_counter_limits % m_colors.size()]);
		    r = color.get_red();
		    g = color.get_green();
		    b = color.get_blue();
		}
		if (limitMap->existValue("value")) {
		    value = limitMap->getDouble("value");
		} else {
		    value = 1.0;
		}
		std::ostringstream oss;
		oss << _("limit ") << m_counter_limits;
		limit = oss.str();
		Gdk::Color color;
		color.set_rgb(r, g, b);
		addLimit(limit, color, value);
	    }
	}
	if (map->existValue("value")) {
	    const value::Map& valueMap = map->getMap("value");
	    m_useValue->set_active(true);
	    sensitiveArea(VALUE);
	    if (valueMap.existValue("min")) {
		m_spinValueMin->set_value(valueMap.getDouble("min"));
	    }
	    if (valueMap.existValue("max")) {
		m_spinValueMax->set_value(valueMap.getDouble("max"));
	    }
	}
	if (map->existValue("scrolling")) {
	    m_useScrolling->set_active(true);
	    sensitiveArea(SCROLLING);
	    m_checkScrolling->set_active(map->getBoolean("scrolling"));
	}
	if (map->existValue("window")) {
	    m_useWindow->set_active(true);
	    sensitiveArea(WINDOW);
	    m_spinWindow->set_value(map->getDouble("window"));
	}
    }
}

void CairoPlot::assign(vpz::Output& output)
{
    value::Map* map = new value::Map();
    value::Set* setCurves = new value::Set();

    // add curves
    for(Curves::iterator it = m_curves.begin(); it != m_curves.end(); ++it) {
	value::Map* cMap = new value::Map();
	cMap->addInt("color_red", it->second->get_red());
	cMap->addInt("color_green", it->second->get_green());
	cMap->addInt("color_blue", it->second->get_blue());
	cMap->addString("type", it->second->get_type());
	cMap->addString("name", it->second->get_name());
	setCurves->add(*cMap);
    }
    map->add("curves", setCurves);

    // add window
    if (m_useWindow->get_active()) {
	map->addDouble("window", m_spinWindow->get_value());
    }

    // add value
    if (m_useValue->get_active()) {
	value::Map* vMap = new value::Map();
	if (m_spinValueMin->get_value() > m_spinValueMax->get_value()) {
	    Error(_("value.min must be lower than value.max"));
	} else {
	    vMap->addDouble("min", m_spinValueMin->get_value());
	    vMap->addDouble("max", m_spinValueMax->get_value());
	    map->add("value", vMap);
	}
    }

    // add scrolling
    if (m_useScrolling->get_active()) {
	map->addBoolean("scrolling", m_checkScrolling->get_active());
    }

    // add limits
    if (m_useLimits->get_active()) {
	value::Set* setLimits = new value::Set();
	for (Limits::iterator it = m_limits.begin();
	     it != m_limits.end(); it++) {
	    value::Map* lMap = new value::Map();
	    lMap->addInt("color_red", it->second->get_red());
	    lMap->addInt("color_green", it->second->get_green());
	    lMap->addInt("color_blue", it->second->get_blue());
	    lMap->addDouble("value", it->second->get_value());
	    setLimits->add(*lMap);
	}
	map->add("limits", setLimits);
    }

    output.setData(map);
}

void CairoPlot:: onAddCurve()
{
    Gdk::Color color(m_colors[m_counter_curves % m_colors.size()]);
    std::ostringstream oss;
    oss << _("curve ") << ++m_counter_curves;
    std::string curve = oss.str();
    std::string type("real");

    addCurve(curve, color, type, curve);
}

void CairoPlot::addCurve(std::string& curve, Gdk::Color& color,
			 std::string& type, std::string& name)
{
    CurveSettings* curvebox = new CurveSettings(this, curve, color, type, name);
    m_curves.insert(std::make_pair<std::string, CurveSettings*>(
			curve, curvebox));

    m_vboxCurves->pack_start(*curvebox);
    m_vboxCurves->show();
}

void CairoPlot::removeCurve(std::string& curve)
{
    Curves::iterator it = m_curves.find(curve);

    if (it != m_curves.end()) {
	m_vboxCurves->remove(*it->second);
	m_curves.erase(it);
    }
    m_vboxCurves->show();
}

void CairoPlot:: onAddLimit()
{
    std::ostringstream oss;
    oss << _("limit ") << ++m_counter_limits;
    std::string limit = oss.str();
    Gdk::Color color(m_colors[m_counter_limits % m_colors.size()]);

    addLimit(limit, color, 1.0);
}

void CairoPlot::addLimit(std::string& limit, Gdk::Color& color, double value)
{
    LimitSettings* limitbox = new LimitSettings(this, limit, color, value);
    m_limits.insert(std::make_pair<std::string, LimitSettings*>(
			limit, limitbox));

    m_vboxLimits->pack_start(*limitbox);
    m_vboxLimits->show();
}

void CairoPlot::removeLimit(std::string& limit)
{
    Limits::iterator it = m_limits.find(limit);

    if (it != m_limits.end()) {
	m_vboxLimits->remove(*it->second);
	m_limits.erase(it);
    }
    m_vboxLimits->show();
}

void CairoPlot:: sensitiveArea(const Area& area)
{
    switch (area) {
    case WINDOW:
	m_expandOptions->set_expanded(true);
	m_spinWindow->set_sensitive(m_useWindow->get_active());
	break;
    case VALUE:
	m_expandOptions->set_expanded(true);
	m_spinValueMin->set_sensitive(m_useValue->get_active());
	m_spinValueMax->set_sensitive(m_useValue->get_active());
	break;
    case SCROLLING:
	m_expandOptions->set_expanded(true);
	m_checkScrolling->set_sensitive(m_useScrolling->get_active());
	break;
    case LIMITS:
	m_expandOptions->set_expanded(true);
	m_expandLimits->set_expanded(true);
	m_addLimit->set_sensitive(m_useLimits->get_active());
	m_vboxLimits->set_sensitive(m_useLimits->get_active());
	break;
    default:
	break;
    }
}

}}} // namespace vle gvle outputs

DECLARE_GVLE_OUTPUTPLUGIN(vle::gvle::outputs::CairoPlot)

