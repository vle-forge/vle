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

static void split(const std::string& path, std::string *package,
                  std::string *library)
{
    std::size_t pos = path.find('/');

    if (not path.empty()) {
        if (pos == std::string::npos) {
            library->assign(path);
        } else if (pos == 0) {
            library->assign(path, 1, std::string::npos);
        } else if (pos == path.size() - 1) {
            library->assign(path, 0, path.size() - 1);
        } else {
            package->assign(path, 0, pos);
            library->assign(path, pos, std::string::npos);
        }
    }
}

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
    std::string package, library;

    split(name, &package, &library);

    OutputPluginList::iterator it = m_outs.find(Key(package, library));
    if (it == m_outs.end()) {
        throw utils::InternalError(fmt(_(
                    "Outputplugin '%1%' is not available")) % name);
    }
    return it;
}

PluginFactory::OutputPluginList::const_iterator PluginFactory::getO(
    const std::string& name) const
{
    std::string package, library;

    split(name, &package, &library);

    OutputPluginList::const_iterator it = m_outs.find(Key(package, library));
    if (it == m_outs.end()) {
        throw utils::InternalError(fmt(_(
                    "Outputplugin '%1%' is not available")) % name);
    }
    return it;
}

PluginFactory::ModelingPluginList::iterator PluginFactory::getM(
    const std::string& name)
{
    std::string package, library;

    split(name, &package, &library);

    ModelingPluginList::iterator it = m_mods.find(Key(package, library));
    if (it == m_mods.end()) {
        throw utils::InternalError(fmt(_(
                    "ModelingPlugin '%1%' is not available")) % name);
    }
    return it;
}

PluginFactory::ModelingPluginList::const_iterator PluginFactory::getM(
    const std::string& name) const
{
    std::string package, library;

    split(name, &package, &library);

    ModelingPluginList::const_iterator it = m_mods.find(Key(package, library));
    if (it == m_mods.end()) {
        throw utils::InternalError(fmt(_(
                    "Modelingplugin '%1%' is not available")) % name);
    }
    return it;
}

PluginFactory::OovPluginList::iterator PluginFactory::getD(
    const std::string& name)
{
    std::string package, library;

    split(name, &package, &library);

    OovPluginList::iterator it = m_oov.find(Key(package, library));
    if (it == m_oov.end()) {
        throw utils::InternalError(fmt(_(
                    "OovPlugin '%1%' is not available")) % name);
    }
    return it;
}

PluginFactory::OovPluginList::const_iterator PluginFactory::getD(
    const std::string& name) const
{
    std::string package, library;

    split(name, &package, &library);

    OovPluginList::const_iterator it = m_oov.find(Key(package, library));
    if (it == m_oov.end()) {
        throw utils::InternalError(fmt(_(
                    "Oovplugin '%1%' is not available")) % name);
    }
    return it;
}

void PluginFactory::readOutputPlugins()
{
    std::multimap < std::string, std::string > modules;
    std::multimap < std::string, std::string >::iterator it;

    m_modulemgr.browse(utils::MODULE_GVLE_OUTPUT, &modules);

    for (it = modules.begin(); it != modules.end(); ++it) {
        if (m_outs.find(Key(it->first, it->second)) == m_outs.end()) {

            void *symbol = m_modulemgr.get(it->first, it->second,
                                           utils::MODULE_GVLE_OUTPUT);

            GvleOutputPluginSlot fct = utils::functionCast <
                GvleOutputPluginSlot >(symbol);

            OutputPlugin *plg = fct(it->second);

            if (plg) {
                OutputPlg out(it->second,
                              utils::ModuleManager::buildModuleFilename(
                                  it->first, it->second,
                                  utils::MODULE_GVLE_OUTPUT),
                              it->first, plg);
                m_outs.insert(std::make_pair(
                        std::make_pair(it->first, it->second), out));
            }
        }
    }
}

void PluginFactory::readModelingPlugins()
{
    std::multimap < std::string, std::string > modules;
    std::multimap < std::string, std::string >::iterator it;

    m_modulemgr.browse(utils::MODULE_GVLE_MODELING, &modules);

    for (it = modules.begin(); it != modules.end(); ++it) {
        if (m_outs.find(Key(it->first, it->second)) == m_outs.end()) {

            void *symbol = m_modulemgr.get(it->first, it->second,
                                           utils::MODULE_GVLE_MODELING);

            GvleModelingPluginSlot fct = utils::functionCast <
                GvleModelingPluginSlot >(symbol);

            ModelingPlugin *plg = fct(it->second);

            if (plg) {
                ModelingPlg mod(it->second,
                                utils::ModuleManager::buildModuleFilename(
                                    it->first, it->second,
                                    utils::MODULE_GVLE_MODELING),
                                it->first, plg);
                m_mods.insert(std::make_pair(
                        std::make_pair(it->first, it->second), mod));
            }
        }
    }
}

void PluginFactory::readOovPlugins()
{
    std::multimap < std::string, std::string > modules;
    std::multimap < std::string, std::string >::iterator it;

    m_modulemgr.browse(utils::MODULE_OOV, &modules);

    for (it = modules.begin(); it != modules.end(); ++it) {
        if (m_outs.find(Key(it->first, it->second)) == m_outs.end()) {
            OovPlg oov(it->second,
                       utils::ModuleManager::buildModuleFilename(
                           it->first, it->second,
                           utils::MODULE_OOV),
                       it->first,
                       static_cast < oov::Plugin* >(0));
            m_oov.insert(std::make_pair(
                    std::make_pair(it->first, it->second), oov));
        }
    }
}

}} // namespace vle gvle
