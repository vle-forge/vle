/**
 * @file vle/devs/Dynamics.hpp
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


#ifndef DEVS_DYNAMICS_HPP
#define DEVS_DYNAMICS_HPP

#include <vle/devs/DllDefines.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/devs/Event.hpp>
#include <vle/devs/Time.hpp>
#include <vle/devs/EventList.hpp>
#include <vle/devs/EventTable.hpp>
#include <vle/devs/InternalEvent.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/ExternalEventList.hpp>
#include <vle/devs/ObservationEvent.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/String.hpp>
#include <vle/utils/Rand.hpp>
#include <string>


#define DECLARE_DYNAMICS(mdl) \
    extern "C" { \
        vle::devs::Dynamics* \
        makeNewDynamics(const vle::graph::AtomicModel& model, \
                        const vle::devs::InitEventList& events) \
        { return new mdl(model, events); } \
    }

#define DECLARE_NAMED_DYNAMICS(name, mdl) \
    extern "C" { \
        vle::devs::Dynamics* \
        makeNewDynamics##name(const vle::graph::AtomicModel& model, \
                              const vle::devs::InitEventList& events) \
        { return new mdl(model, events); } \
    }

namespace vle { namespace devs {

    class RootCoordinator;

    /**
     * @brief Dynamics class represent a part of the DEVS simulator. This class
     * must be inherits to build simulation components.
     *
     */
    class VLE_DEVS_EXPORT Dynamics
    {
    public:
	/**
         * @brief Constructor of the dynamics of an atomic model. Process the
         * init events: these events occurs only at the beginning of the
         * simulation and initialize the state of the model.
	 * @param model the atomic model to which belongs the dynamics
	 * @param event the init event list.
	 */
        Dynamics(const vle::graph::AtomicModel& model,
                 const vle::devs::InitEventList&  /* events */) :
            m_model(model),
            m_rand(0)
        { }

	/**
	 * @brief Destructor (nothing to do)
	 */
        virtual ~Dynamics()
        { }

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
        virtual vle::devs::Time init(const vle::devs::Time& /* time */)
        { return Time::infinity; }

	/**
	 * @brief Process the output function: compute the output function.
         * @param time the time of the occurrence of output function.
         * @param output the list of external events (output parameter).
	 */
        virtual void output(const vle::devs::Time& /* time */,
                            vle::devs::ExternalEventList& /* output */) const
        { }

	/**
         * @brief Process the time advance function: compute the duration of the
         * current state.
         * @return duration of the current state.
	 */
        virtual vle::devs::Time timeAdvance() const
        { return Time::infinity; }

	/**
         * @brief Process an internal transition: compute the new state of the
         * model with the internal transition function.
	 * @param time the date of occurence of this event.
	 */
        virtual void internalTransition(
            const vle::devs::Time& /* time */)
        { }

	/**
         * @brief Process an external transition: compute the new state of the
         * model when an external event occurs.
         * @param event the external event with of the port.
         * @param time the date of occurrence of this event.
	 */
        virtual void externalTransition(
	    const vle::devs::ExternalEventList& /* event */,
            const vle::devs::Time& /* time */)
        { }

        /**
         * @brief Process the confluent transition: select the transition to
         * call when an internal and one or more external event appear in the
         * same time.
         * @param internal the internal event.
         * @param extEventlist the external events list.
         * @return Event::INTERNAL if internal is priority or Event::EXTERNAL.
         */
        virtual vle::devs::Event::EventType confluentTransitions(
	    const vle::devs::Time& /* time */,
            const vle::devs::ExternalEventList& /* extEventlist */) const
        { return Event::INTERNAL; }

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
        virtual void request(
	    const vle::devs::RequestEvent& /* event */,
            const vle::devs::Time& /* time */,
            vle::devs::ExternalEventList& /* output */) const
        { }

	/**
         * @brief Process an observation event: compute the current state of the
         * model at a specified time and for a specified port.
	 * @param event the state event with of the port
	 * @return the value of state variable
	 */
        virtual vle::value::Value* observation(
	    const vle::devs::ObservationEvent& /* event */) const
        { return 0; }

        /**
         * @brief When the simulation of the atomic model is finished, the
         * finish method is invoked.
	 */
        virtual void finish()
        { }

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief If this function return true, then a cast to an Executive
         * object is produce and the set_coordinator function is call. Executive
         * permit to manipulate graph::CoupledModel and devs::Coordinator at
         * runtime of the simulation.
         * @return false if Dynamics is not an Executive.
         */
        inline virtual bool isExecutive() const
        { return false; }

        /**
         * @brief If this function return true, then a cast to a DynamicsWrapper
         * object is produce and the set_model and set_library function are call.
         * @return false if Dynamics is not a DynamicsWrapper.
         */
        inline virtual bool isWrapper() const
        { return false; }

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	/**
         * @brief Return a reference of the atomic model to which belongs the
         * dynamics
	 * @return pointer on the atomic model
	 */
        inline const vle::graph::AtomicModel& getModel() const
        { return m_model; }

	/**
	 * Return the name of the atomic model to which belongs the dynamics
	 *
	 * @return name of the atomic model
	 */
        inline const std::string& getModelName() const
        { return m_model.getName(); }

	/**
	 * Build a Double object from a double value
	 *
	 * @param value the double value
	 *
	 * @return the Double object
	 */
        inline vle::value::Double* buildDouble(double value) const
        { return value::Double::create(value); }

	/**
	 * Build a Integer object from a long value
	 *
	 * @param value the long value
	 *
	 * @return the Integer object
	 */
        inline vle::value::Integer* buildInteger(long value) const
        { return value::Integer::create(value); }

	/**
	 * Build a Boolean object from a bool value
	 *
	 * @param value the bool value
	 *
	 * @return the Boolean object
	 */
        inline vle::value::Boolean* buildBoolean(bool value) const
        { return value::Boolean::create(value); }

	/**
	 * Build a String object from a string value
	 *
	 * @param value the string value
	 *
	 * @return the String object
	 */
        inline vle::value::String* buildString(const std::string& value) const
        { return value::String::create(value); }

	/**
	 * Build an empty event list
	 *
	 * @return the empty event list
	 */
        vle::devs::ExternalEventList* noEvent() const
        { return new ExternalEventList(); }

	/**
	 * Build an event list with a single event on a specified port at
	 * a specified time
	 *
	 * @param portName the name of port where the event will post
	 *
	 * @return the event list with the event
	 */
        vle::devs::ExternalEvent* buildEvent(const std::string& portName) const;

	/**
	 * Build an event list with a single event which is attached a
	 * double attribute
	 *
	 * @param portName the name of port where the event will post
	 * @param attributeName the name of the attribute attached to
	 * the event
	 * @param attributeValue the double value given to the attribute
	 *
	 * @return the event list with the event
	 */
        vle::devs::ExternalEvent* buildEventWithADouble(
                           const std::string& portName,
                           const std::string& attributeName,
                           double attributeValue) const;

	/**
	 * Build an event list with a single event which is attached a
	 * long attribute
	 *
	 * @param portName the name of port where the event will post
	 * @param attributeName the name of the attribute attached to
	 * the event
	 * @param attributeValue the long value given to the attribute
	 *
	 * @return the event list with the event
	 */
        vle::devs::ExternalEvent* buildEventWithAInteger(
                            const std::string& portName,
                            const std::string& attributeName,
                            long attributeValue) const;

	/**
	 * Build an event list with a single event which is attached a
	 * bool attribute
	 *
	 * @param portName the name of port where the event will post
	 * @param attributeName the name of the attribute attached to
	 * the event
	 * @param attributeValue the bool value given to the attribute
	 *
	 * @return the event list with the event
	 */
        vle::devs::ExternalEvent* buildEventWithABoolean(
                           const std::string& portName,
                           const std::string& attributeName,
                           bool attributeValue) const;

	/**
	 * Build an event list with a single event which is attached a
	 * string attribute
	 *
	 * @param portName the name of port where the event will post
	 * @param attributeName the name of the attribute attached to
	 * the event
	 * @param attributeValue the string value given to the attribute
	 *
	 * @return the event list with the event
	 */
        vle::devs::ExternalEvent* buildEventWithAString(
                           const std::string& portName,
                           const std::string& attributeName,
                           const std::string& attributeValue) const;

        /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

        /**
         * @brief Return a reference to the RootCoordinator utils::Rand pseudo
         * random generator.
         * @return Return a reference to the utils::Rand object.
         */
        inline utils::Rand& rand() const
        { return *m_rand; }

        friend void RootCoordinator::setRand(Dynamics& dyn);

    private:
        const graph::AtomicModel&   m_model;
        utils::Rand*                m_rand;
    };

}} // namespace vle devs

#endif
