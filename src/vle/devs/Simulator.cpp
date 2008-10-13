/**
 * @file vle/devs/Simulator.cpp
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


#include <vle/devs/Simulator.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/Time.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Trace.hpp>


namespace vle { namespace devs {

Simulator::Simulator(graph::AtomicModel* atomic) :
    m_lastTime(0),
    m_dynamics(0),
    m_atomicModel(atomic)
{
    Assert(utils::InternalError, atomic,
           "Simulator if not connected to an atomic model.");
}

Simulator::~Simulator()
{
    delete m_dynamics;
}

void Simulator::clear()
{
    delete m_dynamics;
    m_dynamics = 0;
    m_atomicModel = 0;
}

void Simulator::addDynamics(Dynamics* dynamics)
{
    if (m_dynamics) {
        delete m_dynamics;
    }
    m_dynamics = dynamics;
}

const std::string& Simulator::getName() const
{
    Assert(utils::InternalError, m_atomicModel, "Simulator destroyed");

    return m_atomicModel->getName();
}

const std::string& Simulator::getParent()
{
    Assert(utils::InternalError, m_atomicModel, "Simulator destroyed");

    if (m_parents.empty()) {
        m_parents.assign(m_atomicModel->getParentName());
    }

    return m_parents;
}

Time Simulator::getLastTime() const
{
    return m_lastTime;
}

graph::AtomicModel* Simulator::getStructure() const
{
    return m_atomicModel;
}

graph::Model * Simulator::findModel(const std::string & name) const
{
    if (m_atomicModel and m_atomicModel->getName() == name) {
        return (graph::Model *)m_atomicModel;
    } else {
        return 0;
    }
}

void Simulator::setLastTime(const Time& time)
{
    m_lastTime = time;
}

InternalEvent * Simulator::buildInternalEvent(const Time& currentTime)
{
    Time time(timeAdvance());

    if (not time.isInfinity()) {
        return new InternalEvent(currentTime + time, this);
    } else {
	return 0;
    }
}

void Simulator::finish()
{
    DTraceDebug(boost::format("                     %1% finish") %
                getName());

    m_dynamics->finish();
}

void Simulator::output(const Time& currentTime, ExternalEventList& output)
{
    DTraceDebug(boost::format("%1$20.10g %2% output") % currentTime %
                getName());

    m_dynamics->output(currentTime, output);
}

Time Simulator::timeAdvance()
{
    DTraceDebug(boost::format("                     %1% ta") %
                getName());

    return m_dynamics->timeAdvance();
}

InternalEvent* Simulator::init(const Time& time)
{
    DTraceDebug(boost::format("%1$20.10g %2% init") % time %
                getName());

    return new InternalEvent(m_dynamics->init(time) + time, this);
}

Event::EventType Simulator::confluentTransitions(
    const InternalEvent& internal,
    const ExternalEventList& extEventlist) const
{
    DTraceDebug(boost::format("%1$20.10g %2% confluent transition: [%3%]") %
                internal.getTime() % getName() % extEventlist);

    return m_dynamics->confluentTransitions(internal.getTime(), extEventlist);
}

InternalEvent* Simulator::internalTransition(const InternalEvent& event)
{
    DTraceDebug(boost::format("%1$20.10g %2% internal transition") %
                event.getTime() % getName());

    m_dynamics->internalTransition(event.getTime());
    return buildInternalEvent(event.getTime());
}

InternalEvent* Simulator::externalTransition(
    const ExternalEventList& event,
    const Time& time)
{
    DTraceDebug(boost::format("%1$20.10g %2% external transition: [%3%]") % time
                % getName() % event);

    m_dynamics->externalTransition(event, time);
    return buildInternalEvent(time);
}

void Simulator::request(const RequestEvent& event, const Time& time,
                        ExternalEventList& output)
{
    DTraceDebug(boost::format("%1$20.10g %2% request: [%3%]") % time %
                getName() % event);

    m_dynamics->request(event, time, output);
}

ObservationEvent* Simulator::observation(const ObservationEvent& event) const
{
    DTraceDebug(boost::format("%1$20.10g %2% observation: [%3%]") %
                event.getTime() % getName() % event);

    value::Value val;
    ObservationEvent* nevent = 0;

    if (m_atomicModel) {
        val = m_dynamics->observation(event);
        nevent = new ObservationEvent(event);
        nevent->putAttribute(event.getPortName(), val);
    }

    if (not val.get()) {
        TraceDebug((boost::format(
                    "Simulator %1% return an empty value on event %2%") %
                getName() % event));
    }

    return nevent;
}

}} // namespace vle devs
