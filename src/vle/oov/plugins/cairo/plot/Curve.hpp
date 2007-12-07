/** 
 * @file Curve.hpp
 * @brief 
 * @author The vle Development Team
 * @date ven, 27 jan 2006 16:07:13 +0100
 */

/*
 * Copyright (C) 2004-2007 - The vle Development Team
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

#ifndef VLE_OOV_PLUGINS_CAIRO_PLOT_CURVE_HPP
#define VLE_OOV_PLUGINS_CAIRO_PLOT_CURVE_HPP

#include <string>

namespace vle { namespace oov { namespace plugin {

class Curve
{
private :
    std::string mName;
    unsigned int mRedColor;
    unsigned int mGreenColor;
    unsigned int mBlueColor;

public :
    Curve(const std::string& s = std::string(),
	  unsigned int r = 0,
	  unsigned int g = 0,
	  unsigned int b = 0
	) :
	mName(s),
	mRedColor(r),
	mGreenColor(g),
	mBlueColor(b)
        {
        }
    
    virtual ~Curve() { }
    
    inline std::string get_name() { return mName; }
    
    inline unsigned int get_red_color() { return mRedColor; }
    
    inline unsigned int get_green_color() { return mGreenColor; }
    
    inline unsigned int get_blue_color() { return mBlueColor; }
    
    virtual bool is_integer() { return false; }
    
    virtual bool is_double() { return false; }
    
    virtual const int get_number_value() = 0;
    
    virtual const double get_date(int index) = 0;
    
    virtual const int get_size() = 0;
};
	    
}}} // namespace vle oov plugin
    
#endif
