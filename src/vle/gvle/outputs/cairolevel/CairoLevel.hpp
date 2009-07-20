 /**
 * @file vle/gvle/outputs/cairolevel/CairoLevel.hpp
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

#ifndef VLE_GVLE_OUTPUTS_CAIROLEVEL_HPP
#define VLE_GVLE_OUTPUTS_CAIROLEVEL_HPP

#include <vle/gvle/OutputPlugin.hpp>
#include <gtkmm.h>

namespace vle { namespace gvle { namespace outputs {

class CairoLevel : public OutputPlugin
{
public:
    CairoLevel(const std::string& name);

    virtual ~CairoLevel();

    virtual bool start(vpz::Output& output, vpz::View& view);

private:
    class CurveSettings : public Gtk::Frame {
    public:
	/**
	 * @brief Constructor
	 * @param parent class parent
	 * @param curve  The curve label
	 * @param color  The color used with this curve
	 * @param min    The min value of this curve
	 * @param max    The max value of this curve
	 */
	CurveSettings(CairoLevel* parent, std::string& curve, Gdk::Color color,
		      double min, double max);

	void init();

        // Getter, Setter
	Gdk::Color get_color()
	    { return m_color.get_color(); }
	void set_color(Gdk::Color& color)
	    { m_color.set_color(color); }
	double get_min()
	    { return m_min.get_value(); }
	void set_min(double min)
	    { m_min.set_value(min); }
	double get_max()
	    { return m_max.get_value(); }
	void set_max(double max)
	    { m_max.set_value(max); }

    private:
	CairoLevel*      m_parent;
	Gtk::Alignment   m_alignment;
	Gtk::Button      m_delete;
	Gtk::VBox        m_vbox;

	std::string      m_curve;
	Gtk::ColorButton m_color;
	Gtk::SpinButton  m_min;
	Gtk::SpinButton  m_max;
    };

    /**
     * @brief A map to store CurveBox
     */
    typedef std::map < std::string, CurveSettings* > Curves;

    // GUI widget
    Gtk::Dialog*        m_dialog;
    Gtk::Button*        m_addCurve;
    Gtk::SpinButton*    m_minx;
    Gtk::SpinButton*    m_maxx;
    Gtk::SpinButton*    m_miny;
    Gtk::SpinButton*    m_maxy;
    Gtk::VBox*          m_curvesVBox;

    // class attributes
    int                      m_counter;
    Curves                   m_curves;
    std::vector<std::string> m_colors;

    // class methods
    void init(vpz::Output& output);
    void assign(vpz::Output& output);
    void onAddCurve();
    void addCurve(std::string& curve, Gdk::Color& color, double min, double max);
    void removeCurve(std::string& curve);
};

}}} // namespace vle gvle outputs

#endif
