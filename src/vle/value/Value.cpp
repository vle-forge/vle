/**
 * @file vle/value/Value.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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


#include <vle/value/Value.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/XML.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/utils/Tools.hpp>
#include <sstream>
#include <boost/serialization/export.hpp>

namespace vle { namespace value {


std::string Value::writeToFile() const
{
    std::ostringstream out;
    writeFile(out);
    return out.str();
}

std::string Value::writeToString() const
{
    std::ostringstream out;
    writeString(out);
    return out.str();
}

std::string Value::writeToXml() const
{
    std::ostringstream out;
    writeXml(out);
    return out.str();
}

const Boolean& Value::toBoolean() const
{
    if (not isBoolean()) {
        throw utils::CastError(_("Value is not a boolean"));
    }
    return static_cast < const Boolean& >(*this);
}

const Integer& Value::toInteger() const
{
    if (not isInteger()) {
        throw utils::CastError(_("Value is not an integer"));
    }
    return static_cast < const Integer& >(*this);
}

const Double& Value::toDouble() const
{
    if (not isDouble()) {
        throw utils::CastError(_("Value is not a double"));
    }
    return static_cast < const Double& >(*this);
}

const String& Value::toString() const
{
    if (not isString()) {
        throw utils::CastError(_("Value is not a string"));
    }
    return static_cast < const String& >(*this);
}

const Set& Value::toSet() const
{
    if (not isSet()) {
        throw utils::CastError(_("Value is not a set"));
    }
    return static_cast < const Set& >(*this);
}

const Map& Value::toMap() const
{
    if (not isMap()) {
        throw utils::CastError(_("Value is not a map"));
    }
    return static_cast < const Map& >(*this);
}

const Tuple& Value::toTuple() const
{
    if (not isTuple()) {
        throw utils::CastError(_("Value is not a tuple"));
    }
    return static_cast < const Tuple& >(*this);
}

const Table& Value::toTable() const
{
    if (not isTable()) {
        throw utils::CastError(_("Value is not a table"));
    }
    return static_cast < const Table& >(*this);
}

const Xml& Value::toXml() const
{
    if (not isXml()) {
        throw utils::CastError(_("Value is not an xml"));
    }
    return static_cast < const Xml& >(*this);
}

const Null& Value::toNull() const
{
    if (not isNull()) {
        throw utils::CastError(_("Value is not a null"));
    }
    return static_cast < const Null& >(*this);
}

const Matrix& Value::toMatrix() const
{
    if (not isMatrix()) {
        throw utils::CastError(_("Value is not a matrix"));
    }
    return static_cast < const Matrix& >(*this);
}

Boolean& Value::toBoolean()
{
    if (not isBoolean()) {
        throw utils::CastError(_("Value is not a boolean"));
    }
    return static_cast < Boolean& >(*this);
}

Integer& Value::toInteger()
{
    if (not isInteger()) {
        throw utils::CastError(_("Value is is not an integer"));
    }
    return static_cast < Integer& >(*this);
}

Double& Value::toDouble()
{
    if (not isDouble()) {
        throw utils::CastError(_("Value is not a double"));
    }
    return static_cast < Double& >(*this);
}

String& Value::toString()
{
    if (not isString()) {
        throw utils::CastError(_("Value is not a string"));
    }
    return static_cast < String& >(*this);
}

Set& Value::toSet()
{
    if (not isSet()) {
        throw utils::CastError(_("Value is not a set"));
    }
    return static_cast < Set& >(*this);
}

Map& Value::toMap()
{
    if (not isMap()) {
        throw utils::CastError(_("Value is not a map"));
    }
    return static_cast < Map& >(*this);
}

Tuple& Value::toTuple()
{
    if (not isTuple()) {
        throw utils::CastError(_("Value is not a tuple"));
    }
    return static_cast < Tuple& >(*this);
}

Table& Value::toTable()
{
    if (not isTable()) {
        throw utils::CastError(_("Value is not a table"));
    }
    return static_cast < Table& >(*this);
}

Xml& Value::toXml()
{
    if (not isXml()) {
        throw utils::CastError(_("Value is not an xml"));
    }
    return static_cast < Xml& >(*this);
}

Null& Value::toNull()
{
    if (not isNull()) {
        throw utils::CastError(_("Value is not a null"));
    }
    return static_cast < Null& >(*this);
}

Matrix& Value::toMatrix()
{
    if (not isMatrix()) {
        throw utils::CastError(_("Value is not a matrix"));
    }
    return static_cast < Matrix& >(*this);
}

void init()
{
    utils::init();
#ifdef VLE_HAVE_POOL
    value::Pools::init();
#endif

    boost::serialization::void_cast_register(
        static_cast<Boolean*>(0), static_cast<Value*>(0));
    boost::serialization::void_cast_register(
        static_cast<Double*>(0), static_cast<Value*>(0));
    boost::serialization::void_cast_register(
        static_cast<Map*>(0), static_cast<Value*>(0));
    boost::serialization::void_cast_register(
        static_cast<Integer*>(0), static_cast<Value*>(0));
    boost::serialization::void_cast_register(
        static_cast<String*>(0), static_cast<Value*>(0));
    boost::serialization::void_cast_register(
        static_cast<Set*>(0), static_cast<Value*>(0));
    boost::serialization::void_cast_register(
        static_cast<Tuple*>(0), static_cast<Value*>(0));
    boost::serialization::void_cast_register(
        static_cast<Table*>(0), static_cast<Value*>(0));
    boost::serialization::void_cast_register(
        static_cast<Xml*>(0), static_cast<Value*>(0));
    boost::serialization::void_cast_register(
        static_cast<Null*>(0), static_cast<Value*>(0));
    boost::serialization::void_cast_register(
        static_cast<Matrix*>(0), static_cast<Value*>(0));
}

void finalize()
{
#ifdef VLE_HAVE_POOL
    value::Pools::kill();
#endif
}

}} // namespace vle value

#ifndef __WIN32__
BOOST_CLASS_EXPORT(vle::value::Boolean)
BOOST_CLASS_EXPORT(vle::value::Double)
BOOST_CLASS_EXPORT(vle::value::Map)
BOOST_CLASS_EXPORT(vle::value::Integer)
BOOST_CLASS_EXPORT(vle::value::String)
BOOST_CLASS_EXPORT(vle::value::Set)
BOOST_CLASS_EXPORT(vle::value::Tuple)
BOOST_CLASS_EXPORT(vle::value::Table)
BOOST_CLASS_EXPORT(vle::value::Xml)
BOOST_CLASS_EXPORT(vle::value::Null)
BOOST_CLASS_EXPORT(vle::value::Matrix)
#endif
