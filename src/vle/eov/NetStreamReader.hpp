/**
 * @file vle/eov/NetStreamReader.hpp
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


#ifndef VLE_EOV_NETSTREAMREADER_HPP
#define VLE_EOV_NETSTREAMREADER_HPP

#include <vle/eov/DllDefines.hpp>
#include <vle/oov/NetStreamReader.hpp>
#include <vle/eov/Plugin.hpp>
#include <glibmm/thread.h>
#include <gtkmm/main.h>

namespace vle { namespace eov {

    class Window;

    class VLE_EOV_EXPORT NetStreamReader : public oov::NetStreamReader
    {
    public:
        NetStreamReader(int port, Window& main) :
            oov::NetStreamReader(port),
            m_main(main),
            m_finish(false)
        {}

        virtual ~NetStreamReader() {}

        inline Glib::Mutex& mutex()
        { return m_mutex; }

	/**
	 * @brief Call the oov::NetStreamReader::process function. Close the
	 * stream if the oov::NetStreamReader throws error.
	 */
	void process();

	/**
	 * @brief Return true if the NetStreamReader is closed by vle.
	 * @return true if the connection is closed, false otherwise.
	 */
	bool isFinish() const
	{ return m_finish; }

	/**
	 * @brief Return the error statement if the oov::NetStreamReader throws
	 * error.
	 * @return The statement error or an empty string.
	 */
	const std::string& finishError() const
	{ return m_finisherror; }

    private:
        /**
         * @brief Call to initialise plug-in. Just before the Plugin
         * constructor. This function is used to initialise the Plugin with
         * parameter provided by the devs::StreamWritter class.
         */
        virtual void onParameter(const std::string& plugin,
                                 const std::string& location,
                                 const std::string& file,
                                 value::Value* parameters,
                                 const double& time);

        /**
         * @brief Call when a new observable (the devs::Simulator and port name)
         * is attached to a view.
         */
        virtual void onNewObservable(const std::string& simulator,
                                     const std::string& parent,
                                     const std::string& portname,
                                     const std::string& view,
                                     const double& time);

        /**
         * @brief Call whe a observable (the devs::Simulator and port name) is
         * deleted from a view.
         */
        virtual void onDelObservable(const std::string& simulator,
                                     const std::string& parent,
                                     const std::string& portname,
                                     const std::string& view,
                                     const double& time);

        /**
         * @brief Call when an observation event is send to the view.
         */
        virtual void onValue(const std::string& simulator,
                             const std::string& parent,
                             const std::string& port,
                             const std::string& view,
                             const double& time,
                             value::Value* value);

        /**
         * @brief Call when the simulation is finished.
         */
        virtual void onClose(const double& time);

        void getGtkPlugin(const std::string& name);

        void getDefaultPlugin();

        void runWindow();

        Window&             m_main;
        PluginPtr           m_plugin;
        Glib::Mutex         m_mutex;
        std::string         m_newpluginname;
	bool		    m_finish;
	std::string         m_finisherror;

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

            PluginPtr build(oov::PluginPtr oovplugin, NetStreamReader* net);

        private:
            Glib::Module*   m_module;
            std::string     m_plugin;
        };
    };

}} // namespace vle eov

#endif
