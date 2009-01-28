/**
 * @file vle/devs/Executive.hpp
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


#ifndef VLE_DEVS_EXECUTIVE_HPP
#define VLE_DEVS_EXECUTIVE_HPP

#include <vle/devs/Coordinator.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/vpz/Dynamics.hpp>
#include <vle/vpz/Conditions.hpp>
#include <vle/vpz/Observables.hpp>

namespace vle { namespace devs {

    class Simulator;

    /**
     * @brief Dynamics class for the Barros DEVS extension. Provite graph
     * manipulation at runtime using namespace graph and the coordinator. By
     * default, this model does nothing and it will be inherit to build
     * simulation components.
     */
    class Executive : public Dynamics
    {
    public:
	/**
	 * @brief Constructor of the Executive of an atomic model
	 * @param model the atomic model to which belongs the dynamics
	 */
        Executive(const graph::AtomicModel& model,
                  const devs::InitEventList& events) :
            Dynamics(model, events),
            m_coordinator(0)
        { }

	/**
	 * @brief Destructor (nothing to do)
	 * @return none
	 */
        virtual ~Executive()
        { }

        /**
         * @brief If this function return true, then a cast to an Executive
         * object is produce and the set_coordinator function is call. Executive
         * permit to manipulate graph::CoupledModel and devs::Coordinator at
         * runtime of the simulation.
         * @return false if Dynamics is not an Executive.
         */
        inline virtual bool isExecutive() const
        { return true; }

        ////
        //// Functions used by Executive models to manage DsDevs simulation.
        ////

        /**
         * @brief Get a constant reference to the list of vpz::Dynamics objects.
         * @return A constant reference to the list of vpz::Dynamics objects.
         */
        const vpz::Dynamics& dynamics() const
        { return coordinator().dynamics(); }

        /**
         * @brief Get a reference to the list of vpz::Dynamics objects.
         * @return A reference to the list of vpz::Dynamics objects.
         */
        vpz::Dynamics& dynamics()
        { return coordinator().dynamics(); }

        /**
         * @brief Get a constant reference to the list of vpz::Conditions objects.
         * @return A constant reference to the list of vpz::Conditions objects.
         */
        const vpz::Conditions& conditions() const
        { return coordinator().conditions(); }

        /**
         * @brief Get a reference to the list of vpz::Conditions objects.
         * @return A reference to the list of vpz::Conditions objects.
         */
        vpz::Conditions& conditions()
        { return coordinator().conditions(); }

        /**
         * @brief Get a constant reference to the list of vpz::Observables objects.
         * @return A constant reference to the list of vpz::Observables objects.
         */
        const vpz::Observables& observables() const
        { return coordinator().observables(); }

        /**
         * @brief Get a reference to the list of vpz::Conditions objects.
         * @return A reference to the list of vpz::Conditions objects.
         */
        vpz::Observables& observables()
        { return coordinator().observables(); }

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
        void createModel(graph::AtomicModel* model, const std::string& dynamics,
                         const vpz::Strings& conditions,
                         const std::string& observable)
        { return coordinator().createModel(model, dynamics, conditions, observable); }

        /**
         * @brief Build a new devs::Simulator from the vpz::Classes information.
         * @param classname the name of the class to clone.
         * @param parent the parent of the model.
         * @param modelname the new name of the model.
         * @throw utils::badArg if modelname already exist.
         */
        graph::Model* createModelFromClass(const std::string& classname,
                                           graph::CoupledModel* parent,
                                           const std::string& modelname)
        { return coordinator().createModelFromClass(classname, parent, modelname); }

        /**
         * @brief Add an observable, ie. a reference and a model to the
         * specified view.
         * @param model the model to attach to the view.
         * @param portname the port of the model to attach.
         * @param view the view.
         */
        void addObservableToView(graph::AtomicModel* model,
                                 const std::string& portname,
                                 const std::string& view)
        { coordinator().addObservableToView(model, portname, view); }

        /**
         * @brief Delete the specified model from coupled model. All
         * connection are deleted, Simulator are deleted and all events are
         * deleted from event table.
         * @param parent the coupled model parent of model to delete.
         * @param modelname the name of model to delete.
         */
        void delModel(graph::CoupledModel* parent, const std::string& modelname)
        { coordinator().delModel(parent, modelname); }

        /**
         * @brief Get a reference to the current coupled model.
         * @return A constant reference to the coupled model.
         */
        const graph::CoupledModel& coupledmodel() const;

        /**
         * @brief Get a reference to the current coupled model.
         * @return A reference to the coupled model.
         */
        graph::CoupledModel& coupledmodel();

        ////
        //// Function to assign a coordinator reference to the executive object.
        ////

        /**
         * @brief Assign a coordinator to the Executive model to give access to
         * vle::graph, vle::devs and vle::vpz API.
         * @param coordinator A reference to the coordinator.
         */
        friend void Coordinator::setCoordinator(Executive& exe);

    private:
        Coordinator& coordinator();

        const Coordinator& coordinator() const;

        Coordinator*    m_coordinator; /** Reference to the coordinator object
                                         to build, delete or construct new
                                         models at runtime. */

        void isInitialized() const;
    };

}} // namespace vle devs

#endif
