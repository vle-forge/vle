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
#include <vle/value/Map.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/XML.hpp>
#include <vle/value/Null.hpp>
#include <vle/utils/Debug.hpp>



namespace vle { namespace value {

SetFactory::SetFactory(const SetFactory& setfactory) :
    ValueBase(setfactory)
{
    m_value.resize(setfactory.size());

    std::transform(setfactory.begin(), setfactory.end(),
                   m_value.begin(), CloneValue());
}

Set SetFactory::create()
{
    return Set(new SetFactory());
}

Value SetFactory::clone() const
{
    return Value(new SetFactory(*this));
}

void SetFactory::addNullValue()
{
    m_value.push_back(NullFactory::create());
}

void SetFactory::addBooleanValue(bool value)
{
    m_value.push_back(BooleanFactory::create(value));
}

bool SetFactory::getBooleanValue(const size_t i) const
{
    return toBoolean(getValue(i));
}

void SetFactory::addDoubleValue(double value)
{
    m_value.push_back(DoubleFactory::create(value));
}

double SetFactory::getDoubleValue(const size_t i) const
{
    return toDouble(getValue(i));
}

void SetFactory::addIntValue(int value)
{
    m_value.push_back(IntegerFactory::create(value));
}

int SetFactory::getIntValue(const size_t i) const
{
    return toInteger(getValue(i));
}

void SetFactory::addLongValue(long value)
{
    m_value.push_back(IntegerFactory::create(value));
}

long SetFactory::getLongValue(const size_t i) const
{
    return toLong(getValue(i));
}

void SetFactory::addStringValue(const std::string& value)
{
    m_value.push_back(StringFactory::create(value));
}

const std::string& SetFactory::getStringValue(const size_t i) const
{
    return value::toString(getValue(i));
}

void SetFactory::addXMLValue(const std::string& value)
{
    m_value.push_back(XMLFactory::create(value));
}

const std::string& SetFactory::getXMLValue(const size_t i) const
{
    return value::toXml(getValue(i));
}

const Value& SetFactory::getValue(const size_t i) const
{
    Assert(utils::ArgError, i < size(), boost::format(
            "The index '%1%' is to big for this Set (size='%2%')") % i % size());

    return m_value[i];
}

Value& SetFactory::getValue(const size_t i)
{
    Assert(utils::ArgError, i < size(), boost::format(
            "The index '%1%' is to big for this Set (size='%2%')") % i % size());

    return m_value[i];
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
    Assert(utils::ArgError, value->getType() == ValueBase::SET,
           "Value is not a Set");
    return boost::static_pointer_cast < SetFactory >(value);
}

const VectorValue& toSet(const Value& value)
{
    Assert(utils::ArgError, value->getType() == ValueBase::SET,
           "Value is not a Set");
    return boost::static_pointer_cast < SetFactory >(value)->getValue();
}

}} // namespace vle value
