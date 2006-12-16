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

namespace vle { namespace vpz {

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

VLEStack::VLEStack()
{
}

VLEStack::~VLEStack()
{
}

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
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VLESaxParser::on_start_element(
    const Glib::ustring& name,
    const xmlpp::SaxParser::AttributeList& /*attributes */)
{
    if (name == "boolean") {
    } else if (name == "integer") {
    } else if (name == "double") {
    } else if (name == "string") {
    } else if (name == "set") {
    } else if (name == "map") {
    } else if (name == "pointer") {
    } else if (name == "speed") {
    } else if (name == "coordinate") {
    } else if (name == "direction") {
    }
}

}} // namespace vle vpz
