/**
 * @file vle/utils/Host.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
 * Copyright (C) 2003-2009 ULCO http://www.univ-littoral.fr
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


#include <vle/utils/Host.hpp>
#include <vle/utils/Preferences.hpp>
#include <vle/utils/Debug.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>


namespace vle { namespace utils {

Host::Host(const std::string& hostname, const std::string& port,
           const std::string& process)
    : mHostname(hostname), mPort(0), mProcess(0)
{
    if (hostname.empty()) {
        throw utils::InternalError(_("Host error: empty host name"));
    }

    try {
        mPort = boost::lexical_cast < int >(port);
        mProcess = boost::lexical_cast < int >(process);
    } catch (const std::exception& e) {
        throw utils::InternalError(_("Host error: bad port or process"));
    }
}

Hosts::~Hosts()
{
    if (mIsModified) {
        write();
    }
}

void Hosts::read()
{
    std::string hosts, ports, processes;

    {
        Preferences prefs;
        prefs.load();
        hosts = prefs.getAttributes("vle.daemon", "hosts");
        ports = prefs.getAttributes("vle.daemon", "ports");
        processes = prefs.getAttributes("vle.daemon", "processes");
    }

    std::vector < std::string > hostsl, portsl, processesl;

    boost::split(hostsl, hosts, boost::is_any_of(","));
    boost::split(portsl, ports, boost::is_any_of(","));
    boost::split(processesl, processes, boost::is_any_of(","));

    if (hostsl.size() != portsl.size() or hostsl.size() != processes.size() or
        portsl.size() != processes.size()) {
        throw utils::InternalError(_("Bad format in vle.daemon section"));
    }

    const std::vector < std::string >::size_type end = mHosts.size();
    if (end) {
        for (std::vector < std::string >::size_type i = 0; i < end; ++i) {
            boost::trim(hostsl[i]);
            boost::trim(portsl[i]);
            boost::trim(processesl[i]);
            mHosts.insert(Host(hostsl[i], portsl[i], processesl[i]));
        }
    } else {
        mHosts.insert(Host("localhost", 8000, 1));
    }

    mIsModified = false;
}

void Hosts::write()
{
    std::string hosts, ports, processes;

    const_iterator it = begin();
    while (it != end()) {
        hosts += it->hostname();
        ports += boost::lexical_cast < std::string >(it->port());
        processes += boost::lexical_cast < std::string >(it->process());
        ++it;

        if (it != end()) {
            hosts += ',';
            ports += ',';
            processes += ',';
        }
    }

    Preferences prefs;
    prefs.load();

    prefs.setAttributes("vle.daemon", "host", hosts);
    prefs.setAttributes("vle.daemon", "ports", ports);
    prefs.setAttributes("vle.daemon", "processes", processes);

    prefs.save();

    mIsModified = false;
}

void Hosts::add(const Host& host)
{
    mIsModified = true;
    mHosts.insert(host);
}

void Hosts::del(const std::string& hostname)
{
    const_iterator it = mHosts.find(Host(hostname, 0, 0));
    if (it != mHosts.end()) {
        mIsModified = true;
        mHosts.erase(it);
    }
}

const Host& Hosts::get(const std::string& hostname) const
{
    const_iterator it = mHosts.find(Host(hostname, 0, 0));

    if (it == mHosts.end()) {
        throw utils::ArgError(fmt(_("Cannot get the host '%1%'")) % hostname);
    }

    return (*it);
}

}} // namespace vle utils
