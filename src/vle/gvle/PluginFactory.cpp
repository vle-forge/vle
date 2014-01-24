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


#include <vle/gvle/PluginFactory.hpp>
#include <vle/utils/ModuleManager.hpp>
#include <vle/utils/Algo.hpp>
#include <map>

namespace vle { namespace gvle {

typedef std::map < void *, GlobalPluginPtr > GlobalPluginList;
typedef std::map < void *, ModelingPluginPtr > ModelingPluginList;
typedef std::map < void *, OutputPluginPtr > OutputPluginList;

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

    GlobalPluginList mGlobalPluginList;
    ModelingPluginList mModelingPluginList;
    OutputPluginList mOutputPluginList;

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

GlobalPluginPtr PluginFactory::getGlobalPlugin(const std::string& package,
					       const std::string& library,
					       GVLE* gvle)
{
    GlobalPluginPtr globalPluginPtr;
    GlobalPluginList::iterator it;

    void *symbol = NULL;

    symbol = mPimpl->mManager.get(package, library,
				  utils::MODULE_GVLE_GLOBAL);

    it = mPimpl->mGlobalPluginList.find(symbol);

    if (it != mPimpl->mGlobalPluginList.end()) {
        globalPluginPtr = it->second;
    } else {
        GvleGlobalPluginSlot fct = NULL;

        fct = utils::functionCast < GvleGlobalPluginSlot >(symbol);

        globalPluginPtr = GlobalPluginPtr(fct(package, library, gvle));
        mPimpl->mGlobalPluginList[symbol] = globalPluginPtr;
    }

    return globalPluginPtr;
}

GlobalPluginPtr PluginFactory::getGlobalPlugin(const std::string& pluginname,
					       GVLE* gvle)
{
    std::string package, library;

    buildPackageLibraryNames(pluginname, &package, &library);

    return getGlobalPlugin(package, library, gvle);
}

ModelingPluginPtr PluginFactory::getModelingPlugin(
        const std::string& package,
        const std::string& library,
        const std::string& curr_package)
{
    ModelingPluginPtr modelingPluginPtr;
    ModelingPluginList::iterator it;

    void *symbol = NULL;

    symbol = mPimpl->mManager.get(package, library,
                                  utils::MODULE_GVLE_MODELING);

    it = mPimpl->mModelingPluginList.find(symbol);

    if (it != mPimpl->mModelingPluginList.end()) {
        modelingPluginPtr = it->second;
    } else {
        GvleModelingPluginSlot fct = NULL;

        fct = utils::functionCast < GvleModelingPluginSlot >(symbol);

        modelingPluginPtr = ModelingPluginPtr(
                fct(package, library, curr_package));
        mPimpl->mModelingPluginList[symbol] = modelingPluginPtr;
    }

    return modelingPluginPtr;
}

ModelingPluginPtr PluginFactory::getModelingPlugin(
        const std::string& pluginname,
        const std::string& curr_package)
{
    std::string package, library;

    buildPackageLibraryNames(pluginname, &package, &library);

    return getModelingPlugin(package, library, curr_package);
}

OutputPluginPtr PluginFactory::getOutputPlugin(const std::string& package,
                                               const std::string& library)
{
    OutputPluginPtr outputPluginPtr;
    OutputPluginList::iterator it;

    void *symbol = NULL;

    symbol = mPimpl->mManager.get(package, library,
				  utils::MODULE_GVLE_OUTPUT);

    it = mPimpl->mOutputPluginList.find(symbol);

    if (it != mPimpl->mOutputPluginList.end()) {
        outputPluginPtr = it->second;
    } else {
        GvleOutputPluginSlot fct = NULL;

        fct = utils::functionCast < GvleOutputPluginSlot >(symbol);

        outputPluginPtr = OutputPluginPtr(fct(package, library));
        mPimpl->mOutputPluginList[symbol] = outputPluginPtr;
    }

    return outputPluginPtr;
}

OutputPluginPtr PluginFactory::getOutputPlugin(const std::string& pluginname)
{
    std::string package, library;

    buildPackageLibraryNames(pluginname, &package, &library);

    return getOutputPlugin(package, library);
}
void PluginFactory::getGvleGlobalPlugins(utils::ModuleList *lst)
{
    mPimpl->mManager.fill(utils::MODULE_GVLE_GLOBAL, lst);
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
