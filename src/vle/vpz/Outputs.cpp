/** 
 * @file vpz/Outputs.cpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 31 jan 2006 17:30:32 +0100
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

#include <vle/vpz/Outputs.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {

using namespace vle::utils;

void Outputs::init(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "OUTPUTS");

    m_outputs.clear();

    xmlpp::Node::NodeList lst = elt->get_children("OUTPUT");
    xmlpp::Node::NodeList::iterator it = lst.begin();
    while (it != lst.end()) {
        std::string name(xml::get_attribute((xmlpp::Element*)(*it), "NAME"));
        Output o;
        o.init((xmlpp::Element*)(*it));
        addOutput(name, o);
        ++it;
    }
}

void Outputs::write(xmlpp::Element* elt) const
{
    AssertI(elt);

    if (not m_outputs.empty()) {
        xmlpp::Element* outputs = elt->add_child("OUTPUTS");

        std::map < std::string, Output >::const_iterator it;
        for (it = m_outputs.begin(); it != m_outputs.end(); ++it) {
            xmlpp::Element* output = outputs->add_child("OUTPUT");
            output->set_attribute("NAME", (*it).first);
            (*it).second.write(output);
        }
    }
}

void Outputs::addTextStream(const std::string& name,
                            const std::string& location)
{
    Output o;
    o.setTextStream(location);
    addOutput(name, o);
}

void Outputs::addSdmlStream(const std::string& name,
                            const std::string& location)
{
    Output o;
    o.setSdmlStream(location);
    addOutput(name, o);
}

void Outputs::addEovStream(const std::string& name,
                           const std::string& plugin,
                           const std::string& location)
{
    Output o;
    o.setEovStream(plugin, location);
    addOutput(name, o);
}

void Outputs::clear()
{
    m_outputs.clear();
}

void Outputs::delOutput(const std::string& name)
{
    std::map < std::string, Output >::iterator it = m_outputs.find(name);
    if (it != m_outputs.end()) {
        m_outputs.erase(it);
    }
}

void Outputs::addOutputs(const Outputs& o)
{
    std::map < std::string, Output >::const_iterator it;
    for (it = o.outputs().begin(); it != o.outputs().end(); ++it) {
        addOutput((*it).first, (*it).second);
    }
}

void Outputs::addOutput(const std::string& name, const Output& o)
{
    std::map < std::string, Output >::iterator it = m_outputs.find(name);
    Assert(utils::InternalError, it == m_outputs.end(),
           boost::format("An output have already this name '%1%'\n") %
           name);

    m_outputs[name] = o;
}

Output& Outputs::find(const std::string& name)
{
    std::map < std::string, Output >::iterator it = m_outputs.find(name);
    Assert(utils::InternalError, it != m_outputs.end(),
           boost::format("Unknow output '%1%'\n") % name);

    return (*it).second;
}

const Output& Outputs::find(const std::string& name) const
{
    std::map < std::string, Output >::const_iterator it = m_outputs.find(name);
    Assert(utils::InternalError, it != m_outputs.end(),
           boost::format("Unknow output '%1%'\n") % name);

    return (*it).second;
}

std::list < std::string > Outputs::outputsname() const
{
    std::list < std::string > result;
    std::map < std::string, Output >::const_iterator it;
    for (it = m_outputs.begin(); it != m_outputs.end(); ++it) {
        result.push_back((*it).first);
    }
    return result;
}

bool Outputs::exist(const std::string& name) const
{
    std::map < std::string, Output >::const_iterator it = m_outputs.find(name);
    return it != m_outputs.end();
}

}} // namespace vle vpz
