/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2003-2018 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2018 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2018 INRA http://www.inra.fr
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

#include <string>
#include <vle/DllDefines.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/ExternalEventList.hpp>
#include <vle/devs/InitEventList.hpp>
#include <vle/devs/ObservationEvent.hpp>
#include <vle/devs/Time.hpp>
#include <vle/utils/Context.hpp>
#include <vle/utils/PackageTable.hpp>
#include <vle/utils/Types.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Value.hpp>
#include <vle/vle.hpp>
#include <vle/vpz/AtomicModel.hpp>

#define DECLARE_DYNAMICS(mdl)                                                 \
    extern "C"                                                                \
    {                                                                         \
        VLE_MODULE vle::devs::Dynamics* vle_make_new_dynamics(                \
          const vle::devs::DynamicsInit& init,                                \
          const vle::devs::InitEventList& events)                             \
        {                                                                     \
            return new mdl(init, events);                                     \
        }                                                                     \
                                                                              \
        VLE_MODULE void vle_api_level(std::uint32_t* major,                   \
                                      std::uint32_t* minor,                   \
                                      std::uint32_t* patch)                   \
        {                                                                     \
            auto version = vle::version();                                    \
            *major = std::get<0>(version);                                    \
            *minor = std::get<1>(version);                                    \
            *patch = std::get<2>(version);                                    \
        }                                                                     \
    }

namespace vle {
namespace devs {

class RootCoordinator;
struct DynamicsInit;

using PackageId = utils::PackageTable::index;

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
    Dynamics(const DynamicsInit& init, const InitEventList& events);

    /**
     * @brief Destructor (nothing to do)
     */
    virtual ~Dynamics() = default;

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
    virtual Time init(Time /* time */)
    {
        return infinity;
    }

    /**
     * @brief Process the output function: compute the output function.
     * @param time the time of the occurrence of output function.
     * @param output the list of external events (output parameter).
     *
     *
     * @code
     *
     * //
     * // output method's body examples
     * // filling events
     * //
     * ...
     * // send an empty event to the "output" port
     * output.emplace_back("output");
     * // send a string event to the "outputString" port
     * output.emplace_back("outputString");
     * output.back().addString("a String");
     * // send a double event
     * output.emplace_back("outputDouble");
     * output.back().addDouble(.0);
     * // send a map event
     * output.emplace_back("outputMap");
     * value::Map& m = output.back().addMap();
     * m.addString("name",it->first);
     * m.addDouble("value", it->second);
     * ...
     * @endcode
     */
    virtual void output(Time /* time */, ExternalEventList& /* output */) const
    {}

    /**
     * @brief Process the time advance function: compute the duration of the
     * current state.
     * @return duration of the current state.
     */
    virtual Time timeAdvance() const
    {
        return infinity;
    }

    /**
     * @brief Process an internal transition: compute the new state of the
     * model with the internal transition function.
     * @param time the date of occurence of this event.
     */
    virtual void internalTransition(Time /* time */)
    {}

    /**
     * @brief Process an external transition: compute the new state of the
     * model when an external event occurs.
     * @param event the external event with of the port.
     * @param time the date of occurrence of this event.
     *
     *
     * @code
     * //
     * // externalTransition method's body examples
     * // to get the content of an event
     * //
     * ...
     * vle::devs::ExternalEventList::const_iterator
     * it = events.begin();
     * while (it != events.end()) {
     *    // to get a double
     *    if (it->attributes()->isDouble()) {
     *        Double aDouble = it->getDouble().value();
     *    // to get a integer
     *    } else if (it->attributes()->isInteger()) {
     *        Int aInteger = it->getInteger().value();
     *    // to get a string
     *    } else if (it->attributes()->isString()) {
     *       std::string aString = it->getString().value();
     *    // to get a map and inside values
     *    } else if (it->attributes()->isMap()) {
     *       const value::Map& attrs = it->getMap();
     *       std::string aString = attrs.getString("name").value();
     *       double aDouble = attrs.getDouble("value").value();
     *       ...
     *    } else {
     *    ...
     *    }
     * }
     * ...
     * @endcode
     */
    virtual void externalTransition(const ExternalEventList& /* event */,
                                    Time /* time */)
    {}

