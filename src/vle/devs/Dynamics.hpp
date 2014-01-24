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


#ifndef DEVS_DYNAMICS_HPP
#define DEVS_DYNAMICS_HPP

#include <vle/DllDefines.hpp>
#include <vle/devs/Time.hpp>
#include <vle/devs/InitEventList.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/ExternalEventList.hpp>
#include <vle/devs/ObservationEvent.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/String.hpp>
#include <vle/utils/PackageTable.hpp>
#include <vle/version.hpp>
#include <string>

#define DECLARE_DYNAMICS(mdl)                                           \
    extern "C" {                                                        \
        VLE_MODULE vle::devs::Dynamics*                                 \
        vle_make_new_dynamics(const vle::devs::DynamicsInit& init,      \
                              const vle::devs::InitEventList& events)   \
        {                                                               \
            return new mdl(init, events);                               \
        }                                                               \
                                                                        \
        VLE_MODULE void                                                 \
        vle_api_level(vle::uint32_t* major,                             \
                      vle::uint32_t* minor,                             \
                      vle::uint32_t* patch)                             \
        {                                                               \
            *major = VLE_MAJOR_VERSION;                                 \
            *minor = VLE_MINOR_VERSION;                                 \
            *patch = VLE_PATCH_VERSION;                                 \
        }                                                               \
    }

namespace vle { namespace devs {

    class RootCoordinator;

    /**
     * @brief A PackageId defines a reference to an element of the
     * vle::utils::PackageTable.
     */
    typedef utils::PackageTable::index PackageId;

    class VLE_API DynamicsInit
    {
    public:
        DynamicsInit(const vpz::AtomicModel& model,
                     PackageId packageid)
            : m_model(model), m_packageid(packageid)
        {}

        virtual ~DynamicsInit()
        {}

        const vpz::AtomicModel& model() const { return m_model; }
        PackageId packageid() const { return m_packageid; }

    private:
        const vpz::AtomicModel&       m_model;
        PackageId                       m_packageid;
    };

    /**
     * @brief Dynamics class represent a part of the DEVS simulator. This class
     * must be inherits to build simulation components.
     */
    class VLE_API Dynamics
    {
    public:
        /**
         * @brief Constructor of Dynamics for an atomic model.
         *
         * @param init The initialiser of Dynamics.
         * @param events The parameter from the experimental frame.
         */
        Dynamics(const DynamicsInit& init,
                 const vle::devs::InitEventList&  /* events */)
            : m_model(init.model()), m_packageid(init.packageid())
        {}

	/**
	 * @brief Destructor (nothing to do)
	 */
        virtual ~Dynamics()
        {}

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
        { return infinity; }

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
        { return infinity; }

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
         * @brief Process the confluent transition, by default,
         * confluentTransitions call internalTransition and externalTransition
         * but this function can be overidden to develop its own dynamics.
         * @param time the time of the simulation.
         * @param extEventlist the external events list.
         */
        virtual void confluentTransitions(
	    const vle::devs::Time& time,
            const vle::devs::ExternalEventList& extEventlist)
        {
            internalTransition(time);
            externalTransition(extEventlist, time);
        }

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
         * permit to manipulate vpz::CoupledModel and devs::Coordinator at
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
        inline const vle::vpz::AtomicModel& getModel() const
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
         * @brief Get the package directory.
         * @return The package path.
         */
        std::string getPackageDir() const;

        /**
         * @brief Get the library package directory.
         * @return The library package path.
         */
        std::string getPackageSimulatorDir() const;

        /**
         * @brief Get the source package directory.
         * @return The source package path.
         */
        std::string getPackageSrcDir() const;

        /**
         * @brief Get the data package directory.
         * @return The data package path.
         */
        std::string getPackageDataDir() const;

        /**
         * @brief Get the document package directory.
         * @return The document package path.
         */
        std::string getPackageDocDir() const;

        /**
         * @brief Get the experiment package directory.
         * @return The experiment package path.
         */
        std::string getPackageExpDir() const;

        /**
         * @brief Get the path of a package file.
         * @param name The name of the file.
         * @return The patch of the file.
         */
        std::string getPackageFile(const std::string& name) const;

        /**
         * @brief Get the path of a library package file.
         * @param name The name of the file.
         * @return The patch of the file.
         */
        std::string getPackageLibFile(const std::string& name) const;

        /**
         * @brief Get the path of a source package file.
         * @param name The name of the file.
         * @return The patch of the file.
         */
        std::string getPackageSrcFile(const std::string& name) const;

        /**
         * @brief Get the path of a data package file.
         * @param name The name of the file.
         * @return The patch of the file.
         */
        std::string getPackageDataFile(const std::string& name) const;

        /**
         * @brief Get the path of a document package file.
         * @param name The name of the file.
         * @return The patch of the file.
         */
        std::string getPackageDocFile(const std::string& name) const;

        /**
         * @brief Get the path of an experiment package file.
         * @param name The name of the file.
         * @return The patch of the file.
         */
        std::string getPackageExpFile(const std::string& name) const;

        /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

        /**
         * @brief Get a constant reference to the element of the
         * vle::utils::PackageTable string table.
         * @return A consttant reference.
         */
        inline PackageId packageid() const { return m_packageid; }

    private:
        const vpz::AtomicModel& m_model; /**< A constant reference to the
                                             atomic model node of the graph.
                                               */

        PackageId m_packageid; /**< An iterator to std::set of the
                                 vle::utils::PackageTable. */
    };

}} // namespace vle devs

#endif
