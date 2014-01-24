/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
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


#include <vle/devs/View.hpp>
#include <vle/devs/Simulator.hpp>

namespace vle { namespace devs {

View::~View()
{
    delete m_stream;
}

void View::addObservable(Simulator* model,
                         const std::string& portname,
                         const Time& currenttime)
{
    assert(model);

    if (not exist(model, portname)) {
        m_observableList.insert(value_type(model, portname));
        m_stream->processNewObservable(model, portname, currenttime,
                                       getName());
    }
}

void View::finish(const Time& time)
{
    m_stream->close(time);
}

void View::removeObservable(Simulator* sim)
{
    assert(sim);

    std::pair < ObservableList::iterator, ObservableList::iterator > result;
    ObservableList::iterator it;

    result = m_observableList.equal_range(sim);
    for (it = result.first; it != result.second; ++it) {
        m_stream->processRemoveObservable(it->first, it->second, 0.0,
                                          getName());
    }

    m_observableList.erase(result.first, result.second);
}

bool View::exist(Simulator* simulator, const std::string& portname) const
{
    std::pair < ObservableList::const_iterator,
                ObservableList::const_iterator > result;
    ObservableList::const_iterator it;

    result = m_observableList.equal_range(simulator);
    for (it = result.first; it != result.second; ++it) {
        if (it->second == portname) {
            return true;
        }
    }
    return false;
}

bool View::exist(Simulator* simulator) const
{
    return m_observableList.count(simulator) > 0;
}

void View::run(const Time& time)
{
    if (not m_observableList.empty()) {
        for (ObservableList::iterator it = m_observableList.begin();
             it != m_observableList.end(); ++it) {
            ObservationEvent event(time, it->first, getName(), it->second);
            value::Value* val = it->first->observation(event);
            m_stream->process(it->first, it->second, time, getName(), val);
        }
    } else {
        m_stream->process(0, std::string(), time, getName(), 0);
    }
}

value::Matrix * View::matrix() const
{
    if (m_stream->plugin()) {
        return m_stream->plugin()->matrix();
    }

    return NULL;
}

}} // namespace vle devs
