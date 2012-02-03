/*
 * @file vle/utils/RemoteManager.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#ifndef VLE_UTILS_REMOTEMANAGER_HPP
#define VLE_UTILS_REMOTEMANAGER_HPP

#include <vle/DllDefines.hpp>
#include <iosfwd>
#include <string>

namespace vle { namespace utils {

/**
 * \c RemoteManagerActions is available action for the \c RemoteManager class.
 *
 * Use the \c RemoteManagerActions in the \c start function of the \c
 * RemoteManager class to update database, install, search or show packages.
 */
enum RemoteManagerActions
{
    REMOTE_MANAGER_UPDATE, /**< vle --remote update. */
    REMOTE_MANAGER_SOURCE, /**< vle --remote source glue. */
    REMOTE_MANAGER_INSTALL, /**< vle --remove install glue. */
    REMOTE_MANAGER_SEARCH, /**< vle --remote search '*lu*'. */
    REMOTE_MANAGER_SHOW /**< vle --remote show glue. */
};

/**
 * \c RemoteManager allow to manager ftp/http repositories.
 *
 * A class to access to the  \c RemoteManager repositories. By default, a file
 * in \c VLE_HOME/packages stores the packages availables. Each update will
 * rewrite this file with newly available packages.
 *
 * This class uses the \c libxml2 \c nanohttp function to download resources
 * from the http protocol. \c RemoteManager is thread-safe and multi-threaded
 * class. \c RemoteManager can not be copied or assigned to another \c
 * RemoteManager.
 *
 * @code
 * utils::RemoteManager mgr;
 *
 * mgr.start(REMOTE_MANAGER_UPDATE, std::string(), &std::cout);
 * mgr.join();
 *
 * mgr.start(REMOTE_MANAGER_INSTALL, "glue-1.0", &std::cout);
 * @endcode
 */
class VLE_API RemoteManager
{
public:
    /**
     * Build a new RemoteManager object.
     *
     * - Read the \c VLE_HOME/vle.conf file to get the list of remote host.
     * - Read the \c VLE_HOME/package file to get the list of available package.
     */
    RemoteManager();

    /**
     * If an action is running, \c join it otherwise do nothing.
     */
    ~RemoteManager();

    /**
     * Start a process.
     *
     * If no thread are alive, a thread is started to perform the \c action.
     *
     * @param[in] action The type of the action to start.
     * @param[in] arg The argument of the action.
     * @param[out] os The output stream of the action.
     */
    void start(RemoteManagerActions action,
               const std::string& arg,
               std::ostream* os);

    /**
     * Join the current thread: wait until the thread stop.
     */
    void join();

    /**
     * Try to stop the current thread.
     *
     * This function does not try to \c join the thread. You need to use \c
     * join() to ensure the end of the thread.
     *
     * @code
     * utils::RemoteManager mgr;
     *
     * mgr.start(REMOTE_MANAGER_INSTALL, "glue-1.0", &std::cout);
     *
     * // some source code
     * // [...]
     *
     * mgr.stop(); // stop the action if it is possible.
     * mgr.join(); // join the current action.
     *
     * @endcode
     */
    void stop();

private:
    /**
     * Uncopyable class.
     */
    RemoteManager(const RemoteManager& RemoteManager);

    /**
     * Uncopyable class.
     */
    RemoteManager& operator=(const RemoteManager& RemoteManager);

    class Pimpl;
    Pimpl* mPimpl;
};

}} // namespace vle utils

#endif
