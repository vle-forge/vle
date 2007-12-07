/** 
 * @file Limit.hpp
 * @brief 
 * @author The vle Development Team
 * @date ven, 27 jan 2006 16:09:08 +0100
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

#ifndef VLE_OOV_PLUGINS_CAIRO_PLOT_LIMIT_HPP
#define VLE_OOV_PLUGINS_CAIRO_PLOT_LIMIT_HPP

#include <vle/oov/plugins/cairo/plot/Parameter.hpp>

namespace vle { namespace oov { namespace plugin {

class Limit
{
private :
    double mValue;
    unsigned int mRedColor;
    unsigned int mGreenColor;
    unsigned int mBlueColor;
    
public :
    Limit(double value,
	  unsigned int r = 0,
	  unsigned int g = 0,
	  unsigned int b = 0) :
	mValue(value),
	mRedColor(r),
	mGreenColor(g),
	mBlueColor(b)
        { }
    
    virtual ~Limit() { }
    
    inline double get_value() { return mValue; }
    
    inline unsigned int get_red_color() { return mRedColor; }
    
    inline unsigned int get_green_color() { return mGreenColor; }
    
    inline unsigned int get_blue_color() { return mBlueColor; }
};

}}} // namespace vle oov plugin

#endif
