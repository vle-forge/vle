/** 
 * @file vpz/Views.cpp
 * @brief 
 * @author The vle Development Team
 * @date lun, 13 fév 2006 18:53:56 +0100
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

#include <vle/vpz/Views.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/XML.hpp>

namespace vle { namespace vpz {

using namespace vle::utils;

void Views::write(std::ostream& out) const
{
    out << "<views>\n";

    if (not m_outputs.empty()) {
        m_outputs.write(out);
    }

    if (not m_observables.empty()) {
        m_observables.write(out);
    }

    if (not empty()) {
        for (const_iterator it = begin(); it != end(); ++it) {
            it->second.write(out);
        }
    }

    out << "</views>\n";
}

Output& Views::addLocalStreamOutput(const std::string& name,
                                    const std::string& location,
                                    const std::string& plugin)
{
    return m_outputs.addLocalStream(name, location, plugin);
}

Output& Views::addDistantStreamOutput(const std::string& name,
                                      const std::string& location,
                                      const std::string& plugin)
{
    return m_outputs.addDistantStream(name, location, plugin);
}

void Views::delOutput(const std::string& name)
{
    m_outputs.del(name);
}

Observable& Views::addObservable(const Observable& obs)
{
    return m_observables.add(obs);
}

Observable& Views::addObservable(const std::string& name)
{
    return m_observables.add(name);
}

void Views::delObservable(const std::string& name)
{
    m_observables.del(name);
    std::map < std::string, View >::clear();
}

void Views::clear()
{
    std::map < std::string, View >::clear();
    m_outputs.clear();
    m_observables.clear(); 
}

void Views::add(const Views& views)
{
    for (const_iterator it = views.begin(); it != views.end(); ++it) {
        add(it->second);
    }

    m_outputs.add(views.outputs());
    m_observables.add(views.observables());
}

View& Views::add(const View& view)
{
    Assert(utils::SaxParserError, not exist(view.name()),
           (boost::format("View %1% already exist") % view.name()));

    return (*insert(std::make_pair < std::string, View >(
                view.name(), view)).first).second;
}

View& Views::addEventView(const std::string& name,
                          const std::string& output,
                          const std::string& library)
{
    Assert(utils::SaxParserError, not exist(name),
           (boost::format("View %1% already exist") % name));

    View m(name);
    m.setEventView(output, library);
    return add(m);
}

View& Views::addTimedView(const std::string& name,
                          double timestep,
                          const std::string& output,
                          const std::string& library)
{
    Assert(utils::SaxParserError, not exist(name),
           (boost::format("View %1% already exist") % name));

    View m(name);
    m.setTimedView(timestep, output, library);
    return add(m);
}

void Views::del(const std::string& name)
{
    erase(name);
}

const View& Views::get(const std::string& name) const
{
    const_iterator it = find(name);
    Assert(utils::SaxParserError, it != end(),
           boost::format("Unknow view '%1%'\n") % name);

    return it->second;
}

View& Views::get(const std::string& name)
{
    iterator it = find(name);
    Assert(utils::SaxParserError, it != end(),
           boost::format("Unknow view '%1%'\n") % name);

    return it->second;
}

}} // namespace vle vpz
