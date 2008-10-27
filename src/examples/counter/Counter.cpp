/**
 * @file examples/counter/Counter.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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

namespace vle { namespace examples { namespace counter {

void Counter::output(const devs::Time& /* time */,
                     devs::ExternalEventList& output) const
{
    output.addEvent(buildEvent("out"));
}

devs::Time Counter::timeAdvance() const
{
    if (m_active) return devs::Time(m_delay);
    else return devs::Time::infinity;
}

devs::Time Counter::init(const devs::Time& /* time */)
{
    m_counter = 0;
    m_active = false;
    return devs::Time::infinity;
}

void Counter::internalTransition(const devs::Time& /* event */)
{
    m_active = false;
}

void Counter::externalTransition(const devs::ExternalEventList& /* event */,
                                 const devs::Time& /* time */)
{
    m_counter++;
    m_active = true;
}

value::Value* Counter::observation(const devs::ObservationEvent& event) const
{
    if (event.onPort("c")) return buildDouble(m_counter);
    return 0;
}

}}} // namespace vle examples counter
