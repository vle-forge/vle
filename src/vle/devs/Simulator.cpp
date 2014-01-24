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


#include <vle/devs/Simulator.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/Time.hpp>
#include <vle/vpz/AtomicModel.hpp>

namespace vle { namespace devs {

Simulator::Simulator(vpz::AtomicModel* atomic) :
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
        std::map < vpz::AtomicModel*, devs::Simulator* >& simulators)
{
    mTargets.erase(port);

    vpz::ModelPortList result;
    m_atomicModel->getAtomicModelsTarget(port, result);

    if (result.begin() == result.end()) {
        mTargets.insert(value_type(port, TargetSimulator(
                   (Simulator*)0, std::string())));
    } else {
        for (vpz::ModelPortList::iterator it = result.begin(); it !=
                result.end(); ++it) {

            std::map < vpz::AtomicModel*, devs::Simulator* >::iterator target;
            target = simulators.find(
                    reinterpret_cast < vpz::AtomicModel*>(it->first));

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
    std::map < vpz::AtomicModel*, devs::Simulator* >& simulators)
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

    if (it != mTargets.end()) {
        mTargets.erase(it);
    }
}

void Simulator::addTargetPort(const std::string& port)
{
    assert(mTargets.find(port) == mTargets.end());

    mTargets.insert(value_type(port,
                               TargetSimulator((Simulator*)0,
                                               std::string())));
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

    if (not isInfinity(time)) {
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

InternalEvent* Simulator::init(const Time& currentTime)
{
    Time time = m_dynamics->init(currentTime);

    if (time < 0.0)
        throw utils::ModellingError(
            fmt(_("Negative init function in '%1%' (%2%)")) % getName() %
            time);

    if (isInfinity(time))
        return 0;

    return new InternalEvent(currentTime + time, this);
}

InternalEvent* Simulator::confluentTransitions(
    const InternalEvent& internal,
    const ExternalEventList& extEventlist)
{
    m_dynamics->confluentTransitions(internal.getTime(), extEventlist);
    return buildInternalEvent(internal.getTime());
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

value::Value* Simulator::observation(const ObservationEvent& event) const
{
    return m_dynamics->observation(event);
}

}} // namespace vle devs
