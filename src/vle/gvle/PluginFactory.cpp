/*
 * @file vle/gvle/PluginFactory.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#include <vle/gvle/PluginFactory.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Algo.hpp>
#include <vle/utils/i18n.hpp>
#include <cassert>

namespace vle { namespace gvle {

PluginFactory::PluginFactory()
{
    readOutputPlugins();
    readModelingPlugins();
    readOovPlugins();
}

PluginFactory::~PluginFactory()
{
}

void PluginFactory::update()
{
    readOutputPlugins();
    readModelingPlugins();
    readOovPlugins();
}

PluginFactory::OutputPluginList::iterator PluginFactory::getO(
    const std::string& name)
{
    OutputPluginList::iterator it = m_outs.find(name);
    if (it == m_outs.end()) {
        throw utils::InternalError(fmt(_(
                    "Outputplugin '%1%' is not available")) % name);
    }
    return it;
}

PluginFactory::OutputPluginList::const_iterator PluginFactory::getO(
    const std::string& name) const
{
    OutputPluginList::const_iterator it = m_outs.find(name);
    if (it == m_outs.end()) {
        throw utils::InternalError(fmt(_(
                    "Outputplugin '%1%' is not available")) % name);
    }
    return it;
}

PluginFactory::ModelingPluginList::iterator PluginFactory::getM(
    const std::string& name)
{
    ModelingPluginList::iterator it = m_mods.find(name);
    if (it == m_mods.end()) {
        throw utils::InternalError(fmt(_(
                    "ModelingPlugin '%1%' is not available")) % name);
    }
    return it;
}

PluginFactory::ModelingPluginList::const_iterator PluginFactory::getM(
    const std::string& name) const
{
    ModelingPluginList::const_iterator it = m_mods.find(name);
    if (it == m_mods.end()) {
        throw utils::InternalError(fmt(_(
                    "Modelingplugin '%1%' is not available")) % name);
    }
    return it;
}

PluginFactory::OovPluginList::iterator PluginFactory::getD(
    const std::string& name)
{
    OovPluginList::iterator it = m_oov.find(name);
    if (it == m_oov.end()) {
        throw utils::InternalError(fmt(_(
                    "OovPlugin '%1%' is not available")) % name);
    }
    return it;
}

PluginFactory::OovPluginList::const_iterator PluginFactory::getD(
    const std::string& name) const
{
    OovPluginList::const_iterator it = m_oov.find(name);
    if (it == m_oov.end()) {
        throw utils::InternalError(fmt(_(
                    "Oovplugin '%1%' is not available")) % name);
    }
    return it;
}

void PluginFactory::readOutputPlugins()
{
    utils::PathList paths = utils::Path::path().getOutputDirs();

    std::vector < utils::ModuleCache::iterator > modules;
    std::vector < utils::ModuleCache::iterator >::iterator it;

    modules = utils::ModuleCache::instance().browse(paths);

    for (it = modules.begin(); it != modules.end(); ++it) {
        if (m_outs.find((*it)->path()) == m_outs.end()) {
            void* symbol = (*it)->get("makeNewOutputPlugin");
            if (symbol) {
                typedef OutputPlugin* (*function)(const std::string&);
                function fct(utils::pointer_to_function < function >(symbol));

                std::string package(utils::Path::getPackageFromPath(
                        (*it)->path()));

                std::string module(utils::Module::getPluginName(
                        (*it)->path()));

                OutputPlugin* plg = fct(utils::Module::getPluginName(module));

                if (plg) {
                    OutputPlg out(module, (*it)->path(), package, plg);
                    m_outs.insert(std::make_pair(out.string(), out));
                }
            }
        }
    }
}

void PluginFactory::readModelingPlugins()
{
    utils::PathList paths = utils::Path::path().getModelingDirs();

    std::vector < utils::ModuleCache::iterator > modules;
    std::vector < utils::ModuleCache::iterator >::iterator it;

    modules = utils::ModuleCache::instance().browse(paths);

    for (it = modules.begin(); it != modules.end(); ++it) {
        if (m_mods.find((*it)->path()) == m_mods.end()) {
            void* symbol = (*it)->get("makeNewModelingPlugin");
            if (symbol) {
                typedef ModelingPlugin* (*function)(const std::string&);
                function fct(utils::pointer_to_function < function >(symbol));

                std::string package(utils::Path::getPackageFromPath(
                        (*it)->path()));

                std::string module(utils::Module::getPluginName(
                        (*it)->path()));

                ModelingPlugin* plg = fct(utils::Module::getPluginName(module));

                if (plg) {
                    ModelingPlg mod(module, (*it)->path(), package, plg);
                    m_mods.insert(std::make_pair(mod.string(), mod));
                }
            }
        }
    }
}

void PluginFactory::readOovPlugins()
{
    utils::PathList paths = utils::Path::path().getStreamDirs();

    std::vector < utils::ModuleCache::iterator > modules;
    std::vector < utils::ModuleCache::iterator >::iterator it;

    modules = utils::ModuleCache::instance().browse(paths);

    for (it = modules.begin(); it != modules.end(); ++it) {
        if (m_oov.find((*it)->path()) == m_oov.end()) {
            void* symbol = (*it)->get("makeNewOovPlugin");
            if (symbol) {
                std::string package(utils::Path::getPackageFromPath(
                        (*it)->path()));

                std::string module(utils::Module::getPluginName(
                        (*it)->path()));

                OovPlg oov(module, (*it)->path(), package, (oov::Plugin*)0);
                m_oov.insert(std::make_pair(oov.string(), oov));
            }
        }
    }
}

}} // namespace vle gvle
