/**
 * @file value/Value.cpp
 * @author The VLE Development Team.
 * @brief Virtual class to assign Value into Event object.
 */

/*
 * Copyright (c) 2004, 2005 The vle Development Team
 *
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <vle/value/Value.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Coordinate.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/XML.hpp>

namespace vle { namespace value {

const Value ValueBase::empty;

std::vector < Value > ValueBase::getValues(xmlpp::Element* root)
{
    std::vector < Value > result;

    if (root) {
        xmlpp::Node::NodeList lst = root->get_children();
        for (xmlpp::Node::NodeList::iterator it = lst.begin();
             it != lst.end(); ++it) {
            if (vle::utils::xml::is_element(*it)) {
                result.push_back(getValue((xmlpp::Element*)*it));
            }
        }
    }

    return result;
}

Value ValueBase::getValue(xmlpp::Element* root)
{
    if (root) {
        Glib::ustring name(root->get_name());
        xmlpp::Attribute* att(root->get_attribute("VALUE"));

        if (att == 0)
            exit(0);

        Glib::ustring value(att->get_value());

        if (name == "DOUBLE") {
            return DoubleFactory::create(utils::to_double(value));
        } else if (name == "BOOLEAN") {
            return BooleanFactory::create(utils::to_boolean(value));
        } else if (name == "INTEGER") {
            return IntegerFactory::create(utils::to_int(value));
        } else if (name == "STRING") {
            return StringFactory::create(value);
        } else if (name == "SET") {// FIXME MACHER PAS !!! {
            return SetFactory::create();

        } else if (name == "MAP") {// FIXME MACHER PAS !!!
            return MapFactory::create();
        }
    }
    return empty;
}

void ValueBase::cleanValues(std::vector < Value >& vals)
{
    for (std::vector < Value >::iterator it = vals.begin();
         it != vals.end(); it++) {
        (*it).reset();
    }
}

bool ValueBase::isComplex(xmlpp::Element* root)
{
    if (root) {
        Glib::ustring name(root->get_name());
        return name == "SET" or name == "MAP";
    }
    return 0;
}

Boolean to_boolean(Value v)
{
    Assert(utils::InternalError, v->getType() == ValueBase::BOOLEAN,
           "Value is not a Boolean");
    return boost::static_pointer_cast < BooleanFactory >(v);
}

Integer to_integer(Value v)
{
    Assert(utils::InternalError, v->getType() == ValueBase::INTEGER,
           "Value is not an Integer");
    return boost::static_pointer_cast < IntegerFactory >(v);
}

String to_string(Value v)
{
    Assert(utils::InternalError, v->getType() == ValueBase::STRING,
           "Value is not a String");
    return boost::static_pointer_cast < StringFactory >(v);
}

Double to_double(Value v)
{
    Assert(utils::InternalError, v->getType() == ValueBase::DOUBLE,
           "Value is not a Double");
    return boost::static_pointer_cast < DoubleFactory >(v);
}

Map to_map(Value v)
{
    Assert(utils::InternalError, v->getType() == ValueBase::MAP,
           "Value is not a Map");
    return boost::static_pointer_cast < MapFactory >(v);
}

Set to_set(Value v)
{
    Assert(utils::InternalError, v->getType() == ValueBase::SET,
           "Value is not a Set");
    return boost::static_pointer_cast < SetFactory >(v);
}

CoordinateInt1 to_coordinateInt1(Value v)
{
    Assert(utils::InternalError, v->getType() == ValueBase::COORDINATE,
           "Value is not a Coordinate");
    return boost::static_pointer_cast
        < CoordinateFactory < 1, int > >(v);
}

CoordinateInt2 to_coordinateInt2(Value v)
{
    Assert(utils::InternalError, v->getType() == ValueBase::COORDINATE,
           "Value is not a Coordinate");
    return boost::static_pointer_cast
        < CoordinateFactory < 2, int > >(v);
}

CoordinateInt3 to_coordinateInt3(Value v)
{
    Assert(utils::InternalError, v->getType() == ValueBase::COORDINATE,
           "Value is not a Coordinate");
    return boost::static_pointer_cast
        < CoordinateFactory < 3, int > >(v);
}

CoordinateDouble1 to_coordinateDouble1(Value v)
{
    Assert(utils::InternalError, v->getType() == ValueBase::COORDINATE,
           "Value is not a Coordinate");
    return boost::static_pointer_cast
        < CoordinateFactory < 1, double > >(v);
}

CoordinateDouble2 to_coordinateDouble2(Value v)
{
    Assert(utils::InternalError, v->getType() == ValueBase::COORDINATE,
           "Value is not a Coordinate");
    return boost::static_pointer_cast
        < CoordinateFactory < 2, double > >(v);
}

CoordinateDouble3 to_coordinateDouble3(Value v)
{
    Assert(utils::InternalError, v->getType() == ValueBase::COORDINATE,
           "Value is not a Coordinate");
    return boost::static_pointer_cast
        < CoordinateFactory < 3, double > >(v);
}

}} // namespace vle value
