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


#ifndef DEVS_MODELFACTORY_HPP
#define DEVS_MODELFACTORY_HPP

#include <vle/DllDefines.hpp>
#include <vle/vpz/BaseModel.hpp>
#include <vle/vpz/Classes.hpp>
#include <vle/vpz/Dynamics.hpp>
#include <vle/vpz/Model.hpp>
#include <vle/vpz/Experiment.hpp>
#include <vle/devs/InitEventList.hpp>
#include <vle/devs/ExternalEventList.hpp>
#include <vle/utils/ModuleManager.hpp>
#include <boost/noncopyable.hpp>

namespace vle { namespace devs {

class RootCoordinator;
class Coordinator;
class Simulator;
class Dynamics;


/**
 * @brief Read simulations plugin from models directories and manage models
 * classes.
 */
class VLE_API ModelFactory : boost::noncopyable
{
public:
    /**
     * @brief Build a new ModelFactory using specified dynamics.
     *
     * @param sim the simulator attached to this ModelFactory.
     * @param dyn the root dynamics of vpz::Dynamics to load.
     * @param cls the vpz::classes to parse vpz::Dynamics to load.
     */
    ModelFactory(const utils::ModuleManager& modulemgr,
                 const vpz::Dynamics& dyn,
                 const vpz::Classes& cls,
                 const vpz::Experiment& experiment,
                 RootCoordinator& root);

    /**
     * @brief Return the reference to the list of initiale conditions for
     * each models.
     * @return A constant reference to the vpz::Conditions.
     */
    inline const vpz::Conditions& conditions() const
    { return mExperiment.conditions(); }

    /**
     * @brief Return the reference to the list of dynamcis.
     * @return A constant reference to the vpz::Dynamics.
     */
    inline const vpz::Dynamics& dynamics() const
    { return mDynamics; }

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

    /**
     * @brief Return the reference to the observables object.
     * @return A constant reference to the vpz::Observables.
     */
    inline const vpz::Observables& observables() const
    { return mExperiment.views().observables(); }

    /**
     * @brief Return the reference to the list of initiale conditions for
     * each models.
     * @return A reference to the vpz::Conditions.
     */
    inline vpz::Conditions& conditions()
    { return mExperiment.conditions(); }

    /**
     * @brief Return the reference to the list of dynamcis.
     * @return A constant reference to the vpz::Dynamics.
     */
    inline vpz::Dynamics& dynamics()
    { return mDynamics; }

    /**
     * @brief Return the reference to the list of views.
     * @return A reference to the vpz::Views.
     */
    inline vpz::Views& views()
    { return mExperiment.views(); }

    /**
     * @brief Return the reference to the list of outputs.
     * @return A reference to the vpz::Outputs.
     */
    inline vpz::Outputs& outputs()
    { return mExperiment.views().outputs(); }

    /**
     * @brief Return the reference to the experiment object.
     * @return A reference to the vpz::Experiment.
     */
    inline vpz::Experiment& experiment()
    { return mExperiment; }

    /**
     * @brief Return the reference to the observables object.
     * @return A constant reference to the vpz::Observables.
     */
    inline vpz::Observables& observables()
    { return mExperiment.views().observables(); }

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
     * @param model the vpz::AtomicGraphModel reference source of
     * devs::Simulator.
     * @param coordinator the coordinator where attach the simulator.
     * @param dynamics the name of the dynamics to attach.
     * @param condition the name of the condition to attach.
     * @param observable the name of the observable to attach.
     * @throw utils::InternalError if dynamics not exist.
     */
    void createModel(Coordinator& coordinator,
                     vpz::AtomicModel* model,
                     const std::string& dynamics,
                     const std::vector < std::string >& conditions,
                     const std::string& observable);

    /**
     * @brief Build a list of devs::Simulator from the dynamics library
     * corresponding to the atomic models from the specified graph
     * hierarchy.
     * @param coordinator the coordinator where attach the simulator.
     * @param model the hierachy of model (coupled model) or atomic model.
     */
    void createModels(Coordinator& coordinator, const vpz::Model& vpmdl);

    /**
     * @brief Build a new devs::Simulator from the vpz::Classes information.
     * @param classname the name of the class to clone.
     * @param modelname the new name of the model.
     * @throw utils::badArg if modelname already exist or if the classname
     * doest not exist.
     */
    vpz::BaseModel* createModelFromClass(Coordinator& coordinator,
                                       vpz::CoupledModel* parent,
                                       const std::string& classname,
                                       const std::string& modelname);

private:
    ModelFactory(const ModelFactory& other);
    ModelFactory& operator=(const ModelFactory& other);

    const utils::ModuleManager& mModuleMgr; /**< A reference to the
                                              utils::ModuleManager. */

    vpz::Dynamics           mDynamics; /**< List of available vpz::Dynamics. */
    vpz::Classes            mClasses; /**< List of available vpz::Classes. */
    vpz::Experiment         mExperiment; /**< A reference to the
                                           vpz::Experiment. */
    RootCoordinator&        mRoot;

    /**
     * Try to open the plug-in and return the type of opened plugin
     * (MODULE_DYNAMICS, MODULE_DYNAMICS_WRAPPER or MODULE_EXECUTIVE).
     *
     * @param dyn
     * @param path [out] assign the path of the plug-in.
     *
     * @throw utils::ArgError if an error occured during the loading of the
     * shared library.
     *
     * @return Return a constant.
     */
    utils::ModuleType open(const vpz::Dynamic& dyn,
                           std::string* path);

    /**
     * @brief Attach to the specified devs::Simulator reference a
     * devs::Dynamics structures load from a new Glib::Module.
     * @param coordinator the coordinator where attach the dynamics.
     * @param atom the devs::Simulator to attach devs::Dynamic.
     * @param dyn the io::Dynamic to initialise devs::Dynamic.
     * @param module the simulation dynamic library plugin.
     * @return A pointer to the allocated dynamics.
     * @throw Exception::Internal if XML cannot be parse.
     */
    devs::Dynamics* attachDynamics(Coordinator& coordinator,
                                   devs::Simulator* atom,
                                   const vpz::Dynamic& dyn,
                                   const InitEventList& events);
};

}} // namespace vle devs

#endif
