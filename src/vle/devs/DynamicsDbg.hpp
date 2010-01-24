/**
 * @file vle/devs/DynamicsDbg.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
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


#ifndef VLE_DEVS_DYNAMICSDBG_HPP
#define VLE_DEVS_DYNAMICSDBG_HPP

#include <vle/devs/DllDefines.hpp>
#include <vle/devs/Dynamics.hpp>

#define DECLARE_DYNAMICS_DBG(mdl) \
    extern "C" { \
        vle::devs::Dynamics* \
        makeNewDynamics(const vle::devs::DynamicsInit& init, \
                        const vle::devs::InitEventList& events) \
        { \
            vle::devs::DynamicsDbg* x__ = new vle::devs::DynamicsDbg( \
                init, events); \
            x__->set(new mdl(init, events)); \
            return x__; \
        } \
    }

#define DECLARE_NAMED_DYNAMICS_DBG(name, mdl) \
    extern "C" { \
        vle::devs::Dynamics* \
        makeNewDynamics##name(const vle::devs::DynamicsInit& init, \
                              const vle::devs::InitEventList& events) \
        { \
            vle::devs::DynamicsDbg* x__ = new vle::devs::DynamicsDbg( \
                init, events); \
            x__->set(new mdl(init, events)); \
            return x__; \
        } \
    }

namespace vle { namespace devs {

    /**
     * @brief A Dynamics debug class which wrap an another Dynamics to show
     * debug information. This class inherits the DEVS standard API.
     */
    class VLE_DEVS_EXPORT DynamicsDbg : public Dynamics
    {
    public:
        /**
         * @brief Constructor of Dynamics for an atomic model.
         *
         * @param init The initialiser of Dynamics.
         * @param events The parameter from the experimental frame.
         */
        DynamicsDbg(const DynamicsInit& init,
                    const InitEventList& events);

        /**
         * @brief Destructor (nothing to do)
         */
        virtual ~DynamicsDbg() {}

        /**
         * @brief Assign a Dynamics to debug for this DynamicsDbg class.
         *
         * @param dyn The Dynamics to attach.
         */
        void set(Dynamics* dyn) { mDynamics = dyn; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
          * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Process the initialization of the model by initializing the
         * initial state and return the duration (or timeAdvance) of the initial
         * state.
         * @param time the time of the creation of this model.
         * @return duration of the initial state.
         */
        virtual Time init(const Time& time);

        /**
         * @brief Process the output function: compute the output function.
         * @param time the time of the occurrence of output function.
         * @param output the list of external events (output parameter).
         */
        virtual void output(const Time& time, ExternalEventList& output) const;

        /**
         * @brief Process the time advance function: compute the duration of the
         * current state.
         * @return duration of the current state.
         */
        virtual Time timeAdvance() const;

        /**
         * @brief Process an internal transition: compute the new state of the
         * model with the internal transition function.
         * @param time the date of occurence of this event.
         */
        virtual void internalTransition(const Time& time);

        /**
         * @brief Process an external transition: compute the new state of the
         * model when an external event occurs.
         * @param event the external event with of the port.
         * @param time the date of occurrence of this event.
         */
        virtual void
            externalTransition(const ExternalEventList& event,
                               const Time& time);

        /**
         * @brief Process the confluent transition: select the transition to
         * call when an internal and one or more external event appear in the
         * same time.
         * @param internal the internal event.
         * @param extEventlist the external events list.
         * @return Event::INTERNAL if internal is priority or Event::EXTERNAL.
         */
        virtual Event::EventType
            confluentTransitions(const Time& time,
                                 const ExternalEventList& extEventlist) const;

        /**
         * @brief Process a request event: these functions occurs when an
         * RequestEvent is push into an ExternalEventList by the
         * output function.
         * @param event RequestEvent to process.
         * @param time the date of occurrence of this event.
         * @param output the list of external events (output parameter).
         * @return a response to the model. This bag can include External and
         * Request event.
         */
        virtual void
            request(const RequestEvent& event,
                    const Time& time,
                    ExternalEventList& output) const;

        /**
         * @brief Process an observation event: compute the current state of the
         * model at a specified time and for a specified port.
         * @param event the state event with of the port
         * @return the value of state variable
         */
        virtual vle::value::Value*
            observation(const ObservationEvent& event) const;

        /**
         * @brief When the simulation of the atomic model is finished, the
         * finish method is invoked.
         */
        virtual void finish();

    private:
        Dynamics* mDynamics;
    };

}} // namespace vle devs

#endif
