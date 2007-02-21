/** 
 * @file vpz/Output.cpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 31 jan 2006 17:29:47 +0100
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

#include <vle/vpz/Output.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {

using namespace vle::utils;

Output::Output()
{
    setTextStream();
}

Output::~Output()
{
}

void Output::init(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "OUTPUT");

    Glib::ustring format(xml::get_attribute(elt, "FORMAT"));

    if (format == "text") {
        setTextStream(xml::get_attribute(elt, "LOCATION"));
    } else if (format == "sdml") {
        setSdmlStream(xml::get_attribute(elt, "LOCATION"));
    } else if (format == "eov") {
        setEovStream(xml::get_attribute(elt, "PLUGIN"),
                     xml::get_attribute(elt, "LOCATION"));
    } else if (format == "net") {
        setNetStream(xml::get_attribute(elt, "PLUGIN"),
		     xml::get_attribute(elt, "LOCATION"));
    } else {
        Throw(utils::ParseError, "Unknow output format");
    }

    if (xml::has_children(elt)) {
        m_xml.assign(xml::write_to_string(elt));
    }
}

void Output::write(xmlpp::Element* elt) const
{
    AssertI(elt->get_name() == "OUTPUT");
    
    elt->set_attribute("LOCATION", m_location);

    switch (m_format) {
    case Output::TEXT:
        elt->set_attribute("FORMAT", "text");
        break;
    case Output::SDML:
        elt->set_attribute("FORMAT", "sdml");
        break;
    case Output::EOV:
        elt->set_attribute("FORMAT", "eov");
        elt->set_attribute("PLUGIN", m_plugin);
        break;
    case Output::NET:
        elt->set_attribute("FORMAT", "net");
        elt->set_attribute("PLUGIN", m_plugin);
        break;
    }

    if (not m_xml.empty()) {
        xml::import_children_nodes_without_parent(elt, m_xml);
    }
}

void Output::setTextStream(const std::string& location)
{
    m_location.assign(location);
    m_format = Output::TEXT;
}

void Output::setSdmlStream(const std::string& location)
{
    m_location.assign(location);
    m_format = Output::SDML;
}

void Output::setEovStream(const std::string& plugin,
                          const std::string& location)
{
    AssertI(not plugin.empty());

    m_format = Output::EOV;
    m_location = (location.empty()) ? "localhost:8000" : location;
    m_plugin.assign(plugin);
}

void Output::setNetStream(const std::string& plugin,
                          const std::string& location)
{
    AssertI(not plugin.empty());

    m_format = Output::NET;
    m_location = (location.empty()) ? "localhost:8000" : location;
    m_plugin.assign(plugin);
}

void Output::setXML(const std::string& xml)
{
    m_xml.assign(xml);
}

}} // namespace vle vpz
