/**
 * @file vle/oov/plugins/cairo/caview/Colors.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2009 The VLE Development Team
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
#include <vle/utils/Tools.hpp>
#include <vle/utils/XML.hpp>
#include <cmath>

namespace vle { namespace oov { namespace plugin {

void cairo_color::build_color(const std::string & value)
{
    switch (mType) {
    case INTEGER: {
        r = mColorList[utils::to_int(value)].r / 65535.;
        g = mColorList[utils::to_int(value)].g / 65535.;
        b = mColorList[utils::to_int(value)].b / 65535.;
        break;
    }
    case REAL: {
        double v_value = utils::to_double(value);
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
                                   xmlpp::Node::NodeList &lst)
{
    if (type == "integer")
        mType = INTEGER;
    else if (type == "real")
        mType = REAL;
    else if (type == "boolean")
        mType = BOOLEAN;

    xmlpp::Node::NodeList::iterator it = lst.begin();

    switch (mType) {
    case INTEGER: {
        while (it != lst.end()) {
            xmlpp::Element * v_valueNode = (xmlpp::Element*)(*it);
            int v_value(utils::to_int(utils::xml::get_attribute(
                        v_valueNode, "value")));
            int red = utils::to_int(utils::xml::get_attribute(
                    v_valueNode, "red"));
            int green = utils::to_int(utils::xml::get_attribute(
                    v_valueNode, "green"));
            int blue = utils::to_int(utils::xml::get_attribute(
                    v_valueNode, "blue"));
            mColorList[v_value] = color(red, green, blue);
            ++it;
        }
        break;
    }
    case REAL: {
        while (it != lst.end()) {
            xmlpp::Element * v_valueNode = (xmlpp::Element*)(*it);
            double v_minValue =
                utils::to_double(
                    utils::xml::get_attribute(v_valueNode, "min"));
            double v_maxValue =
                utils::to_double(
                    utils::xml::get_attribute(v_valueNode, "max"));
            std::string v_color =
                utils::xml::get_attribute(v_valueNode, "color");
            RealColor::color_type v_type = RealColor::LINEAR;
            double v_coef = 0.;

            if (utils::xml::get_attribute(
                    v_valueNode, "type") == "linear")
                v_type = RealColor::LINEAR;
            if (utils::xml::get_attribute(
                    v_valueNode, "type") == "highvalue")
                v_type = RealColor::HIGHVALUE;
            if (utils::xml::get_attribute(
                    v_valueNode, "type") == "lowvalue")
                v_type = RealColor::LOWVALUE;
            if (v_type == RealColor::HIGHVALUE or
                v_type == RealColor::LOWVALUE)
                v_coef = utils::to_double(
                    utils::xml::get_attribute(v_valueNode, "coef"));

            mRealColorList.push_back( RealColor(
                    v_minValue, v_maxValue, v_color, v_type, v_coef));
            ++it;
        }
        break;
    }
    case BOOLEAN: {
        while (it != lst.end()) {
            xmlpp::Element * v_valueNode = (xmlpp::Element*)(*it);
            int red_true = utils::to_int(
                utils::xml::get_attribute(v_valueNode, "red_true"));
            int green_true = utils::to_int(
                utils::xml::get_attribute(v_valueNode, "green_true"));
            int blue_true = utils::to_int(
                utils::xml::get_attribute(v_valueNode, "blue_true"));
            int red_false = utils::to_int(
                utils::xml::get_attribute(v_valueNode, "red_false"));
            int green_false = utils::to_int(
                utils::xml::get_attribute(v_valueNode,
                                          "green_false"));
            int blue_false = utils::to_int(
                utils::xml::get_attribute(v_valueNode, "blue_false"));

            mColorList[0] = color(red_false, green_false, blue_false);
            mColorList[1] = color(red_true, green_true, blue_true);
            ++it;
        }
        break;
    }
    }
}

}}}
