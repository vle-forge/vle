/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#ifndef VLE_DEVS_DYNAMICSOBSERVER_HPP
#define VLE_DEVS_DYNAMICSOBSERVER_HPP

#include <cassert>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/View.hpp>

namespace vle {
namespace devs {

/**
 * A Dynamics proxy class that wraps an another Dynamics to use
 * observation system. This class inherits \e Dynamics class.
 */
class DynamicsObserver : public Dynamics
{
    /// A reference to the underlying \e devs::Dynamics model (or \e
    /// devs::DynamicsDbg).
    std::unique_ptr<Dynamics> mDynamics;

    /// A pointer to the \e Simulator observation's vector that wraps the \e
    /// devs::Dynamics model.
    std::vector<Observation>& mObservations;

public:
    // These vectors stores View and observation's port for earch function to
    // observer.
    std::vector<std::tuple<View*, std::string>> ppOutput;
    std::vector<std::tuple<View*, std::string>> ppInternal;
    std::vector<std::tuple<View*, std::string>> ppExternal;
    std::vector<std::tuple<View*, std::string>> ppConfluent;
    std::vector<std::tuple<View*, std::string>> ppFinish;

    /**
     * Constructor of Dynamics for an atomic model.
     *
     * @param init The initialiser of Dynamics.
     * @param events The parameter from the experimental frame.
     */
    DynamicsObserver(const DynamicsInit& init,
                     const InitEventList& events,
                     std::vector<Observation>& observations);

    /**
     * Destructor.
     */
    virtual ~DynamicsObserver() = default;

    //
    // \e isExecutive and \e isWrapper use the wrapped \e Dynamics instead
    // of the local function.
    //

    /**
     * @brief If this function return true, then a cast to an Executive
     * object is produce and the set_coordinator function is call. Executive
     * permit to manipulate vpz::CoupledModel and devs::Coordinator at
     * runtime of the simulation.
     * @return false if Dynamics is not an Executive.
     */
    inline virtual bool isExecutive() const override;

    /**
     * @brief If this function return true, then a cast to a DynamicsWrapper
     * object is produce and the set_model and set_library function are call.
     * @return false if Dynamics is not a DynamicsWrapper.
     */
    virtual bool isWrapper() const override;

    // Specifics functions.

    /**
     * Assign a user Dynamics to be observed.
     *
     * @param dyn The Dynamics to attach.
     */
    void set(std::unique_ptr<Dynamics> dyn) noexcept;

    //
    // DSDE functions.
    //

    /**
     * Process the initialization of the model by initializing the initial
     * state and return the duration (or timeAdvance) of the initial
     * state.
     *
     * @param time the time of the creation of this model.
     * @return duration of the initial state.
     */
    virtual Time init(Time time) override;

    /**
     * Process the output function: compute the output function.
     * @param time the time of the occurrence of output function.
     * @param output the list of external events (output parameter).
     */
    virtual void output(Time time, ExternalEventList& output) const override;

    /**
     * Process the time advance function: compute the duration of the
     * current state.
     * @return duration of the current state.
     */
    virtual Time timeAdvance() const override;

    /**
     * Process an internal transition: compute the new state of the
     * model with the internal transition function.
     * @param time the date of occurence of this event.
     */
    virtual void internalTransition(Time time) override;

    /**
     * Process an external transition: compute the new state of the
     * model when an external event occurs.
     * @param event the external event with of the port.
     * @param time the date of occurrence of this event.
     */
    virtual void externalTransition(const ExternalEventList& event,
                                    Time time) override;

    /**
     * Process the confluent transition: select the transition to
     * call when an internal and one or more external event appear in the
     * same time.
     * @param internal the internal event.
     * @param extEventlist the external events list.
     */
    virtual void confluentTransitions(
      Time time,
      const ExternalEventList& extEventlist) override;

    /**
     * Process an observation event: compute the current state of the
     * model at a specified time and for a specified port.
     * @param event the state event with of the port
     * @return the value of state variable
     */
    virtual std::unique_ptr<vle::value::Value> observation(
      const ObservationEvent& event) const override;

