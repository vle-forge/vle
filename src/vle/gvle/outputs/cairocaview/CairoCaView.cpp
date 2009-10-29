/**
 * @file vle/gvle/outputs/cairocaview/CairoCaView.cpp
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


#include <vle/gvle/outputs/cairocaview/CairoCaView.hpp>
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

CairoCaView::ValueSettings::ValueSettings(CairoCaView* parent,
					  const std::string& title,
                                          Type type)
    : Gtk::Frame(title), m_parent(parent), m_delete(Gtk::Stock::DELETE),
    m_type(type)
{
    m_delete.signal_clicked().connect(
	sigc::bind(sigc::mem_fun(*m_parent, &CairoCaView::removeValue), title));
}

CairoCaView::BooleanSettings::BooleanSettings(CairoCaView* parent,
					      const std::string& title,
					      Gdk::Color& color_false,
					      Gdk::Color& color_true) :
    CairoCaView::ValueSettings(parent, title, BOOLEAN), m_color_false(color_false),
    m_color_true(color_true)
{
    m_vbox.set_spacing(5);
    init();
    show();
}

void CairoCaView::BooleanSettings::init()
{
    Gtk::HBox* hbox = new Gtk::HBox();
    hbox->show();
    hbox->pack_start(*new Gtk::Label(_("color true:")), false, false, 0);
    hbox->pack_start(m_color_true, false, false, 10);
    hbox->pack_end(getButtonDelete(), false, false, 5);
    hbox->show_all_children();
    m_vbox.pack_start(*hbox, false, false, 0);

    hbox = new Gtk::HBox();
    hbox->show();
    hbox->pack_start(*new Gtk::Label(_("color false:")), false, false, 0);
    hbox->pack_start(m_color_false, false, false, 10);
    hbox->show_all_children();
    m_vbox.pack_start(*hbox, false, false, 0);

    m_vbox.show();
    add(m_vbox);
}

value::Map* CairoCaView::BooleanSettings::toMap()
{
    value::Map* map = new value::Map();

    map->addInt("red_true", m_color_true.get_color().get_red());
    map->addInt("green_true", m_color_true.get_color().get_green());
    map->addInt("blue_true", m_color_true.get_color().get_blue());
    map->addInt("red_false", m_color_false.get_color().get_red());
    map->addInt("green_false", m_color_false.get_color().get_green());
    map->addInt("blue_false", m_color_false.get_color().get_blue());
    return map;
}

CairoCaView::IntegerSettings::IntegerSettings(CairoCaView* parent,
					      const std::string& title,
					      double value, Gdk::Color& color) :
    ValueSettings(parent, title, INTEGER), m_color(color)
{
    m_value.set_range(std::numeric_limits<int>::min(),
		      std::numeric_limits<int>::max());
    m_value.set_increments(1, 10);
    m_value.set_value(value);
    m_vbox.set_spacing(5);

    init();
    show();
}

void CairoCaView::IntegerSettings::init()
{
    Gtk::HBox* hbox = new Gtk::HBox();
    hbox->show();
    hbox->pack_start(*new Gtk::Label(_("color:")), false, false, 0);
    hbox->pack_start(m_color, false, false, 10);
    hbox->pack_end(getButtonDelete(), false, false, 5);
    hbox->show_all_children();
    m_vbox.pack_start(*hbox, false, false, 0);

    hbox = new Gtk::HBox();
    hbox->show();
    hbox->pack_start(*new Gtk::Label(_("value:")), false, false, 0);
    hbox->pack_start(m_value, false, false, 10);
    hbox->show_all_children();
    m_vbox.pack_start(*hbox, false, false, 0);

    m_vbox.show();
    add(m_vbox);
}

value::Map* CairoCaView::IntegerSettings::toMap()
{
    value::Map* map = new value::Map();

    map->addDouble("value", m_value.get_value());
    map->addInt("red", m_color.get_color().get_red());
    map->addInt("green", m_color.get_color().get_green());
    map->addInt("blue", m_color.get_color().get_blue());
    return map;
}

CairoCaView::RealSettings::RealSettings(CairoCaView* parent,
					const std::string& title,
					double min, double max,
					const std::string& color,
					const std::string& type, double coef) :
    ValueSettings(parent, title, REAL)
{
    m_min.set_range(std::numeric_limits<int>::min(),
		    std::numeric_limits<int>::max());
    m_min.set_increments(0.1, 1.0);
    m_min.set_digits(3);
    m_min.set_value(min);
    m_max.set_range(std::numeric_limits<int>::min(),
		    std::numeric_limits<int>::max());
    m_max.set_increments(0.1, 1.0);
    m_max.set_value(max);
    m_max.set_digits(3);
    m_coef.set_range(std::numeric_limits<int>::min(),
		     std::numeric_limits<int>::max());
    m_coef.set_digits(5);
    m_coef.set_increments(0.1, 1.0);
    m_coef.set_value(coef);

    m_type.append_text("linear");
    m_type.append_text("highvalue");
    m_type.append_text("lowvalue");
    m_type.set_active_text(type);
    m_type.signal_changed().connect(
	sigc::mem_fun(*this, &CairoCaView::RealSettings::onTypeChanged));

    m_color.append_text("red");
    m_color.append_text("green");
    m_color.append_text("blue");
    m_color.append_text("white");
    m_color.append_text("yellow");
    m_color.append_text("white_green");
    m_color.append_text("red_only");
    m_color.append_text("green_only");
    m_color.set_active_text(color);

    m_vbox.set_spacing(5);

    init();
    show();
}

void CairoCaView::RealSettings::init()
{
    Gtk::HBox* hbox = new Gtk::HBox();
    hbox->show();
    hbox->pack_start(*new Gtk::Label(_("min:")), false, false, 0);
    hbox->pack_start(m_min, false, false, 10);
    hbox->pack_start(*new Gtk::Label(_("max:")), false, false, 0);
    hbox->pack_start(m_max, false, false, 10);
    hbox->pack_end(getButtonDelete(), false, false, 5);
    hbox->show_all_children();
    m_vbox.pack_start(*hbox, false, false, 0);

    hbox = new Gtk::HBox();
    hbox->show();
    hbox->pack_start(*new Gtk::Label(_("color:")), false, false, 0);
    hbox->pack_start(m_color, false, false, 5);
    hbox->show_all_children();
    m_vbox.pack_start(*hbox, false, false, 0);

    hbox = new Gtk::HBox();
    hbox->show();
    hbox->pack_start(*new Gtk::Label(_("type:")), false, false, 0);
    hbox->pack_start(m_type, false, false, 5);
    hbox->pack_end(m_coef, false, false, 5);
    hbox->pack_end(*new Gtk::Label(_("coef:")), false, false, 0);
    m_coef.set_sensitive(m_type.get_active_text() != "linear");
    hbox->show_all_children();
    m_vbox.pack_start(*hbox, false, false, 0);

    m_vbox.show();
    add(m_vbox);
}

void CairoCaView::RealSettings::onTypeChanged()
{
    m_coef.set_sensitive(m_type.get_active_text() != "linear");
}

value::Map* CairoCaView::RealSettings::toMap()
{
    if (m_min.get_value() > m_max.get_value()) {
	gvle::Error(get_label() + _(": min must be lower than max."));
	return new value::Map();
    }
    value::Map* map = new value::Map();

    map->addDouble("min", m_min.get_value());
    map->addDouble("max", m_max.get_value());
    map->addString("color", m_color.get_active_text());
    map->addString("type", m_type.get_active_text());
    if (m_type.get_active_text() != "linear") {
	map->addDouble("coef", m_coef.get_value());
    }
    return map;
}

CairoCaView::ObjectSettings::ObjectSettings(CairoCaView* parent,
					    const std::string& title,
					    const std::string& name, int number,
					    const std::string& shape,
                                            Gdk::Color& color) :
    Gtk::Frame(title), m_parent(parent), m_delete(Gtk::Stock::DELETE),
    m_color(color)
{
    m_name.set_text(name);
    m_number.set_range(std::numeric_limits<int>::min(),
		       std::numeric_limits<int>::max());
    m_number.set_increments(0.1, 1.0);
    m_number.set_digits(2);
    m_number.set_value(number);

    m_shape.append_text("circle");
    m_shape.append_text("square");
    m_shape.set_active_text(shape);

    m_delete.signal_clicked().connect(
	sigc::bind(sigc::mem_fun(*m_parent, &CairoCaView::removeObject), title));

    m_vbox.set_spacing(5);

    init();
    show();
}

void CairoCaView::ObjectSettings::init()
{
    Gtk::HBox* hbox = new Gtk::HBox();
    hbox->show();
    hbox->pack_start(*new Gtk::Label(_("color:")), false, false, 0);
    hbox->pack_start(m_color, false, false, 10);
    hbox->pack_end(m_delete, false, false, 5);
    hbox->show_all_children();
    m_vbox.pack_start(*hbox, false, false, 0);

    hbox = new Gtk::HBox();
    hbox->show();
    hbox->pack_start(*new Gtk::Label(_("name:")), false, false, 0);
    hbox->pack_end(m_name, false, false, 5);
    hbox->show_all_children();
    m_vbox.pack_start(*hbox, false, false, 0);

    hbox = new Gtk::HBox();
    hbox->show();
    hbox->pack_start(*new Gtk::Label(_("number:")), false, false, 0);
    hbox->pack_start(m_number, false, false, 10);
    hbox->pack_start(*new Gtk::Label(_("shape:")), false, false, 0);
    hbox->pack_start(m_shape, false, false, 10);
    hbox->show_all_children();
    m_vbox.pack_start(*hbox, false, false, 0);

    m_vbox.show();
    add(m_vbox);
}

value::Map* CairoCaView::ObjectSettings::toMap()
{
    value::Map* map = new value::Map();

    map->addString("name", m_name.get_text());
    map->addInt("number", m_number.get_value());
    map->addString("shape", m_shape.get_active_text());
    map->addInt("color_red", m_color.get_color().get_red());
    map->addInt("color_green", m_color.get_color().get_green());
    map->addInt("color_blue", m_color.get_color().get_blue());
    return map;
}

CairoCaView::CairoCaView(const std::string& name)
    : OutputPlugin(name), m_dialog(0),
      m_counter_values(0), m_counter_objects(0)
{
    using namespace boost::assign;

    m_colors += "purple1", "red", "green", "blue", "black", "yellow",
	"pink", "orange", "chocolate", "navy"; /* /!\ it will start
						* with 'red'
						*/
}

