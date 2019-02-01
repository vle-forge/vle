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

#ifndef VLE_DEVS_COORDINATOR_HPP
#define VLE_DEVS_COORDINATOR_HPP 1

#include "Thread.hpp"
#include <vle/DllDefines.hpp>
#include <vle/devs/ModelFactory.hpp>
#include <vle/devs/Scheduler.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/devs/Time.hpp>
#include <vle/devs/View.hpp>
#include <vle/utils/Context.hpp>

namespace vle {
namespace devs {

class Executive;

/**
 * @brief Represent the DEVS Coordinator class. This class provide a non
 * hierarchical DEVS Coordinator ie. all models are in the same coupled
 * model.
 *
 */
class VLE_LOCAL Coordinator
{
public:
    Coordinator(utils::ContextPtr context,
                const vpz::Dynamics& dyn,
                const vpz::Classes& cls,
                const vpz::Experiment& experiment);

    ~Coordinator() = default;

    /**
     * @brief Initialise Coordinator before running simulation.
     *
     * Send to all Simulator the first init event found and call for each
     * Simulator the processInitEvent. Before this, dispatchStateEvent is call
     * for all StateEvent.
     *
     * @param instance Negative instance have not effect on devs::View and
     *     devs::Observation and output plug-ins. If the instance have a value
     *     greater or equal to 0, the devs::Coordinator will add this instance
     *     value to the file name. This parameter come from the vpz::Project
     *     instance attribute.
     *
     * @throw Exception::Internal if a condition have no model port name
     * associed.
     */
    void init(const vpz::Model& mdls,
              Time current,
              Time duration,
              long instance);

    /**
     * @brief Pop the next devs::CompleteEventBagModel from the
     * devs::EventTable and call devs::Simulator function.
     * @return 0.
     */
    void run();

    /**
     * @brief Build a new devs::Simulator from the dynamics library. Attach
     * to this model information of dynamics, condition and observable.
     * @param model the vpz::AtomicModel reference source of
     * devs::Simulator.
     * @param dynamics the name of the dynamics to attach.
     * @param condition the name of the condition to attach.
     * @param observable the name of the observable to attach.
     * @throw utils::InternalError if dynamics not exist.
     */
    void createModel(vpz::AtomicModel* model,
                     const vpz::Conditions& experiment_conditions,
                     const std::string& dynamics,
                     const std::vector<std::string>& conditions,
                     const std::string& observable);

    /**
     * @brief Build a new devs::Simulator from the vpz::Classes information.
     * @param classname the name of the class to clone.
     * @param parent the parent of the model.
     * @param modelname the new name of the model.
     * @param conditions, the conditions for initalization of the models.
     * @param inputs the list of input ports to add.
     * @param outputs the list of output ports to add.
     * @throw utils::badArg if modelname already exist.
     */
    vpz::BaseModel* createModelFromClass(
      const std::string& classname,
      vpz::CoupledModel* parent,
      const std::string& modelname,
      const vpz::Conditions& conditions,
      const std::vector<std::string>& inputs,
      const std::vector<std::string>& outputs);

    /**
     * @brief Add an observable, ie. a reference and a model to the
     * specified view.
     * @param model the model to attach to the view.
     * @param portname the port of the model to attach.
     * @param view the view.
     */
    void addObservableToView(vpz::AtomicModel* model,
                             const std::string& portname,
                             const std::string& view);

    /**
     * @brief Delete the specified model from coupled model. All
     * connection are deleted, Simulator are deleted and all events are
     * deleted from event table.
     * @param parent the coupled model parent of model to delete.
     * @param modelname the name of model to delete.
     */
    // void delModel(vpz::CoupledModel* parent,
    //               const std::string& modelname);

    void getSimulatorsSource(
      vpz::BaseModel* model,
      std::vector<std::pair<Simulator*, std::string>>& lst);

    void getSimulatorsSource(
      vpz::BaseModel* model,
      const std::string& port,
      std::vector<std::pair<Simulator*, std::string>>& lst);

    void updateSimulatorsTarget(
      std::vector<std::pair<Simulator*, std::string>>& lst);

    void removeSimulatorTargetPort(vpz::AtomicModel* model,
                                   const std::string& port);

    //
    ///
    //// Some usefull functions.
    ///
    //

    /**
     * Add a newly \c Simulator into the list of simulators and retrieves
     * a pointer to the underlying simulator.
     *
     * \param model A pointer to the \e vpz::AtomicModel attached to this
     * newly allocated Simulator.
     *
     * \return A pointer the the newly allocated Simulator.
     */
    Simulator* addModel(vpz::AtomicModel* model);

    //
    ///
    //// Get/Set functions.
    ///
    //

    inline Time getCurrentTime() const
    {
        return m_currentTime;
    }

    /**
     * @brief Get a constant reference to the list of vpz::Dynamics objects.
     * @return A constant reference to the list of vpz::Dynamics objects.
     */
    const vpz::Dynamics& dynamics() const
    {
        return m_modelFactory.dynamics();
    }

