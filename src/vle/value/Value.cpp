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
#include <vle/utils/Debug.hpp>

namespace vle { namespace value {

    const Value ValueBase::empty;

    //VectorValue Value::getValues(xmlpp::Element* root)
    ////{
    //std::vector < Value* > result;
    //
    //if (root) {
    //xmlpp::Node::NodeList lst = root->get_children();
    //for (xmlpp::Node::NodeList::iterator it = lst.begin();
    //it != lst.end(); ++it) {
    //if (vle::utils::xml::is_element(*it)) {
    //result.push_back(Value::getValue((xmlpp::Element*)*it));
    //}
    //}
    //}
    //
    //return result;
    //}
    //
    //Value* Value::getValue(xmlpp::Element* root)
    //{
    //if (root) {
    //Glib::ustring name(root->get_name());
    //if (name == "DOUBLE")
    //return new Double(root);
    //else if (name == "BOOLEAN")
    //return new Boolean(root);
    //else if (name == "INTEGER")
    //return new Integer(root);
    //else if (name == "STRING")
    //return new String(root);
    //else if (name == "SET")
    //return new Set(root);
    //else if (name == "MAP")
    //return new Map(root);
    //}
    //return 0;
    //}
    //
    //void Value::cleanValues(std::vector < Value* >& vals)
    //{
    //for (std::vector < Value* >::iterator it = vals.begin();
    //it != vals.end(); it++) {
    //delete (*it);
    //(*it) = 0;
    //}
    //}
    //
    //bool Value::isComplex(xmlpp::Element* root)
    //{
    //if (root) {
    //Glib::ustring name(root->get_name());
    //return name == "SET" or name == "MAP";
    //}
    //return 0;
    //}

Boolean to_boolean(Value v)
{
    Assert(vle::utils::InternalError, v->getType() == ValueBase::BOOLEAN,
           "Value is not a Boolean");

    return boost::static_pointer_cast < BooleanFactory >(v);
}

Integer to_integer(Value v)
{
    Assert(vle::utils::InternalError, v->getType() == ValueBase::INTEGER,
           "Value is not an Integer");

    return boost::static_pointer_cast < IntegerFactory >(v);
}

String to_string(Value v)
{
    Assert(vle::utils::InternalError, v->getType() == ValueBase::STRING,
           "Value is not a String");

    return boost::static_pointer_cast < StringFactory >(v);
}

Double to_double(Value v)
{
    Assert(vle::utils::InternalError, v->getType() == ValueBase::DOUBLE,
           "Value is not a Double");

    return boost::static_pointer_cast < DoubleFactory >(v);

}

Map to_map(Value v)
{
    Assert(vle::utils::InternalError, v->getType() == ValueBase::MAP,
           "Value is not a Map");

    return boost::static_pointer_cast < MapFactory >(v);
}


Set to_set(Value v)
{
    Assert(vle::utils::InternalError, v->getType() == ValueBase::SET,
           "Value is not a Set");

    return boost::static_pointer_cast < SetFactory >(v);
}

}} // namespace vle value
