/**
 * @file src/vle/oov/StreamReader.cpp
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




#include <vle/oov/StreamReader.hpp>
#include <vle/oov/PluginFactory.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace oov {

PluginPtr StreamReader::plugin() const
{
    Assert(utils::InternalError, m_plugin.get(),
            "Plugin are not loaded and cannot respond to the StreamReader");

    return m_plugin;
}

void StreamReader::initPlugin(const std::string& plugin,
                              const std::string& location)
{
    utils::Path::PathList lst(utils::Path::path().getStreamDirs());
    utils::Path::PathList::const_iterator it;

    std::string error((boost::format(
                "Error opening oov plugin '%1%' in:") % plugin).str());

    for (it = lst.begin(); it != lst.end(); ++it) {
        try {
            PluginFactory pf(plugin, *it);
            m_plugin = pf.build(location);
            return;
        } catch (const std::exception& e) {
            error += e.what();
        }
    }

    Throw(utils::InternalError, error);
}

void StreamReader::onParameter(const vpz::ParameterTrame& trame)
{
    initPlugin(trame.plugin(), trame.location());
    plugin()->onParameter(trame);
}

void StreamReader::onNewObservable(const vpz::NewObservableTrame& trame)
{
   plugin()->onNewObservable(trame); 
}

void StreamReader::onDelObservable(const vpz::DelObservableTrame& trame)
{
   plugin()->onDelObservable(trame); 
}

void StreamReader::onValue(const vpz::ValueTrame& trame)
{
    plugin()->onValue(trame);
}
        
void StreamReader::onClose(const vpz::EndTrame& trame)
{
    plugin()->close(trame);
}

}} // namespace vle oov
