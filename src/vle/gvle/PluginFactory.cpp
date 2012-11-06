/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2012 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
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


#include <vle/gvle/PluginFactory.hpp>
#include <vle/gvle/OutputPlugin.hpp>
#include <vle/gvle/ModelingPlugin.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/utils/ModuleManager.hpp>
#include <vle/utils/Algo.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/i18n.hpp>
#include <cassert>

namespace vle { namespace gvle {

class PluginFactory::Pimpl
{
public:
    Pimpl()
    {
        mManager.browse(); /* Browse all the plug-ins available in every
                              packages. */
    }

    ~Pimpl()
    {
    }

    utils::ModuleManager mManager;

private:
    Pimpl(const Pimpl&);
    Pimpl& operator=(const Pimpl&);
};

PluginFactory::PluginFactory()
    : mPimpl(new PluginFactory::Pimpl())
{
}

PluginFactory::~PluginFactory()
{
    delete mPimpl;
}

void PluginFactory::update()
{
    mPimpl->mManager.browse();
}

ModelingPluginPtr PluginFactory::getModelingPlugin(const std::string& package,
                                                   const std::string& library)
{
    void *symbol = NULL;
    GvleModelingPluginSlot fct = NULL;

    symbol = mPimpl->mManager.get(package, library,
                                  utils::MODULE_GVLE_MODELING);

    fct = utils::functionCast < GvleModelingPluginSlot >(symbol);

    return ModelingPluginPtr(fct(package, library));
}

ModelingPluginPtr PluginFactory::getModelingPlugin(const std::string& pluginname)
{
    std::string package, library;

    buildPackageLibraryNames(pluginname, &package, &library);

    return getModelingPlugin(package, library);
}

OutputPluginPtr PluginFactory::getOutputPlugin(const std::string& pluginname)
{
    std::string package, library;

    buildPackageLibraryNames(pluginname, &package, &library);

    return getOutputPlugin(package, library);
}

OutputPluginPtr PluginFactory::getOutputPlugin(const std::string& package,
                                               const std::string& library)
{
    void *symbol = NULL;
    GvleOutputPluginSlot fct = NULL;

    symbol = mPimpl->mManager.get(package, library, utils::MODULE_GVLE_OUTPUT);

    fct = utils::functionCast < GvleOutputPluginSlot >(symbol);

    return OutputPluginPtr(fct(package, library));
}

void PluginFactory::getGvleModelingPlugins(utils::ModuleList *lst)
{
    mPimpl->mManager.fill(utils::MODULE_GVLE_MODELING, lst);
}

void PluginFactory::getGvleOutputPlugins(utils::ModuleList *lst)
{
    mPimpl->mManager.fill(utils::MODULE_GVLE_OUTPUT, lst);
}

void PluginFactory::getOutputPlugins(utils::ModuleList *lst)
{
    mPimpl->mManager.fill(utils::MODULE_OOV, lst);
}

void PluginFactory::getDynamicsPlugins(utils::ModuleList *lst)
{
    mPimpl->mManager.fill(utils::MODULE_DYNAMICS, lst);
}

void PluginFactory::getDynamicsPlugins(const std::string& package,
                                       utils::ModuleList *lst)
{
    mPimpl->mManager.fill(package, utils::MODULE_DYNAMICS, lst);
}

void PluginFactory::buildPluginName(const std::string& package,
                                    const std::string& library,
                                    std::string *pluginname)
{
    *pluginname = package + "/" + library;
}

void PluginFactory::buildPackageLibraryNames(const std::string& pluginname,
                                             std::string *package,
                                             std::string *library)
{
    std::string::size_type index = pluginname.find('/');

    if (index != std::string::npos and index != pluginname.size()) {
        package->assign(pluginname, 0, index);
        library->assign(pluginname, index + 1, std::string::npos);
    } else {
        throw utils::ArgError(fmt(
                _("Bad name for a plug-in: `%1%'")) % pluginname);
    }
}

}} // namespace vle gvle
