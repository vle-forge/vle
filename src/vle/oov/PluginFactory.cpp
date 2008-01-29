/** 
 * @file PluginFactory.cpp
 * @author The vle Development Team
 * @date 2008-01-31
 */

/*
 * Copyright (C) 2008 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <vle/oov/PluginFactory.hpp>
#include <vle/utils/Debug.hpp>


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
    Plugin* call = 0;
    void*   makeNewOovPlugin = 0;

    if (not m_module->get_symbol("makeNewOovPlugin", makeNewOovPlugin)) {
        Throw(utils::InternalError, boost::format(
                "Error when searching makeNewOovPlugin function in plugin %1%")
            % m_plugin);
    }

    call = ((Plugin*(*)(const std::string&))(makeNewOovPlugin))(location);
    if (not call) {
        Throw(utils::InternalError, boost::format(
                "Error when calling makeNewOovPlugin function in plugin %1%")
            % m_plugin);
    }

    PluginPtr plugin(call);
    return plugin;
}

}} // namespace vle oov