CairoCaView::~CairoCaView()
{
}

bool CairoCaView::start(vpz::Output& output, vpz::View& /* view */)
{
    assert(output.plugin() == "cairocaview");

    std::string glade = utils::Path::path().getOutputGladeFile("cairocaview.glade");
    Glib::RefPtr<Gnome::Glade::Xml> ref = Gnome::Glade::Xml::create(glade);

    ref->get_widget("dialog", m_dialog);
    ref->get_widget("spinSizeX", m_spinSizeX);
    ref->get_widget("spinSizeY", m_spinSizeY);
    ref->get_widget("entryCellName", m_entryCellName);
    ref->get_widget("entryStateName", m_entryStateName);
    ref->get_widget("buttonAddValue", m_buttonAddValue);
    ref->get_widget("buttonAddObject", m_buttonAddObject);
    ref->get_widget("vboxValues", m_vboxValues);
    ref->get_widget("vboxObjects", m_vboxObjects);
    ref->get_widget("hboxGeometry", m_hboxGeometry);
    ref->get_widget("hboxStates", m_hboxStates);

    assert(m_dialog);
    assert(m_spinSizeX);
    assert(m_spinSizeY);
    assert(m_comboGeometryType);
    assert(m_comboStateType);
    assert(m_entryCellName);
    assert(m_entryStateName);
    assert(m_buttonAddValue);
    assert(m_buttonAddObject);
    assert(m_vboxValues);
    assert(m_vboxObjects);

    m_comboGeometryType = new Gtk::ComboBoxText();
    m_comboGeometryType->show();
    m_comboGeometryType->append_text("hexa");
    m_comboGeometryType->append_text("square");
    m_comboGeometryType->set_active_text("hexa");
    m_hboxGeometry->pack_start(*m_comboGeometryType);

    m_comboStateType = new Gtk::ComboBoxText();
    m_comboStateType->show();
    m_comboStateType->append_text("boolean");
    m_comboStateType->append_text("integer");
    m_comboStateType->append_text("real");
    m_comboStateType->set_active_text("boolean");
    m_previousType = m_comboStateType->get_active_text();
    m_hboxStates->pack_start(*m_comboStateType);

    m_buttonAddValue->signal_clicked().connect(
	sigc::mem_fun(*this, &CairoCaView::onAddValue));
    m_buttonAddObject->signal_clicked().connect(
	sigc::mem_fun(*this, &CairoCaView::onAddObject));
    m_comboStateType->signal_changed().connect(
	sigc::mem_fun(*this, &CairoCaView::onTypeChanged));

    init(output);

    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
	assign(output);
    }
    m_dialog->hide();
    return true;
}

