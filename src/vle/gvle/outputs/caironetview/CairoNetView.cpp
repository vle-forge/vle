/**
 * @file vle/gvle/outputs/storage/CairoNetView.cpp
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

#include <vle/gvle/outputs/caironetview/CairoNetView.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Double.hpp>
#include <vle/utils/Path.hpp>
#include <gtkmm/filechooserdialog.h>
#include <libglademm/xml.h>
#include <cassert>
#include <boost/lexical_cast.hpp>
#include <boost/assign/std/vector.hpp>

namespace vle { namespace gvle { namespace outputs {

CairoNetView::ValueSettings::ValueSettings(CairoNetView* parent,
					  const std::string& title,
					  Type type) :
    Gtk::Frame(title), m_parent(parent), m_delete(Gtk::Stock::DELETE),
    m_type(type)
{
    m_delete.signal_clicked().connect(
	sigc::bind(
	    sigc::mem_fun(*m_parent, &CairoNetView::removeValue), title));
}

CairoNetView::BooleanSettings::BooleanSettings(CairoNetView* parent,
					      const std::string& title,
					      Gdk::Color& color_false,
					      Gdk::Color& color_true) :
    CairoNetView::ValueSettings(parent, title, BOOLEAN),
    m_color_false(color_false),
    m_color_true(color_true)
{
    m_vbox.set_spacing(5);
    init();
    show();
}

void CairoNetView::BooleanSettings::init()
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

value::Map* CairoNetView::BooleanSettings::toMap()
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

CairoNetView::IntegerSettings::IntegerSettings(CairoNetView* parent,
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

void CairoNetView::IntegerSettings::init()
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

value::Map* CairoNetView::IntegerSettings::toMap()
{
    value::Map* map = new value::Map();

    map->addInt("value", m_value.get_value());
    map->addInt("red", m_color.get_color().get_red());
    map->addInt("green", m_color.get_color().get_green());
    map->addInt("blue", m_color.get_color().get_blue());
    return map;
}

CairoNetView::RealSettings::RealSettings(CairoNetView* parent,
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
	sigc::mem_fun(*this, &CairoNetView::RealSettings::onTypeChanged));

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

void CairoNetView::RealSettings::init()
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

void CairoNetView::RealSettings::onTypeChanged()
{
    m_coef.set_sensitive(m_type.get_active_text() != "linear");
}

value::Map* CairoNetView::RealSettings::toMap()
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

CairoNetView::CairoNetView(const std::string& name)
    : OutputPlugin(name), m_dialogPlugin(0),
      m_dialogMatrix(0), m_counter_values(0)
{
    using namespace boost::assign;

    m_colors += "purple1", "red", "green", "blue", "black", "yellow",
	"pink", "orange", "chocolate", "navy"; /* /!\ it will start
						* with 'red'
						*/
}

CairoNetView::~CairoNetView()
{
}

