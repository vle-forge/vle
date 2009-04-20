/**
 * @file vle/vpz/Observable.cpp
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
#include <vle/utils/Algo.hpp>

namespace vle { namespace vpz {

void ObservablePort::write(std::ostream& out) const
{
    if (m_list.empty()) {
        out << "<port name=\"" << m_name.c_str() << "\" />\n";
    } else {
        out << "<port name=\"" << m_name.c_str() << "\" >\n";
        for (const_iterator it = begin(); it != end(); ++it) {
            out << " <attachedview name=\"" << it->c_str() << "\" />\n";
        }
        out << "</port>\n";
    }
}

void ObservablePort::add(const std::string& portname)
{
    Assert < utils::ArgError >(not exist(portname),
           (boost::format("The port %1% have already a view %2%") %
            m_name % portname));

    m_list.push_back(portname);
}

void ObservablePort::del(const std::string& portname)
{
    m_list.erase(std::remove_if(begin(), end(),
                                std::bind2nd(std::equal_to < std::string >(),
                                             portname)), end());;
}

bool ObservablePort::exist(const std::string& portname) const
{
    return std::find_if(begin(), end(),
                        std::bind2nd(std::equal_to < std::string >(), portname))
        != end();
}

void Observable::write(std::ostream& out) const
{
    if (not m_list.empty()) {
        out << "<observable name=\"" << m_name.c_str() << "\" >\n";
        std::transform(begin(), end(),
                       std::ostream_iterator < ObservablePort >(out, "\n"),
                       utils::select2nd < ObservablePortList::value_type >());
        out << "</observable>\n";
    } else {
        out << "<observable name=\"" << m_name.c_str() << "\" />\n";
    }
}

ObservablePort& Observable::add(const std::string& portname)
{
    std::pair < iterator, bool > x;

    x = m_list.insert(std::make_pair < std::string, ObservablePort >(
            portname, ObservablePort(portname)));

    Assert < utils::ArgError >(x.second,
           (boost::format("The observable %1% have already a port %2%") %
            m_name, portname));

    return x.first->second;
}

ObservablePort& Observable::add(const ObservablePort& obs)
{
    std::pair < iterator, bool > x;

    x = m_list.insert(std::make_pair < std::string, ObservablePort >(
                obs.name(), obs));

    Assert < utils::ArgError >(x.second,
           (boost::format("The observable %1% have already a port %2%") %
            m_name, obs.name()));

    return x.first->second;
}

ObservablePort& Observable::get(const std::string& portname)
{
    iterator it = m_list.find(portname);
    Assert < utils::ArgError >(it != m_list.end(),
           (boost::format("The observable %1% have not port %2%") %
            m_name, portname));
    return it->second;
}

const ObservablePort& Observable::get(const std::string& portname) const
{
    const_iterator it = m_list.find(portname);
    Assert < utils::ArgError >(it != m_list.end(),
           (boost::format("The observable %1% have not port %2%") %
            m_name, portname));
    return it->second;
}

void Observable::del(const std::string& portname)
{
    iterator it = m_list.find(portname);
    if (it != end()) {
        m_list.erase(it);
    }
}

bool Observable::hasView(const std::string& name) const
{
    return utils::find_if(m_list.begin(), m_list.end(), HasView(name))
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
