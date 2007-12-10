/**
 * @file devs/Simulator.cpp
 * @author The VLE Development Team.
 * @brief Represent a couple devs::AtomicModel and devs::Dynamic class to
 * represent the DEVS simulator.
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
           (boost::format("Simulator if not connected to an atomic model.")));
}

Simulator::~Simulator()
{
    delete m_dynamics;
}

void Simulator::addDynamics(Dynamics* dynamics)
{
    if (m_dynamics) delete m_dynamics;
    m_dynamics = dynamics;
}

const std::string& Simulator::getName() const
{
    return m_atomicModel->getName();
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
    if (m_atomicModel->getName() == name)
        return (graph::Model *)m_atomicModel;
    else
	return 0;
}

void Simulator::setLastTime(const Time& time)
{
    m_lastTime = time;
}

InternalEvent * Simulator::buildInternalEvent(const Time& currentTime)
{
    Time time(getTimeAdvance());

    if (not time.isInfinity()) {
        return new InternalEvent(currentTime + time, this);
    } else {
	return 0;
    }
}

void Simulator::finish()
{
    DTraceDebug(boost::format("                     %1% finish") %
                m_atomicModel->getName());

    m_dynamics->finish();
}

void Simulator::getOutputFunction(const Time& currentTime,
                                  ExternalEventList& output)
{
    DTraceDebug(boost::format("%1$20.10g %2% output") % currentTime %
                m_atomicModel->getName());

    m_dynamics->getOutputFunction(currentTime, output);
}

Time Simulator::getTimeAdvance()
{
    DTraceDebug(boost::format("                     %1% ta") %
                m_atomicModel->getName());

    return m_dynamics->getTimeAdvance();
}

InternalEvent* Simulator::init(const Time& time)
{
    DTraceDebug(boost::format("%1$20.10g %2% init") % time %
                m_atomicModel->getName());

    return new InternalEvent(m_dynamics->init() + time, this);
}

Event::EventType Simulator::processConflict(
    const InternalEvent& internal,
    const ExternalEventList& extEventlist) const
{
    DTraceDebug(boost::format("%1$20.10g %2% conf event: [%3%]") %
                internal.getTime() % m_atomicModel->getName() % extEventlist);

    return m_dynamics->processConflict(internal, extEventlist);
}

void Simulator::processInitEvents(const InitEventList& event)
{
    DTraceDebug(boost::format("%1$20.10g init event") %
                m_atomicModel->getName());

    m_dynamics->processInitEvents(event);
}

InternalEvent* Simulator::processInternalEvent(const InternalEvent& event)
{
    DTraceDebug(boost::format("%1$20.10g %2% int event") % event.getTime() %
                m_atomicModel->getName());

    m_dynamics->processInternalEvent(event);
    return buildInternalEvent(event.getTime());
}

InternalEvent* Simulator::processExternalEvents(
    const ExternalEventList& event,
    const Time& time)
{
    DTraceDebug(boost::format("%1$20.10g %2% ext event: %3%") % time %
                m_atomicModel->getName() % event);

    m_dynamics->processExternalEvents(event, time);
    return buildInternalEvent(time);
}

void Simulator::processInstantaneousEvent(
    const InstantaneousEvent& event,
    const Time& time,
    ExternalEventList& output)
{
    DTraceDebug(boost::format("%1$20.10g %2% inst event: [%3%]") % time %
                m_atomicModel->getName() % event);

    m_dynamics->processInstantaneousEvent(event, time, output);
}

StateEvent* Simulator::processStateEvent(const StateEvent& event) const
{
    DTraceDebug(boost::format("%1$20.10g %2% state event: [%3%]") %
                event.getTime() % m_atomicModel->getName() %
                event);

    value::Value val = m_dynamics->processStateEvent(event);

    if (val.get()) {
        StateEvent* nevent = new StateEvent(event);
        nevent->putAttribute(event.getPortName(), val);
        return nevent;
    } else {
        TraceDebug((boost::format(
                    "Simulator %1% return an empty value on port %2%") %
                m_dynamics->getModel().getName() % event.getPortName()).str());
    }
    return 0;
}

}} // namespace vle devs
