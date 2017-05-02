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

#ifndef VLE_DEVS_OBSERVATION_EVENT_HPP
#define VLE_DEVS_OBSERVATION_EVENT_HPP 1

#include <vector>
#include <vle/DllDefines.hpp>
#include <vle/devs/Time.hpp>
#include <vle/utils/Exception.hpp>

namespace vle {
namespace devs {

class Simulator;

/**
 * @brief State event use to get information from graph::Model using
 * TimedView or EventView.
 *
 */
class VLE_API ObservationEvent
{
public:
    ObservationEvent() = delete;
    ObservationEvent(const ObservationEvent& other) = delete;
    ObservationEvent& operator=(const ObservationEvent& other) = delete;

    ObservationEvent(const Time& time,
                     const std::string& viewname,
                     const std::string& portName)
      : m_time(time)
      , m_viewName(viewname)
      , m_portName(portName)
    {
    }

    ~ObservationEvent() = default;

    const std::string& getViewName() const
    {
        return m_viewName;
    }

    const std::string& getPortName() const
    {
        return m_portName;
    }

    bool onPort(std::string const& portName) const
    {
        return m_portName == portName;
    }

    /**
     * @return arrived time.
     */
    const Time& getTime() const
    {
        return m_time;
    }

private:
    Time m_time;
    std::string m_viewName;
    std::string m_portName;
};
}
} // namespace vle devs

#endif
