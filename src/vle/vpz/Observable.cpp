/** 
 * @file vpz/Observable.cpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 31 jan 2006 17:30:53 +0100
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

#include <vle/vpz/Observable.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {

using namespace vle::utils;

//
///
//// ObservablePort class.
///
//

ObservablePort::ObservablePort(const std::string& portname) :
    m_name(portname)
{
}

void ObservablePort::write(std::ostream& out) const
{
    if (empty()) {
        out << "<port name=\"" << m_name << "\" />\n";
    } else {
        out << "<port name=\"" << m_name << "\" >\n";
        for (const_iterator it = begin(); it != end(); ++it) {
            out << " <view name=\"" << *it << "\" />\n";
        }
        out << "</port>\n";
    }
}

void ObservablePort::add(const std::string& portname)
{
    Assert(utils::SaxParserError, not exist(portname),
           (boost::format("The port %1% have already a view %2%") %
            m_name % portname));

    push_back(portname);
}

void ObservablePort::del(const std::string& portname)
{
    for (iterator it = begin(); it != end(); ++it) {
        if ((*it) == portname) {
            erase(it);
            return;
        }
    }
}

bool ObservablePort::exist(const std::string& portname) const
{
    for (const_iterator it = begin(); it != end(); ++it) {
        if ((*it) == portname) {
            return true;
        }
    }
    return false;
}

//
///
//// Observable class.
///
//

Observable::Observable(const std::string& name) :
    m_name(name)
{
}

void Observable::write(std::ostream& out) const
{
    if (not empty()) {
        out << "<observable name=\"" << m_name << "\" >";
        for (const_iterator it = begin(); it != end(); ++it) {
            it->second.write(out);
        }
    } else {
        out << "<observable name=\"" << m_name << "\" />";
    }
}

ObservablePort& Observable::add(const std::string& portname)
{
    Assert(utils::SaxParserError, not exist(portname),
           (boost::format("The observable %1% have already a port %2%") %
            m_name, portname));

    return (*insert(std::make_pair < std::string, ObservablePort >(
                portname, ObservablePort(portname))).first).second;
}

ObservablePort& Observable::add(const ObservablePort& obs)
{
    Assert(utils::SaxParserError, not exist(obs.name()),
           (boost::format("The observable %1% have already a port %2%") %
            m_name, obs.name()));

    return (*insert(std::make_pair < std::string, ObservablePort >(
                obs.name(), obs)).first).second;
}

ObservablePort& Observable::get(const std::string& portname)
{
    iterator it = find(portname);
    Assert(utils::SaxParserError, it != end(),
           (boost::format("The observable %1% have not port %2%") %
            m_name, portname));
    return it->second;
}

const ObservablePort& Observable::get(const std::string& portname) const
{
    const_iterator it = find(portname);
    Assert(utils::SaxParserError, it != end(),
           (boost::format("The observable %1% have not port %2%") %
            m_name, portname));
    return it->second;
}

}} // namespace vle vpz
