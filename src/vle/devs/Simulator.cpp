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
    m_atomicModel(atomic),
    m_tn(negativeInfinity),
    m_have_handle(false)
{
    assert(atomic);
}

void Simulator::clear()
{
    m_dynamics.reset(nullptr);
    m_atomicModel = nullptr;
}

void Simulator::updateSimulatorTargets(
    const std::string& port,
    std::map < vpz::AtomicModel*, devs::Simulator* >& simulators)
{
    mTargets.erase(port);

    vpz::ModelPortList result;
    m_atomicModel->getAtomicModelsTarget(port, result);

    if (result.begin() == result.end()) {
        mTargets.insert(value_type(port, TargetSimulator(
                   (Simulator*)nullptr, std::string())));
    } else {
        for (auto & elem : result) {

            std::map < vpz::AtomicModel*, devs::Simulator* >::iterator target;
            target = simulators.find(
                    reinterpret_cast < vpz::AtomicModel*>(elem.first));

            if (target == simulators.end()) {
                mTargets.erase(port);
                break;
            } else {
                mTargets.insert(std::make_pair(port, TargetSimulator(
                            target->second, elem.second)));
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
    } else if (x.first->second.first == nullptr) {
        x = make_pair(mTargets.end(), mTargets.end());
    }

    return x;
}

void Simulator::removeTargetPort(const std::string& port)
{
    auto it = mTargets.find(port);

    if (it != mTargets.end()) {
        mTargets.erase(it);
    }
}

void Simulator::addTargetPort(const std::string& port)
{
    assert(mTargets.find(port) == mTargets.end());

    mTargets.insert(value_type(port,
                               TargetSimulator((Simulator*)nullptr,
                                               std::string())));
}

void Simulator::addDynamics(std::unique_ptr<Dynamics> dynamics)
{
    m_dynamics = std::unique_ptr<Dynamics>(std::move(dynamics));
}

const std::string& Simulator::getName() const
{
    if (not m_atomicModel)
        throw utils::InternalError(_("Simulator destroyed"));

    return m_atomicModel->getName();
}

const std::string& Simulator::getParent()
{
    if (not m_atomicModel)
        throw utils::InternalError(_("Simulator destroyed"));

    if (m_parents.empty())
        m_parents.assign(m_atomicModel->getParentName());

    return m_parents;
}

void Simulator::finish()
{
    m_dynamics->finish();
}

void Simulator::output(ExternalEventList& output, Time time)
{
    m_dynamics->output(time, output);
}

Time Simulator::timeAdvance()
{
    Time tn = m_dynamics->timeAdvance();

    if (tn < 0.0)
        throw utils::ModellingError(
            fmt(_("Negative time advance in '%1%' (%2%)")) % getName() % tn);

    return tn;
}

Time Simulator::init(Time time)
{
    Time tn = m_dynamics->init(time);

    if (tn < 0.0)
        throw utils::ModellingError(
            fmt(_("Negative init function in '%1%' (%2%)")) % getName() % tn);

    m_tn = tn + time;
    return m_tn;
}

Time Simulator::confluentTransitions(const ExternalEventList& events, Time time)
{
    m_dynamics->confluentTransitions(time, events);

    m_tn = timeAdvance() + time;
    return m_tn;
}

Time Simulator::internalTransition(Time time)
{
    m_dynamics->internalTransition(time);

    m_tn = timeAdvance() + time;
    return m_tn;
}

Time Simulator::externalTransition(const ExternalEventList& events, Time time)
{
    m_dynamics->externalTransition(events, time);

    m_tn = timeAdvance() + time;
    return m_tn;
}

std::unique_ptr<value::Value>
Simulator::observation(const ObservationEvent& event) const
{
    return m_dynamics->observation(event);
}

}} // namespace vle devs