void CairoCaView::init(vpz::Output& output)
{
    m_values.clear(); m_counter_values = 0;
    m_objects.clear(); m_counter_objects = 0;

    const value::Value* init = output.data();
    if (init and init->isMap()) {
	const value::Map* map = value::toMapValue(init);

	if (map->existValue("size")) {
	    const value::Map& sizeMap = map->getMap("size");
	    if (sizeMap.existValue("x"))
		m_spinSizeX->set_value(sizeMap.getInt("x"));
	    if (sizeMap.existValue("y"))
		m_spinSizeY->set_value(sizeMap.getInt("y"));
	}

	if (map->existValue("geometry")) {
	    const value::Map& geoMap = map->getMap("geometry");
	    if (geoMap.existValue("type"))
		m_comboGeometryType->set_active_text(geoMap.getString("type"));
	}

	if (map->existValue("cellName")) {
	    m_entryCellName->set_text(map->getString("cellName"));
	}

	if (map->existValue("states")) {
	    const value::Map& statesMap = map->getMap("states");

	    if (statesMap.existValue("name"))
		m_entryStateName->set_text(statesMap.getString("name"));
	    if (statesMap.existValue("type"))
		m_comboStateType->set_active_text(statesMap.getString("type"));
	    if (statesMap.existValue("values")) {
		const value::Set& valuesSet = statesMap.getSet("values");

		for (value::Set::const_iterator it = valuesSet.begin();
		     it != valuesSet.end(); ++it) {
		    value::Map* valueMap = value::toMapValue(*it);

		    m_counter_values++;
		    switch (stringToType(statesMap.getString("type"))) {
		    case BOOLEAN:
		    {
			int rf, gf, bf, rt, gt, bt;

			if (valueMap->existValue("red_true")
			    and valueMap->existValue("green_true")
			    and valueMap->existValue("blue_true"))
			{
			    rt = valueMap->getInt("red_true");
			    gt = valueMap->getInt("green_true");
			    bt = valueMap->getInt("blue_true");
			} else {
			    Gdk::Color color = Gdk::Color(
				m_colors[m_counter_values % m_colors.size()]);
			    rt = color.get_red();
			    gt = color.get_green();
			    bt = color.get_blue();
			}
			if (valueMap->existValue("red_false")
			    and valueMap->existValue("green_false")
			    and valueMap->existValue("blue_false"))
			{
			    rf = valueMap->getInt("red_false");
			    gf = valueMap->getInt("green_false");
			    bf = valueMap->getInt("blue_false");
			} else {
			    Gdk::Color color = Gdk::Color(
				m_colors[(m_colors.size() - m_counter_values +1)
					 % m_colors.size()]);
			    rf = color.get_red();
			    gf = color.get_green();
			    bf = color.get_blue();
			}

			Gdk::Color color_true, color_false;
			color_true.set_rgb(rt, gt, bt);
			color_false.set_rgb(rf, gf, bf);
			std::string title("boolean ");
			title += boost::lexical_cast<std::string>(m_counter_values);
			BooleanSettings* boolean = new BooleanSettings(
			    this, title, color_true, color_false);
			addValue(boolean);
			break;
		    }
		    case INTEGER:
		    {
			int r, g, b;
			double value;

			if (valueMap->existValue("red")
			    and valueMap->existValue("green")
			    and valueMap->existValue("blue"))
			{
			    r = valueMap->getInt("red");
			    g = valueMap->getInt("green");
			    b = valueMap->getInt("blue");
			} else {
			    Gdk::Color color = Gdk::Color(
				m_colors[m_counter_values % m_colors.size()]);
			    r = color.get_red();
			    g = color.get_green();
			    b = color.get_blue();
			}
			if (valueMap->existValue("value"))
			    value = valueMap->getDouble("value");
			else
			    value = 1.0;

			Gdk::Color color;
			color.set_rgb(r, g, b);
			std::string title("integer ");
			title += boost::lexical_cast<std::string>(m_counter_values);
			IntegerSettings* integer = new IntegerSettings(
			    this, title, value, color);
			addValue(integer);
			break;
		    }
		    case REAL:
		    {
			double min, max, coef;
			std::string color, type;
			if (valueMap->existValue("min"))
			    min = valueMap->getDouble("min");
			else
			    min = 0.0;
			if (valueMap->existValue("max"))
			    max = valueMap->getDouble("max");
			else
			    max = 10.0;
			if (valueMap->existValue("coef"))
			    coef = valueMap->getDouble("coef");
			else
			    coef = 1.0;
			if (valueMap->existValue("color"))
			    color = valueMap->getString("color");
			else
			    color = "red";
			if (valueMap->existValue("type"))
			    type = valueMap->getString("type");
			else
			    type = "linear";

			std::string title("real ");
			title += boost::lexical_cast<std::string>(m_counter_values);
			RealSettings* real = new RealSettings(
			    this, title, min, max, color, type, coef);
			addValue(real);
			break;
		    }
		    default:
			break;
		    }
		}
	    }
	}
	if (map->existValue("objects")) {
	    const value::Set& setObjects = map->getSet("objects");

	    for (value::Set::const_iterator it = setObjects.begin();
		 it != setObjects.end(); ++it) {
		value::Map* objMap = value::toMapValue(*it);
		m_counter_objects++;

		int number, r, g, b;
		std::string name, shape;
		if (objMap->existValue("color_red")
		    and objMap->existValue("color_green")
		    and objMap->existValue("color_blue"))
		{
		    r = objMap->getInt("color_red");
		    g = objMap->getInt("color_green");
		    b = objMap->getInt("color_blue");
		} else {
		    Gdk::Color color = Gdk::Color(
			m_colors[m_counter_objects % m_colors.size()]);
		    r = color.get_red();
		    g = color.get_green();
		    b = color.get_blue();
		}
		if (objMap->existValue("number"))
		    number = objMap->getInt("number");
		else
		    number = m_counter_values;
		if (objMap->existValue("name"))
		    name = objMap->getString("name");
		else {
		    name = "object ";
		    name += boost::lexical_cast<std::string>(m_counter_objects);
		}
		if (objMap->existValue("shape"))
		    shape = objMap->getString("shape");
		else
		    shape = "circle";

		Gdk::Color color;
		color.set_rgb(r, g, b);
		std::string title("object ");
		title += boost::lexical_cast<std::string>(m_counter_objects);

		addObject(title, name, number, shape, color);
	    }
	}
    }
}

