/**
 * @file devs/Observer.cpp
 * @author The VLE Development Team.
 * @brief Represent an observer on a devs::Simulator and a port name.
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

#include <vle/devs/Observer.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/devs/Stream.hpp>

namespace vle { namespace devs {

Observer::Observer(const std::string& name, Stream* p_stream) :
    m_name(name),
    m_stream(p_stream),
    m_size(0)
{
}

Observer::~Observer()
{
    delete m_stream;
}

StateEvent* Observer::addObservable(Simulator* model,
                                    const std::string& portname,
                                    const Time& currenttime)
{
    Assert(utils::InternalError, model, 
           "Cannot add an empty simulator to observe");

    if (not exist(model, portname)) {
        m_size++;
        m_observableList.push_back(Observable(model, portname));
        return new StateEvent(currenttime, model, m_name, portname);
    }
    return 0;
}

void Observer::finish()
{
    m_stream->writeData();
    m_stream->writeTail();
    m_stream->close();
}

const std::string & Observer::getFirstPortName() const
{
    return m_observableList.front().portname();
}

void Observer::removeObservable(Simulator* model)
{
    ObservableList::iterator it;

    it = m_observableList.begin();
    while (it != m_observableList.end()) {
        if (it->simulator() == model) {
	    m_observableList.erase(it);
	    --m_size;
	    it = m_observableList.begin();
        } else {
            ++it;
        }
    }
}

void Observer::removeObservable(graph::AtomicModel* model)
{
    ObservableList::iterator it;

    it = m_observableList.begin();
    while (it != m_observableList.end()) {
        if ((*it).simulator()->getStructure() == model) {
	    m_observableList.erase(it);
	    --m_size;
	    it = m_observableList.begin();
        } else {
            ++it;
        }
    }
}

bool Observer::exist(Simulator* simulator, const std::string& portname) const
{
    for (ObservableList::const_iterator it = m_observableList.begin();
         it != m_observableList.end(); ++it) {
        if (it->simulator() == simulator and it->portname() == portname) {
            return true;
        }
    }
    return false;
}

}} // namespace vle devs
