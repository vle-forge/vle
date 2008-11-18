/**
 * @file vle/oov/LocalStreamReader.cpp
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


#include <vle/oov/LocalStreamReader.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/utils/Debug.hpp>
#include <boost/format.hpp>
#include <config.h>

#ifdef HAVE_CAIRO
#   include <vle/oov/CairoPlugin.hpp>
#endif


namespace vle { namespace oov {

void LocalStreamReader::onValue(const std::string& simulator,
                                const std::string& parent,
                                const std::string& port,
                                const std::string& view,
                                const double& time,
                                value::Value* value)
{
#ifdef HAVE_CAIRO
    if (plugin()->isCairo()) {
        CairoPluginPtr plg = toCairoPlugin(plugin());
        plg->needCopy();
        plugin()->onValue(simulator, parent, port, view, time, value);
        if (plg->isCopyDone()) {
            plg->stored()->write_to_png(
                Glib::build_filename(plg->location(),
                                     (boost::format("img-%1$08d.png") %
                                      m_image).str()));
            m_image++;
        }
    } else {
#endif
        plugin()->onValue(simulator, parent, port, view, time, value);
#ifdef HAVE_CAIRO
    }
#endif
}

}} // namespace vle oov
