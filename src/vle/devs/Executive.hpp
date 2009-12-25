/**
 * @file vle/devs/Executive.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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

#include <vle/devs/DllDefines.hpp>
#include <vle/devs/Coordinator.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/vpz/Dynamics.hpp>
#include <vle/vpz/Conditions.hpp>
#include <vle/vpz/Observables.hpp>

#define DECLARE_EXECUTIVE(mdl) \
    extern "C" { \
        vle::devs::Dynamics* \
        makeNewExecutive(const vle::devs::ExecutiveInit& init, \
                         const vle::devs::InitEventList& events) \
        { return new mdl(init, events); } \
    }

#define DECLARE_NAMED_EXECUTIVE(name, mdl) \
    extern "C" { \
        vle::devs::Dynamics* \
        makeNewExecutive##name(const vle::devs::ExecutiveInit& init, \
                               const vle::devs::InitEventList& events) \
        { return new mdl(init, events); } \
    }

namespace vle { namespace devs {

    class Simulator;

    class VLE_DEVS_EXPORT ExecutiveInit : public DynamicsInit
    {
    public:
        ExecutiveInit(const graph::AtomicModel& model,
                      utils::Rand& rnd,
                      PackageId packageid,
                      Coordinator& coordinator)
            : DynamicsInit(model, rnd, packageid), m_coordinator(coordinator)
        {}

        virtual ~ExecutiveInit()
        {}

        Coordinator& coordinator() const { return m_coordinator; }

    private:
        Coordinator& m_coordinator;
    };

    /**
     * @brief Dynamics class for the Barros DEVS extension. Provite graph
     * manipulation at runtime using namespace graph and the coordinator. By
     * default, this model does nothing and it will be inherit to build
     * simulation components.
     */
    class VLE_DEVS_EXPORT Executive : public Dynamics
    {
    public:
	/**
	 * @brief Constructor of the Executive of an atomic model
	 * @param model the atomic model to which belongs the dynamics
	 */
        Executive(const ExecutiveInit& init,
                  const InitEventList& events)
            : Dynamics(init, events), m_coordinator(init.coordinator())
        {}

	/**
	 * @brief Destructor (nothing to do)
	 * @return none
	 */
        virtual ~Executive()
        {}

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
        { return m_coordinator.dynamics(); }

        /**
         * @brief Get a reference to the list of vpz::Dynamics objects.
         * @return A reference to the list of vpz::Dynamics objects.
         */
        vpz::Dynamics& dynamics()
        { return m_coordinator.dynamics(); }

        /**
         * @brief Get a constant reference to the list of vpz::Conditions
         * objects.
         * @return A constant reference to the list of vpz::Conditions objects.
         */
        const vpz::Conditions& conditions() const
        { return m_coordinator.conditions(); }

        /**
         * @brief Get a reference to the list of vpz::Conditions objects.
         * @return A reference to the list of vpz::Conditions objects.
         */
        vpz::Conditions& conditions()
        { return m_coordinator.conditions(); }

        /**
         * @brief Get a constant reference to the list of vpz::Observables
         * objects.
         * @return A constant reference to the list of vpz::Observables objects.
         */
        const vpz::Observables& observables() const
        { return m_coordinator.observables(); }

        /**
         * @brief Get a reference to the list of vpz::Conditions objects.
         * @return A reference to the list of vpz::Conditions objects.
         */
        vpz::Observables& observables()
        { return m_coordinator.observables(); }

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
        { m_coordinator.addObservableToView(model, portname, view); }

        // / / / /
        //
        // DynamicDEVS API
        //
        // / / / /

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
        const graph::AtomicModel*
            createModel(
                const std::string& name,
                const std::vector < std::string >& inputs,
                const std::vector < std::string >& outputs,
                const std::string& dynamics,
                const vpz::Strings& conditions,
                const std::string& observable);

        /**
         * @brief Build a new devs::Simulator from the vpz::Classes information.
         * @param classname the name of the class to clone.
         * @param parent the parent of the model.
         * @param modelname the new name of the model.
         * @throw utils::badArg if modelname already exist.
         */
        const graph::Model*
            createModelFromClass(
                const std::string& classname,
                const std::string& modelname);

        /**
         * @brief Delete the specified model from coupled model. All
         * connection are deleted, Simulator are deleted and all events are
         * deleted from event table.
         * @param modelname the name of model to delete.
         */
        void delModel(const std::string& modelname);

        void addConnection(const std::string& modelsource,
                           const std::string& outputport,
                           const std::string& modeldestination,
                           const std::string& inputport);
        void removeConnection(const std::string& modelsource,
                              const std::string& outputport,
                              const std::string& modeldestination,
                              const std::string& inputport);
        void addInputPort(const std::string& modelName,
                          const std::string& portName);
        void addOutputPort(const std::string& modelName,
                           const std::string& portName);
        void removeInputPort(const std::string& modelName,
                             const std::string& portName);
        void removeOutputPort(const std::string& modelName,
                              const std::string& portName);


        // / / / /
        //
        // Give access to attributes
        //
        // / / / /

        /**
         * @brief Get a reference to the current coupled model.
         * @return A constant reference to the coupled model.
         */
        const graph::CoupledModel& coupledmodel() const
        { return *getModel().getParent(); }

        /**
         * @brief Get the name of the coupled model.
         * @return A constant reference to the name of the coupled model.
         */
        const std::string& coupledmodelName() const
        { return coupledmodel().getName(); }

    private:
        Coordinator& m_coordinator; /**< A reference to the coordinator of this
                                      executive to allow modification of
                                      coupled model. */

        /**
         * @brief Get a reference to the current coupled model.
         * @return A reference to the coupled model.
         */
        graph::CoupledModel* coupledmodel()
        { return getModel().getParent(); }
    };

}} // namespace vle devs

#endif
