/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sstream>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/User.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/XML.hpp>

namespace vle {
namespace value {

std::string
Value::writeToFile() const
{
    std::ostringstream out;
    writeFile(out);
    return out.str();
}

std::string
Value::writeToString() const
{
    std::ostringstream out;
    writeString(out);
    return out.str();
}

std::string
Value::writeToXml() const
{
    std::ostringstream out;
    writeXml(out);
    return out.str();
}

const Boolean&
Value::toBoolean() const
{
    if (not isBoolean()) {
        throw utils::CastError(_("Value is not a boolean"));
    }
    return static_cast<const Boolean&>(*this);
}

const Integer&
Value::toInteger() const
{
    if (not isInteger()) {
        throw utils::CastError(_("Value is not an integer"));
    }
    return static_cast<const Integer&>(*this);
}

const Double&
Value::toDouble() const
{
    if (not isDouble()) {
        throw utils::CastError(_("Value is not a double"));
    }
    return static_cast<const Double&>(*this);
}

const String&
Value::toString() const
{
    if (not isString()) {
        throw utils::CastError(_("Value is not a string"));
    }
    return static_cast<const String&>(*this);
}

const Set&
Value::toSet() const
{
    if (not isSet()) {
        throw utils::CastError(_("Value is not a set"));
    }
    return static_cast<const Set&>(*this);
}

const Map&
Value::toMap() const
{
    if (not isMap()) {
        throw utils::CastError(_("Value is not a map"));
    }
    return static_cast<const Map&>(*this);
}

const Tuple&
Value::toTuple() const
{
    if (not isTuple()) {
        throw utils::CastError(_("Value is not a tuple"));
    }
    return static_cast<const Tuple&>(*this);
}

const Table&
Value::toTable() const
{
    if (not isTable()) {
        throw utils::CastError(_("Value is not a table"));
    }
    return static_cast<const Table&>(*this);
}

const Xml&
Value::toXml() const
{
    if (not isXml()) {
        throw utils::CastError(_("Value is not an xml"));
    }
    return static_cast<const Xml&>(*this);
}

const Null&
Value::toNull() const
{
    if (not isNull()) {
        throw utils::CastError(_("Value is not a null"));
    }
    return static_cast<const Null&>(*this);
}

const Matrix&
Value::toMatrix() const
{
    if (not isMatrix()) {
        throw utils::CastError(_("Value is not a matrix"));
    }
    return static_cast<const Matrix&>(*this);
}

const User&
Value::toUser() const
{
    if (not isUser()) {
        throw utils::CastError(_("Value is not a user value"));
    }
    return static_cast<const User&>(*this);
}

Boolean&
Value::toBoolean()
{
    if (not isBoolean()) {
        throw utils::CastError(_("Value is not a boolean"));
    }
    return static_cast<Boolean&>(*this);
}

Integer&
Value::toInteger()
{
    if (not isInteger()) {
        throw utils::CastError(_("Value is is not an integer"));
    }
    return static_cast<Integer&>(*this);
}

Double&
Value::toDouble()
{
    if (not isDouble()) {
        throw utils::CastError(_("Value is not a double"));
    }
    return static_cast<Double&>(*this);
}

String&
Value::toString()
{
    if (not isString()) {
        throw utils::CastError(_("Value is not a string"));
    }
    return static_cast<String&>(*this);
}

Set&
Value::toSet()
{
    if (not isSet()) {
        throw utils::CastError(_("Value is not a set"));
    }
    return static_cast<Set&>(*this);
}

Map&
Value::toMap()
{
    if (not isMap()) {
        throw utils::CastError(_("Value is not a map"));
    }
    return static_cast<Map&>(*this);
}

Tuple&
Value::toTuple()
{
    if (not isTuple()) {
        throw utils::CastError(_("Value is not a tuple"));
    }
    return static_cast<Tuple&>(*this);
}

Table&
Value::toTable()
{
    if (not isTable()) {
        throw utils::CastError(_("Value is not a table"));
    }
    return static_cast<Table&>(*this);
}

Xml&
Value::toXml()
{
    if (not isXml()) {
        throw utils::CastError(_("Value is not an xml"));
    }
    return static_cast<Xml&>(*this);
}

Null&
Value::toNull()
{
    if (not isNull()) {
        throw utils::CastError(_("Value is not a null"));
    }
    return static_cast<Null&>(*this);
}

Matrix&
Value::toMatrix()
{
    if (not isMatrix()) {
        throw utils::CastError(_("Value is not a matrix"));
    }
    return static_cast<Matrix&>(*this);
}

User&
Value::toUser()
{
    if (not isUser()) {
        throw utils::CastError(_("Value is not a user value"));
    }
    return static_cast<User&>(*this);
}

std::shared_ptr<Value>
clone(std::shared_ptr<Value> v)
{
    return clone(std::const_pointer_cast<const Value>(v));
}

std::shared_ptr<Value>
clone(std::shared_ptr<const Value> v)
{
    if (not v)
        return std::shared_ptr<Value>();

    switch (v->getType()) {
    case Value::INTEGER:
        return std::static_pointer_cast<Value>(
          std::make_shared<Integer>(v->toInteger()));
    case Value::BOOLEAN:
        return std::static_pointer_cast<Value>(
          std::make_shared<Boolean>(v->toBoolean()));
    case Value::DOUBLE:
        return std::static_pointer_cast<Value>(
          std::make_shared<Double>(v->toDouble()));
    case Value::STRING:
        return std::static_pointer_cast<Value>(
          std::make_shared<String>(v->toString()));
    case Value::SET:
        return std::static_pointer_cast<Value>(
          std::make_shared<Set>(v->toSet()));
    case Value::MAP:
        return std::static_pointer_cast<Value>(
          std::make_shared<Map>(v->toMap()));
    case Value::TUPLE:
        return std::static_pointer_cast<Value>(
          std::make_shared<Tuple>(v->toTuple()));
    case Value::TABLE:
        return std::static_pointer_cast<Value>(
          std::make_shared<Table>(v->toTable()));
    case Value::XMLTYPE:
        return std::static_pointer_cast<Value>(
          std::make_shared<Xml>(v->toXml()));
    case Value::NIL:
        return std::static_pointer_cast<Value>(
          std::make_shared<Null>(v->toNull()));
    case Value::MATRIX:
        return std::static_pointer_cast<Value>(
          std::make_shared<Matrix>(v->toMatrix()));
    case Value::USER:
        return std::shared_ptr<Value>();
    }

    return std::shared_ptr<Value>();
}

bool
is_composite(const std::unique_ptr<Value>& val)
{
    if (not val)
        return false;

    switch (val->getType()) {
    case Value::MAP:
    case Value::SET:
    case Value::MATRIX:
        return true;

    default:
        return false;
    }
}

const Value&
reference(const std::unique_ptr<Value>& value)
{
    if (not value)
        throw utils::ArgError(_("Null value"));

    return *value.get();
}

const Value&
reference(std::shared_ptr<const Value> value)
{
    if (not value)
        throw utils::ArgError(_("Null value"));

    return *value.get();
}

const Value&
reference(std::shared_ptr<Value> value)
{
    if (not value)
        throw utils::ArgError(_("Null value"));

    return *value.get();
}
}
} // namespace vle value
