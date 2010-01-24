/**
 * @file vle/oov/plugins/cairo/netview/Colors.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
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


#ifndef VLE_OOV_PLUGINS_CAIRO_COLORS_HPP
#define VLE_OOV_PLUGINS_CAIRO_COLORS_HPP

#include <vle/value/Set.hpp>
#include <map>
#include <string>
#include <list>

namespace vle { namespace oov { namespace plugin {

    class RealColor
    {
    public:
        enum color_type { LINEAR, HIGHVALUE, LOWVALUE };

        double m_min;
        double m_max;
        std::string m_color;
        color_type m_type;
        double m_coef;

        RealColor(double min, double max, const std::string & color,
                  color_type type, double coef)
            : m_min(min), m_max(max), m_color(color), m_type(type),
            m_coef(coef)
        {}
    };


    class color
    {
    public:
        int r; int g; int b;
        color(int _r = 0, int _g = 0, int _b = 0):r(_r), g(_g), b(_b) { }
    };

    class cairo_color
    {
    public:
        cairo_color() :
            r(0.), g(0.), b(0.)
        {}

        ~cairo_color() {}

        void build_color(const std::string& value);

        void build_color_list(const std::string &type,
			      const value::Set& values);

        double r; double g; double b;

    private:
        enum type { INTEGER, REAL, BOOLEAN };
        type mType;
        std::map < int, color > mColorList;
        std::list < RealColor > mRealColorList;
    };

}}}
#endif
