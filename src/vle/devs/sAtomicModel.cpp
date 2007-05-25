/**
 * @file devs/sAtomicModel.cpp
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

#include <vle/devs/sAtomicModel.hpp>
#include <vle/devs/sCoupledModel.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/Time.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/CoupledModel.hpp>



namespace vle { namespace devs {

sAtomicModel::sAtomicModel(graph::AtomicModel* a, Simulator* simulator) :
    sModel(simulator),
    m_modelIndex(-1),
    m_lastTime(0),
    m_dynamics(0),
    m_atomicModel(a)
{
}

sAtomicModel::~sAtomicModel()
{
    delete m_dynamics;
}

void sAtomicModel::addDynamics(Dynamics* dynamics)
{
    if (m_dynamics) delete m_dynamics;
    m_dynamics = dynamics;
}

long sAtomicModel::getModelIndex() const
{
    return m_modelIndex;
}

const std::string& sAtomicModel::getName() const
{
    return m_atomicModel->getName();
}

Time sAtomicModel::getLastTime() const
{
    return m_lastTime;
}

graph::AtomicModel* sAtomicModel::getStructure() const
{
    return m_atomicModel;
}

bool sAtomicModel::isAtomic() const
{
    return true;
}

bool sAtomicModel::isCoupled() const
{
    return false;
}

Model * sAtomicModel::findModel(const std::string & name) const
{
    if (m_atomicModel->getName() == name)
	return (Model *)m_atomicModel;
    else
	return 0;
}

void sAtomicModel::setIndex(long index)
{
    m_modelIndex = index;
}

void sAtomicModel::setLastTime(const Time& time)
{
    m_lastTime = time;
}

InternalEvent * sAtomicModel::buildInternalEvent(const Time& currentTime)
{
    Time v_time = getTimeAdvance();

    if (!v_time.isInfinity()) {
	InternalEvent *v_event =
	    new InternalEvent(currentTime + v_time, this);
	return v_event;
    } else {
	return 0;
    }
}

void sAtomicModel::finish()
{
    m_dynamics->finish();
}

ExternalEventList* sAtomicModel::getOutputFunction(const Time& currentTime)
{
    return m_dynamics->getOutputFunction(currentTime);
}

Time sAtomicModel::getTimeAdvance()
{
    return m_dynamics->getTimeAdvance();
}

InternalEvent* sAtomicModel::init()
{
    return new InternalEvent(m_dynamics->init(), this);
}

bool sAtomicModel::parse(const std::string& data)
{
    return m_dynamics->parse(data);
}

Event::EventType sAtomicModel::processConflict(
                            const InternalEvent& internal,
                            const ExternalEventList& extEventlist,
                            const InstantaneousEventList& instEventlist)
{
    return m_dynamics->processConflict(internal, extEventlist, instEventlist);
}

bool sAtomicModel::processConflict(const InternalEvent& internal,
                                   const ExternalEventList& extEventlist)
{
    return m_dynamics->processConflict(internal, extEventlist);
}

bool sAtomicModel::processConflict(const InternalEvent& internal,
                                   const InstantaneousEventList& instEventlist)
{
    return m_dynamics->processConflict(internal, instEventlist);
}

bool sAtomicModel::processConflict(
                            const ExternalEventList& extEventlist,
                            const InstantaneousEventList& instEventlist)
{
    return m_dynamics->processConflict(extEventlist, instEventlist);
}

size_t sAtomicModel::processConflict(const ExternalEventList& eventlist)
{
    return m_dynamics->processConflict(eventlist);
}

void sAtomicModel::processInitEvent(InitEvent* event)
{
    m_dynamics->processInitEvent(event);
}

InternalEvent* sAtomicModel::processInternalEvent(InternalEvent * event)
{
    m_dynamics->processInternalEvent(event);
    return buildInternalEvent(event->getTime());
}

InternalEvent* sAtomicModel::processExternalEvent(ExternalEvent* event)
{
    m_dynamics->processExternalEvent(event);
    return buildInternalEvent(event->getTime());
}

ExternalEventList* sAtomicModel::processInstantaneousEvent(
                                            InstantaneousEvent* event)
{
    return m_dynamics->processInstantaneousEvent(event);
}

StateEvent* sAtomicModel::processStateEvent(StateEvent* event) const
{
    value::Value val = m_dynamics->processStateEvent(event);
    StateEvent* v_event = new StateEvent(*event);

    if (val) {
        v_event->putAttribute(event->getPortName(), val);
    }
    return v_event;
}

}} // namespace vle devs
