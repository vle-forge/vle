/*
 * @file examples/gens/Counter.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
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


#include <Counter.hpp>
#include <iomanip>

namespace vle { namespace examples { namespace gens {

Counter::Counter(const devs::DynamicsInit& model,
                 const devs::InitEventList& events) :
    devs::Dynamics(model, events),
    m_counter(0),
    m_active(false)
{
}

devs::Time Counter::init(const devs::Time& /* time */)
{
    m_counter = 0;
    return devs::Time::infinity;
}

void Counter::output(const devs::Time& /* time */,
                     devs::ExternalEventList& output) const
{
    output.addEvent(buildEvent("out"));
}

devs::Time Counter::timeAdvance() const
{
    if (m_active) {
        return devs::Time(0.0);
    }
    else {
        return devs::Time::infinity;
    }
}

void Counter::internalTransition(const devs::Time& /* event */)
{
    m_active = false;
}

void Counter::externalTransition(const devs::ExternalEventList& events,
                                 const devs::Time& /* time */)
{
    m_counter += events.size();
    m_active = true;
}

value::Value* Counter::observation(const devs::ObservationEvent&  ev) const
{
    if (ev.onPort("c")) {
        if (m_counter > 100 and m_counter < 500) {
            return 0;
        } else {
            return buildDouble(m_counter);
        }
    } else if (ev.onPort("value")) {
        return buildInteger(0);
    }
    return 0;
}

}}} // namespace vle examples gens
