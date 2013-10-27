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


#include <vle/oov/StreamReader.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/utils/Algo.hpp>
#include <vle/version.hpp>
#include <vle/oov/CairoPlugin.hpp>

namespace vle { namespace oov {

PluginPtr StreamReader::plugin() const
{
    if (not m_plugin.get()) {
        throw utils::InternalError(
            _("Plugin are not loaded and cannot respond to the StreamReader"));
    }

    return m_plugin;
}

void StreamReader::initPlugin(const std::string& plugin,
                              const std::string& package,
                              const std::string& location,
                              const utils::ModuleManager& modulemgr)
{
    void *symbol = 0;

    try {
        symbol = modulemgr.get(package, plugin, utils::MODULE_OOV);
        OovPluginSlot fct(utils::functionCast < OovPluginSlot>(symbol));
        PluginPtr ptr(fct(location));
        m_plugin = ptr;
    } catch(const std::exception& e) {
        throw utils::InternalError(fmt(
                _("Oov module: error when loading the module: %1%")) %
            e.what());
    }
}

void StreamReader::onParameter(const std::string& pluginname,
                               const std::string& package,
                               const std::string& location,
                               const std::string& file,
                               value::Value* parameters,
                               const double& time)
{
    initPlugin(pluginname, package, location, m_modulemgr);

    /*
     * For cairo plug-ins, we build the cairo graphics context via the
     * CairoPlugin::init function.
     */
    if (plugin()->isCairo()) {
        CairoPluginPtr plg = toCairoPlugin(plugin());
        plg->init();
    }

    plugin()->onParameter(pluginname, location, file, parameters, time);
}

void StreamReader::onNewObservable(const std::string& simulator,
                                   const std::string& parent,
                                   const std::string& portname,
                                   const std::string& view,
                                   const double& time)
{
    plugin()->onNewObservable(simulator, parent, portname, view, time);
}

void StreamReader::onDelObservable(const std::string& simulator,
                                   const std::string& parent,
                                   const std::string& portname,
                                   const std::string& view,
                                   const double& time)
{
    plugin()->onDelObservable(simulator, parent, portname, view, time);
}

void StreamReader::onValue(const std::string& simulator,
                           const std::string& parent,
                           const std::string& port,
                           const std::string& view,
                           const double& time,
                           value::Value* value)
{
    plugin()->onValue(simulator, parent, port, view, time, value);
}

void StreamReader::onClose(const double& time)
{
    plugin()->close(time);
}

}} // namespace vle oov
