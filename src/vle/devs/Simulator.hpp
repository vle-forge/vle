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


#ifndef VLE_DEVS_SIMULATOR_HPP
#define VLE_DEVS_SIMULATOR_HPP

#include <vle/DllDefines.hpp>
#include <vle/devs/Time.hpp>
#include <vle/devs/InternalEvent.hpp>
#include <vle/devs/ObservationEvent.hpp>
#include <vle/devs/ExternalEventList.hpp>
#include <vle/devs/Scheduler.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/vpz/AtomicModel.hpp>

namespace vle { namespace devs {

class Dynamics;

/**
 * @brief Represent a couple devs::AtomicModel and devs::Dynamic class to
 * represent the DEVS simulator.
 *
 */
class VLE_LOCAL Simulator
{
public:
    typedef std::pair < Simulator*, std::string > TargetSimulator;
    typedef std::multimap < std::string, TargetSimulator >
        TargetSimulatorList;
    typedef TargetSimulatorList::const_iterator const_iterator;
    typedef TargetSimulatorList::iterator iterator;
    typedef TargetSimulatorList::size_type size_type;
    typedef TargetSimulatorList::value_type value_type;

    /**
     * @brief Build a new devs::Simulator with an empty devs::Dynamics, a
     * null last time but a vpz::AtomicModel node.
     * @param a The atomic model.
     * @throw utils::InternalError if the atomic model does not exist.
     */
    Simulator(vpz::AtomicModel* a);

    /**
     * @brief Delete the attached devs::Dynamics user's model.
     */
    ~Simulator() = default;

    /**
     * @brief Assign a new dynamics to the current Simulator. If a dynamic
     * already exists, it will be delete.
     * @param dynamics The new dynamics.
     */
    void addDynamics(std::unique_ptr<Dynamics> dynamics);

    /**
     * @brief Get the name of the vpz::AtomicModel node.
     * @return the name of the vpz::AtomicModel.
     * @throw utils::InternalEvent if the model is destroyed.
     */
    const std::string& getName() const;

    /**
     * @brief Get the atomic model attached to the Simulator.
     * @return A reference.
     */
    vpz::AtomicModel* getStructure() const
    { return m_atomicModel; }

    /**
     * @brief Delete the dynamics and erase reference to the AtomicModel.
     */
    void clear();

    /**
     * @brief Return a constant reference to the devs::Dynamics.
     * @return
     */
    const std::unique_ptr<Dynamics>& dynamics() const
    {
        return m_dynamics;
    }

    /*-*-*-*-*-*-*-*-*-*/

    /**
     * @brief Call this function to browse the model's structure (atomic
     * and coupled models) to find all devs::Simulator connected to the
     * specified output port.
     * @param port The output port used to build simulators' target list.
     * @param simulators list of available simulators.
     */
    void updateSimulatorTargets(
        const std::string& port,
        std::map < vpz::AtomicModel*, devs::Simulator* >& simulators);

    /**
     * @brief Get two iterators (begin, end) on TargetSimulator.
     * @param port The output port to get the simulators' target list.
     * @param simulators list of available simulators.
     * @return Two iterators.
     */
    std::pair < iterator, iterator > targets(
        const std::string& port,
        std::map < vpz::AtomicModel*, devs::Simulator* >& simulators);

    /**
     * @brief Add an empty target port.
     * @param port Name of the port.
     */
    void removeTargetPort(const std::string& port);

    /**
     * @brief Remove a target port.
     * @param port Name of the port to remove.
     */
    void addTargetPort(const std::string& port);

    /*-*-*-*-*-*-*-*-*-*/

    Time init(Time time);
    Time timeAdvance();
    void finish();
    void output(ExternalEventList& output, Time time);
    Time internalTransition(Time time);
    Time externalTransition(Time time);
    Time confluentTransitions(Time time);
    std::unique_ptr<value::Value> observation(const ObservationEvent& event) const;


    Time getTn() const noexcept
    {
        return m_tn;
    }

    HandleT handle() const noexcept
    {
        assert(m_have_handle && "Simulator: handle is not defined");
        return m_handle;
    }

    bool haveHandle() const noexcept
    {
        return m_have_handle;
    }

    void setHandle(HandleT handle) noexcept
    {
        m_have_handle = true;
        m_handle = handle;
    }

    void resetHandle() noexcept
    {
        m_have_handle = false;
    }

    bool haveExternalEvents() const noexcept
    {
        return not m_external_events.empty();
    }

    void addExternalEvents(Simulator *simulator,
                           std::shared_ptr<value::Value> values,
                           const std::string& portname)
    {
        m_external_events_nextbag.emplace_back(simulator, values, portname);
    }

    void clearExternalEvents()
    {
        m_external_events.clear();
    }

    void swapExternalEvents()
    {
        m_external_events.clear();
        m_external_events.swap(m_external_events_nextbag);
    }

    void setInternalEvent() noexcept
    {
        m_have_internal = true;
    }

    bool haveInternalEvent() const noexcept
    {
        return m_have_internal;
    }

private:
    std::unique_ptr<Dynamics> m_dynamics;
    vpz::AtomicModel*   m_atomicModel;
    TargetSimulatorList mTargets;
    ExternalEventList   m_external_events;
    ExternalEventList   m_external_events_nextbag;
    std::string         m_parents;
    Time                m_tn;
    HandleT             m_handle;
    bool                m_have_handle;
    bool                m_have_internal;
};

}} // namespace vle devs

#endif
