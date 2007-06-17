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

#include <vle/devs/Simulator.hpp>
#include <vle/devs/Coordinator.hpp>
#include <vle/graph/Model.hpp>
#include <vle/vpz/Dynamics.hpp>
#include <vle/vpz/Classes.hpp>
#include <vle/vpz/Model.hpp>
#include <map>
#include <glibmm/ustring.h>
#include <glibmm/module.h>

namespace vle { namespace devs {

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
        ModelFactory(Coordinator& coordinator,
                     const vpz::Dynamics& dyn,
                     const vpz::Classes& cls,
                     const vpz::AtomicModelList& atom);

        /** 
         * @brief To delete all Glib::Module and class.
         */
        ~ModelFactory()
        { }

	/**
         * @brief Return Simulator list to result map from Atomic Models
         * list.
	 *
	 * @param lst atomics models list.
	 * @param result output parameter to fill with Simulator.
         *
         * @return a list of newly Simulator build.
	 */
        devs::SimulatorList createModels(
                            const graph::AtomicModelVector& lst,
                            SimulatorMap& result);

        /** 
         * @brief Build a clone of stored class.
         * 
         * @param classname the classe name to clone.
         * @param lst a reference to a list for the new Simulator build.
         * @param result a reference to add
         * 
         * @return a devs::Hierarchy of models.
         *
         * @throw Exception::BadArgument if classname does not exist.
         */
	graph::Model* createModels(const std::string& classname,
                                   SimulatorList& lst,
                                   SimulatorMap& result);

    private:
	Coordinator&            mCoordinator;
        vpz::Dynamics           mDynamics;
        vpz::Classes            mClasses;
        vpz::AtomicModelList    mAtomics;


        /** 
         * @brief Return a Simulator list to result map from Atomic Models
         * list.
         * 
         * @param lst the atomic model list.
         * @param result the associated graph::AtomicModel to
         * devs::Simulator. Output parameter.
         * @param dyn the dynamics stock of dynamic. Can be the VPZ or a
         * classes.
         * 
         * @return 
         */
        devs::SimulatorList createModelsFromDynamics(
            const graph::AtomicModelVector& lst,
            SimulatorMap& result,
            const vpz::Dynamics& dyn);


        /** 
         * @brief Load a new Glib::Module plugin from dynamic parameter in
         * default model dir or user model dir.
         * 
         * @param dyn A reference to the dynamic to load.
         *
         * @return a reference to a Glib::Module.
         *
         * @throw Exception::Internal if file is not found.
         */
        Glib::Module* buildPlugin(const vpz::Dynamic& dyn);

        /** 
         * @brief Load a new Glib::Module plugin for the specified dynamicname.
         * If dynamicname is not found in mDynamics variable it will search into
         * classes dynamics.
         * 
         * @param dynamicname the dynamic name to load.
         * 
         * @return a reference to a Glib::Module.
         *
         * @throw Exception::Internal if file is not found or if dynamicname is
         * not found.
         */
        Glib::Module* getPlugin(const std::string& dynamicname);

        /** 
         * @brief Attach to the specified devs::Simulator reference a
         * devs::Dynamics structures load from a new Glib::Module.
         * 
         * @param atom the devs::Simulator to attach devs::Dynamic.
         * @param dyn the io::Dynamic to initialise devs::Dynamic.
         * @param module the simulation dynamic library plugin.
         *
         * @throw Exception::Internal if XML cannot be parse.
         */
        void attachDynamics(devs::Simulator* atom, const vpz::Dynamic& dyn,
                            Glib::Module* module);
    };

}} // namespace vle devs

#endif
