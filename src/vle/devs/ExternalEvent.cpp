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

#include <vle/devs/ExternalEvent.hpp>
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
#include <vle/value/XML.hpp>

namespace vle {
namespace devs {

value::Boolean&
ExternalEvent::addBoolean(bool value)
{
    return pp_add<value::Boolean>(value);
}

value::Double&
ExternalEvent::addDouble(double value)
{
    return pp_add<value::Double>(value);
}

value::Integer&
ExternalEvent::addInteger(int32_t value)
{
    return pp_add<value::Integer>(value);
}

value::String&
ExternalEvent::addString(const std::string& value)
{
    return pp_add<value::String>(value);
}

value::Xml&
ExternalEvent::addXml(const std::string& value)
{
    return pp_add<value::Xml>(value);
}

value::Tuple&
ExternalEvent::addTuple(std::size_t size, double value)
{
    return pp_add<value::Tuple>(size, value);
}

value::Table&
ExternalEvent::addTable(std::size_t width, std::size_t height)
{
    return pp_add<value::Table>(width, height);
}

value::Map&
ExternalEvent::addMap()
{
    return pp_add<value::Map>();
}

value::Set&
ExternalEvent::addSet()
{
    return pp_add<value::Set>();
}

value::Matrix&
ExternalEvent::addMatrix(std::size_t columns,
                         std::size_t rows,
                         std::size_t columnmax,
                         std::size_t rowmax,
                         std::size_t resizeColumns,
                         std::size_t resizeRows)
{
    return pp_add<value::Matrix>(
      columns, rows, columnmax, rowmax, resizeColumns, resizeRows);
}

const value::Boolean&
ExternalEvent::getBoolean() const
{
    if (not m_attributes or not m_attributes->isBoolean())
        throw utils::ArgError((fmt(_("ExternalEvent: getAttributes is empty or"
                                     " is not a map.")))
                                .str());

    return m_attributes->toBoolean();
}

value::Boolean&
ExternalEvent::getBoolean()
{
    if (not m_attributes or not m_attributes->isBoolean())
        throw utils::ArgError((fmt(_("ExternalEvent: getAttributes is empty or"
                                     " is not a map.")))
                                .str());

    return m_attributes->toBoolean();
}

const value::Double&
ExternalEvent::getDouble() const
{
    if (not m_attributes or not m_attributes->isDouble())
        throw utils::ArgError((fmt(_("ExternalEvent: getAttributes is empty or"
                                     " is not a map.")))
                                .str());

    return m_attributes->toDouble();
}

value::Double&
ExternalEvent::getDouble()
{
    if (not m_attributes or not m_attributes->isDouble())
        throw utils::ArgError((fmt(_("ExternalEvent: getAttributes is empty or"
                                     " is not a map.")))
                                .str());

    return m_attributes->toDouble();
}

const value::Integer&
ExternalEvent::getInteger() const
{
    if (not m_attributes or not m_attributes->isInteger())
        throw utils::ArgError((fmt(_("ExternalEvent: getAttributes is empty or"
                                     " is not a map.")))
                                .str());

    return m_attributes->toInteger();
}

value::Integer&
ExternalEvent::getInteger()
{
    if (not m_attributes or not m_attributes->isInteger())
        throw utils::ArgError((fmt(_("ExternalEvent: getAttributes is empty or"
                                     " is not a map.")))
                                .str());

    return m_attributes->toInteger();
}

const value::String&
ExternalEvent::getString() const
{
    if (not m_attributes or not m_attributes->isString())
        throw utils::ArgError((fmt(_("ExternalEvent: getAttributes is empty or"
                                     " is not a map.")))
                                .str());

    return m_attributes->toString();
}

value::String&
ExternalEvent::getString()
{
    if (not m_attributes or not m_attributes->isString())
        throw utils::ArgError((fmt(_("ExternalEvent: getAttributes is empty or"
                                     " is not a map.")))
                                .str());

    return m_attributes->toString();
}

const value::Xml&
ExternalEvent::getXml() const
{
    if (not m_attributes or not m_attributes->isXml())
        throw utils::ArgError((fmt(_("ExternalEvent: getAttributes is empty or"
                                     " is not a map.")))
                                .str());

    return m_attributes->toXml();
}

value::Xml&
ExternalEvent::getXml()
{
    if (not m_attributes or not m_attributes->isXml())
        throw utils::ArgError((fmt(_("ExternalEvent: getAttributes is empty or"
                                     " is not a map.")))
                                .str());

    return m_attributes->toXml();
}

const value::Tuple&
ExternalEvent::getTuple() const
{
    if (not m_attributes or not m_attributes->isTuple())
        throw utils::ArgError((fmt(_("ExternalEvent: getAttributes is empty or"
                                     " is not a map.")))
                                .str());

    return m_attributes->toTuple();
}

value::Tuple&
ExternalEvent::getTuple()
{
    if (not m_attributes or not m_attributes->isTuple())
        throw utils::ArgError((fmt(_("ExternalEvent: getAttributes is empty or"
                                     " is not a map.")))
                                .str());

    return m_attributes->toTuple();
}

const value::Table&
ExternalEvent::getTable() const
{
    if (not m_attributes or not m_attributes->isTable())
        throw utils::ArgError((fmt(_("ExternalEvent: getAttributes is empty or"
                                     " is not a map.")))
                                .str());

    return m_attributes->toTable();
}

value::Table&
ExternalEvent::getTable()
{
    if (not m_attributes or not m_attributes->isTable())
        throw utils::ArgError((fmt(_("ExternalEvent: getAttributes is empty or"
                                     " is not a map.")))
                                .str());

    return m_attributes->toTable();
}

const value::Map&
ExternalEvent::getMap() const
{
    if (not m_attributes or not m_attributes->isMap())
        throw utils::ArgError((fmt(_("ExternalEvent: getAttributes is empty or"
                                     " is not a map.")))
                                .str());

    return m_attributes->toMap();
}

value::Map&
ExternalEvent::getMap()
{
    if (not m_attributes or not m_attributes->isMap())
        throw utils::ArgError((fmt(_("ExternalEvent: getAttributes is empty or"
                                     " is not a map.")))
                                .str());

    return m_attributes->toMap();
}

const value::Set&
ExternalEvent::getSet() const
{
    if (not m_attributes or not m_attributes->isSet())
        throw utils::ArgError((fmt(_("ExternalEvent: getAttributes is empty or"
                                     " is not a map.")))
                                .str());

    return m_attributes->toSet();
}

value::Set&
ExternalEvent::getSet()
{
    if (not m_attributes or not m_attributes->isSet())
        throw utils::ArgError((fmt(_("ExternalEvent: getAttributes is empty or"
                                     " is not a map.")))
                                .str());

    return m_attributes->toSet();
}

const value::Matrix&
ExternalEvent::getMatrix() const
{
    if (not m_attributes or not m_attributes->isMatrix())
        throw utils::ArgError((fmt(_("ExternalEvent: getAttributes is empty or"
                                     " is not a map.")))
                                .str());

    return m_attributes->toMatrix();
}

value::Matrix&
ExternalEvent::getMatrix()
{
    if (not m_attributes or not m_attributes->isMatrix())
        throw utils::ArgError((fmt(_("ExternalEvent: getAttributes is empty or"
                                     " is not a map.")))
                                .str());

    return m_attributes->toMatrix();
}
}
} // namespace vle devs