    /**
     * When the simulation of the atomic model is finished, the
     * finish method is invoked.
     */
    virtual void finish() override;
};

inline DynamicsObserver::DynamicsObserver(
  const DynamicsInit& init,
  const InitEventList& events,
  std::vector<Observation>& observations)
  : Dynamics(init, events)
  , mObservations(observations)
{
}

inline bool
DynamicsObserver::isExecutive() const
{
    assert(mDynamics && "DynamicsObserver: missing set(Dynamics)");

    return mDynamics->isExecutive();
}

inline bool
DynamicsObserver::isWrapper() const
{
    assert(mDynamics && "DynamicsObserver: missing set(Dynamics)");

    return mDynamics->isWrapper();
}

inline void
DynamicsObserver::set(std::unique_ptr<Dynamics> dyn) noexcept
{
    mDynamics = std::move(dyn);
}

inline Time
DynamicsObserver::init(Time time)
{
    assert(mDynamics && "DynamicsObserver: missing set(Dynamics)");

    return mDynamics->init(time);
}

inline void
DynamicsObserver::output(Time time, ExternalEventList& output) const
{
    assert(mDynamics && "DynamicsObserver: missing set(Dynamics)");

    mDynamics->output(time, output);

    for (const auto& elem : ppOutput) {
        ObservationEvent event(
          time, std::get<0>(elem)->name(), std::get<1>(elem));

        mObservations.emplace_back();
        mObservations.back().view = std::get<0>(elem);
        mObservations.back().portname = std::get<1>(elem);
        mObservations.back().value = mDynamics->observation(event);
    }
}

inline Time
DynamicsObserver::timeAdvance() const
{
    assert(mDynamics && "DynamicsObserver: missing set(Dynamics)");

    return mDynamics->timeAdvance();
}

inline void
DynamicsObserver::internalTransition(Time time)
{
    assert(mDynamics && "DynamicsObserver: missing set(Dynamics)");

    mDynamics->internalTransition(time);

    for (const auto& elem : ppInternal) {
        ObservationEvent event(
          time, std::get<0>(elem)->name(), std::get<1>(elem));

        mObservations.emplace_back();
        mObservations.back().view = std::get<0>(elem);
        mObservations.back().portname = std::get<1>(elem);
        mObservations.back().value = mDynamics->observation(event);
    }
}

inline void
DynamicsObserver::externalTransition(const ExternalEventList& event, Time time)
{
    assert(mDynamics && "DynamicsObserver: missing set(Dynamics)");

    mDynamics->externalTransition(event, time);

    for (const auto& elem : ppExternal) {
        ObservationEvent event(
          time, std::get<0>(elem)->name(), std::get<1>(elem));

        mObservations.emplace_back();
        mObservations.back().view = std::get<0>(elem);
        mObservations.back().portname = std::get<1>(elem);
        mObservations.back().value = mDynamics->observation(event);
    }
}

inline void
DynamicsObserver::confluentTransitions(Time time,
                                       const ExternalEventList& extEventlist)
{
    mDynamics->confluentTransitions(time, extEventlist);

    for (const auto& elem : ppConfluent) {
        ObservationEvent event(
          time, std::get<0>(elem)->name(), std::get<1>(elem));

        mObservations.emplace_back();
        mObservations.back().view = std::get<0>(elem);
        mObservations.back().portname = std::get<1>(elem);
        mObservations.back().value = mDynamics->observation(event);
    }
}

inline std::unique_ptr<vle::value::Value>
DynamicsObserver::observation(const ObservationEvent& event) const
{
    assert(mDynamics && "DynamicsObserver: missing set(Dynamics)");

    return mDynamics->observation(event);
}

inline void
DynamicsObserver::finish()
{
    assert(mDynamics && "DynamicsObserver: missing set(Dynamics)");

    mDynamics->finish();

    for (const auto& elem : ppFinish) {
        ObservationEvent event(
          devs::infinity, std::get<0>(elem)->name(), std::get<1>(elem));

        mObservations.emplace_back();
        mObservations.back().view = std::get<0>(elem);
        mObservations.back().portname = std::get<1>(elem);
        mObservations.back().value = mDynamics->observation(event);
    }
}
}
} // namespace vle devs

#endif
