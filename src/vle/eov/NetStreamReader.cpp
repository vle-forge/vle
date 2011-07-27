/*
 * @file vle/eov/NetStreamReader.cpp
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


#include <vle/eov/NetStreamReader.hpp>
#include <vle/eov/Window.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Algo.hpp>
#include <vle/value/String.hpp>
#include <gtkmm/main.h>
#include <gdkmm/drawable.h>
#include <gdkmm/gc.h>


namespace vle { namespace eov {

void NetStreamReader::process()
{
    try {
	waitConnection();
	readConnection();
	closeConnection();
    } catch (std::exception& e) {
	m_finisherror.assign(e.what());
    } catch (...) {
	m_finisherror.assign(_("unknown error"));
    }
    try {
	oov::NetStreamReader::close();
    } catch (std::exception& e) {
	m_finisherror += "\n";
	m_finisherror += e.what();
    } catch (...) {
    }
    m_finish = true;
}

void NetStreamReader::onParameter(const std::string& pluginname,
                                  const std::string& package,
                                  const std::string& location,
                                  const std::string& file,
                                  value::Value* parameters,
                                  const double& time)
{
    oov::NetStreamReader::onParameter(pluginname, package, location, file,
                                      parameters, time);

    oov::PluginPtr poov = plugin();

    if (not poov->isCairo()) {
        throw utils::InternalError(fmt(_(
                "Eov: plugin '%1%' is not a oov::CairoPlugin")) %
            pluginname);
    }

    runWindow(pluginname, package);

    {
        Glib::Mutex::Lock lock(m_mutex);
        oov::CairoPluginPtr cr = oov::toCairoPlugin(poov);
        int width, height;
        cr->preferredSize(width, height);
        m_main.resize(width, height);
    }

    m_newpluginname = pluginname;
}

void NetStreamReader::onNewObservable(const std::string& simulator,
                                      const std::string& parent,
                                      const std::string& portname,
                                      const std::string& view,
                                      const double& time)
{
    oov::NetStreamReader::onNewObservable(simulator, parent, portname, view,
                                          time);
}

void NetStreamReader::onDelObservable(const std::string& simulator,
                                      const std::string& parent,
                                      const std::string& portname,
                                      const std::string& view,
                                      const double& time)
{
    oov::NetStreamReader::onDelObservable(simulator, parent, portname, view,
                                          time);
}

void NetStreamReader::onValue(const std::string& simulator,
                              const std::string& parent,
                              const std::string& port,
                              const std::string& view,
                              const double& time,
                              value::Value* value)
{
    plugin()->onValue(simulator, parent, port, view, time, value);
}

void NetStreamReader::onClose(const double& time)
{
    oov::NetStreamReader::onClose(time);

    {
        Glib::Mutex::Lock lock(m_mutex);
        m_main.killClock();
    }
}

void NetStreamReader::getPlugin(const std::string& pluginname,
                                const std::string& package)
{
    void *symbol = 0;

    try {
        if (not package.empty()) {
            symbol = getModuleManager().get(package, pluginname,
                                            utils::MODULE_EOV);
        } else {
            symbol = getModuleManager().get(pluginname, utils::MODULE_EOV);
        }

        EovPluginSlot fct(utils::pointer_to_function < EovPluginSlot >(symbol));
        Plugin* ptr = fct(plugin(), this);
        PluginPtr plg(ptr);
        m_plugin = plg;
    } catch (const std::exception& e) {
        throw utils::InternalError(fmt(
                _("Eov module: error when loadin the module: %1%")) % e.what());
    }
}

void NetStreamReader::runWindow(const std::string& pluginname,
                                const std::string& package)
{
    std::string error;

    try {
        getPlugin(pluginname, package);
    } catch (const std::exception& e) {
        error.assign(e.what());
    }

    try {
        getPlugin("gtkdefault", std::string());
    } catch (const std::exception& e) {
        error += '\n' + e.what();
        throw utils::InternalError(error);
    }

    m_main.setPlugin(m_plugin);
}

}} // namespace vle eov
