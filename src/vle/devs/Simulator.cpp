/**
 * @file vle/devs/Simulator.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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
#include <vle/value/Null.hpp>

namespace vle { namespace devs {

Simulator::Simulator(graph::AtomicModel* atomic) :
    m_dynamics(0),
    m_atomicModel(atomic)
{
    if (not atomic) {
        throw utils::InternalError(_(
            "Simulator is not connected to an atomic model."));
    }
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

void
Simulator::updateSimulatorTargets(
        const std::string& port,
        std::map < graph::AtomicModel*, devs::Simulator* >& simulators)
{
    mTargets.erase(port);

    graph::ModelPortList result;
    m_atomicModel->getAtomicModelsTarget(port, result);

    if (result.begin() == result.end()) {
        mTargets.insert(
                std::make_pair(port, TargetSimulator(0, std::string())));
    } else {
        for (graph::ModelPortList::iterator it = result.begin(); it !=
                result.end(); ++it) {

            std::map < graph::AtomicModel*, devs::Simulator* >::iterator target;
            target = simulators.find(
                    reinterpret_cast < graph::AtomicModel*>(it->first));

            if (target == simulators.end()) {
                mTargets.erase(port);
                break;
            } else {
                mTargets.insert(std::make_pair(port, TargetSimulator(
                            target->second, it->second)));
            }
        }
    }
}

std::pair < Simulator::iterator, Simulator::iterator >
Simulator::targets(
    const std::string& port,
    std::map < graph::AtomicModel*, devs::Simulator* >& simulators)
{
    std::pair < iterator, iterator > x = mTargets.equal_range(port);

    if (x.first == x.second) {
        updateSimulatorTargets(port, simulators);
        x = mTargets.equal_range(port);
    } else if (x.first->second.first == 0) {
        x = make_pair(mTargets.end(), mTargets.end());
    }

    return x;
}

void Simulator::removeTargetPort(const std::string& port)
{
    iterator it = mTargets.find(port);

    assert(it != mTargets.end());

    mTargets.erase(it);
}

void Simulator::addTargetPort(const std::string& port)
{
    assert(mTargets.find(port) == mTargets.end());

    mTargets.insert(std::make_pair(port, TargetSimulator(0, std::string())));
}

void Simulator::addDynamics(Dynamics* dynamics)
{
    delete m_dynamics;
    m_dynamics = dynamics;
}

const std::string& Simulator::getName() const
{
    if (not m_atomicModel) {
        throw utils::InternalError(_("Simulator destroyed"));
    }

    return m_atomicModel->getName();
}

const std::string& Simulator::getParent()
{
    if (not m_atomicModel) {
        throw utils::InternalError(_("Simulator destroyed"));
    }

    if (m_parents.empty()) {
        m_parents.assign(m_atomicModel->getParentName());
    }

    return m_parents;
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
    m_dynamics->finish();
}

void Simulator::output(const Time& currentTime, ExternalEventList& output)
{
    m_dynamics->output(currentTime, output);
}

Time Simulator::timeAdvance()
{
    Time result = m_dynamics->timeAdvance();
    if (result < 0.0) {
        throw utils::ModellingError(fmt(
                _("Negative time advance in '%1%' (%2%)")) % getName() %
            result);
    }
    return result;
}

InternalEvent* Simulator::init(const Time& time)
{
    return new InternalEvent(m_dynamics->init(time) + time, this);
}

Event::EventType Simulator::confluentTransitions(
    const InternalEvent& internal,
    const ExternalEventList& extEventlist) const
{
    return m_dynamics->confluentTransitions(internal.getTime(), extEventlist);
}

InternalEvent* Simulator::internalTransition(const InternalEvent& event)
{
    m_dynamics->internalTransition(event.getTime());
    return buildInternalEvent(event.getTime());
}

InternalEvent* Simulator::externalTransition(
    const ExternalEventList& event,
    const Time& time)
{
    m_dynamics->externalTransition(event, time);
    return buildInternalEvent(time);
}

InternalEvent* Simulator::internalTransitionConflict(
    const InternalEvent& event,
    const ExternalEventList& events)
{
    m_dynamics->internalTransition(event.getTime());
    m_dynamics->externalTransition(events, event.getTime());
    return buildInternalEvent(event.getTime());
}

InternalEvent* Simulator::externalTransitionConflict(
    const InternalEvent& event,
    const ExternalEventList& events)
{
    m_dynamics->externalTransition(events, event.getTime());
    m_dynamics->internalTransition(event.getTime());

    return buildInternalEvent(event.getTime());
}

void Simulator::request(const RequestEvent& event, const Time& time,
                        ExternalEventList& output)
{
    m_dynamics->request(event, time, output);
}

ObservationEvent* Simulator::observation(const ObservationEvent& event) const
{
    value::Value* val;
    ObservationEvent* nevent = 0;

    if (m_atomicModel) {
        val = m_dynamics->observation(event);
        nevent = new ObservationEvent(event);

        if (val) {
            nevent->putAttribute(event.getPortName(), val);
        }
    }
    return nevent;
}

}} // namespace vle devs
