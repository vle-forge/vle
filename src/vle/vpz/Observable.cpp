/**
 * @file src/vle/vpz/Observable.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */




#include <vle/vpz/Observable.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {

////
//// ObservablePort class.
////

ObservablePort::ObservablePort(const std::string& portname) :
    m_name(portname)
{
}

void ObservablePort::write(std::ostream& out) const
{
    if (m_list.empty()) {
        out << "<port name=\"" << m_name << "\" />\n";
    } else {
        out << "<port name=\"" << m_name << "\" >\n";
        for (ViewNameList::const_iterator it = m_list.begin(); it !=
             m_list.end(); ++it) {
            out << " <attachedview name=\"" << *it << "\" />\n";
        }
        out << "</port>\n";
    }
}

void ObservablePort::add(const std::string& portname)
{
    Assert(utils::SaxParserError, not exist(portname),
           (boost::format("The port %1% have already a view %2%") %
            m_name % portname));

    m_list.push_back(portname);
}

void ObservablePort::del(const std::string& portname)
{
    m_list.erase(std::remove_if(m_list.begin(), m_list.end(),
                                std::bind2nd(std::equal_to < std::string >(),
                                             portname)), m_list.end());;
}

bool ObservablePort::exist(const std::string& portname) const
{
    return std::find_if(m_list.begin(), m_list.end(), 
                     std::bind2nd(std::equal_to < std::string >(), portname))
        != m_list.end();
}

////
//// Observable class.
////

Observable::Observable(const std::string& name) :
    m_name(name)
{
}

void Observable::write(std::ostream& out) const
{
    if (not m_list.empty()) {
        out << "<observable name=\"" << m_name << "\" >\n";
        for (ObservablePortList::const_iterator it = m_list.begin(); it !=
             m_list.end(); ++it) {
            it->second.write(out);
        }
        out << "</observable>\n";
    } else {
        out << "<observable name=\"" << m_name << "\" />\n";
    }
}

ObservablePort& Observable::add(const std::string& portname)
{
    Assert(utils::SaxParserError, not exist(portname),
           (boost::format("The observable %1% have already a port %2%") %
            m_name, portname));

    return (*m_list.insert(std::make_pair < std::string, ObservablePort >(
                portname, ObservablePort(portname))).first).second;
}

ObservablePort& Observable::add(const ObservablePort& obs)
{
    Assert(utils::SaxParserError, not exist(obs.name()),
           (boost::format("The observable %1% have already a port %2%") %
            m_name, obs.name()));

    return (*m_list.insert(std::make_pair < std::string, ObservablePort >(
                obs.name(), obs)).first).second;
}

ObservablePort& Observable::get(const std::string& portname)
{
    ObservablePortList::iterator it = m_list.find(portname);
    Assert(utils::SaxParserError, it != m_list.end(),
           (boost::format("The observable %1% have not port %2%") %
            m_name, portname));
    return it->second;
}

const ObservablePort& Observable::get(const std::string& portname) const
{
    ObservablePortList::const_iterator it = m_list.find(portname);
    Assert(utils::SaxParserError, it != m_list.end(),
           (boost::format("The observable %1% have not port %2%") %
            m_name, portname));
    return it->second;
}

bool Observable::hasView(const std::string& name) const
{
    return std::find_if(m_list.begin(), m_list.end(), HasView(name))
        != m_list.end();
}

PortNameList Observable::getPortname(const std::string& name) const
{
    std::list < std::string > result;

    std::for_each(m_list.begin(), m_list.end(), 
                  AddAttachedViewPortname(result, name));

    return result;
}

}} // namespace vle vpz
