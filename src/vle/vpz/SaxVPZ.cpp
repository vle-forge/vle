/** 
 * @file SaxVPZ.cpp
 * @brief A class to parse VPZ XML with Sax parser.
 * @author The vle Development Team
 * @date jeu, 14 déc 2006 10:34:50 +0100
 */

/*
 * Copyright (C) 2006 - The vle Development Team
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

#include <vle/vpz/SaxVPZ.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/String.hpp>


namespace vle { namespace vpz {

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VLEStack::push(vpz::Base* element)
{
    Assert(utils::SaxParserError, element, "Push an empty element ?");
    m_stack.push(element);
}

void VLEStack::pop()
{
    m_stack.pop();
}

vpz::Base* VLEStack::top() const
{
    Assert(utils::SaxParserError, not m_stack.empty(),
           "Empty sax parser stack for the top() operation");

    return m_stack.top();
}

bool VLEStack::empty() const
{
    return m_stack.empty();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void ValueStack::push(const value::Value& val)
{
    Assert(utils::SaxParserError, val, "Push an empty value ?");
    m_stack.push(val);
}

void ValueStack::pop()
{
    m_stack.pop();
}

value::Value& ValueStack::top()
{
    Assert(utils::SaxParserError, not m_stack.empty(),
           "Empty value sax parser stack for the top() operation");

    return m_stack.top();
}

bool ValueStack::empty() const
{
    return m_stack.empty();
}

value::Value ValueStack::clear()
{
    value::Value last;

    if (not empty()) {
        last = top();
        pop();
        while (not empty()) {
            last = top();
            pop();
        }
    }
    return last;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

VLESaxParser::VLESaxParser() :
    m_isValue(false),
    m_isVPZ(false)
{
}

VLESaxParser::~VLESaxParser()
{
}

void VLESaxParser::on_start_document()
{
}

void VLESaxParser::on_end_document()
{
}

void VLESaxParser::on_start_element(
    const Glib::ustring& name,
    const xmlpp::SaxParser::AttributeList& /*attributes */)
{
    if (name == "boolean" or name == "integer" or name == "double"
        or name == "string") {
        // rien ?!?
    } else if (name == "set") {
        addValue(value::SetFactory::create());
    } else if (name == "map") {
        addValue(value::MapFactory::create());
    }
}

void VLESaxParser::on_end_element(const Glib::ustring& name)
{
    if (name == "boolean") {
        addValue(value::BooleanFactory::create(
                utils::to_boolean(lastCharactersStored())));
    } else if (name == "integer") {
        addValue(value::IntegerFactory::create(
                utils::to_int(lastCharactersStored())));
    } else if (name == "double") {
        addValue(value::DoubleFactory::create(
                utils::to_double(lastCharactersStored())));
    } else if (name == "string") {
        addValue(value::StringFactory::create(
                utils::to_string(lastCharactersStored())));
    } else if (name == "set" and name == "map") {
    }
}

void VLESaxParser::on_characters(const Glib::ustring& characters)
{
    storeCharacters(characters);
}

void VLESaxParser::on_comment(const Glib::ustring& /* text */)
{
}

void VLESaxParser::on_warning(const Glib::ustring& text)
{
    DTRACE(text);
}

void VLESaxParser::on_error(const Glib::ustring& text)
{
    Throw(utils::SaxParserError, text);
}

void VLESaxParser::on_fatal_error(const Glib::ustring& text)
{
    Throw(utils::SaxParserError, text);
}

void VLESaxParser::on_cdata_block(const Glib::ustring& /* text */)
{
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VLESaxParser::startValue(value::Value val)
{
    Assert(utils::SaxParserError, m_value.empty(),
           "Start value with a no-empty stack");

    m_value.push(val);
}

void VLESaxParser::addValue(value::Value val)
{
    if (val->isBoolean() or val->isInteger() or val->isDouble() or
        val->isString()) {
        if (m_stack.top() == 0) {
            if (not m_isVPZ) m_isValue = true;
            // on ne lit plus rien après !
        }
        if (m_value.top()) {
            if (not m_isVPZ) m_isValue = true;
            // vérif père == MAP / SET / (factor?!?) ou rien)
        } else if (val->isMap() and val->isSet()) {
            if (not m_isVPZ) m_isValue = true;
            // vérif père == MAP / SET / (factor?!?) ou rien)
        }
    }
}

const Glib::ustring& VLESaxParser::lastCharactersStored() const
{
    return m_lastCharacters;
}

void VLESaxParser::clearLastCharactersStored()
{
    m_lastCharacters.clear();
}

void VLESaxParser::storeCharacters(const Glib::ustring& characters)
{
    Assert(utils::SaxParserError, not m_lastCharacters.empty(),
           "Characters buffer empty.");

    m_lastCharacters.assign(characters);
}

}} // namespace vle vpz
