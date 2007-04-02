/** 
 * @file manager/Manager.hpp
 * @brief VLE in manager mode distribute the VPZ instances to VLE simulators.
 * @author The vle Development Team
 * @date lun, 23 jan 2006 14:04:28 +0100
 */

/*
 * Copyright (C) 2004, 05, 06 - The vle Development Team
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

#ifndef VLE_MANAGER_MANAGER_HPP
#define VLE_MANAGER_MANAGER_HPP

#include <string>
#include <vector>
#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Host.hpp>
#include <vle/utils/Socket.hpp>



namespace vle { namespace manager {

    /**
     * @brief VLE manager provide two types: immediate or daemon execution.
     * Manager parse the user hosts file to know Simulator position and lauch
     * simulation on hosts in immediate mode. The daemon wait for client
     * connections on socket and wait a stream.
     */
    class Manager
    {
    public:
        /**
         * Build a new Manager and read Hosts file from user home path.
         *
         * @param savevpz True if you want to save all VPZ instance.
         */
        Manager(bool savevpz);

        ~Manager();

        /**
         * @brief Run manager on localhost to build VPZI file and call vle
         * program with 'run' parameter to all VPZI. Use this mode to simulate
         * all simulator on the localhost.
         *
         * @param filename VPZ file to simulate.
         */
        void run_all_in_localhost(const std::string& filename);

        /**
         * @brief Run manager on localhost to build VPZI file and call vle
         * program with 'run' parameter to all VPZI. Use this mode to simulate
         * all simulator on the localhost.
         * 
         * @param file an already vpz::Vpz file open.
         */
        void run_all_in_localhost(const vpz::Vpz& file);

        /**
         * @brief Run manager on localhost and send the file to the hosts in
         * hosts read in host XML file. If no simulator daemon are found, launch
         * the simulator on localhost.
         *
         * @param filename VPZ file to simulate.
         */
        void run_localhost(const std::string& filename);

        /**
         * @brief Run manager on localhost and send the file to the hosts in
         * hosts read in host XML file. If no simulator daemon are found, launch
         * the simulator on localhost.
         *
         * @param file VPZ filename to simulate.
         */
        void run_localhost(const vpz::Vpz& file);

        /**
         * Run manager in daemon mode. It wait connection and VPZ file from
         * specified port. If no simulator daemon are found, launch the
         * simulator on localhost.
         *
         * @param port the port to listen.
         */
        void run_daemon(int port);

    private:
        void open_connection_with_simulators();
        void close_connection_with_simulators();
        void scheduller();

        gint32 get_max_processor(utils::net::Client& cl);
        gint32 get_current_number_vpzi(utils::net::Client& cl);
        void send_vpzi(utils::net::Client& cl, const Glib::ustring& filename);

        vpz::Vpz                            mFile;
        utils::Hosts                        mHost;
        std::list < utils::net::Client* >   mClients;
        bool                                mSaveVPZ;
    };

}} // namespace vle manager

#endif