    /**
     * @brief Get a reference to the list of vpz::Dynamics objects.
     * @return A reference to the list of vpz::Dynamics objects.
     */
    vpz::Dynamics& dynamics()
    {
        return m_modelFactory.dynamics();
    }

    /**
     * @brief Get a constant reference to the list of vpz::Conditions
     * objects.
     * @return A constant reference to the list of vpz::Conditions objects.
     */
    const vpz::Conditions& conditions() const
    {
        return m_modelFactory.conditions();
    }

    /**
     * @brief Get a reference to the list of vpz::Conditions objects.
     * @return A reference to the list of vpz::Conditions objects.
     */
    vpz::Conditions& conditions()
    {
        return m_modelFactory.conditions();
    }

    /**
     * @brief Get a constant reference to the list of vpz::Observables
     * objects.
     * @return A constant reference to the list of vpz::Observables objects.
     */
    const vpz::Observables& observables() const
    {
        return m_modelFactory.observables();
    }

    /**
     * @brief Get a reference to the list of vpz::Conditions objects.
     * @return A reference to the list of vpz::Conditions objects.
     */
    vpz::Observables& observables()
    {
        return m_modelFactory.observables();
    }

    bool isStarted() const
    {
        return m_isStarted;
    }

    void processInit(Simulator* simulator);

    /**
     * Retrieves for all Views the \c vle::value::Matrix result.
     *
     * @return NULL if the views do not have storage
     * plug-ins. Or the map of matrices cloned into oov plugins
     */
    std::unique_ptr<value::Map> getMap() const;

    /**
     * Called when the simulation finishes
     *
     * @return the map of matrices built in oov storage
     */
    std::unique_ptr<value::Map> finish();

    /**
     * Retrives access to all event (output, internal, external, ...) \e
     * Views.
     *
     * @return A constatn
     */
    const std::map<std::string, View>& getEventViewList() const;

    /** An executive adds a model (atomic or coupled) to be delete at the
     * end of the \e Coordinator::run() function.
     *
     * \param model The model (Atomic or Coupled) to delete.
     */
    void prepare_dynamic_deletion(vpz::BaseModel*);

    /** For each element of the \e m_delete_model list, delete the \c
     * vpz::BaseModel information from the structure of the model. Delete
     * the \c Simulator from scheduller and the Simulator himself.
     */
    void dynamic_deletion();

private:
    Coordinator(const Coordinator& other);
    Coordinator& operator=(const Coordinator& other);

    utils::ContextPtr m_context;
    Time m_currentTime;
    Time m_durationTime;
    SimulatorProcessParallel m_simulators_thread_pool;
    std::vector<std::unique_ptr<Simulator>> m_simulators;
    Scheduler m_eventTable;
    TimedObservationScheduler m_timed_observation_scheduler;
    std::map<std::string, View> m_eventViewList;
    std::map<std::string, View> m_timedViewList;
    ModelFactory m_modelFactory;

    std::vector<vpz::BaseModel*> m_delete_model;

    bool m_isStarted;

    /**
     * @brief Build, for each vpz::View a StreamWriter and View.
     *
     * @param instance Negative instance have not effect on devs::View and
     *     devs::Observation and output plug-ins. If the instance have a value
     *     greater or equal to 0, the devs::Coordinator will add this instance
     *     value to the file name. This parameter come from the vpz::Project
     *     instance attribute.
     *
     * @throw utils::ArgError if the output or the view does not exist.
     */
    void buildViews(long instance);

    /**
     * @brief build the simulator from the vpz::BaseModel stock.
     * @param model
     */
    void addModels(const vpz::Model& model);

    /**
     * Read all ExternalEventList including External and Instantaneous
     * events and found the destination models. If event is an
     * Instantaneous then, a new Event is attached to
     * CompleteEventBagModel otherwise, if event is an External then is
     * push int the Schuduller.
     *
     * @param sim the simulator that dispatch external events.
     */
    void dispatchExternalEvent(std::vector<Simulator*>& sim,
                               const std::size_t number);

    /**
     * @brief Delete the atomic model from Graph, the Simulator from
     * Coordinator and clean all events on devs::EventTable. Do not
     * use the AtomicModel after this function, it is delete.
     *
     * @param atom the model to delete.
     */
    void delAtomicModel(vpz::AtomicModel* atom,
                        std::vector<Simulator*>& to_delete);

    /**
     * @brief Delete the coupled model from Devs::Graph. All the
     * Simulator from Coordinator and clean all events on
     * devs::EventTable. Do not use the CoupledModel child or
     * CoupledModel after this function.  All are delete. This
     * function is recursive.
     *
     * @param mdl the model to delete.
     */
    void delCoupledModel(vpz::CoupledModel* mdl,
                         std::vector<Simulator*>& to_delete);

    /**
     * Set a new date to the Coordinator.
     * @param time the new date to affect Coordinator.
     */
    inline void updateCurrentTime(const Time& time)
    {
        m_currentTime = time;
    }
};
}
} // namespace vle devs

#endif
