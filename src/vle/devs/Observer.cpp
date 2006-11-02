/**
 * @file devs/Observer.cpp
 * @author The VLE Development Team.
 * @brief Represent an observer on a devs::sAtomicModel and a port name.
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
#include <vle/devs/sAtomicModel.hpp>
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

void Observer::addObservable(sAtomicModel* p_model,
                             const std::string & p_portName,
                             const std::string & p_group,
                             size_t p_index)
{
    Observable v_observable(p_model, p_portName, p_group, p_index);
    m_size++;
    m_observableList.push_back(v_observable);
}

const std::vector < Observable >& Observer::getObservableList() const
{
    return m_observableList;
}

StateEventList Observer::init()
{
    m_stream->writeHead(m_observableList);

    StateEventList v_eventList;
    std::vector < Observable >::iterator it = m_observableList.begin();

    while (it != m_observableList.end()) {
        v_eventList.push_back(
	    new StateEvent(Time(0), it->model, m_name, it->portName));
        ++it;
    }
    return v_eventList;
}

void Observer::finish()
{
    m_stream->writeData();
    m_stream->writeTail();
    m_stream->close();
}

const std::string & Observer::getFirstPortName() const
{
    return m_observableList.front().portName;
}

const std::string & Observer::getFirstGroup() const
{
    return m_observableList.front().group;
}

void Observer::removeObservable(sAtomicModel* model)
{
    std::vector < Observable >::iterator it;

    it = m_observableList.begin();
    while (it != m_observableList.end()) {
        if (it->model == model) {
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
    std::vector < Observable >::iterator it;

    it = m_observableList.begin();
    while (it != m_observableList.end()) {
        if ((*it).model->getStructure() == model) {
	    m_observableList.erase(it);
	    --m_size;
	    it = m_observableList.begin();
        } else {
            ++it;
        }
    }
}

}} // namespace vle devs
