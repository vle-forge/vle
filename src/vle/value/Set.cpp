/**
 * @file vle/value/Set.cpp
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
#include <vle/value/Deleter.hpp>
#include <vle/utils/Debug.hpp>
#include <boost/utility.hpp>

namespace vle { namespace value {

Set::Set(const Set& setfactory) :
    Value(setfactory)
{
    m_value.resize(setfactory.size());

    std::transform(setfactory.begin(), setfactory.end(),
                   m_value.begin(), CloneValue());
}

Set::~Set()
{
    clear();
}

void Set::writeFile(std::ostream& out) const
{
    for (VectorValue::const_iterator it = m_value.begin();
         it != m_value.end(); ++it) {
	if (it != m_value.begin()) {
	    out << ",";
        }
        if (*it) {
            (*it)->writeFile(out);
        } else {
            out << "NA";
        }
    }
}

void Set::writeString(std::ostream& out) const
{
    out << "(";
    for (VectorValue::const_iterator it = m_value.begin();
         it != m_value.end(); ++it) {
	if (it != m_value.begin()) {
	    out << ",";
        }
        if (*it) {
            (*it)->writeString(out);
        } else {
            out << "NA";
        }
    }
    out << ")";
}

void Set::writeXml(std::ostream& out) const
{
    out << "<set>";
    for (VectorValue::const_iterator it = m_value.begin();
         it != m_value.end(); ++it) {
        if (*it) {
            (*it)->writeXml(out);
        } else {
            out << "<null />";
        }
    }
    out << "</set>";
}

void Set::add(Value* value)
{
    m_value.push_back(value);
}

void Set::addNull()
{
    m_value.push_back(Null::create());
}

void Set::addBoolean(bool value)
{
    m_value.push_back(Boolean::create(value));
}

bool Set::getBoolean(const size_type i) const
{
    return get(i).toBoolean().value();
}

void Set::addDouble(double value)
{
    m_value.push_back(Double::create(value));
}

double Set::getDouble(const size_type i) const
{
    return get(i).toDouble().value();
}

void Set::addInt(int value)
{
    m_value.push_back(Integer::create(value));
}

int Set::getInt(const size_type i) const
{
    return get(i).toInteger().intValue();
}

void Set::addLong(long value)
{
    m_value.push_back(Integer::create(value));
}

long Set::getLong(const size_type i) const
{
    return get(i).toInteger().value();
}

void Set::addString(const std::string& value)
{
    m_value.push_back(String::create(value));
}

const std::string& Set::getString(const size_type i) const
{
    return value::toString(get(i));
}

void Set::addXml(const std::string& value)
{
    m_value.push_back(Xml::create(value));
}

const std::string& Set::getXml(const size_type i) const
{
    return value::toXml(get(i));
}

const Value& Set::get(const size_type i) const
{
    if (i >= size()) {
        throw std::out_of_range("Set: too big index");
    }

    if (m_value[i] == 0) {
        throw utils::ArgError("Set: no value in this cell");
    }

    return *m_value[i];
}

Value& Set::get(const size_type i)
{
    if (i >= size()) {
        throw std::out_of_range("Set: too big index");
    }

    if (m_value[i] == 0) {
        throw utils::ArgError("Set: no value in this cell");
    }

    return *m_value[i];
}

Value* Set::give(const size_type& i)
{
    if (i >= size()) {
        throw std::out_of_range("Set: too big index");
    }

    Value* result = m_value[i];
    m_value[i] = 0;
    return result;
}

void Set::del(const size_type i)
{
    if (i >= size()) {
        throw std::out_of_range("Set: too big index");
    }

    delete m_value[i];
    m_value[i] = 0;
    m_value.erase(begin() + i);
}

void Set::clear()
{
    std::stack < Value* > composite;

    for (iterator it = begin(); it != end(); ++it) {
        if (*it) {
            if (isComposite(*it)) {
                composite.push(*it);
            } else {
                delete *it;
                *it = 0;
            }
        }
    }

    deleter(composite);
    m_value.clear();
}

}} // namespace vle value

BOOST_CLASS_EXPORT(vle::value::Set)

