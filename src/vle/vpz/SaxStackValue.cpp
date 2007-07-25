/** 
 * @file SaxStackValue.cpp
 * @brief 
 * @author The vle Development Team
 * @date dim, 25 fév 2007 20:20:04 +0100
 */

/*
 * Copyright (C) 2007 - The vle Development Team
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

#include <vle/vpz/SaxStackValue.hpp>

namespace vle { namespace vpz {

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

void ValueStackSax::push_tuple()
{
    if (not m_valuestack.empty()) {
        AssertS(utils::SaxParserError, is_composite_parent());
    }

    push_on_vector_value(value::TupleFactory::create());
}

void ValueStackSax::push_table(const size_t width, const size_t height)
{
    if (not m_valuestack.empty()) {
        AssertS(utils::SaxParserError, is_composite_parent());
    }

    push_on_vector_value(value::TableFactory::create(width, height));
}

void ValueStackSax::push_xml()
{
    if (not m_valuestack.empty()) {
        AssertS(utils::SaxParserError, is_composite_parent());
    }
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

void ValueStackSax::push_on_vector_value(const value::Value& val)
{
    if (not m_valuestack.empty()) {
        if (m_valuestack.top()->isSet()) {
            value::toSetValue(m_valuestack.top())->addValue(val);
        } else if (m_valuestack.top()->isMap()) {
            value::toMapValue(m_valuestack.top())->addValue(
                m_lastkey, val);
        }
    } else {
        m_result.push_back(val);
    }

    if (val->isSet() or val->isMap() or val->isTuple() or val->isTable()) {
        m_valuestack.push(val);
    }
}

void ValueStackSax::clear()
{
    while (not m_valuestack.empty())
        m_valuestack.pop();

    m_result.clear();
}


}} // namespace vle vpz
