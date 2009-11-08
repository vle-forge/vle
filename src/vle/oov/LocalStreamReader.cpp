/**
 * @file vle/oov/LocalStreamReader.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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
#include <vle/utils/Path.hpp>
#include <vle/version.hpp>
#include <boost/format.hpp>

#ifdef VLE_HAVE_CAIRO
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
#ifdef VLE_HAVE_CAIRO
    if (plugin()->isCairo()) {
        CairoPluginPtr plg = toCairoPlugin(plugin());
        plg->needCopy();
        plugin()->onValue(simulator, parent, port, view, time, value);
        if (plg->isCopyDone()) {
            std::string file(utils::Path::buildFilename(
                    plg->location(), (fmt("img-%1$08d.png") % m_image).str()));

            try {
                plg->stored()->write_to_png(file);
                m_image++;
            } catch(const std::exception& /*e*/) {
                throw utils::InternalError(fmt(
                        _("oov: cannot write image '%1%'")) % file);
            }
        }
    } else {
#endif
        plugin()->onValue(simulator, parent, port, view, time, value);
#ifdef VLE_HAVE_CAIRO
    }
#endif
}

}} // namespace vle oov
