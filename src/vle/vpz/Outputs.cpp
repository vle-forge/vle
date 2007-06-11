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
#include <vle/utils/Trace.hpp>

namespace vle { namespace vpz {

using namespace vle::utils;

void Outputs::write(std::ostream& out) const
{
    if (not m_outputs.empty()) {
        out << "<ouputs>";

        for (const_iterator it = m_outputs.begin(); it != m_outputs.end();
             ++it) {
            it->second.write(out);
        }

        out << "</outputs>";
    }

}

void Outputs::addTextStream(const std::string& name,
                            const std::string& location)
{
    Output o;
    o.setName(name);
    o.setTextStream(location);
    addOutput(o);
}

void Outputs::addSdmlStream(const std::string& name,
                            const std::string& location)
{
    Output o;
    o.setName(name);
    o.setSdmlStream(location);
    addOutput(o);
}

void Outputs::addEovStream(const std::string& name,
                           const std::string& plugin,
                           const std::string& location)
{
    Output o;
    o.setName(name);
    o.setEovStream(plugin, location);
    addOutput(o);
}

void Outputs::clear()
{
    m_outputs.clear();
}

void Outputs::delOutput(const std::string& name)
{
    iterator it = m_outputs.find(name);

    if (it != m_outputs.end()) {
        m_outputs.erase(it);
    }
}

void Outputs::addOutputs(const Outputs& o)
{
    for (OutputList::const_iterator it = o.outputs().begin();
         it != o.outputs().end(); ++it) {
        addOutput(it->second);
    }
}

void Outputs::addOutput(const Output& o)
{
    Assert(utils::InternalError, exist(o.name()) == false,
           boost::format("An output have already this name '%1%'\n") %
           o.name());

    m_outputs[o.name()] = o;
}

Output& Outputs::find(const std::string& name)
{
    OutputList::iterator it = m_outputs.find(name);
    Assert(utils::InternalError, it != m_outputs.end(),
           boost::format("Unknow output '%1%'\n") % name);

    return it->second;
}

const Output& Outputs::find(const std::string& name) const
{
    OutputList::const_iterator it = m_outputs.find(name);
    Assert(utils::InternalError, it != m_outputs.end(),
           boost::format("Unknow output '%1%'\n") % name);

    return it->second;
}

std::list < std::string > Outputs::outputsname() const
{
    std::list < std::string > result;

    OutputList::const_iterator it;
    for (it = m_outputs.begin(); it != m_outputs.end(); ++it) {
        result.push_back(it->second.name());
    }
    return result;
}

bool Outputs::exist(const std::string& name) const
{
    return m_outputs.find(name) != m_outputs.end();
}

}} // namespace vle vpz
