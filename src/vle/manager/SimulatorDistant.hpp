/**
 * @file src/vle/manager/SimulatorDistant.hpp
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




#ifndef VLE_MANAGER_SIMULATORDISTANT_HPP
#define VLE_MANAGER_SIMULATORDISTANT_HPP

#include <glibmm/thread.h>
#include <glibmm/threadpool.h>
#include <queue>
#include <vle/utils/Socket.hpp>
#include <vle/manager/JustRun.hpp>


namespace vle { namespace vpz {

    class Vpz;

}} // namespace vle vpz


namespace vle { namespace manager {

    /**
     * @brief VLE simulator wait VLE manager from TCP/IP network, and launch
     * simulation on controlled processor. The message between manager and
     * simulator daemon are, "proc" for max processor, "size" the number of vpzi to
     * run, "file" to send a buffer size and "exit" to close connection.
     */
    class SimulatorDistant
    {
    public:
        /**
         * Run simulator on specified TCP/IP port and wait VPZi file.
         *
         * @param cpu number of CPU allowed to VLE simulators.
         * @param port TCP/IP port to listen VLE manager.
         */
        SimulatorDistant(std::ostream& out, size_t cpu, int port);

        /**
         * Destroy TCP/IP connection, delete all process.
         *
         */
        ~SimulatorDistant();

        /**
         * Start the daemon.
         *
         */
        void start();

    private:
        void wait();
        void run();

        void daemonSendMaxProcessor();
        void daemonSendNumberVpzi();
        void daemonRecvFile();
        void daemonExit();
        void sendResult();

        std::ostream&                   m_out;
        size_t                          mNbCPU; /// number of CPU
        int                             mPort; /// port to listen
        utils::net::Server*             mServer; /// server socket
        OutputSimulationDistantList     mOutputs;

        std::queue < std::string >      mFileNames; /// list of works
        Glib::ThreadPool                mPool; /// a pool of thread
        Glib::Thread*                   mRun; /// waiting process

        Glib::Mutex                     mMutex; /// mutex on list of works
        Glib::Cond                      mCondWait;
        Glib::Cond                      mCondRun;
    };

}} // namespace vle manager

#endif
