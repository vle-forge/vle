/** 
 * @file StreamReader.hpp
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

#ifndef VLE_OOV_STREAMREADER_HPP
#define VLE_OOV_STREAMREADER_HPP

#include <vle/oov/Plugin.hpp>
#include <boost/shared_ptr.hpp>
#include <glibmm/module.h>
#include <string>



namespace vle { namespace oov {

    class StreamReader
    {
    public:
	StreamReader()
        { }

        virtual ~StreamReader()
        { }
        
        virtual void onParameter(const vpz::ParameterTrame& trame);

        virtual void onNewObservable(const vpz::NewObservableTrame& trame);

        virtual void onDelObservable(const vpz::DelObservableTrame& trame);

        virtual void onValue(const vpz::ValueTrame& trame);
        
        virtual void onClose(const vpz::EndTrame& trame);

        ///
        /// Get/Set functions
        ///

        PluginPtr plugin() const;

    private:
        PluginPtr   m_plugin;

        /** 
         * @brief Load the specified output plugin from the StreamDirs location.
         * @param plugin the name of the plugin.
         * @param location the location of the plugin output.
         * @throw utils::InternalError if the plugin was not found.
         */
        virtual void initPlugin(const std::string& plugin,
                                const std::string& location);

    };

}} // namespace vle oov

#endif