    /**
     * @brief Process the confluent transition, by default,
     * confluentTransitions call internalTransition and externalTransition
     * but this function can be overidden to develop its own dynamics.
     * @param time the time of the simulation.
     * @param extEventlist the external events list.
     */
    virtual void confluentTransitions(Time time,
                                      const ExternalEventList& extEventlist)
    {
        internalTransition(time);
        externalTransition(extEventlist, time);
    }

    /**
     * @brief Process an observation event: compute the current state of the
     * model at a specified time and for a specified port.
     * @param event the state event with of the port
     * @return the value of state variable
     *
     *
     * @code
     * //
     * // observation method's body example
     * // to observe different Types of values
     * //
     * ...
     * const std::string& port = event.getPortName();
     * if (port == "doubleObservablePort") {
     *    return value::Double::create(.0);
     * } else if (port == "integerObservablePort") {
     *    return value::Integer::create(1);
     * } else if (port == "stringObservablePort") {
     *    return  value::String::create("a string");
     * } else if (port == "setObservablePort") {
     *    std::unique_ptr<value::Set> set(new vle::value::Set());
     *    set->add(value::Integer::create(1));
     *    set->add(value::String::create("a string"));
     *    ...
     *    return set;
     * }
     * ...
     * @endcode
     */
    virtual std::unique_ptr<vle::value::Value> observation(
      const ObservationEvent& /* event */) const
    {
        return {};
    }

    /**
     * @brief When the simulation of the atomic model is finished, the
     * finish method is invoked.
     */
    virtual void finish()
    {}

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
    {
        return false;
    }

    /**
     * @brief If this function return true, then a cast to a DynamicsWrapper
     * object is produce and the set_model and set_library function are call.
     * @return false if Dynamics is not a DynamicsWrapper.
     */
    inline virtual bool isWrapper() const
    {
        return false;
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Return a reference of the atomic model to which belongs the
     * dynamics
     * @return pointer on the atomic model
     */
    inline const vle::vpz::AtomicModel& getModel() const
    {
        return m_model;
    }

    /**
     * Return the name of the atomic model to which belongs the dynamics
     *
     * @return name of the atomic model
     */
    inline const std::string& getModelName() const
    {
        return m_model.getName();
    }

    /**
     * Build an event list with a single event on a specified port at
     * a specified time
     *
     * @param portName the name of port where the event will post
     *
     * @return the event list with the event
     */
    ExternalEvent* buildEvent(const std::string& portName) const;

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
     * Get a Context shared pointer.
     */
    utils::ContextPtr context() const noexcept
    {
        return m_context;
    }

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

    /** @brief Sends message to the ContextPtr logging system.
     *
     * Sends the message produced by :c format and @c ... to the ContextPtr
     * logging system if the priority less than ContextPtr get_log_priority
     * function.
     *
     * @param priority An integer in range [0-7] define message type.
     * @param format The printf function like message type.
     */
    void Trace(int priority, const char* format, ...) const;

    /** @brief Sends message to the ContextPtr logging system.
     *
     * Sends the string message to the ContextPtr logging system if the
     * priority less than ContextPtr get_log_priority function.
     *
     * @param priority An integer in range [0-7] define message type.
     * @param str The message to be send.
     */
    void Trace(int priority, const std::string& str) const;

private:
    ///< A reference to the context.
    utils::ContextPtr m_context;

    ///< A constant reference to the atomic model node of the graph.
    const vpz::AtomicModel& m_model;

    ///< An iterator to std::set of the vle::utils::PackageTable.
    PackageId m_packageid;
};

/** @brief Sends message to the ContextPtr logging system.
 *
 * Sends the message produced by :c format and @c ... to the ContextPtr logging
 * system if the priority less than ContextPtr get_log_priority function.
 *
 * @param ctx ContextPtr to store message.
 * @param priority An integer in range [0-7] define message type.
 * @param format The printf function like message type.
 */
VLE_API void
Trace(utils::ContextPtr ctx, int priority, const char* format, ...);
}
} // namespace vle devs

#endif
