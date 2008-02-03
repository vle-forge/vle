/**
 * @file devs/Coordinator.hpp
 * @author The VLE Development Team.
 * @brief Represent the DEVS coordinator class. This class provide a non
 * hierarchical DEVS coordinator ie. all models are in the same coupled model.
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

#ifndef DEVS_COORDINATOR_HPP
#define DEVS_COORDINATOR_HPP

#include <vle/devs/Simulator.hpp>
#include <vle/devs/EventView.hpp>
#include <vle/devs/EventTable.hpp>
#include <vle/devs/View.hpp>
#include <vle/devs/Time.hpp>
#include <vle/devs/TimedView.hpp>
#include <vle/devs/FinishView.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/oov/Plugin.hpp>

namespace vle { namespace devs {

    class ModelFactory;
    class Executive;
    
    typedef std::vector < Simulator* > SimulatorList;
    typedef std::map < graph::AtomicModel*, devs::Simulator* > SimulatorMap;

    /**
     * @brief Represent the DEVS Coordinator class. This class provide a non
     * hierarchical DEVS Coordinator ie. all models are in the same coupled
     * model.
     *
     */
    class Coordinator
    {
    public:
        Coordinator(ModelFactory& modelfactory);

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
        void init(const vpz::Model& mdls);
    
        /** 
         * @brief Return the top devs::Time of the devs::EventTable.
         * @return A devs::Time.
         */
	const Time& getNextTime();

        /** 
         * @brief Pop the next devs::CompleteEventBagModel from the
         * devs::EventTable and call devs::Simulator function.
         * @return 0.
         */
        ExternalEventList* run();

        /** 
         * @brief Delete all devs::Simulator and all devs::View of this
         * simulator.
         */
	void finish();

        //
        ///
        //// Functions use by Executive models to manage DsDevs simulation.
        ///
        //

        /** 
         * @brief Add a permanent vpz::Dynamic into cache.
         * @param dynamics The new vpz::Dynamic to push into cache.
         * @throw utils::InternalError if dynamics already exist.
         */
        void addPermanent(const vpz::Dynamic& dynamics);

        /** 
         * @brief Add a permanent vpz::Condition into cache.
         * @param condition The new vpz::Condition to push into cache.
         * @throw utils::InternalError if condition already exist.
         */
        void addPermanent(const vpz::Condition& condition);
        
        /** 
         * @brief Add a permanent vpz::Observable into cache.
         * @param observable The new vpz::Observable to push into cache.
         * @throw utils::InternalError if observable already exist.
         */
        void addPermanent(const vpz::Observable& observable);

        /** 
         * @brief Build a new devs::Simulator from the dynamics library. Attach
         * to this model information of dynamics, condition and observable.
         * @param model the graph::AtomicModel reference source of
         * devs::Simulator.
         * @param dynamics the name of the dynamics to attach.
         * @param condition the name of the condition to attach.
         * @param observable the name of the observable to attach.
         * @throw utils::InternalError if dynamics not exist.
         */
        void createModel(graph::AtomicModel* model,
                         const std::string& dynamics,
                         const vpz::StringVector& conditions,
                         const std::string& observable);

        /** 
         * @brief Build a new devs::Simulator from the vpz::Classes information.
         * @param classname the name of the class to clone.
         * @param parent the parent of the model.
         * @param modelname the new name of the model.
         * @throw utils::badArg if modelname already exist.
         */
        graph::Model* createModelFromClass(const std::string& classname,
                                           graph::CoupledModel* parent,
                                           const std::string& modelname);
        
        /** 
         * @brief Add an observable, ie. a reference and a model to the
         * specified view.
         * @param model the model to attach to the view.
         * @param portname the port of the model to attach.
         * @param view the view.
         */
        void addObservableToView(graph::AtomicModel* model,
                                 const std::string& portname,
                                 const std::string& view);

        /** 
         * @brief Delete the specified model from coupled model. All
         * connection are deleted, Simulator are deleted and all events are
         * deleted from event table.
         * @param parent the coupled model parent of model to delete.
         * @param modelname the name of model to delete.
         */
        void delModel(graph::CoupledModel* parent,
                      const std::string& modelname);

        //
        ///
        //// Some usefull functions.
        ///
        //
        
        /** 
         * @brief Attach the specified simulator to the graph::AtomicModel and
         * install it on bus.
         * @param model 
         * @param simulator 
         */
        void addModel(graph::AtomicModel* model, Simulator* simulator);

	/**
	 * Return the devs::Simulator with a specified graph::AtomicModel.
         * Complexity: log O(log(n)).
	 * @param model the atomicmodel reference to search.
	 * @return A reference to the devs::Simulator or 0 if not found.
	 */
	Simulator* getModel(const graph::AtomicModel* model) const;

	/**
	 * Return the devs::Simulator with a specified atomic model name.
         * Complexity: linear O(n).
	 * @param model the name of atomic model to search.
	 * @return a reference to the devs::Simulator or 0 if not found.
	 */
	Simulator* getModel(const std::string& model) const;

        /** 
         * @brief Return the devs::View from a specified View name.
         * Complexity: log O(log(n)).
         * @param name the name of the View to search.
         * @return A reference to the devs::View or 0 if not found.
         */
	devs::View* getView(const std::string& name) const;

        //
        ///
        //// Get/Set functions.
        ///
        //
        
        inline Time getCurrentTime() const
        { return m_currentTime; }

        inline const EventTable& eventtable() const
        { return m_eventTable; }

        inline EventTable& eventtable()
        { return m_eventTable; }

        inline const SimulatorMap& modellist() const
        { return m_modelList; }

        oov::PluginViewList outputs() const;

        /** 
         * @brief Friend member to give Coordinator access to excutive model.
         * @param exe The executive model.
         */
        void setCoordinator(Executive& exe);

    private:
	Time                        m_currentTime;
	SimulatorMap                m_modelList;
	EventTable                  m_eventTable;
	ViewList                    m_viewList;
	EventViewList               m_eventViewList;
	TimedViewList               m_timedViewList;
        FinishViewList              m_finishViewList;
        ModelFactory&               m_modelFactory;
        SimulatorList               m_deletedSimulator;
        SimulatorList::size_type    m_toDelete;

        void buildViews();

	void addView(devs::View* view);

        void processInternalEvent(Simulator* sim,
                                  EventBagModel& modelbag);

        void processExternalEvents(Simulator* sim,
                                   EventBagModel& modelbag);

        void processRequestEvents(Simulator* sim,
                                        EventBagModel& modelbag);

        void processTimedObservationEvents(CompleteEventBagModel& bag);

        /** 
         * @brief build the simulator from the vpz::Model stock.
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
         * @brief Delete the atomic model from Devs::Graph, the Simulator 
         * from Coordinator and clean all events on devs::EventTable. Do not use
         * the AtomicModel after this function, it is delete.
         * 
         * @param parent a reference to the parent.
         * @param atom the model to delete.
         */
        void delAtomicModel(graph::CoupledModel* parent,
                            graph::AtomicModel* atom);

        /** 
         * @brief Delete the coupled model from Devs::Graph. All the
         * Simulator from Coordinator and clean all events on
         * devs::EventTable.  Do not use the AtomicModel child or CoupledModel
         * after this function.  All are delete. This function is recursive.
         * 
         * @param parent a reference to the parent.
         * @param mdl the model to delete.
         */
        void delCoupledModel(graph::CoupledModel* parent,
                             graph::CoupledModel* mdl);

        void processEventView(Simulator& model, Event* event = 0);

        /**
         * Set a new date to the Coordinator.
         * @param time the new date to affect Coordinator.
         */
        inline void updateCurrentTime(const Time& time)
        { m_currentTime = time; }

        /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */
        
        /** 
         * @brief A functor to be use with std::for_each to facilitate
         * transformation from an ViewList to the PluginViewList.
         */
        struct GetSerializablePlugins
        {
            oov::PluginViewList& lst;

            GetSerializablePlugins(oov::PluginViewList& lst) : lst(lst) { }

            inline void operator()(const ViewList::value_type& x)
            { if (x.second->plugin()->isSerializable()) {
                  lst.insert(std::make_pair(x.first, x.second->plugin())); }}
        };
    };

}} // namespace vle devs

#endif
