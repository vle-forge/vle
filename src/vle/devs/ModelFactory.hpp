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
#include <vle/vpz/Dynamics.hpp>
#include <vle/vpz/Experiment.hpp>
#include <glibmm/module.h>
#include <boost/noncopyable.hpp>

namespace vle { namespace devs {

    class Coordinator;
    class Simulator;

    /** 
     * @brief ModuleList is a cache for store the Glib::Module a get only
     * reference to build new devs::Dynamics without loading the dynamic library
     * if is was already loaded.
     */
    typedef std::map < std::string, Glib::Module* > ModuleList;


    /** 
     * @brief Module cache allow the store de Glib::Module of the ModelFactory.
     * Usefull to only store unique Glib::Module when using multiples instances
     * of a devs::Dynamics.
     */
    class ModuleCache
    {
    public:
        /** 
         * @brief Delete all the Glib::Module from the cache.
         */
        ~ModuleCache();

        /** 
         * @brief Add a new couple ModuleId, Glib::Module to the cache.
         * @param library The name of the library.
         * @param module The dynamic library.
         * @throw utils::InternalError if Module already exist.
         */
        void add(const std::string& library, Glib::Module* module);

        /** 
         * @brief Check if a couple ModuleId, Glib::Module already exist into
         * the cache.
         * @param library The name of the library.
         * @return true if the Module already exist, false otherwise.
         */
        bool exist(const std::string& library) const;

        /** 
         * @brief Get a reference to the loaded Glib::Module.
         * @param library The name of the library.
         * @return A reference to a loaded Glib::Module or 0 if the library is
         * not in the cache.
         */
        Glib::Module* get(const std::string& library) const;

    private:
        ModuleList  m_lst;

        /** 
         * @brief A functor to delete all Glib::Module of
         * ModuleList::value_type. To use with the for_each algorith.
         */
        struct DeleteModule
        {
            void operator()(ModuleList::value_type& x) const
            { delete x.second; x.second = 0; }
        };
    };

    /**
     * @brief Read simulations plugin from models directories and manage models
     * classes.
     */
    class ModelFactory : boost::noncopyable
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
        ~ModelFactory();

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
         * @throw utils::InternalError if dynamics not exist.
         */
        void createModel(Coordinator& coordinator,
                         graph::AtomicModel* model,
                         const std::string& dynamics,
                         const std::string& condition,
                         const std::string& observable);

        /** 
         * @brief Build a list of devs::Simulator from the dynamics library
         * corresponding to the atomic models from the specified graph
         * hierarchy.
         * @param coordinator the coordinator where attach the simulator.
         * @param model the hierachy of model (coupled model) or atomic model.
         */
        void createModels(Coordinator& coordinator, const vpz::Model& mdl);

        /** 
         * @brief Build a new devs::Simulator from the vpz::Classes information.
         * @param classname the name of the class to clone.
         * @param modelname the new name of the model.
         * @throw utils::badArg if modelname already exist or if the classname
         * doest not exist.
         */
        graph::Model* createModelFromClass(Coordinator& coordinator,
                                           graph::CoupledModel* parent,
                                           const std::string& classname,
                                           const std::string& modelname);

    private:
        vpz::Dynamics           mDynamics;
        vpz::Classes            mClasses;
        vpz::Experiment         mExperiment;
        vpz::AtomicModelList    mAtoms;
        ModuleCache             mModule;

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
