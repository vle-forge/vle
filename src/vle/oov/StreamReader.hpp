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

        //
        ///
        /// Get/Set functions
        ///
        //

        PluginPtr plugin();

    protected:
        void init_plugin(const std::string& plugin,
                         const std::string& location);

    private:
        PluginPtr   m_plugin;

	/**
	 * @brief Plugin factory is use to build reference to the
	 * Plugin load dynamically from the dynamics libraries.
	 *
	 */
        class PluginFactory
        {
        public:
	    /**
	     * @brief Constructor to load plugin from pathname.
	     * @param plugin the name of the plugin to load.
	     * @param pathname the name of the path where the plugin is.
	     * @throw utils::InternalError if the plugin does not exist
	     * in the pathname.
	     */
            PluginFactory(const std::string& plugin,
                          const std::string& pathname);

            ~PluginFactory();

            PluginPtr build(const std::string& location);

        private:
            Glib::Module*   module__;
            std::string     plugin__;

        };
    };

}} // namespace vle oov

#endif
