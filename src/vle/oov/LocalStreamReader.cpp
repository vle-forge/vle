/** 
 * @file LocalStreamReader.cpp
 * @brief 
 * @author The vle Development Team
 * @date 2007-07-21
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

#include <vle/oov/LocalStreamReader.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/utils/Debug.hpp>



namespace vle { namespace oov {

LocalStreamReader::LocalStreamReader() :
  StreamReader()
{
}

LocalStreamReader::~LocalStreamReader()
{
}

void LocalStreamReader::init(const std::string& plugin,
                             const std::string& location)
{
    try {
        init_plugin(plugin, location);
    } catch(const std::exception& e) {
        Throw(utils::InternalError, (boost::format(
                    "Local stream reader cannot open plugin %1%. "
                    "Error reported")) % plugin % e.what());
    }
}

void LocalStreamReader::onParameter(const vpz::ParameterTrame& trame)
{
    plugin()->onParameter(trame);
}

void LocalStreamReader::onNewObservable(const vpz::NewObservableTrame& trame)
{
   plugin()->onNewObservable(trame); 
}

void LocalStreamReader::onDelObservable(const vpz::DelObservableTrame& trame)
{
   plugin()->onDelObservable(trame); 
}

void LocalStreamReader::onValue(const vpz::ValueTrame& trame)
{
    plugin()->onValue(trame);
}
        
void LocalStreamReader::onClose(const vpz::EndTrame& trame)
{
    plugin()->close(trame);
}

}} // namespace vle oov
