/**
 * @file devs/View.cpp
 * @author The VLE Development Team.
 * @brief Represent an View on a devs::Simulator and a port name.
 */

/*
 * Copyright (c) 2004, 2005 The VLE Development Team.
 *
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <vle/devs/View.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/devs/StreamWriter.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace devs {

View::View(const std::string& name, StreamWriter* stream) :
    m_name(name),
    m_stream(stream),
    m_size(0)
{
}

View::~View()
{
    delete m_stream;
}

ObservationEvent* View::addObservable(Simulator* model,
                                    const std::string& portname,
                                    const Time& currenttime)
{
    Assert(utils::InternalError, model, 
           "Cannot add an empty simulator to observe");

    if (not exist(model, portname)) {
        m_observableList.insert(
            std::make_pair < Simulator*, std::string >(model, portname));
        m_stream->processNewObservable(model, portname, currenttime,
                                       getName());
        if (isTimed()) {
            return new ObservationEvent(currenttime, model, m_name, portname);
        }
    }
    return 0;
}

void View::finish(const Time& time)
{
    m_stream->close(time);
}

void View::removeObservable(Simulator* model)
{
    ObservableList::iterator it = m_observableList.find(model);
    if (it != m_observableList.end()) {
        m_observableList.erase(model);
        m_stream->processRemoveObservable(model, "", 0.0, getName());
    }
}

bool View::exist(Simulator* simulator, const std::string& portname) const
{
    std::pair < ObservableList::const_iterator,
                ObservableList::const_iterator > result;

    result = m_observableList.equal_range(simulator);
    for (ObservableList::const_iterator jt = result.first;
         jt != result.second; ++jt) {
        if (jt->second == portname) {
            return true;
        }
    }
    return false;
}

bool View::exist(Simulator* simulator) const
{
    return m_observableList.count(simulator) > 0;
}

std::list < std::string > View::get(Simulator* simulator)
{
    std::pair < ObservableList::iterator,
                ObservableList::iterator > result;

    result = m_observableList.equal_range(simulator);

    std::list < std::string > toreturn;

    for (ObservableList::iterator it = result.first;
         it != result.second; ++it) {
        toreturn.push_back(it->second);
    }
    return toreturn;
}

oov::PluginPtr View::plugin() const
{
    return m_stream->plugin();
}

}} // namespace vle devs