bool CairoNetView::start(vpz::Output& output, vpz::View& /* view */)
{
    assert(output.plugin() == "caironetview");

    std::string glade =
	utils::Path::path().getOutputGladeFile("caironetview.glade");
    Glib::RefPtr<Gnome::Glade::Xml> ref = Gnome::Glade::Xml::create(glade);

    ref->get_widget("dialogPlugin", m_dialogPlugin);
    ref->get_widget("dialogMatrix", m_dialogMatrix);
    ref->get_widget("dialogPositions", m_dialogPositions);
    ref->get_widget("textviewMatrix", m_textviewMatrix);
    ref->get_widget("textviewPositions", m_textviewPositions);
    ref->get_widget("hboxNodes", m_hboxNodes);
    ref->get_widget("hboxPositions", m_hboxPositions);
    ref->get_widget("hboxStates", m_hboxStates);
    ref->get_widget("spinDimensionX", m_spinDimensionX);
    ref->get_widget("spinDimensionY", m_spinDimensionY);
    ref->get_widget("entryExecutiveName", m_entryExecutiveName);
    ref->get_widget("entryNodesNames", m_entryNodesNames);
    ref->get_widget("entryNodesPrefix", m_entryNodesPrefix);
    ref->get_widget("entryStateName", m_entryStateName);
    ref->get_widget("useDisplayNames", m_useDisplayNames);
    ref->get_widget("chekDisplayNames", m_chekDisplayNames);
    ref->get_widget("useStatesName", m_useStatesName);
    ref->get_widget("buttonAddValue", m_buttonAddValue);
    ref->get_widget("buttonEditMatrix", m_buttonEditMatrix);
    ref->get_widget("buttonEditPositions", m_buttonEditPositions);
    ref->get_widget("vboxValues", m_vboxValues);

    assert(m_dialogPlugin);
    assert(m_dialogMatrix);
    assert(m_dialogPositions);
    assert(m_textviewMatrix);
    assert(m_textviewPositions);
    assert(m_hboxNodes);
    assert(m_hboxPositions);
    assert(m_hboxStates);
    assert(m_spinDimensionX);
    assert(m_spinDimensionY);
    assert(m_entryExecutiveName);
    assert(m_entryNodesNames);
    assert(m_entryNodesPrefix);
    assert(m_entryStateName);
    assert(m_useDisplayNames);
    assert(m_chekDisplayNames);
    assert(m_useStatesName);
    assert(m_buttonAddValue);
    assert(m_buttonEditMatrix);
    assert(m_buttonEditPositions);
    assert(m_vboxValues);

    m_comboNodesType = new Gtk::ComboBoxText();
    m_comboNodesType->show();
    m_comboNodesType->append_text("");
    m_comboNodesType->append_text("set");
    m_comboNodesType->set_active_text("");
    m_hboxNodes->pack_start(*m_comboNodesType);

    m_comboPositionsType = new Gtk::ComboBoxText();
    m_comboPositionsType->show();
    m_comboPositionsType->append_text("auto");
    m_comboPositionsType->append_text("set");
    m_comboPositionsType->set_active_text("auto");
    m_hboxPositions->pack_start(*m_comboPositionsType);

    m_comboStateType = new Gtk::ComboBoxText();
    m_comboStateType->show();
    m_comboStateType->append_text("boolean");
    m_comboStateType->append_text("integer");
    m_comboStateType->append_text("real");
    m_comboStateType->set_active_text("boolean");
    m_previousType = m_comboStateType->get_active_text();
    m_hboxStates->pack_start(*m_comboStateType);

    m_buttonAddValue->signal_clicked().connect(
	sigc::mem_fun(*this, &CairoNetView::onAddValue));
    m_buttonEditMatrix->signal_clicked().connect(
	sigc::mem_fun(*this, &CairoNetView::onEditMatrix));
    m_buttonEditPositions->signal_clicked().connect(
	sigc::mem_fun(*this, &CairoNetView::onEditPositions));

    m_comboStateType->signal_changed().connect(
	sigc::mem_fun(*this, &CairoNetView::onStatesTypeChanged));
    m_comboNodesType->signal_changed().connect(
	sigc::bind(sigc::mem_fun(*this, &CairoNetView::sensitiveArea), NODES));
    m_comboPositionsType->signal_changed().connect(
	sigc::bind(
	    sigc::mem_fun(*this, &CairoNetView::sensitiveArea), POSITIONS));

    m_useDisplayNames->signal_toggled().connect(
	sigc::bind(
	    sigc::mem_fun(*this, &CairoNetView::sensitiveArea), DISPLAY));
    m_useStatesName->signal_toggled().connect(
	sigc::bind(sigc::mem_fun(*this, &CairoNetView::sensitiveArea), STATES));

    init(output);

    if (m_dialogPlugin->run() == Gtk::RESPONSE_ACCEPT) {
        assign(output);
    }

    m_dialogPlugin->hide();
    return true;
}