void CairoCaView::assign(vpz::Output& output)
{
    value::Map* map = new value::Map();

    value::Map* mapSize = new value::Map();
    mapSize->addInt("x", m_spinSizeX->get_value());
    mapSize->addInt("y", m_spinSizeY->get_value());
    map->add("size", mapSize);

    value::Map* mapGeo = new value::Map();
    mapGeo->addString("type", m_comboGeometryType->get_active_text());
    map->add("geometry", mapGeo);

    map->addString("cellName", m_entryCellName->get_text());

    value::Map* mapStates = new value::Map();
    mapStates->addString("name", m_entryStateName->get_text());
    mapStates->addString("type", m_comboStateType->get_active_text());
    value::Set* setValues = new value::Set();
    for (Values::iterator it = m_values.begin(); it != m_values.end(); ++it) {
	setValues->add(*it->second->toMap());
    }
    mapStates->add("values", setValues);
    map->add("states", mapStates);

    value::Set* setObjects = new value::Set();
    for (Objects::iterator it = m_objects.begin(); it != m_objects.end(); ++it) {
	setObjects->add(*it->second->toMap());
    }
    map->add("objects", setObjects);

    output.setData(map);
}

void CairoCaView::onAddValue()
{
    ValueSettings* value = createValue(
	stringToType(m_comboStateType->get_active_text()));

    if (value) {
	addValue(value);
    }
}

