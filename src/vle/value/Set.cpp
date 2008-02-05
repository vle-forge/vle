/**
 * @file src/vle/value/Set.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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
