/**
 * @file value/Set.cpp
 * @author The VLE Development Team.
 * @brief Set Value a container to Value class.
 */

/*
 * Copyright (C) 2003-2007 - The vle Development Team
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

#include <vle/value/Set.hpp>
#include <vle/utils/Debug.hpp>



namespace vle { namespace value {

SetFactory::SetFactory(const SetFactory& setfactory) :
    ValueBase(setfactory)
{
    const size_t sz = setfactory.m_value.size();
    for (size_t i = 0; i < sz; ++i) {
        //addValue(setfactory.m_value[i]->clone());
        addValue(setfactory.m_value[i]);
    }
}

Set SetFactory::create()
{
    return Set(new SetFactory());
}

Value SetFactory::clone() const
{
    return Value(new SetFactory(*this));
}

void SetFactory::addValue(Value value)
{
    m_value.push_back(value);
}

std::string SetFactory::toFile() const
{
    std::string s;
    VectorValue::const_iterator it = m_value.begin();

    while (it != m_value.end()) {
	s += (*it)->toFile();
	++it;
	if (it != m_value.end())
	    s += " ";
    }
    return s;
}

std::string SetFactory::toString() const
{
    std::string s = "(";
    VectorValue::const_iterator it = m_value.begin();

    while (it != m_value.end()) {
	s += (*it)->toString();
	++it;
	if (it != m_value.end())
	    s += ",";
    }
    s += ")";
    return s;
}

std::string SetFactory::toXML() const
{
    std::string s="<set>";
    VectorValue::const_iterator it = m_value.begin();

    while (it != m_value.end()) {
	s += (*it)->toXML();
	++it;
    }
    s += "</set>";
    return s;
}

Set toSetValue(const Value& value)
{
    Assert(utils::InternalError, value->getType() == ValueBase::SET,
           "Value is not a Set");
    return boost::static_pointer_cast < SetFactory >(value);
}

const VectorValue& toSet(const Value& value)
{
    Assert(utils::InternalError, value->getType() == ValueBase::SET,
           "Value is not a Set");
    return boost::static_pointer_cast < SetFactory >(value)->getValue();
}

}} // namespace vle value
