/**
 * @file examples/random/GeneratorWrapper.cpp
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


#include <GeneratorWrapper.hpp>
#include <vle/value.hpp>
#include <vle/utils.hpp>

#include <LogNormalGenerator.hpp>
#include <NormalGenerator.hpp>
#include <RegularGenerator.hpp>
#include <UniformGenerator.hpp>

namespace vle { namespace examples { namespace generator {

GeneratorWrapper::GeneratorWrapper(const graph::AtomicModel& model,
                                   const devs::InitEventList& events) :
    devs::Dynamics(model, events),
    m_events(events),
    m_generator(0),
    m_timeStep(0.0),
    m_time(0.0)
{
}

GeneratorWrapper::~GeneratorWrapper()
{
    delete m_generator;
}

devs::Time GeneratorWrapper::init(const vle::devs::Time& /* time */)
{
    std::string formalism(value::toString(m_events.get("formalism")));

    if (formalism == "regular") {
        m_generator = new RegularGenerator(rand(),
            value::toDouble(m_events.get("timestep")));
    } else if (formalism == "uniform") {
        m_generator = new UniformGenerator(rand(),
            value::toDouble(m_events.get("min")),
            value::toDouble(m_events.get("max")));
    } else if (formalism == "normal") {
        m_generator = new NormalGenerator(rand(),
            value::toDouble(m_events.get("average")),
            value::toDouble(m_events.get("stdvariation")));
    } else if (formalism == "lognormal") {
        m_generator = new LogNormalGenerator(rand(),
            value::toDouble(m_events.get("average")),
            value::toDouble(m_events.get("stdvariation")));
    } else {
        throw utils::InternalError(fmt(_("Unknow generator %1%")) %
              formalism);
    }

    m_generator->init();
    m_timeStep = 0;
    return devs::Time(0);
}

void GeneratorWrapper::output(const devs::Time& /* time */,
                              devs::ExternalEventList& output) const
{
    output.addEvent(new devs::ExternalEvent("out"));
}

devs::Time GeneratorWrapper::timeAdvance() const
{
    return devs::Time(m_timeStep);
}

void GeneratorWrapper::internalTransition(const devs::Time& /* event */)
{
    m_time += m_timeStep;
    m_timeStep = m_generator->generate();
}

}}} // namespace vle examples generator
