/**
 * @file vle/gvle/outputs/caironetview/CairoNetView.hpp
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

#ifndef VLE_GVLE_OUTPUTS_CAIRONETVIEW_HPP
#define VLE_GVLE_OUTPUTS_CAIRONETVIEW_HPP

#include <vle/gvle/OutputPlugin.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Map.hpp>
#include <gtkmm.h>

namespace vle { namespace gvle { namespace outputs {

class CairoNetView : public OutputPlugin
{
public:
    CairoNetView(const std::string& name);

    virtual ~CairoNetView();

    virtual bool start(vpz::Output& output, vpz::View& view);

private:
    // Forward declarations
    class ValueSettings;

    // Typedef
    enum Area { NODES, POSITIONS, DISPLAY, STATES };
    enum Type { BOOLEAN, INTEGER, REAL, UNKNOWN };
    typedef std::map < std::string, ValueSettings* > Values;
    typedef std::vector < std::string > Colors;

    // Inner classes
    class ValueSettings : public Gtk::Frame {
    public:
	ValueSettings(CairoNetView* parent, const std::string& title, Type type);

	Type get_type()
	    { return m_type; }
	Gtk::Button& getButtonDelete()
	    { return m_delete; }
	const std::string get_key()
	    { return get_label(); }

	virtual value::Map* toMap() = 0;

    private:
	CairoNetView* m_parent;
	Gtk::Button  m_delete;
	Type         m_type;
    };

    class BooleanSettings : public ValueSettings {
    public:
	BooleanSettings(CairoNetView* parent, const std::string& title,
			Gdk::Color& color_false, Gdk::Color& color_true);
	void init();
	value::Map* toMap();
    private:
	CairoNetView*     m_parent;
	Gtk::ColorButton m_color_false;
	Gtk::ColorButton m_color_true;
	Gtk::VBox        m_vbox;
    };

    class IntegerSettings : public ValueSettings {
    public:
	IntegerSettings(CairoNetView* parent, const std::string& title,
			double value, Gdk::Color& color);
	void init();
	value::Map* toMap();
    private:
	Gtk::SpinButton  m_value;
	Gtk::ColorButton m_color;
	Gtk::VBox        m_vbox;
    };

    class RealSettings : public ValueSettings {
    public:
	RealSettings(CairoNetView* parent, const std::string& title,
		     double min, double max, const std::string& color,
		     const std::string& type, double coef = 1.0);
	void init();
	void onTypeChanged();
	value::Map* toMap();
    private:
	Gtk::SpinButton   m_min;
	Gtk::SpinButton   m_max;
	Gtk::ComboBoxText m_color;
	Gtk::ComboBoxText m_type;
	Gtk::SpinButton   m_coef;
	Gtk::VBox         m_vbox;
    };

    // Gtk Widgets
    Gtk::Dialog*        m_dialog;
    Gtk::HBox*          m_hboxNodes;
    Gtk::HBox*          m_hboxPositions;
    Gtk::HBox*          m_hboxStates;
    Gtk::ComboBoxText*  m_comboNodesType;
    Gtk::ComboBoxText*  m_comboPositionsType;
    Gtk::ComboBoxText*  m_comboStateType;
    Gtk::SpinButton*    m_spinDimensionX;
    Gtk::SpinButton*    m_spinDimensionY;
    Gtk::Entry*         m_entryExecutiveName;
    Gtk::Entry*         m_entryNodesNames;
    Gtk::Entry*         m_entryNodesPrefix;
    Gtk::Entry*         m_entryMatrix;
    Gtk::Entry*         m_entryPositionsValues;
    Gtk::Entry*         m_entryStateName;
    Gtk::CheckButton*   m_useDisplayNames;
    Gtk::CheckButton*   m_chekDisplayNames;
    Gtk::CheckButton*   m_useStatesName;
    Gtk::Button*        m_buttonAddValue;
    Gtk::VBox*          m_vboxValues;

    // Class attributes
    Colors              m_colors;
    Values              m_values;
    int                 m_counter_values;
    std::string         m_previousType;

    // Class methods
    void init(vpz::Output& output);
    void assign(vpz::Output& output);

    void onAddValue();
    void addValue(ValueSettings* value);
    void removeValue(const std::string& value);

    void clearValuesList();
    void onStatesTypeChanged();

    void sensitiveArea(const Area& area);
    Type stringToType(const std::string& type);
    ValueSettings* createValue(Type type);
};

}}} // namespace vle gvle outputs

#endif