void CairoCaView::addValue(ValueSettings* value)
{
    m_values.insert(std::make_pair<std::string, ValueSettings*>(
			value->get_key(), value));
    m_vboxValues->pack_start(*value);
    m_vboxValues->show();
}

void CairoCaView::removeValue(const std::string& value)
{
    Values::iterator it = m_values.find(value);

    if (it != m_values.end()) {
	m_vboxValues->remove(*it->second);
	m_values.erase(it);
    }
}

void CairoCaView::onAddObject()
{
    std::string title("object ");
    title += boost::lexical_cast<std::string>(++m_counter_objects);
    std::string shape("circle");
    Gdk::Color color(m_colors[m_counter_objects % m_colors.size()]);

    addObject(title, title, m_counter_objects, shape, color);
}

void CairoCaView::addObject(const std::string& title, const std::string& name,
			    int number, const std::string& shape,
			    Gdk::Color& color)
{
    ObjectSettings* object = new ObjectSettings(this, title, name, number,
						shape, color);
    m_objects.insert(std::make_pair<std::string, ObjectSettings*>(
			 title, object));
    m_vboxObjects->pack_start(*object);
    m_vboxObjects->show();
}

void CairoCaView::removeObject(const std::string& object)
{
    Objects::iterator it = m_objects.find(object);

    if (it != m_objects.end()) {
	m_vboxObjects->remove(*it->second);
	m_objects.erase(it);
    }
}

