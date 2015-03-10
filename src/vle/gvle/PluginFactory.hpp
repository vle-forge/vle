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


#ifndef VLE_GVLE_PLUGINFACTORY_HPP
#define VLE_GVLE_PLUGINFACTORY_HPP 1

#include <vle/gvle/DllDefines.hpp>
#include <vle/utils/ModuleManager.hpp>
#include <vle/gvle/GlobalPlugin.hpp>
#include <vle/gvle/ModelingPlugin.hpp>
#include <vle/gvle/OutputPlugin.hpp>

namespace vle { namespace gvle {

/**
 * @brief Use a @e vle::utils::ModuleManager to manage modules.
 *
 * The @vle::utils::ModuleManager is a no-copyable and no-assignable class to
 * avoid modules problems.
 */
class GVLE_API PluginFactory
{
public:
    /**
     * @brief Build a cache with all installed modules.
     *
     * Use the @e vle::utils::ModuleManager to browse all installed modules in
     * simulators, output, gvle's modeling and gvle's output directory.
     */
    PluginFactory();

    /**
     * @brief Delete all loaded plug-ins.
     */
    ~PluginFactory();

    /**
     * @brief Update the cache with all new installed modules.
     */
    void update();

    GlobalPluginPtr getGlobalPlugin(const std::string& package,
				    const std::string& library,
				    GVLE* gvle);

    GlobalPluginPtr getGlobalPlugin(const std::string& pluginname,
				    GVLE* gvle);

    ModelingPluginPtr getModelingPlugin(const std::string& package,
                                        const std::string& library,
                                        const std::string& curr_package);

    ModelingPluginPtr getModelingPlugin(const std::string& pluginname,
                                        const std::string& curr_package);

    OutputPluginPtr getOutputPlugin(const std::string& package,
                                    const std::string& library);

    OutputPluginPtr getOutputPlugin(const std::string& pluginname);

    void getGvleGlobalPlugins(utils::ModuleList *lst);

    void getGvleModelingPlugins(utils::ModuleList *lst);

    void getGvleOutputPlugins(utils::ModuleList *lst);

    void getOutputPlugins(utils::ModuleList *lst);

    void getDynamicsPlugins(utils::ModuleList *lst);

    void getDynamicsPlugins(const std::string& package, utils::ModuleList *lst);

    static void buildPluginName(const std::string& package,
                                const std::string& library,
                                std::string *pluginname);

    static void buildPackageLibraryNames(const std::string& pluginname,
                                         std::string *package,
                                         std::string *library);

private:
    PluginFactory(const PluginFactory& other);
    PluginFactory& operator=(const PluginFactory& other);

    class Pimpl; /**< We hide the implementation details. */
    Pimpl *mPimpl;

    /**
     * @brief Get a reference to the @e vle::utils::ModuleManager.
     *
     * @return A reference to the @e vle::utils::ModuleManager.
     */
    utils::ModuleManager& getManager();

};

}} // namespace vle gvle

#endif
