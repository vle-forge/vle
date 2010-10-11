/*
 * @file vle/vpz/Observables.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#include <vle/vpz/Observables.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Algo.hpp>
#include <iterator>
#include <algorithm>

namespace vle { namespace vpz {

void Observables::write(std::ostream& out) const
{
    if (not empty()) {
        out << "<observables>\n";
        std::transform(begin(), end(),
                       std::ostream_iterator < Observable >(out, "\n"),
                       utils::select2nd < ObservableList::value_type >());
        out << "</observables>\n";
    }
}

void Observables::add(const Observables& obs)
{
    std::for_each(obs.begin(), obs.end(), AddObservable(*this));
}

Observable& Observables::add(const Observable& obs)
{
    std::pair < iterator, bool > x;

    x = m_list.insert(std::make_pair < std::string, Observable >(
            obs.name(), obs));

    if (not x.second) {
        throw utils::ArgError(fmt(
                _("Observable %1% already exist")) % obs.name());
    }

    return x.first->second;
}

Observable& Observables::get(const std::string& name)
{
    iterator it = m_list.find(name);

    if (it == m_list.end()) {
        throw utils::ArgError(fmt(_("Observable %1% does not exist")) % name);
    }

    return it->second;
}

const Observable& Observables::get(const std::string& name) const
{
    const_iterator it = m_list.find(name);

    if (it == m_list.end()) {
        throw utils::ArgError(fmt(
                _("Observable %1% doest not exist")) % name);
    }

    return it->second;
}

void Observables::rename(const std::string& old_name,
			 const std::string& new_name)
{
    Observable& obs = get(old_name);
    Observable new_obs(new_name);
    const ObservablePortList& port_list = obs.observableportlist();
    ObservablePortList::const_iterator it_port = port_list.begin();
    while (it_port != port_list.end()) {
        new_obs.add(it_port->second);
        ++it_port;
    }
    del(old_name);
    add(new_obs);
}

void Observables::cleanNoPermanent()
{
    iterator it = m_list.begin();

    while ((it = utils::find_if(it, end(),
                                Observable::IsPermanent())) != end()) {
        iterator del = it++;
        m_list.erase(del);
    }
}

void Observables::updateView(const std::string& oldname,
                             const std::string& newname)
{
    for (iterator itobs = m_list.begin(); itobs != m_list.end(); ++itobs) {
	if (itobs->second.hasView(oldname)) {
	    vpz::Observable& obs = itobs->second;
	    ObservablePortList::iterator itport;
	    for (itport = obs.begin(); itport != obs.end(); ++itport) {
		vpz::ObservablePort& obsport = itport->second;
		ViewNameList::iterator itfind;
		itfind = std::find(obsport.begin(), obsport.end(), oldname);
		if (itfind != obsport.end()) {
		    obsport.del(oldname);
		    obsport.add(newname);
		}
	    }
	}
    }
}

}} // namespace vle vpz
