/** 
 * @file Replicas.cpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 31 jan 2006 17:27:02 +0100
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

#include <vle/vpz/Replicas.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {

using namespace vle::utils;

Replicas::Replicas() :
    m_number(1),
    m_seed(Replicas::RANDOM),
    m_begin(0)
{ }

Replicas::~Replicas()
{
}

void Replicas::init(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "REPLICAS");
    
    m_list.clear();
    m_number = utils::to_uint(xml::get_attribute(elt, "NUMBER"));

    Glib::ustring str(xml::get_attribute(elt, "SEED"));
    if (str == "random") {
        m_seed = Replicas::RANDOM;
        m_seedvalue = utils::to_size_t(
            xml::get_attribute(elt, "SEED_VALUE"));
    } else if (str == "list") {
        m_seed = Replicas::ENUMERATE;
        m_begin = utils::to_uint
            (xml::get_attribute(elt, "BEGIN"));
    } else if (str == "enumerate") {
        m_list.resize(m_number);
        m_seed = Replicas::LIST;
        xmlpp::Node::NodeList lst = elt->get_children("REPLICA");
        for (xmlpp::Node::NodeList::iterator it = lst.begin();
             it != lst.end(); ++it) {
            size_t i = xml::get_size_t_attribute(
                    ((xmlpp::Element*)(*it)), "NUMBER");
            m_list[i] = xml::get_size_t_attribute(
                    ((xmlpp::Element*)(*it)), "SEED");
        }
    }
}

void Replicas::write(xmlpp::Element* elt) const
{
    AssertI(elt);
    
    xmlpp::Element* replicas = elt->add_child("REPLICAS");
    replicas->set_attribute("NUMBER", utils::to_string(m_number));

    switch(m_seed) {
    case Replicas::RANDOM:
        replicas->set_attribute("SEED", "random");
        replicas->set_attribute("SEED_VALUE", utils::to_string(m_seedvalue));
        break;

    case Replicas::LIST:
        replicas->set_attribute("SEED", "list");
        replicas->set_attribute("BEGIN", utils::to_string(m_begin));
        break;

    case Replicas::ENUMERATE:
        replicas->set_attribute("SEED", "enumerate");
        for (size_t i = 0; i < m_number; ++i) {
            xmlpp::Element* replica = replicas->add_child("REPLICA");
            replica->set_attribute("NUMBER", utils::to_string(i));
            replica->set_attribute("SEED", utils::to_string(m_list[i]));
        }
        break;
    }
}

void Replicas::setRandom(size_t number, guint32 seed)
{
    AssertI(number > 0);
    
    m_number = number;
    m_seedvalue = seed;
    m_seed = Replicas::RANDOM;
}

void Replicas::setEnumerate(const std::vector < guint32 >& lst)
{
    m_seed = Replicas::ENUMERATE;
    m_list.resize(lst.size());
    std::copy(lst.begin(), lst.end(), m_list.begin());
}

void Replicas::setList(size_t number, size_t begin)
{
    m_number = number;
    m_begin = begin;
    m_seed = Replicas::LIST;
}

void Replicas::clear()
{
    m_number = 1;
    m_seed = Replicas::RANDOM;
    m_begin = 0;
    m_seedvalue = 0;
    m_list.clear();
}

const std::vector < guint32 >& Replicas::getList()
{
    m_list.resize(m_number);
    size_t begin = m_begin;

    switch (m_seed) {
    case Replicas::RANDOM:
        for (size_t i = 0; i < m_number; ++i)
            m_list[i] = ::rand();
        break;

    case Replicas::ENUMERATE:
        for (size_t i = 0; i < m_number; ++i, ++begin)
            m_list[i] = begin;
        break;

    default:
        break;
    }

    return m_list;
}

std::vector < guint32 > Replicas::getNewList() const
{
    std::vector < guint32 > result;

    result.resize(m_number);
    size_t begin = m_begin;

    switch (m_seed) {
    case Replicas::RANDOM:
        for (size_t i = 0; i < m_number; ++i)
            result[i] = ::rand();
        break;

    case Replicas::ENUMERATE:
        for (size_t i = 0; i < m_number; ++i, ++begin)
            result[i] = begin;
        break;

    default:
        std::copy(m_list.begin(), m_list.end(), result.begin());
        break;
    }

    return result;
}

}} // namespace vle vpz
