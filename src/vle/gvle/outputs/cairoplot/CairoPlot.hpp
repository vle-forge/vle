/**
 * @file vle/gvle/outputs/cairoplot/CairoPlot.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
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


#ifndef VLE_GVLE_OUTPUTS_CAIROPLOT_HPP
#define VLE_GVLE_OUTPUTS_CAIROPLOT_HPP

#include <vle/gvle/OutputPlugin.hpp>
#include <gtkmm.h>

namespace vle { namespace gvle { namespace outputs {

class CairoPlot : public OutputPlugin
{
public:
    CairoPlot(const std::string& name);

    virtual ~CairoPlot();

    virtual bool start(vpz::Output& output, vpz::View& view);

private:
    // Inner classes
    class CurveSettings : public Gtk::Frame {
    public:
	CurveSettings(CairoPlot* parent, std::string& curve, Gdk::Color color,
		      std::string& type, std::string& name);

	int get_red()
	    { return m_color.get_color().get_red(); }
	int get_green()
	    { return m_color.get_color().get_green(); }
	int get_blue()
	    { return m_color.get_color().get_blue(); }
	std::string get_name()
	    { return m_name.get_text(); }
	std::string get_type()
	    { return m_type.get_active_text(); }
    private:
	void init();

	CairoPlot*             m_parent;
	Gtk::Button            m_delete;
	Gtk::VBox              m_vbox;

	Gtk::ColorButton       m_color;
	Gtk::Entry             m_name;
	Gtk::ComboBoxEntryText m_type;
    };

    class LimitSettings : public Gtk::Frame {
    public:
	LimitSettings(CairoPlot* parent, std::string& limit,
		      Gdk::Color color, double value);

	int get_red()
	    { return m_color.get_color().get_red(); }
	int get_green()
	    { return m_color.get_color().get_green(); }
	int get_blue()
	    { return m_color.get_color().get_blue(); }
	double get_value()
	    { return m_value.get_value(); }
    private:
	void init();

	CairoPlot*       m_parent;
	Gtk::Button      m_delete;
	Gtk::VBox        m_vbox;

	Gtk::ColorButton m_color;
	Gtk::SpinButton  m_value;
    };

    // Types
    enum Area { WINDOW, VALUE, SCROLLING, LIMITS };
    typedef std::map < std::string, CurveSettings* > Curves;
    typedef std::map < std::string, LimitSettings* > Limits;
    typedef std::vector<std::string> Colors;

    // GUI Widgets
    Gtk::Dialog*        m_dialog;
    Gtk::Button*        m_addCurve;
    Gtk::Button*        m_addLimit;
    Gtk::VBox*          m_vboxCurves;
    Gtk::VBox*          m_vboxLimits;
    Gtk::CheckButton*   m_useWindow;
    Gtk::CheckButton*   m_useValue;
    Gtk::CheckButton*   m_useScrolling;
    Gtk::CheckButton*   m_useLimits;
    Gtk::CheckButton*   m_checkScrolling;
    Gtk::SpinButton*    m_spinWindow;
    Gtk::SpinButton*    m_spinValueMin;
    Gtk::SpinButton*    m_spinValueMax;
    Gtk::Expander*      m_expandOptions;
    Gtk::Expander*      m_expandLimits;

    // Class attributes
    Curves              m_curves;
    Limits              m_limits;
    int                 m_counter_curves;
    int                 m_counter_limits;
    Colors              m_colors;

    // Class methods
    void init(vpz::Output& output);
    void assign(vpz::Output& output);

    void onAddCurve();
    void addCurve(std::string& curve, Gdk::Color& color,
		  std::string& type, std::string& name);
    void removeCurve(std::string& curve);

    void onAddLimit();
    void addLimit(std::string& limit, Gdk::Color& color, double value);
    void removeLimit(std::string& limit);

    void sensitiveArea(const Area& area);

};

}}} // namespace vle gvle outputs

#endif
