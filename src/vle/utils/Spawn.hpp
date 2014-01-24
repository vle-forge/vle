/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VLE_UTILS_DETAILS_SPAWN_HPP
#define VLE_UTILS_DETAILS_SPAWN_HPP

#include <vle/DllDefines.hpp>
#include <string>
#include <vector>

namespace vle { namespace utils {

/**
 * @e Spawn use fork, exec and pipe function (unix) or CreateProcess,
 * CreatePipe functions (Win32) to execute a process and get its
 * standard and error outputs.
 *
 * @code
 * vle::utils::Spawn spawn;
 *
 * std::vector < std::string > args, envp;
 * args.push_back("vle.toulouse.inra.fr");
 * args.push_back("-c");
 * args.push_back("4");
 *
 * if (spawn.start("/bin/ping", "/home/homer", args; envp)) {
 *     std::string output, error;
 *
 *     while (not spawn.isfinish()) {
 *         std::string output, error;
 *         spawn.get(&output, &error);
 *
 *         std::cout << output << " " << error << "\n";
 *     }
 *     std::string msg;
 *     bool success;
 *     if (spawn.status(&msg, &success))
 *         std::cout << ((success) ? "success" : "failed") << "\n"
 *                   << msg << "\n;"
 * } else {
 *     std::cerr << "failed to start /bin/date\n";
 * }
 * @endcode
 */
class VLE_API Spawn
{
public:
    static const unsigned long int default_buffer_size;

    /**
     * Default constructor.
     *
     */
    Spawn();

    /**
     * Wait the process if it was running and release ressources
     *
     */
    ~Spawn();

    /**
     * Try to execute the command.
     *
     * @param exe The command to execute.
     * @param workingdir The directory where the command need to be
     * executed.
     * @param args The arguments of the command.
     * @param waitchildtimeout The timeout while VLE wait for
     * subprocess [0,1000000] .
     *
     * @return
     */
    bool start(const std::string& exe,
               const std::string& workingdir,
               const std::vector < std::string > &args,
               unsigned int waitchildtimeout = 50000u);

    /**
     * Wait the process.
     *
     *
     * @return true if wait() succeed, false otherwise.
     */
    bool wait();

    /**
     * Return true if the process is running.
     *
     * @return @e true if the process is running, false if it not
     * started or not running.
     */
    bool isstart();

    /**
     * Return true if the process is finished.
     *
     *
     * @return @e true if the process is finished, false if it not
     * started or running.
     */
    bool isfinish();

    /**
     * Get the current buffer from pipe.
     *
     * @param [out] output
     * @param [out] error
     *
     * @return true if success, false otherwise.
     */
    bool get(std::string *output, std::string *error);

    /**
     * Retrieves the status of the ended sub-process. @e status()
     * returns SPAWN_ERROR_NOT_STARTED, SPAWN_ERROR_CHILD or 0 if
     * success.
     *
     * @param [out] msg assign this parameter with error message.
     * @param [out] success true if status is exited is success, false
     * otherwise.
     *
     * @return true if success, false otherwise.
     */
    bool status(std::string *msg, bool *success);

private:
    Spawn(const Spawn&);
    Spawn& operator=(const Spawn&);

    class Pimpl;
    Pimpl *m_pimpl;
};

}}

#endif /* VLE_UTILS_DETAILS_SPAWN_HPP */
