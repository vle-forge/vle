/** 
 * @file Simulator.hpp
 * @brief VLE in simulator mode execute the VPZ instances.
 * @author The vle Development Team
 * @date lun, 23 jan 2006 14:06:15 +0100
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

#ifndef VLE_MANAGER_SIMULATOR_HPP
#define VLE_MANAGER_SIMULATOR_HPP

#include <glibmm/thread.h>
#include <glibmm/threadpool.h>
#include <queue>
#include <vle/utils/Socket.hpp>



namespace vle { namespace manager {

/**
 * @brief VLE simulator wait VLE manager from TCP/IP network, and launch
 * simulation on controlled processor. The message between manager and
 * simulator daemon are, "proc" for max processor, "size" the number of vpzi to
 * run, "file" to send a buffer size and "exit" to close connection.
 */
class Simulator
{
public:
    /**
     * Run simulator on specified TCP/IP port and wait VPZi file.
     *
     * @param cpu number of CPU allowed to VLE simulators.
     * @param port TCP/IP port to listen VLE manager.
     */
    Simulator(size_t cpu, int port);

    /**
     * Destroy TCP/IP connection, delete all process.
     *
     */
    ~Simulator();

    /**
     * Start the daemon.
     *
     */
    void start();

    /** 
     * @brief Run a simulation from specified VPZ file.
     * 
     * @param filename 
     */
    static bool run(const std::string& filename);

private:
    void wait();
    void run();

    void daemon_send_max_processor();
    void daemon_send_number_vpzi();
    void daemon_recv_file();
    void daemon_exit();

    size_t                          mNbCPU; /// number of CPU
    int                             mPort; /// port to listen
    utils::net::Server*             mServer; /// server socket

    std::queue < std::string >      mFileNames; /// list of works
    Glib::ThreadPool                mPool; /// a pool of thread
    Glib::Thread*                   mRun; /// waiting process

    Glib::Mutex                     mMutex; /// mutex on list of works
    Glib::Cond                      mCondWait;
    Glib::Cond                      mCondRun;
};

}} // namespace vle manager

#endif
