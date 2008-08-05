/**
 * @file src/vle/oov/PluginFactory.cpp
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




#include <vle/oov/PluginFactory.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Algo.hpp>


namespace vle { namespace oov {

PluginFactory::PluginFactory(const std::string& plugin,
                             const std::string& pathname) :
    m_module(0),
    m_plugin(plugin)
{
    std::string file(Glib::Module::build_path(pathname, plugin));
    m_module = new Glib::Module(file);
    if (not (*m_module)) {
        delete m_module;
        m_module = 0;
        Throw(utils::InternalError, boost::format(
                "\n[%1%]: %2%") % pathname % Glib::Module::get_last_error());
    }
    m_module->make_resident();
}

PluginFactory::~PluginFactory()
{
    delete m_module;
}

PluginPtr PluginFactory::build(const std::string& location)
{
    /*
     * Define the pointer to fonction of the oov::Plugin plug-in.
     */
    typedef Plugin* (*function)(const std::string&);

    Plugin* call = 0;
    void*   makeNewOovPlugin = 0;

    if (not m_module->get_symbol("makeNewOovPlugin", makeNewOovPlugin)) {
        Throw(utils::InternalError, boost::format(
                "Error when searching makeNewOovPlugin function in plugin %1%")
            % m_plugin);
    }

    function fct(utils::pointer_to_function < function >(makeNewOovPlugin));
    call = fct(location);

    if (not call) {
        Throw(utils::InternalError, boost::format(
                "Error when calling makeNewOovPlugin function in plugin %1%")
            % m_plugin);
    }

    PluginPtr plugin(call);
    return plugin;
}

}} // namespace vle oov
