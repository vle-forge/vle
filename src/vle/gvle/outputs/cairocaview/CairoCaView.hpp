/**
 * @file vle/gvle/outputs/cairocaview/CairoCaView.hpp
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


#ifndef VLE_GVLE_OUTPUTS_CAIROCAVIEW_HPP
#define VLE_GVLE_OUTPUTS_CAIROCAVIEW_HPP

#include <vle/gvle/OutputPlugin.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Map.hpp>
#include <gtkmm.h>

namespace vle { namespace gvle { namespace outputs {

class CairoCaView : public OutputPlugin
{
public:
    CairoCaView(const std::string& name);

    virtual ~CairoCaView();

    virtual bool start(vpz::Output& output, vpz::View& view);

private:
    // Forward declarations
    class ValueSettings;
    class ObjectSettings;

    // Typedef
    enum Type { BOOLEAN, INTEGER, REAL, UNKNOWN };
    typedef std::map < std::string, ValueSettings* > Values;
    typedef std::map < std::string, ObjectSettings* > Objects;
    typedef std::vector < std::string > Colors;

    // Inner classes
    class ValueSettings : public Gtk::Frame {
    public:
	ValueSettings(CairoCaView* parent, const std::string& title, Type type);

	Type get_type()
	    { return m_type; }
	Gtk::Button& getButtonDelete()
	    { return m_delete; }
	const std::string get_key()
	    { return get_label(); }

	virtual value::Map* toMap() = 0;

    private:
	CairoCaView* m_parent;
	Gtk::Button  m_delete;
	Type         m_type;
    };

    class BooleanSettings : public ValueSettings {
    public:
	BooleanSettings(CairoCaView* parent, const std::string& title,
			Gdk::Color& color_false, Gdk::Color& color_true);
	void init();
	value::Map* toMap();
    private:
	CairoCaView*     m_parent;
	Gtk::ColorButton m_color_false;
	Gtk::ColorButton m_color_true;
	Gtk::VBox        m_vbox;
    };

    class IntegerSettings : public ValueSettings {
    public:
	IntegerSettings(CairoCaView* parent, const std::string& title,
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
	RealSettings(CairoCaView* parent, const std::string& title,
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

    class ObjectSettings : public Gtk::Frame {
    public:
	ObjectSettings(CairoCaView* parent, const std::string& title,
		       const std::string& name, int number,
		       const std::string& shape, Gdk::Color& color);
	void init();
	value::Map* toMap();
    private:
	CairoCaView*      m_parent;
	Gtk::Button       m_delete;
	Gtk::Entry        m_name;
	Gtk::SpinButton   m_number;
	Gtk::ComboBoxText m_shape;
	Gtk::ColorButton  m_color;
	Gtk::VBox         m_vbox;
    };

    // GUI widget
    Gtk::Dialog*        m_dialog;
    Gtk::SpinButton*    m_spinSizeX;
    Gtk::SpinButton*    m_spinSizeY;
    Gtk::ComboBoxText*  m_comboGeometryType;
    Gtk::ComboBoxText*  m_comboStateType;
    Gtk::Entry*         m_entryCellName;
    Gtk::Entry*         m_entryStateName;
    Gtk::Button*        m_buttonAddValue;
    Gtk::Button*        m_buttonAddObject;
    Gtk::HBox*          m_hboxGeometry;
    Gtk::HBox*          m_hboxStates;
    Gtk::VBox*          m_vboxValues;
    Gtk::VBox*          m_vboxObjects;

    // Class attributes
    Colors              m_colors;
    Values              m_values;
    Objects             m_objects;
    int                 m_counter_values;
    int                 m_counter_objects;
    std::string         m_previousType;

    // class methods
    void init(vpz::Output& output);
    void assign(vpz::Output& output);

    void onAddValue();
    void addValue(ValueSettings* value);
    void removeValue(const std::string& value);

    void onAddObject();
    void addObject(const std::string& title, const std::string& name,
		   int number, const std::string& shape,
		   Gdk::Color& color);
    void removeObject(const std::string& object);

    void clearValuesList();
    void onTypeChanged();

    Type stringToType(const std::string& type);
    ValueSettings* createValue(Type type);
};

}}} // namespace vle gvle outputs

#endif
