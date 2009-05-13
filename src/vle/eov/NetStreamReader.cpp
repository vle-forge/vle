/**
 * @file vle/eov/NetStreamReader.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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
#include <vle/utils/Debug.hpp>
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
                                  const std::string& location,
                                  const std::string& file,
                                  value::Value* parameters,
                                  const double& time)
{
    oov::NetStreamReader::onParameter(pluginname, location, file, parameters,
                                      time);

    oov::PluginPtr poov = plugin();

    if (not poov->isCairo()) {
        throw utils::InternalError(fmt(_(
                "Eov: plugin '%1%' is not a oov::CairoPlugin")) %
            pluginname);
    }

    runWindow();

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

void NetStreamReader::getGtkPlugin(const std::string& pluginname)
{
    utils::Path::PathList lst(utils::Path::path().getStreamDirs());
    utils::Path::PathList::const_iterator it;

    std::string error((fmt(_(
                "Error opening eov plugin '%1%' in:")) % pluginname).str());

    std::string newfilename(pluginname);

    if (pluginname.size() <= 6 or pluginname.compare(0, 5, "cairo") != 0) {
        throw utils::InternalError(fmt(_(
                "oov plugin must begin by string 'cairo': '%1%'")) %
            pluginname);
    }

    newfilename.replace(0, 5, "gtk");

    for (it = lst.begin(); it != lst.end(); ++it) {
        try {
            PluginFactory pf(newfilename, *it);
            m_plugin = pf.build(plugin(), this);
            return;
        } catch (const std::exception& e) {
            error += e.what();
        }
    }

    throw utils::InternalError(error);
}

void NetStreamReader::getDefaultPlugin()
{
    utils::Path::PathList lst(utils::Path::path().getStreamDirs());
    utils::Path::PathList::const_iterator it;

    std::string error(_("Error opening eov default plugin:"));

    for (it = lst.begin(); it != lst.end(); ++it) {
        try {
            PluginFactory pf("gtkdefault", *it);
            m_plugin = pf.build(plugin(), this);
            return;
        } catch (const std::exception& e) {
            error += e.what();
        }
    }

    throw utils::InternalError(error);
}

void NetStreamReader::runWindow()
{
    std::string error;

    try {
        getGtkPlugin(m_newpluginname);
    } catch (const std::exception& e) {
        error.assign(e.what());
    }

    try {
        getDefaultPlugin();
    } catch (const std::exception& e) {
        error += '\n' + e.what();
        throw utils::InternalError(error);
    }

    m_main.setPlugin(m_plugin);
}

NetStreamReader::PluginFactory::PluginFactory(const std::string& plugin,
                                              const std::string& pathname) :
    m_module(0),
    m_plugin(plugin)
{
    std::string file(Glib::Module::build_path(pathname, plugin));
    m_module = new Glib::Module(file);
    if (not (*m_module)) {
        delete m_module;
        m_module = 0;
        throw utils::InternalError(fmt(_(
                "\n[%1%]: %2%")) % pathname % Glib::Module::get_last_error());
    }
    m_module->make_resident();
}

NetStreamReader::PluginFactory::~PluginFactory()
{
    delete m_module;
}

PluginPtr NetStreamReader::PluginFactory::build(oov::PluginPtr oovplugin,
                                                NetStreamReader* net)
{
    /*
     * Define the pointer to the fonction of the eov::Plugin plug-in.
     */
    typedef Plugin* (*function)(oov::PluginPtr&, NetStreamReader*);

    Plugin* call = 0;
    void*   makeNewEovPlugin = 0;

    if (not m_module->get_symbol("makeNewEovPlugin", makeNewEovPlugin)) {
        throw utils::InternalError(fmt(_(
                "Error when searching makeNewEovPlugin function in plugin %1%"))
            % m_plugin);
    }

    function fct(utils::pointer_to_function < function >(makeNewEovPlugin));
    call = fct(oovplugin, net);

    if (not call) {
        throw utils::InternalError(fmt(_(
                "Error when calling makeNewEovPlugin function in plugin %1%"))
            % m_plugin);
    }

    PluginPtr plugin(call);
    return plugin;
}

}} // namespace vle eov
