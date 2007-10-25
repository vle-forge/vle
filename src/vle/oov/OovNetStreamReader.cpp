/** 
 * @file OovNetStreamReader.cpp
 * @brief 
 * @author The vle Development Team
 * @date 2007-10-25
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

#include <vle/oov/OovNetStreamReader.hpp>
#include <boost/format.hpp>
#include <config.h>

#ifdef HAVE_CAIRO
#   include <vle/oov/CairoPlugin.hpp>
#endif

namespace vle { namespace oov {

void OovNetStreamReader::onValue(const vpz::ValueTrame& trame)
{
    NetStreamReader::onValue(trame);

#ifdef HAVE_CAIRO
    if (plugin()->isCairo()) {
        CairoPluginPtr plg = toCairoPlugin(plugin());
        plg->context()->get_target()->write_to_png((boost::format(
                "%1%_%2$05d.png") % plg->location() % trame.time()).str());
    }
#endif
}

}} // namespace vle oov