void CairoNetView::init(vpz::Output& output)
{
    m_values.clear(); m_counter_values = 0;
    const value::Value* init = output.data();

    if (init and init->isMap()) {
	const value::Map* map = value::toMapValue(init);

	if (map->existValue("dimensions")) {
	    const value::Map& sizeMap = map->getMap("dimensions");
	    if (sizeMap.existValue("x"))
		m_spinDimensionX->set_value(sizeMap.getDouble("x"));
	    if (sizeMap.existValue("y"))
		m_spinDimensionY->set_value(sizeMap.getDouble("y"));
	}

	if (map->existValue("executiveName")) {
	    m_entryExecutiveName->set_text(map->getString("executiveName"));
	}

	if (map->existValue("nodes")) {
	    const value::Map& nodesMap = map->getMap("nodes");
	    if (nodesMap.existValue("type")) {
		m_comboNodesType->set_active_text(nodesMap.getString("type"));
		if (nodesMap.getString("type") == "set") {
		    m_entryNodesNames->set_text(nodesMap.getString("names"));
		} else {
		    m_entryNodesPrefix->set_text(nodesMap.getString("prefix"));
		}
	    }
	}

	if (map->existValue("adjacency_matrix")) {
	    m_textviewMatrix->get_buffer()->set_text(
		map->getString("adjacency_matrix"));
	}

	if (map->existValue("positions")) {
	    const value::Map& positionsMap = map->getMap("positions");
	    if (positionsMap.existValue("type")) {
		m_comboPositionsType->set_active_text(
		    positionsMap.getString("type"));
		if (positionsMap.getString("type") == "set") {
		    if (positionsMap.existValue("values")) {
			m_textviewPositions->get_buffer()->set_text(
			    positionsMap.getString("values"));
		    }
		}
	    }
	}

	if (map->existValue("states")) {
	    const value::Map& statesMap = map->getMap("states");

	    if (statesMap.existValue("name")) {
		m_useStatesName->set_active(true);
		m_entryStateName->set_text(statesMap.getString("name"));
	    }
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
			title +=
			    boost::lexical_cast<std::string>(m_counter_values);
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
			    value = valueMap->getInt("value");
			else
			    value = 1.0;

			Gdk::Color color;
			color.set_rgb(r, g, b);
			std::string title("integer ");
			title +=
			    boost::lexical_cast<std::string>(m_counter_values);
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
			title +=
			    boost::lexical_cast<std::string>(m_counter_values);
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

	if (map->existValue("display_names")) {
	    const value::Map& displayMap = map->getMap("display_names");
	    if (displayMap.existValue("activate")) {
		m_useDisplayNames->set_active(
		    (displayMap.getString("activate") == "yes" ? true : false));
	    }
	}
    }
}

void CairoNetView::assign(vpz::Output& output)
{
    value::Map* map = new value::Map();

    value::Map* mapDimensions = new value::Map();
    mapDimensions->addDouble("x", m_spinDimensionX->get_value());
    mapDimensions->addDouble("y", m_spinDimensionY->get_value());
    map->add("dimensions", mapDimensions);

    map->addString("executiveName", m_entryExecutiveName->get_text());

    value::Map* mapNodes = new value::Map();
    mapNodes->addString("type", m_comboNodesType->get_active_text());
    if (m_comboNodesType->get_active_text() == "set")
	mapNodes->addString("names", m_entryNodesNames->get_text());
    else
	mapNodes->addString("prefix", m_entryNodesPrefix->get_text());
    map->add("nodes", mapNodes);

    map->addString("adjacency_matrix",
		   m_textviewMatrix->get_buffer()->get_text());

    value::Map* mapPositions = new value::Map();
    mapPositions->addString("type", m_comboPositionsType->get_active_text());
    if (m_comboPositionsType->get_active_text() == "set")
	mapPositions->addString("values",
				m_textviewPositions->get_buffer()->get_text());
    map->add("positions", mapPositions);

    value::Map* mapStates = new value::Map();
    if (m_useStatesName->get_active())
	mapStates->addString("name", m_entryStateName->get_text());
    mapStates->addString("type", m_comboStateType->get_active_text());
    value::Set* setValues = new value::Set();
    for (Values::iterator it = m_values.begin(); it != m_values.end(); ++it) {
	setValues->add(*it->second->toMap());
    }
    mapStates->add("values", setValues);
    map->add("states", mapStates);

    if (m_useDisplayNames->get_active()) {
	value::Map* mapDisplay = new value::Map();
	mapDisplay->addString("activate",
			      (m_useDisplayNames->get_active() ? "yes" : "no"));
	map->add("display_names", mapDisplay);
    }

    output.setData(map);
}

void CairoNetView::onEditMatrix()
{
    Glib::ustring previous_content = m_textviewMatrix->get_buffer()->get_text();

    m_dialogMatrix->show();
    if (m_dialogMatrix->run() != Gtk::RESPONSE_ACCEPT) {
	m_textviewMatrix->get_buffer()->set_text(previous_content);
    }
    m_dialogMatrix->hide();
}

void CairoNetView::onEditPositions()
{
    Glib::ustring previous_content = m_textviewPositions->get_buffer()->get_text();

    m_dialogPositions->show();
    if (m_dialogPositions->run() != Gtk::RESPONSE_ACCEPT) {
	m_textviewPositions->get_buffer()->set_text(previous_content);
    }
    m_dialogPositions->hide();
}

void CairoNetView::onAddValue()
{
    ValueSettings* value = createValue(
	stringToType(m_comboStateType->get_active_text()));

    if (value) {
	addValue(value);
    }
}

void CairoNetView::addValue(ValueSettings* value)
{
    m_values.insert(std::make_pair<std::string, ValueSettings*>(
			value->get_key(), value));
    m_vboxValues->pack_start(*value);
    m_vboxValues->show();
}

void CairoNetView::removeValue(const std::string& value)
{
    Values::iterator it = m_values.find(value);

    if (it != m_values.end()) {
	m_vboxValues->remove(*it->second);
	m_values.erase(it);
    }
}

void CairoNetView::clearValuesList()
{
    for (Values::iterator it = m_values.begin(); it != m_values.end(); ++it ) {
	removeValue(it->first);
    }
}

void CairoNetView::onStatesTypeChanged()
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

void CairoNetView::sensitiveArea(const Area& area)
{
    switch (area) {
    case NODES:
	if (m_comboNodesType->get_active_text() == "set") {
	    m_entryNodesPrefix->set_sensitive(false);
	    m_entryNodesNames->set_sensitive(true);
	} else {
	    m_entryNodesPrefix->set_sensitive(true);
	    m_entryNodesNames->set_sensitive(false);
	}
	break;
    case POSITIONS:
	if (m_comboPositionsType->get_active_text() == "auto") {
	    m_buttonEditPositions->set_sensitive(false);
	} else {
	    m_buttonEditPositions->set_sensitive(true);
	}
	break;
    case DISPLAY:
	m_chekDisplayNames->set_sensitive(m_useDisplayNames->get_active());
	break;
    case STATES:
	m_entryStateName->set_sensitive(m_useStatesName->get_active());
	break;
    default:
	break;
    }
}

CairoNetView::Type CairoNetView::stringToType(const std::string& type)
{
    if (type == "boolean")
	return BOOLEAN;
    if (type == "integer")
	return INTEGER;
    if (type == "real")
	return REAL;
    return UNKNOWN;
}

CairoNetView::ValueSettings* CairoNetView::createValue(Type type)
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

DECLARE_GVLE_OUTPUTPLUGIN(vle::gvle::outputs::CairoNetView)

