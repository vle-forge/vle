/**
 * @file vle/oov/plugins/cairo/netview/Colors.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
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


#include <vle/oov/plugins/cairo/netview/Colors.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/String.hpp>
#include <boost/lexical_cast.hpp>
#include <cmath>

namespace vle { namespace oov { namespace plugin {

void cairo_color::build_color(const std::string & value)
{
    switch (mType) {
    case INTEGER: {
        r = mColorList[boost::lexical_cast < int >(value)].r / 65535.;
        g = mColorList[boost::lexical_cast < int >(value)].g / 65535.;
        b = mColorList[boost::lexical_cast < int >(value)].b / 65535.;
        break;
    }
    case REAL: {
        double v_value = boost::lexical_cast < double >(value);
        std::list < RealColor >::iterator it = mRealColorList.begin();
        bool v_found = false;

        while (!v_found && it != mRealColorList.end()) {
            if (it->m_min <= v_value && it->m_max > v_value)
                v_found = true;
            else ++it;
        }
        if (v_found) {
            double d = it->m_max - it->m_min;
            double v = v_value - it->m_min;
            double c = v/d;

            if (it->m_type == RealColor::HIGHVALUE)
                c = std::pow(c,it->m_coef);
            else if (it->m_type == RealColor::LOWVALUE)
                c = std::exp(-(it->m_coef)*c);

            if (it->m_color == "red") {
                r = 1.;
                g = 1. - c;
                b = 0.;;

            } else if (it->m_color == "red_only") {
                r = 1.;
                g = 0.;
                b = 0.;
            } else if (it->m_color == "white") {
                r = 1.;
                g = 1.;
                b = 1.;
            } else if (it->m_color == "blue") {
                r = c;
                g = c;
                b = 1.;
            } else if (it->m_color == "green") {
                r = c;
                g = 1.;
                b = c;
            } else if (it->m_color == "yellow") {
                r = 1.;
                g = 1.;
                b = 1. - c;
            } else if (it->m_color == "white_green") {
                r = 1. - c;
                g = 1.;
                b = 1. - c;
            } else if (it->m_color == "green_only") {
                r = 0.;
                g = 1.;
                b = 0.;
            }
        } else {
            r = 0.;
            g = 0.;
            b = 0.;
        }
        break;
    }
    case BOOLEAN: {
        r = mColorList[(value == "false")?0:1].r / 65535.;
        g = mColorList[(value == "false")?0:1].g / 65535.;
        b = mColorList[(value == "false")?0:1].b / 65535.;
        break;
    }
    }
}

void cairo_color::build_color_list(const std::string &type,
				   const value::Set& values)
{
    if (type == "integer") {
	mType = INTEGER;
    } else if (type == "real") {
        mType = REAL;
    } else if (type == "boolean") {
        mType = BOOLEAN;
    }

    switch (mType) {
    case INTEGER: {
	for(value::Set::const_iterator it = values.begin();
	    it != values.end(); ++it) {
	    value::Map* value = toMapValue(*it);

	    mColorList[toInteger(value->get("value"))] =
		color(toInteger(value->get("red")),
		      toInteger(value->get("green")),
		      toInteger(value->get("blue")));
	}
        break;
    }
    case REAL: {
	for(value::Set::const_iterator it = values.begin();
	    it != values.end(); ++it) {
	    value::Map* value = toMapValue(*it);

	    double v_minValue = toDouble(value->get("min"));
	    double v_maxValue = toDouble(value->get("max"));
	    std::string v_color = toString(value->get("color"));
	    RealColor::color_type v_type = RealColor::LINEAR;
	    double v_coef = 0.;

	    if (toString(value->get("type")) == "linear") {
		v_type = RealColor::LINEAR;
	    } else if (toString(value->get("type")) == "highvalue") {
		v_type = RealColor::HIGHVALUE;
	    } else if (toString(value->get("type")) == "lowvalue") {
		v_type = RealColor::LOWVALUE;
	    }
	    if (v_type == RealColor::HIGHVALUE or
		v_type == RealColor::LOWVALUE) {
		v_coef = toDouble(value->get("coef"));

		mRealColorList.push_back(
		    RealColor(v_minValue, v_maxValue, v_color,
			      v_type, v_coef));
	    }
	}
        break;
    }
    case BOOLEAN: {
	for(value::Set::const_iterator it = values.begin();
	    it != values.end(); ++it) {
	    value::Map* value = toMapValue(*it);

	    mColorList[0] =
		color(toInteger(value->get("red_false")),
		      toInteger(value->get("green_false")),
		      toInteger(value->get("blue_false")));
	    mColorList[1] =
		color(toInteger(value->get("red_true")),
		      toInteger(value->get("green_true")),
		      toInteger(value->get("blue_true")));
	}
        break;
    }
    }
}

}}}
