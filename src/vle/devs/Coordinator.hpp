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
#include <vle/devs/EventObserver.hpp>
#include <vle/devs/EventTable.hpp>
#include <vle/devs/Observer.hpp>
#include <vle/devs/Time.hpp>
#include <vle/devs/TimedObserver.hpp>
#include <vle/graph/Port.hpp>
#include <vle/graph/TargetPort.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Socket.hpp>
#include <libxml++/libxml++.h>

namespace vle { namespace devs {

    class ModelFactory;
    
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
        Coordinator(const vpz::Vpz& io, vpz::Model& mdls);

	~Coordinator();

	void addCondition(const std::string& modelName,
			  const std::string& portName,
			  value::Value* value);

	void addModels(vpz::Model& model);

	void addObserver(devs::Observer* observer);
    
        //
        ///
        //// Functions use by Executive models to manage DsDevs simulation.
        ///
        //
        
        /** 
         * @brief Build new models using ClassModel or Dynamic depends of
         * classNamed parameter that define ClassModel name or Dynamic name.
         * 
         * @param model the coupled where to add new models. 
         * @param className the ClassModel name or Dynamic name.
         * @param xmlDynamics the dynamics of all atomic model build.
         * @param xmlInit to initialise the models.
         * 
         * @return A reference to the top node of build model.
         */
        SimulatorList createModelFromClass(graph::CoupledModel* parent,
                                           const std::string& classname);


        Simulator* createModel(graph::AtomicModel* model,
                               const vpz::Dynamic& dyn,
                               const vpz::Condition& cond,
                               const vpz::Observable& obs);

        SimulatorList* createModels(graph::CoupledModel* model,
                                    const vpz::Dynamics& dyns,
                                    const vpz::Conditions& conds,
                                    const vpz::Observables& views);
        /** 
         * @brief Delete the specified model from coupled model. All
         * connection are deleted, Simulator are deleted and all events are
         * deleted from event table.
         * 
         * @param parent the coupled model parent of model to delete.
         * @param modelname the name of model to delete.
         */
        bool delModel(graph::CoupledModel* parent,
                      const std::string& modelname);

	void finish();

	inline Time getCurrentTime()const { return m_currentTime; }

	/**
	 * Return the Simulator with a specified AtomicModel.
	 *
	 * @param model the atomicmodel reference to search, O(N).
	 *
	 * @return
	 */
	Simulator* getModel(graph::AtomicModel* model) const;

	/**
	 * Return the Simulator with a specified Atomic model name.
	 *
	 * @param model the name of atomic model to search O(N).
	 *
	 * @return a reference to Simulator or 0.
	 */
	Simulator* getModel(const std::string& model) const;

	devs::Observer* getObserver(std::string const & name) const;

	const Time& getNextTime();

        /** 
         * @brief Initialise Coordinator before running simulation. Rand is
         * initialized, send to all Simulator the first init event found and
         * call for each Simulator the processInitEvent. Before this,
         * dispatchStateEvent is call for all StateEvent.
         *
         * @throw Exception::Internal if a condition have no model port name
         * associed.
         */
        void init();

        void parseExperiment();

	ExternalEventList* run();

    private:
        vpz::Experiment         m_experiment;
	Time m_currentTime;

	// Liste des modeles atomiques geres par le simulateur
	SimulatorMap m_modelList;

	// Liste des ports d'entree connectes aux ports de sortie
	//  des modeles atomiques geres par le simulateur
	std::map < std::pair < std::string , std::string > ,
		   std::vector < graph::TargetPort* > > m_targetPortMap;

	// Modele couple racine

	// La fabrique de modèles
	ModelFactory* m_modelFactory;

	// Echeancier
	EventTable m_eventTable;

	// Liste des conditions experimentales
	std::map < std::string ,
		   std::vector < std::pair < std::string ,
					     value::Value* > > > m_conditionList;

	// Liste des observateurs
	std::map < std::string , devs::Observer* > m_observerList;

	std::map < std::string ,
		   std::vector < devs::EventObserver* > > m_eventObserverList;

	std::vector < devs::TimedObserver* > m_timedObserverList;


        void processInternalEvent(Simulator* sim,
                                  EventBagModel& modelbag,
                                  CompleteEventBagModel& bag);

        void processExternalEvents(Simulator* sim,
                                   EventBagModel& modelbag,
                                   CompleteEventBagModel& bag);

        void processInstantaneousEvents(Simulator* sim,
                                        EventBagModel& modelbag,
                                        CompleteEventBagModel& bag);

        void processStateEvents(CompleteEventBagModel& bag);


        /**
         * Read all ExternalEventList including External and Instantaneous
         * events and found the destination models. If event is an
         * Instantaneous then, a new Event is attached to
         * CompleteEventBagModel otherwise, if event is an External then is
         * push int the Schuduller.
         *
         * @param eventList the external event list to treat.
         * @param bags output parameter to add Instantaneous events.
         * @param sim the simulator that dispatch external events.
         */
        void dispatchExternalEvent(ExternalEventList& eventList,
                                   CompleteEventBagModel& bags,
                                   Simulator* sim);

	void dispatchInternalEvent(InternalEvent * event);

	void dispatchStateEvent(StateEvent * event);

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

        void getTargetPortList(graph::AtomicModel* model,
                               const std::string& portName,
                               graph::TargetModelList& out);

        void startEOVStream();

        /* - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - */

        /** 
         * @brief Search all measures when outputs is in a ouput::Net format.
         */
        void startNetStream();

        /** 
         * @brief Build the devs::Stream object and attach devs::Observer to the
         * output::Net using the vpz::Observable information.
         * 
         * @param measure the measure who push data to the stream.
         * @param output the concerned output.
         */
        void startNetStream(const vpz::View& view,
                            const vpz::Output& output);
        
        /* - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - */

        void startLocalStream();
        
        void attachModelToObserver(Observer* obs, const std::string& viewname);

        void processEventObserver(Simulator& model, Event* event = 0);

        /**
         * Set a new date to the Coordinator.
         *
         * @param time the new date to affect Coordinator.
         */
        inline void updateCurrentTime(const Time& time)
        { m_currentTime = time; }

        /** 
         * @brief Build a new Stream plugin using specified output value.
         * 
         * @param o the output to get a new Stream.
         * 
         * @return a reference to the loaded plugin.
         *
         * @throw Exception::File if problem loading file.
         */
        vle::devs::Stream* getStreamPlugin(const vpz::Output& o);
    };

}} // namespace vle devs

#endif
