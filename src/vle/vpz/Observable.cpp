/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2003-2018 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2018 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2018 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <vle/utils/Algo.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/vpz/Observable.hpp>

#include "utils/i18n.hpp"
#include <algorithm>
#include <iterator>

namespace vle {
namespace vpz {

void
ObservablePort::write(std::ostream& out) const
{
    if (m_list.empty()) {
        out << "<port name=\"" << m_name.c_str() << "\" />\n";
    } else {
        out << "<port name=\"" << m_name.c_str() << "\" >\n";
        for (const auto& elem : *this) {
            out << " <attachedview name=\"" << elem.c_str() << "\" />\n";
        }
        out << "</port>\n";
    }
}

void
ObservablePort::add(const std::string& portname)
{
    if (exist(portname)) {
        throw utils::ArgError(_("The port %s have already a view %s"),
                              m_name.c_str(),
                              portname.c_str());
    }

    m_list.push_back(portname);
}

void
ObservablePort::del(const std::string& portname)
{
    m_list.erase(
      std::remove_if(
        begin(), end(), std::bind2nd(std::equal_to<std::string>(), portname)),
      end());
    ;
}

bool
ObservablePort::exist(const std::string& portname) const
{
    return std::find_if(
             begin(),
             end(),
             std::bind2nd(std::equal_to<std::string>(), portname)) != end();
}

void
Observable::write(std::ostream& out) const
{
    if (not m_list.empty()) {
        out << "<observable name=\"" << m_name.c_str() << "\" >\n";
        std::transform(begin(),
                       end(),
                       std::ostream_iterator<ObservablePort>(out, "\n"),
                       utils::select2nd<ObservablePortList::value_type>());
        out << "</observable>\n";
    } else {
        out << "<observable name=\"" << m_name.c_str() << "\" />\n";
    }
}

ObservablePort&
Observable::add(const std::string& portname)
{
    std::pair<iterator, bool> x;

    x = m_list.insert(value_type(portname, ObservablePort(portname)));

    if (not x.second) {
        throw utils::ArgError(_("The observable %s have already a port %s"),
                              m_name.c_str(),
                              portname.c_str());
    }

    return x.first->second;
}

ObservablePort&
Observable::add(const ObservablePort& obs)
{
    std::pair<iterator, bool> x;

    x = m_list.insert(value_type(obs.name(), obs));

    if (not x.second) {
        throw utils::ArgError(_("The observable %s have already a port %s"),
                              m_name.c_str(),
                              obs.name().c_str());
    }

    return x.first->second;
}

ObservablePort&
Observable::get(const std::string& portname)
{
    auto it = m_list.find(portname);

    if (it == m_list.end()) {
        throw utils::ArgError(_("The observable %s have not port %s"),
                              m_name.c_str(),
                              portname.c_str());
    }

    return it->second;
}

const ObservablePort&
Observable::get(const std::string& portname) const
{
    auto it = m_list.find(portname);

    if (it == m_list.end()) {
        throw utils::ArgError(_("The observable %s have not port %s"),
                              m_name.c_str(),
                              portname.c_str());
    }

    return it->second;
}

void
Observable::del(const std::string& portname)
{
    auto it = m_list.find(portname);
    if (it != end()) {
        m_list.erase(it);
    }
}

bool
Observable::hasView(const std::string& name) const
{
    return utils::findIf(m_list.begin(), m_list.end(), HasView(name)) !=
           m_list.end();
}

PortNameList
Observable::getPortname(const std::string& name) const
{
    std::list<std::string> result;

    std::for_each(
      m_list.begin(), m_list.end(), AddAttachedViewPortname(result, name));

    return result;
}
}
} // namespace vle vpz
