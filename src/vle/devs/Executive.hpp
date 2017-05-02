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

#ifndef VLE_DEVS_EXECUTIVE_HPP
#define VLE_DEVS_EXECUTIVE_HPP 1

#include <vle/DllDefines.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/vle.hpp>
#include <vle/vpz/Conditions.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/vpz/Dynamics.hpp>
#include <vle/vpz/Observables.hpp>

#define DECLARE_EXECUTIVE(mdl)                                                \
    extern "C" {                                                              \
    VLE_MODULE vle::devs::Dynamics* vle_make_new_executive(                   \
      const vle::devs::ExecutiveInit& init,                                   \
      const vle::devs::InitEventList& events)                                 \
    {                                                                         \
        return new mdl(init, events);                                         \
    }                                                                         \
                                                                              \
    VLE_MODULE void vle_api_level(vle::uint32_t* major,                       \
                                  vle::uint32_t* minor,                       \
                                  vle::uint32_t* patch)                       \
    {                                                                         \
        auto version = vle::version();                                        \
        *major = std::get<0>(version);                                        \
        *minor = std::get<1>(version);                                        \
        *patch = std::get<2>(version);                                        \
    }                                                                         \
    }

namespace vle {
namespace devs {

class Simulator;
class Coordinator;
struct ExecutiveInit;

/**
 * @brief Dynamics class for the Barros DEVS extension. Provide graph
 * manipulation at runtime using namespace graph and the coordinator. By
 * default, this model does nothing and it will be inherit to build
 * simulation components.
 */
class VLE_API Executive : public Dynamics
{
public:
    /**
     * @brief Constructor of the Executive of an atomic model
     * @param init The structure to initialise the Executive.
     * @param events Experimentales conditions.
     */
    Executive(const ExecutiveInit& init, const InitEventList& events);

    /**
     * @brief Destructor (nothing to do).
     */
    virtual ~Executive()
    {
    }

    /**
     * @brief If this function return true, then a cast to an Executive object
     * is produce and the set_coordinator function is call. Executive permit to
     * manipulate vpz::CoupledModel and devs::Coordinator at runtime of the
     * simulation.
     * @return false if Dynamics is not an Executive.
     */
    inline virtual bool isExecutive() const override
    {
        return true;
    }

    // / / / /
    //
    // Manage the factory.
    //
    // / / / /

    /**
     * @brief Get a constant reference to the list of vpz::Dynamics objects.
     * @return A constant reference to the list of vpz::Dynamics objects.
     */
    const vpz::Dynamics& dynamics() const;

    /**
     * @brief Get a reference to the list of vpz::Dynamics objects.
     * @return A reference to the list of vpz::Dynamics objects.
     */
    vpz::Dynamics& dynamics();

    /**
     * @brief Get a constant reference to the list of vpz::Conditions
     * objects.
     * @return A constant reference to the list of vpz::Conditions objects.
     */
    const vpz::Conditions& conditions() const;

    /**
     * @brief Get a reference to the list of vpz::Conditions objects.
     * @return A reference to the list of vpz::Conditions objects.
     */
    vpz::Conditions& conditions();

    /**
     * @brief Get a constant reference to the list of vpz::Observables
     * objects.
     * @return A constant reference to the list of vpz::Observables objects.
     */
    const vpz::Observables& observables() const;

    /**
     * @brief Get a reference to the list of vpz::Conditions objects.
     * @return A reference to the list of vpz::Conditions objects.
     */
    vpz::Observables& observables();

    /**
     * @brief Add an observable, ie. a reference and a model to the
     * specified view.
     * @param model the model to attach to the view.
     * @param portname the port of the model to attach.
     * @param view the view.
     * @throw utils::DevsGraphError if model does not exist or if model is
     * not an atomic model.
     */
    void addObservableToView(const std::string& model,
                             const std::string& portname,
                             const std::string& view);

    // / / / /
    //
    // Executive part
    //
    // / / / /

    /**
     * @brief Build a new devs::Simulator from the dynamics library. Attach
     * to this model information of dynamics, condition and observable.
     * @param name the name of the vpz::AtomicModel to create.
     * @param inputs the list of input ports.
     * @param outputs the list of output ports.
     * @param dynamics the name of the dynamics to attach.
     * @param condition the list of condition to attach.
     * @param observable the name of the observable to attach.
     * @throw utils::InternalError if dynamics not exist.
     */
    const vpz::AtomicModel* createModel(
      const std::string& name,
      const std::vector<std::string>& inputs = {},
      const std::vector<std::string>& outputs = {},
      const std::string& dynamics = {},
      const std::vector<std::string>& conditions = {},
      const std::string& observable = {});

