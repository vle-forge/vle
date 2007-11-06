/**
 * @file devs/ModelFactory.hpp
 * @author The VLE Development Team.
 * @brief Read simulations plugin from models directories and manage models
 * classes.
 */

/*
 * Copyright (C) 2004, 05, 06 - The vle Development Team
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef DEVS_MODELFACTORY_HPP
#define DEVS_MODELFACTORY_HPP

#include <vle/graph/Model.hpp>
#include <vle/vpz/Classes.hpp>
#include <vle/vpz/Model.hpp>
#include <glibmm/module.h>

namespace vle { namespace devs {

    class Coordinator;
    class Simulator;

    /**
     * @brief Read simulations plugin from models directories and manage models
     * classes.
     */
    class ModelFactory
    {
    public:
        /** 
         * @brief Build a new ModelFactory using specified dynamics.
         * 
         * @param sim the simulator attached to this ModelFactory.
         * @param dyn the root dynamics of vpz::Dynamics to load.
         * @param cls the vpz::classes to parse vpz::Dynamics to load.
         */
        ModelFactory(const vpz::Dynamics& dyn,
                     const vpz::Classes& cls,
                     const vpz::Experiment& experiment,
                     const vpz::AtomicModelList& atom);

        /** 
         * @brief To delete all Glib::Module and class.
         */
        ~ModelFactory()
        { }

        /** 
         * @brief Return the list of atomics models information ie. the 4-uples
         * of graph::Model*, dynamics, conditions, observables and translators.
         * @return A constant reference to the vpz::AtomicModelList.
         */
        inline const vpz::AtomicModelList& atomics() const
        { return mAtomics; }

        /** 
         * @brief Return the reference to the list of initiale conditions for
         * each models.
         * @return A constant reference to the vpz::Conditions.
         */
        inline const vpz::Conditions& conditions() const
        { return mExperiment.conditions(); }

        /** 
         * @brief Return the reference to the list of views.
         * @return A constant reference to the vpz::Views.
         */
        inline const vpz::Views& views() const
        { return mExperiment.views(); }

        /** 
         * @brief Return the reference to the list of outputs.
         * @return A constant reference to the vpz::Outputs.
         */
        inline const vpz::Outputs& outputs() const
        { return mExperiment.views().outputs(); }

        /** 
         * @brief Return the reference to the experiment object.
         * @return A constant reference to the vpz::Experiment.
         */
        inline const vpz::Experiment& experiment() const
        { return mExperiment; }

        //
        ///
        /// Manage the ModelFactory cache ie. Atomic Model information of
        /// dynamics, conditions and observables value. 
        ///
        //

        /** 
         * @brief Remove all atomic model information that have no the tag
         * permantent in the VPZ format.
         */
        void cleanCache();

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
         * @param coordinator the coordinator where attach the simulator.
         * @param dynamics the name of the dynamics to attach.
         * @param condition the name of the condition to attach.
         * @param observable the name of the observable to attach.
         * @return A reference on the new Simulator builded.
         * @throw utils::InternalError if dynamics not exist.
         */
        Simulator* createModel(Coordinator& coordinator,
                               graph::AtomicModel* model,
                               const std::string& dynamics,
                               const std::string& condition,
                               const std::string& observable);

    private:
        vpz::Dynamics           mDynamics;
        vpz::Classes            mClasses;
        vpz::Experiment         mExperiment;
        vpz::AtomicModelList    mAtomics;

        /** 
         * @brief Load a new Glib::Module plugin from dynamic parameter in
         * default model dir or user model dir.
         * @param dyn A reference to the dynamic to load.
         * @return a reference to a Glib::Module.
         * @throw Exception::Internal if file is not found.
         */
        Glib::Module* buildPlugin(const vpz::Dynamic& dyn);

        /** 
         * @brief Load a new Glib::Module plugin for the specified dynamicname.
         * If dynamicname is not found in mDynamics variable it will search into
         * classes dynamics.
         * @param dynamicname the dynamic name to load.
         * @return a reference to a Glib::Module.
         * @throw Exception::Internal if file is not found or if dynamicname is
         * not found.
         */
        Glib::Module* getPlugin(const std::string& dynamicname);

        /** 
         * @brief Attach to the specified devs::Simulator reference a
         * devs::Dynamics structures load from a new Glib::Module.
         * @param coordinator the coordinator where attach the dynamics.
         * @param atom the devs::Simulator to attach devs::Dynamic.
         * @param dyn the io::Dynamic to initialise devs::Dynamic.
         * @param module the simulation dynamic library plugin.
         * @throw Exception::Internal if XML cannot be parse.
         */
        void attachDynamics(Coordinator& coordinator,
                            devs::Simulator* atom, const vpz::Dynamic& dyn,
                            Glib::Module* module);
    };

}} // namespace vle devs

#endif
