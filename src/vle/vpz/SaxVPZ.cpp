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

void VpzStackSax::push_vpz()
{
    AssertS(utils::SaxParserError, m_stack.empty());
}

void VpzStackSax::push_structure()
{
}

void VpzStackSax::push_model(const xmlpp::SaxParser::AttributeList& /*att*/)
{
}

void VpzStackSax::push_port(const xmlpp::SaxParser::AttributeList& /*att*/)
{
}

void VpzStackSax::push_in()
{
}

void VpzStackSax::push_out()
{
}

void VpzStackSax::push_init()
{
}

void VpzStackSax::push_state()
{
}

void VpzStackSax::push_submodels()
{
}

void VpzStackSax::push_dynamics()
{
}

void VpzStackSax::push_dynamic(const xmlpp::SaxParser::AttributeList& /*att*/)
{
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool ValueStackSax::is_composite_parent() const
{
    if (not m_valuestack.empty()) {
        const value::Value val = m_valuestack.top();

        return val->isMap() or val->isSet();
    }
    return false;
}

void ValueStackSax::push_integer()
{
    if (not m_valuestack.empty()) {
        AssertS(utils::SaxParserError, is_composite_parent());
    }
}

void ValueStackSax::push_boolean()
{
    if (not m_valuestack.empty()) {
        AssertS(utils::SaxParserError, is_composite_parent());
    }
}

void ValueStackSax::push_string()
{
    if (not m_valuestack.empty()) {
        AssertS(utils::SaxParserError, is_composite_parent());
    }
}

void ValueStackSax::push_double()
{
    if (not m_valuestack.empty()) {
        AssertS(utils::SaxParserError, is_composite_parent());
    }
}

void ValueStackSax::push_map()
{
    if (not m_valuestack.empty()) {
        AssertS(utils::SaxParserError, is_composite_parent());
    }

    push_on_vector_value(value::MapFactory::create());
}

void ValueStackSax::push_map_key(const Glib::ustring& key)
{
    if (not m_valuestack.empty()) {
        AssertS(utils::SaxParserError, m_valuestack.top()->isMap());
    }

    m_lastkey.assign(key);
}

void ValueStackSax::push_set()
{
    if (not m_valuestack.empty()) {
        AssertS(utils::SaxParserError, is_composite_parent());
    }

    push_on_vector_value(value::SetFactory::create());
}

void ValueStackSax::pop_value()
{
    if (not m_valuestack.empty()) {
        m_valuestack.pop();
    }
}

const value::Value& ValueStackSax::top_value()
{
    Assert(utils::SaxParserError, not m_valuestack.empty(),
           "Empty sax parser value stack for the top operation");

    return m_valuestack.top();
}

inline void ValueStackSax::push_on_vector_value(const value::Value& val)
{
    if (not m_valuestack.empty()) {
        if (m_valuestack.top()->isSet()) {
            value::to_set(m_valuestack.top())->addValue(val);
        } else if (m_valuestack.top()->isMap()) {
            value::to_map(m_valuestack.top())->addValue(
                m_lastkey, val);
        }
    } else {
        m_result.push_back(val);
    }

    if (val->isSet() or val->isMap()) {
        m_valuestack.push(val);
    }
}

void ValueStackSax::clear()
{
    while (not m_valuestack.empty())
        m_valuestack.pop();

    m_result.clear();
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

void VLESaxParser::clear_parser_state()
{
    m_valuestack.clear();
    m_lastCharacters.clear();
    m_isValue = false;
    m_isVPZ = false;
    m_isTrame = false;
}

void VLESaxParser::on_start_document()
{
    clear_parser_state();
}

void VLESaxParser::on_end_document()
{
}

void VLESaxParser::on_start_element(
    const Glib::ustring& name,
    const xmlpp::SaxParser::AttributeList& att)
{
    clearLastCharactersStored();
    if (name == "boolean") {
        m_valuestack.push_boolean();
    } else if (name == "integer") {
        m_valuestack.push_integer();
    } else if (name == "double") {
        m_valuestack.push_double();
    } else if (name == "string") {
        m_valuestack.push_string();
    } else if (name == "set") {
        m_valuestack.push_set();
    } else if (name == "map") {
        m_valuestack.push_map();
    } else if (name == "key") {
        xmlpp::SaxParser::AttributeList::const_iterator it;
        it = std::find_if(att.begin(), att.end(), 
                           xmlpp::SaxParser::AttributeHasName("name"));
        AssertS(utils::SaxParserError, it != att.end());

        m_valuestack.push_map_key((*it).value);
    } else if (name == "vpz") {
        AssertS(utils::SaxParserError, not m_isValue and not m_isTrame);
        m_isValue = true;
    } else if (name == "structure") {
        m_vpzstack.push_structure();
    } else if (name == "model") {
        m_vpzstack.push_model(att);
    } else if (name == "port") {
        m_vpzstack.push_port(att);
    } else if (name == "submodels") {
        m_vpzstack.push_submodels();
    } else if (name == "dynamics") {
        m_vpzstack.push_dynamics();
    } else if (name == "dynamic") {
        m_vpzstack.push_dynamic(att);
    } else {
        Throw(utils::SaxParserError,
              (boost::format("Unknow element %1%") % name));
    }
}

void VLESaxParser::on_end_element(const Glib::ustring& name)
{
    if (name == "boolean") {
        m_valuestack.push_on_vector_value(
            value::BooleanFactory::create(
                utils::to_boolean(lastCharactersStored())));
    } else if (name == "integer") {
        m_valuestack.push_on_vector_value(
            value::IntegerFactory::create(
                utils::to_int(lastCharactersStored())));
    } else if (name == "double") {
        m_valuestack.push_on_vector_value(
            value::DoubleFactory::create(
                utils::to_double(lastCharactersStored())));
    } else if (name == "string") {
        m_valuestack.push_on_vector_value(
            value::StringFactory::create(
                utils::to_string(lastCharactersStored())));
    } else if (name == "key") {
        // FIXME delete test
    } else if (name == "set" or name == "map") {
        m_valuestack.pop_value();
    }
}

void VLESaxParser::on_characters(const Glib::ustring& characters)
{
    addToCharacters(characters);
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

const std::vector < value::Value >& VLESaxParser::get_values() const
{
    return m_valuestack.get_results();
}

const value::Value& VLESaxParser::get_value(const size_t pos) const
{
    return m_valuestack.get_result(pos);
}

const Glib::ustring& VLESaxParser::lastCharactersStored() const
{
    return m_lastCharacters;
}

void VLESaxParser::clearLastCharactersStored()
{
    m_lastCharacters.clear();
}

void VLESaxParser::addToCharacters(const Glib::ustring& characters)
{
    m_lastCharacters.append(characters);
}

}} // namespace vle vpz
