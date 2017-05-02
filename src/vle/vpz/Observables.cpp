/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#include <algorithm>
#include <iterator>
#include <vector>
#include <vle/utils/Algo.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/vpz/Observables.hpp>

namespace vle {
namespace vpz {

std::set<std::string>
Observables::getKeys()
{
    std::vector<std::string> observableKeys;

    observableKeys.resize(m_list.size());

    std::transform(
      m_list.begin(), m_list.end(), observableKeys.begin(), observableKey);

    std::set<std::string> observableKeysSet(observableKeys.begin(),
                                            observableKeys.end());

    return observableKeysSet;
}

void
Observables::write(std::ostream& out) const
{
    if (not empty()) {
        out << "<observables>\n";
        std::transform(begin(),
                       end(),
                       std::ostream_iterator<Observable>(out, "\n"),
                       utils::select2nd<ObservableList::value_type>());
        out << "</observables>\n";
    }
}

void
Observables::add(const Observables& obs)
{
    std::for_each(obs.begin(), obs.end(), AddObservable(*this));
}

Observable&
Observables::add(const Observable& obs)
{
    std::pair<iterator, bool> x;

    x = m_list.insert(value_type(obs.name(), obs));

    if (not x.second) {
        throw utils::ArgError(
          (fmt(_("Observable %1% already exist")) % obs.name()).str());
    }

    return x.first->second;
}

Observable&
Observables::get(const std::string& name)
{
    auto it = m_list.find(name);

    if (it == m_list.end()) {
        throw utils::ArgError(
          (fmt(_("Observable %1% does not exist")) % name).str());
    }

    return it->second;
}

const Observable&
Observables::get(const std::string& name) const
{
    auto it = m_list.find(name);

    if (it == m_list.end()) {
        throw utils::ArgError(
          (fmt(_("Observable %1% doest not exist")) % name).str());
    }

    return it->second;
}

void
Observables::rename(const std::string& old_name, const std::string& new_name)
{
    Observable& obs = get(old_name);
    Observable new_obs(new_name);
    const ObservablePortList& port_list = obs.observableportlist();
    auto it_port = port_list.begin();
    while (it_port != port_list.end()) {
        new_obs.add(it_port->second);
        ++it_port;
    }
    del(old_name);
    add(new_obs);
}

void
Observables::cleanNoPermanent()
{
    auto it = m_list.begin();

    while ((it = utils::findIf(it, end(), Observable::IsPermanent())) !=
           end()) {
        auto del = it++;
        m_list.erase(del);
    }
}

void
Observables::updateView(const std::string& oldname, const std::string& newname)
{
    for (auto& elem : m_list) {
        if (elem.second.hasView(oldname)) {
            vpz::Observable& obs = elem.second;
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
}
} // namespace vle vpz
