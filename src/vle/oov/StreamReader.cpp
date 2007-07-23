/** 
 * @file StreamReader.cpp
 * @brief Base class of the output stream reader.
 * @author The vle Development Team
 * @date 2007-07-15
 */

/*
 * Copyright (C) 2007 - The vle Development Team
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

#include <vle/oov/StreamReader.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/utils/Path.hpp>

namespace vle { namespace oov {

Plugin* StreamReader::plugin()
{
    Assert(utils::InternalError, m_plugin,
            "Plugin are not loaded and cannot respond to the StreamReader");

    return m_plugin;
}

void StreamReader::init_plugin(const std::string& plugin,
                               const std::string& location)
{
    std::string err;
    try {
        PluginFactory pfd(plugin, utils::Path::path().getDefaultStreamDir());
        m_plugin = pfd.build(location);
        return;
    } catch (const std::exception& edefault) {
        err.assign(edefault.what());
    }

    try {
        PluginFactory pfl(plugin, utils::Path::path().getUserStreamDir());
        m_plugin = pfl.build(location);
        return;
    } catch(const std::exception& euser) {
        Throw(utils::InternalError, boost::format(
                "Cannot open plugin %1%. Error reported:\n%2%\n%3%") %
            plugin % err % euser.what());
    }
}

StreamReader::PluginFactory::PluginFactory(const std::string& plugin,
                                           const std::string& pathname) :
    module__(0),
    plugin__(plugin)
{
    module__ = new Glib::Module(Glib::Module::build_path(pathname, plugin));
    if (not (*module__)) {
        Throw(utils::InternalError, boost::format(
                "Error when building plugin from pathname %1%") % pathname);
    }
    module__->make_resident();
}

StreamReader::PluginFactory::~PluginFactory()
{
    delete module__;
}

Plugin* StreamReader::PluginFactory::build(const std::string& location)
{
    Plugin* call = 0;
    void*   makeNewOovPlugin = 0;

    if (not module__->get_symbol("makeNewOovPlugin", makeNewOovPlugin)) {
        Throw(utils::InternalError, boost::format(
                "Error when searching makeNewOovPlugin function in plugin %1%")
            % plugin__);
    }

    call = ((Plugin*(*)(const std::string&))(makeNewOovPlugin))(location);
    if (not call) {
        Throw(utils::InternalError, boost::format(
                "Error when calling makeNewOovPlugin function in plugin %1%")
            % plugin__);
    }

    return call;
}

}} // namespace vle oov
