/** 
 * @file IntCurve.hpp
 * @brief 
 * @author The vle Development Team
 * @date ven, 27 jan 2006 16:08:53 +0100
 */

/*
 * Copyright (C) 2006-2007 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef VLE_OOV_PLUGINS_CAIRO_PLOT_INTCURVE_HPP
#define VLE_OOV_PLUGINS_CAIRO_PLOT_INTCURVE_HPP

#include <vle/oov/plugins/cairo/plot/Curve.hpp>
#include <vle/oov/plugins/cairo/plot/Parameter.hpp>
#include <vector>
#include <list>
#include <cairomm/cairomm.h>

namespace vle { namespace oov { namespace plugin {

class IntCurve: public Curve
{
    class color 
    {
    public:
	double red;
	double green;
	double blue;
	
	color(double r=0, double g=0, double b=0):red(r), green(g), blue(b) { }
	color(const color& c) 
	    { red = c.red; green = c.green; blue = c.blue; }
    };

public :
    IntCurve(const std::string & name,
	     unsigned int r,
	     unsigned int g,
	     unsigned int b) : Curve(name, r, g, b) 
	{ }

    virtual ~IntCurve() { }

    void add(double date, int value);
    
    void draw(Cairo::RefPtr < Cairo::Context > ctx, Parameter & m_parameter, 
	      std::list < IntCurve * > m_list);
    
    const int get_last_value();
    
    bool is_integer() { return true; }
    
    const int get_size() { return m_valueList.size(); }
    
    const void get_min_max_value(int number_value, int & min, int & max,
				 Parameter & m_parameter);
    
    const int get_min_value(int number_value, 
			    Parameter & m_parameter);
    
    const int get_max_value(int number_value, 
			    Parameter & m_parameter);
    
    const int get_number_value();
    
    const double get_date(int index);
    
    const int get_value(int index);
    
    const void get_min_max_value_prec_index(int indice, int number_value,
					    int & min, int & max);
    
    const int get_min_value_prec_index(int indice, int number_value);
    
    const int get_max_value_prec_index(int indice, int number_value);
    
private :
    std::vector < std::pair < double, int > > m_valueList;
    
    void swap_int(int &value1, int &value2);
};

}}} // namespace vle oov plugin
    
#endif
    
