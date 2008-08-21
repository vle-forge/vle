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

#include <vle/oov/NetStreamReader.hpp>
#include <vle/eov/Plugin.hpp>
#include <vle/eov/Window.hpp>
#include <glibmm/thread.h>
#include <gtkmm/main.h>


namespace vle { namespace eov {

    class NetStreamReader : public oov::NetStreamReader
    {
    public:
        NetStreamReader(int port, Gtk::Main& app);

        virtual ~NetStreamReader();

        inline Glib::Mutex& mutex()
        { return m_mutex; }

    private:
        virtual void onParameter(const vpz::ParameterTrame& trame);
        
        virtual void onNewObservable(const vpz::NewObservableTrame& trame);

        virtual void onDelObservable(const vpz::DelObservableTrame& trame);

        virtual void onValue(const vpz::ValueTrame& trame);
        
        virtual void onClose(const vpz::EndTrame& trame);

        void getGtkPlugin(const std::string& name);

        void getDefaultPlugin();

        void runWindow();


        PluginPtr           m_plugin;
        Glib::Mutex         m_mutex;
        Glib::Thread*       m_thread;
        std::string         m_newpluginname;
        Gtk::Main&          m_app;
        Window*             m_window;

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