void CairoCaView::clearValuesList()
{
    for (Values::iterator it = m_values.begin(); it != m_values.end(); ++it ) {
	removeValue(it->first);
    }
}

void CairoCaView::onTypeChanged()
{
    if ((not m_values.empty())
	and (not (m_comboStateType->get_active_text() == m_previousType))) {
	if (gvle::Question(_("Values list is not empty. "
			     "Do you wish to clear it?"))) {
	    clearValuesList();
	    m_counter_values = 0;
	    m_previousType = m_comboStateType->get_active_text();
	} else {
	    m_comboStateType->set_active_text(m_previousType);
	}
    } else {
	m_previousType = m_comboStateType->get_active_text();
    }
}

CairoCaView::Type CairoCaView::stringToType(const std::string& type)
{
    if (type == "boolean")
	return BOOLEAN;
    if (type == "integer")
	return INTEGER;
    if (type == "real")
	return REAL;
    return UNKNOWN;
}

CairoCaView::ValueSettings* CairoCaView::createValue(CairoCaView::Type type)
{
    switch (type) {
    case BOOLEAN:
    {
	m_counter_values++;
	std::string title("boolean ");
	title += boost::lexical_cast<std::string>(m_counter_values);
	Gdk::Color color_f(m_colors[m_counter_values % m_colors.size()]);
	Gdk::Color color_t(m_colors[(m_colors.size() - m_counter_values + 1) %
				    m_colors.size()]);
	return new BooleanSettings(this, title, color_f, color_t);
    }
    case INTEGER:
    {
	m_counter_values++;
	std::string title("integer ");
	title += boost::lexical_cast<std::string>(m_counter_values);
	double value = 1.0;
	Gdk::Color color(m_colors[m_counter_values % m_colors.size()]);
	return new IntegerSettings(this, title, value, color);
    }
    case REAL:
    {
	m_counter_values++;
	std::string title("real ");
	title += boost::lexical_cast<std::string>(m_counter_values);
	double min = 0.0, max = 10.0;
	std::string color("red");
	std::string type("linear");
	return new RealSettings(this, title, min, max, color,type);
    }
    default:
	Error(_("You must choose a type from the list"));
	return 0;
    }
}

}}} // namespace vle gvle outputs

DECLARE_GVLE_OUTPUTPLUGIN(vle::gvle::outputs::CairoCaView)
