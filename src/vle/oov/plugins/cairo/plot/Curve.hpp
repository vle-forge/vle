/**
 * @file vle/oov/plugins/cairo/plot/Curve.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
 * Copyright (C) 2003-2009 ULCO http://www.univ-littoral.fr
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

    virtual int get_number_value() = 0;

    virtual double get_date(int index) = 0;

    virtual int get_size() = 0;
};

}}} // namespace vle oov plugin

#endif
