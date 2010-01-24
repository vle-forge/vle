/**
 * @file vle/oov/plugins/cairo/plot/Limit.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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
