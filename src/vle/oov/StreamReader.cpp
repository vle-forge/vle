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
#include <vle/utils/Exception.hpp>
#include <vle/version.hpp>

namespace vle { namespace oov {

PluginPtr StreamReader::plugin() const
{
    if (not m_plugin.get()) {
        throw vle::utils::InternalError(
            _("Plugin are not loaded and cannot respond to the StreamReader"));
    }

    return m_plugin;
}

void StreamReader::initPlugin(const std::string& plugin,
                              const std::string& package,
                              const std::string& location,
                              const utils::ModuleManager& modulemgr)
{
    void *symbol = nullptr;

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
                               std::unique_ptr<value::Value> parameters,
                               const double& time)
{
    initPlugin(pluginname, package, location, m_modulemgr);


    plugin()->onParameter(pluginname, location, file,
                          std::move(parameters), time);
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
                           std::unique_ptr<value::Value> value)
{
    plugin()->onValue(simulator, parent, port, view, time, std::move(value));
}

void StreamReader::onClose(const double& time)
{
    plugin()->close(time);
}

}} // namespace vle oov
