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


#ifndef VLE_DEVS_COORDINATOR_HPP
#define VLE_DEVS_COORDINATOR_HPP 1

#include <vle/DllDefines.hpp>
#include <vle/utils/Context.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/devs/Scheduler.hpp>
#include <vle/devs/View.hpp>
#include <vle/devs/Time.hpp>
#include <vle/devs/ModelFactory.hpp>

namespace vle { namespace devs {

class Executive;

typedef std::vector < Simulator* > SimulatorList;
typedef std::map < vpz::AtomicModel*, devs::Simulator* > SimulatorMap;

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
                const vpz::Experiment& experiment,
                RootCoordinator& root);

    ~Coordinator();

    /**
     * @brief Initialise Coordinator before running simulation. Rand is
     * initialized, send to all Simulator the first init event found and
     * call for each Simulator the processInitEvent. Before this,
     * dispatchStateEvent is call for all StateEvent.
     *
     * @throw Exception::Internal if a condition have no model port name
     * associed.
     */
    void init(const vpz::Model& mdls, Time current, Time duration);

    /**
     * \brief Returns the next time.
     * @return A devs::Time.
     */
    // Time getNextTime() noexcept { return m_eventTable.getNextTime(); }

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
                     const std::string& dynamics,
                     const std::vector < std::string >& conditions,
                     const std::string& observable);

    /**
     * @brief Build a new devs::Simulator from the vpz::Classes information.
     * @param classname the name of the class to clone.
     * @param parent the parent of the model.
     * @param modelname the new name of the model.
     * @throw utils::badArg if modelname already exist.
     */
    vpz::BaseModel* createModelFromClass(const std::string& classname,
                                       vpz::CoupledModel* parent,
                                       const std::string& modelname);

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
    void delModel(vpz::CoupledModel* parent,
                  const std::string& modelname);

    void getSimulatorsSource(
        vpz::BaseModel* model,
        std::vector < std::pair < Simulator*, std::string > >& lst);

    void getSimulatorsSource(
        vpz::BaseModel* model,
        const std::string& port,
        std::vector < std::pair < Simulator*, std::string > >& lst);

    void updateSimulatorsTarget(
        std::vector < std::pair < Simulator*, std::string > >& lst);

    void removeSimulatorTargetPort(vpz::AtomicModel* model,
                                   const std::string& port);

    //
    ///
    //// Some usefull functions.
    ///
    //

    /**
     * @brief Attach the specified simulator to the vpz::AtomicModel and
     * install it on bus.
     * @param model
     * @param simulator
     */
    void addModel(vpz::AtomicModel* model, Simulator* simulator);

    /**
     * Return the devs::Simulator with a specified vpz::AtomicModel.
     * Complexity: log O(log(n)).
     * @param model the atomicmodel reference to search.
     * @return A reference to the devs::Simulator or 0 if not found.
     */
    Simulator* getModel(const vpz::AtomicModel* model) const;

    /**
     * Return the devs::Simulator with a specified atomic model name.
     * Complexity: linear O(n).
     * @param model the name of atomic model to search.
     * @return a reference to the devs::Simulator or 0 if not found.
     */
    Simulator* getModel(const std::string& model) const;


    //
    ///
    //// Get/Set functions.
    ///
    //

    inline Time getCurrentTime() const
    { return m_currentTime; }

    inline const SimulatorMap& modellist() const
    { return m_modelList; }

    /**
     * @brief Get a constant reference to the list of vpz::Dynamics objects.
     * @return A constant reference to the list of vpz::Dynamics objects.
     */
    const vpz::Dynamics& dynamics() const
    { return m_modelFactory.dynamics(); }

    /**
     * @brief Get a reference to the list of vpz::Dynamics objects.
     * @return A reference to the list of vpz::Dynamics objects.
     */
    vpz::Dynamics& dynamics() { return m_modelFactory.dynamics(); }

    /**
     * @brief Get a constant reference to the list of vpz::Conditions
     * objects.
     * @return A constant reference to the list of vpz::Conditions objects.
     */
    const vpz::Conditions& conditions() const
    { return m_modelFactory.conditions(); }

    /**
     * @brief Get a reference to the list of vpz::Conditions objects.
     * @return A reference to the list of vpz::Conditions objects.
     */
    vpz::Conditions& conditions()
    { return m_modelFactory.conditions(); }

    /**
     * @brief Get a constant reference to the list of vpz::Observables
     * objects.
     * @return A constant reference to the list of vpz::Observables objects.
     */
    const vpz::Observables& observables() const
    { return m_modelFactory.observables(); }

    /**
     * @brief Get a reference to the list of vpz::Conditions objects.
     * @return A reference to the list of vpz::Conditions objects.
     */
    vpz::Observables& observables()
    { return m_modelFactory.observables(); }

    bool isStarted() const { return m_isStarted; }

    void processInit(Simulator *simulator);
    void processInternalEvent(Bag::value_type& modelbag);
    void processExternalEvents(Bag::value_type& modelbag);
    void processConflictEvents(Bag::value_type& modelbag);

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

private:
    Coordinator(const Coordinator& other);
    Coordinator& operator=(const Coordinator& other);

    utils::ContextPtr           m_context;
    Time                        m_currentTime;
    Time                        m_durationTime;
    SimulatorMap                m_modelList;
    Scheduler                   m_eventTable;
    TimedObservationScheduler   m_timed_observation_scheduler;
    std::map<std::string, View> m_eventViewList;
    std::map<std::string, View> m_timedViewList;
    ModelFactory                m_modelFactory;
    SimulatorList               m_deletedSimulator;
    SimulatorList::size_type    m_toDelete;
    bool                        m_isStarted;

    /**
     * @brief Build, for each vpz::View a StreamWriter and View.
     * @throw utils::ArgError if the output or the view does not exist.
     */
    void buildViews();

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
     * @param eventList the external event list to treat.
     * @param sim the simulator that dispatch external events.
     */
    void dispatchExternalEvent(ExternalEventList& eventList,
                               Simulator* sim);

    /**
     * @brief Delete the atomic model from Graph, the Simulator from
     * Coordinator and clean all events on devs::EventTable. Do not
     * use the AtomicModel after this function, it is delete.
     *
     * @param atom the model to delete.
     */
    void delAtomicModel(vpz::AtomicModel* atom);

    /**
     * @brief Delete the coupled model from Devs::Graph. All the
     * Simulator from Coordinator and clean all events on
     * devs::EventTable. Do not use the CoupledModel child or
     * CoupledModel after this function.  All are delete. This
     * function is recursive.
     *
     * @param mdl the model to delete.
     */
    void delCoupledModel(vpz::CoupledModel* mdl);

    /**
     * Set a new date to the Coordinator.
     * @param time the new date to affect Coordinator.
     */
    inline void updateCurrentTime(const Time& time)
    { m_currentTime = time; }
};

}} // namespace vle devs

#endif
