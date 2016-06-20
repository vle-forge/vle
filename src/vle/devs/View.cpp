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
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/StreamWriter.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <cassert>

namespace vle { namespace devs {

void View::open(const std::string& name, std::unique_ptr<StreamWriter> stream)
{
    m_name = name;

    if (m_stream)
        m_stream->close(devs::infinity);

    m_stream = std::move(stream);
}

void View::addObservable(Dynamics* dynamics,
                         const std::string& portname,
                         const Time& currenttime)
{
    assert(dynamics);
    assert(not exist(dynamics, portname));

    m_observableList.emplace(dynamics, portname);

    if (m_stream)
        m_stream->processNewObservable(
            dynamics->getModel().getName(),
            dynamics->getModel().getParentName(),
            portname, currenttime,
            m_name);
}

void View::removeObservable(Dynamics* dynamics)
{
    assert(dynamics);

    auto result = m_observableList.equal_range(dynamics);

    for (auto it = result.first; it != result.second; ++it)
        m_stream->processRemoveObservable(it->first->getModel().getName(),
                                          it->first->getModel().getParentName(),
                                          it->second, 0.0,
                                          m_name);

    m_observableList.erase(result.first, result.second);
}

bool View::exist(Dynamics* dynamics, const std::string& portname) const
{
    auto result = m_observableList.equal_range(dynamics);
    
    for (auto it = result.first; it != result.second; ++it)
        if (it->second == portname)
            return true;

    return false;
}

bool View::exist(Dynamics* dynamics) const
{
    return m_observableList.find(dynamics) != m_observableList.end();
}

void View::run(const Time& time)
{
    if (not m_observableList.empty()) {
        for (auto & elem : m_observableList) {
            ObservationEvent event(time, m_name, elem.second);
            auto val = elem.first->observation(event);
            m_stream->process(elem.first->getModel().getName(),
                              elem.first->getModel().getParentName(),
                              elem.second, time, m_name,
                              std::move(val));
        }
    } else {
        //
        // Strange behavior.
        //
        m_stream->process(std::string(), std::string(),
                          std::string(), time, m_name, nullptr);
    }
}

void View::run(const Dynamics *dynamics, Time current, const std::string& port)
{
    ObservationEvent event(current, m_name, port);
    auto val = dynamics->observation(event);

    m_stream->process(dynamics->getModel().getName(),
                      dynamics->getModel().getParentName(),
                      port, current, m_name,
                      std::move(val));
}

std::unique_ptr<value::Matrix> View::matrix() const
{
    if (m_stream->plugin())
        return m_stream->plugin()->matrix();

    return std::unique_ptr<value::Matrix>();
}

}} // namespace vle devs
