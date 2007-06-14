/**
 * @file devs/Dynamics.hpp
 * @author The VLE Development Team.
 * @brief Dynamics class represent a part of the DEVS simulator. This class
 * must be inherits to build simulation components.
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

#ifndef DEVS_DYNAMICS_HPP
#define DEVS_DYNAMICS_HPP

#include <vle/devs/Event.hpp>
#include <vle/devs/DevsTypes.hpp>
#include <vle/devs/Time.hpp>
#include <vle/devs/EventList.hpp>
#include <vle/devs/EventTable.hpp>
#include <vle/devs/InitEvent.hpp>
#include <vle/devs/InternalEvent.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/StateEvent.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/String.hpp>
#include <libxml++/libxml++.h>
#include <string>


#define DECLARE_DYNAMICS(x) \
  extern "C" { vle::devs::Dynamics* makeNewDynamics(const vle::graph::AtomicModel& model) \
{ return new x(model); } }

namespace vle { namespace devs {

    class Simulator;
    class Coordinator;

    /**
     * @brief Dynamics class represent a part of the DEVS simulator. This class
     * must be inherits to build simulation components.
     *
     */
    class Dynamics
    {
    public:
	/**
	 * Constructor of the dynamics of an atomic model
	 *
	 * @param model the atomic model to which belongs the dynamics
	 */
        Dynamics(const graph::AtomicModel& model) : 
            m_model(model)
        { }

	/**
	 * Destructor (nothing to do)
	 *
	 * @return none
	 */
        virtual ~Dynamics()
        { }

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	/**
	 * Return a reference of the atomic model to which belongs the dynamics
	 *
	 * @return pointer on the atomic model
	 */
        inline const graph::AtomicModel& getModel() const
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
        inline value::Value buildDouble(double value) const
        { return value::DoubleFactory::create(value); }

	/**
	 * Build a Integer object from a long value
	 *
	 * @param value the long value
	 *
	 * @return the Integer object
	 */
        inline value::Value buildInteger(long value) const
        { return value::IntegerFactory::create(value); }

	/**
	 * Build a Boolean object from a bool value
	 *
	 * @param value the bool value
	 *
	 * @return the Boolean object
	 */
        inline value::Value buildBoolean(bool value) const 
        { return value::BooleanFactory::create(value); }

	/**
	 * Build a String object from a string value
	 *
	 * @param value the string value
	 *
	 * @return the String object
	 */
        inline value::Value buildString(const std::string& value) const
        { return value::StringFactory::create(value); }

	/**
	 * Build an empty event list
	 *
	 * @return the empty event list
	 */
        ExternalEventList* noEvent() const 
        { return new ExternalEventList(); }

	/**
	 * Build an event list with a single event on a specified port at
	 * a specified time
	 *
	 * @param portName the name of port where the event will post
	 *
	 * @return the event list with the event
	 */
        ExternalEventList* buildEvent(const std::string& portName) const;

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
        ExternalEventList* buildEventWithADouble(
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
        ExternalEventList* buildEventWithAInteger(
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
        ExternalEventList* buildEventWithABoolean(
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
        ExternalEventList* buildEventWithAString(
                           const std::string& portName,
                           const std::string& attributeName,
                           const std::string& attributeValue) const;


	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 	  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


	/**
	 * When the simulation of the atomic model is finished, the
	 * finish method is invoked.
	 *
	 */
        virtual void finish()
        { }

	/**
	 * Compute the output function.
	 *
         * @param time the time of the occurrence of output function.
         * @param output the list of external events (output parameter).
	 *
	 */
        virtual void getOutputFunction(
            const Time& /* time */,
            ExternalEventList& /* output */)
        { }


	/**
	 * Time advance function: compute the duration of the current state.
	 *
	 * @return duration of the current state.
	 */
        virtual Time getTimeAdvance()
        { return Time::infinity; }

	/**
	 * Initialize the model by initializing the initial state and return
         * the date of the first time advance.
	 *
	 * @return duration of the initial state.
	 */
        virtual Time init()
        { return Time::infinity; }

        /**
         * Compute the selected event when an external event and an internal
         * event occurs at the same time. By default, the internal event
         * is process. This function  ask modeller to make a choice between the
         * internal event or external event list. 
         *
         * @param internal the internal event.
         * @param extEventlist the external events list.
         *
         * @return Event::INTERNAL if internal is priority or Event::EXTERNAL.
         */
        virtual Event::EventType processConflict(
            const InternalEvent& /* internal */,
            const ExternalEventList& /* extEventlist */) const
        { return Event::INTERNAL; }

	/**
         * Process the init events: these events occurs only at the beginning
         * of the simulation and initialize the state of the model.
	 *
	 * @param event the init event list.
	 */
        virtual void processInitEvents(
            const InitEventList& /* event */)
        { }

	/**
         * Compute the new state of the model with the internal transition
         * function.
	 *
	 * @param event the internal event with of the port
	 */
        virtual void processInternalEvent(
            const InternalEvent& /* event */)
        { }

	/**
         * Compute the new state of the model when an external event occurs.
	 *
         * @param event the external event with of the port.
         * @param time the date of occurrence of this event.
	 */
        virtual void processExternalEvents(
            const ExternalEventList& /* event */,
            const Time& /* time */)
        { }

        /**
         * Process an instantaneous event: these functions occurs when an
         * IntantaneousEvent is push into an ExternalEventList by
         * OutputFunction or by processInstantaneousEvent.
         *
         * @param evt InstantaneousEvent to process.
         * @param time the date of occurrence of this event.
         * @param output the list of external events (output parameter).
         *
         * @return a response to the model. This bag can include External and
         * Instantaneous event.
         */
        virtual void processInstantaneousEvent(
            const InstantaneousEvent& /* event */,
            const Time& /* time */,
            ExternalEventList& /* output */) const
        { }

	/**
	 * Compute the current state of the model at a specified time and
	 * for a specified port ; the name of port is the same of the
	 * name of the state variable
	 *
	 * @param event the state event with of the port
	 *
	 * @return the value of state variable
	 */
        virtual value::Value processStateEvent(
            const StateEvent& /* event */) const
        { return value::ValueBase::empty; }

    private:
        const graph::AtomicModel&   m_model;
    };

}} // namespace vle devs

#endif