    /**
     * @brief Build a new devs::Simulator from the vpz::Classes information.
     * @param classname the name of the class to clone.
     * @param parent the parent of the model.
     * @param modelname the new name of the model.
     * @throw utils::badArg if modelname already exist.
     */
    const vpz::BaseModel* createModelFromClass(const std::string& classname,
                                               const std::string& modelname);

    /**
     * @brief Delete the specified model from coupled model. All
     * connection are deleted, Simulator are deleted and all events are
     * deleted from event table.
     * @param modelname the name of model to delete.
     *
     * @throw utils::DevsGraphError if model does not exist.
     */
    void delModel(const std::string& modelname);

    /**
     * @brief Rename the specified model.
     * @param oldname The name of the model to rename.
     * @param newname The new name of the model.
     * @throw utils::ModelingError if model `oldname' does not exist or if a
     * model `newname' already exists.
     */
    void renameModel(const std::string& oldname, const std::string& newname);

    /**
     * @brief Add an internal, input or output connection in coupled model.
     *
     * @param modelsource An internal model or coupledmodelName() if you want
     * to build an input connection.
     * @param outputport A port of model source.
     * @param modeldestination An output model or coupledmodelName() if you
     * want to build an output connection.
     * @param inputport A port of model destination.
     *
     * @throw utils::DevsGraphError if models or ports do not exist.
     */
    void addConnection(const std::string& modelsource,
                       const std::string& outputport,
                       const std::string& modeldestination,
                       const std::string& inputport);

    /**
     * @brief Remove an internal, input or output connection in coupled model.
     *
     * @param modelsource An internal model or coupledmodelName() if you want
     * to build an input connection.
     * @param outputport A port of model source.
     * @param modeldestination  An output model or coupledmodelName() if you
     * want to build an output connection.
     * @param inputport A port of model destination.
     *
     * @throw utils::DevsGraphError if models or ports do not exist.
     */
    void removeConnection(const std::string& modelsource,
                          const std::string& outputport,
                          const std::string& modeldestination,
                          const std::string& inputport);

    /**
     * @brief Add an input port for an internal model.
     *
     * @param modelName The name of the model.
     * @param portName The port to add.
     *
     * @throw utils::DevsGraphError if model does not exist.
     */
    void addInputPort(const std::string& modelName,
                      const std::string& portName);

    /**
     * @brief Add an output port for an internal model.
     *
     * @param modelName The name of the model.
     * @param portName The port to add.
     *
     * @throw utils::DevsGraphError if model does not exist.
     */
    void addOutputPort(const std::string& modelName,
                       const std::string& portName);

    /**
     * @brief Remove an input port for an internal model.
     *
     * @param modelName The name of the model.
     * @param portName The port to remove.
     *
     * @throw utils::DevsGraphError if model does not exist.
     */
    void removeInputPort(const std::string& modelName,
                         const std::string& portName);

    /**
     * @brief Remove an output port for an internal model.
     *
     * @param modelName The name of the model.
     * @param portName the port to remove.
     *
     * @throw utils::DevsGraphError if model does not exist.
     */
    void removeOutputPort(const std::string& modelName,
                          const std::string& portName);

    // / / / /
    //
    // Give access to attributes
    //
    // / / / /

    /**
     * @brief Dump a valid XML VPZ into the output stream.
     * @param out The stream to flush valid XML VPZ.
     * @param name The name of the experiment.
     * @code
     * virtual internalTransition(const devs::Time& time) const
     * {
     *     std::ofstream file("output.vpz");
     *     dump(file, "dump");
     * }
     * @endcode
     */
    void dump(std::ostream& out, const std::string& name = "default") const;

    /**
     * @brief Get a reference to the current coupled model.
     * @return A constant reference to the coupled model.
     */
    const vpz::CoupledModel& coupledmodel() const
    {
        return *getModel().getParent();
    }

    /**
     * @brief Get the name of the coupled model.
     * @return A constant reference to the name of the coupled model.
     */
    const std::string& coupledmodelName() const
    {
        return coupledmodel().getName();
    }

private:
    /**< A reference to the coordinator of this executive to allow
       modification of coupled model. */
    Coordinator& m_coordinator;

    /*
     * Stores a clone of condition if the user call the @c conditions()
     * non constant function.
     */
    std::unique_ptr<vpz::Conditions> m_conditions;

    /**
     * @brief Get a reference to the current coupled model.
     * @return A reference to the coupled model.
     */
    vpz::CoupledModel* cpled()
    {
        return getModel().getParent();
    }
};
}
} // namespace vle devs

#endif
