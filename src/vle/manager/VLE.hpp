/*
 * @file vle/manager/VLE.hpp
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


#ifndef VLE_MANAGER_VLE_HPP
#define VLE_MANAGER_VLE_HPP 1

#include <vle/manager/DllDefines.hpp>
#include <vle/manager/Types.hpp>
#include <vle/vpz/Vpz.hpp>
#include <set>

namespace vle { namespace manager {

/**
 * @brief A std::set of package names.
 */
typedef std::set < std::string > Depends;

/**
 * @brief The main class of the libvlemanager. Manager provides functions to
 * launch experimental frames, to launch simulator in distant mode or to simply
 * launch simulations.
 */
class VLE_MANAGER_EXPORT Manager
{
public:
    /**
     * @brief Build a Manager. If `quiet' is true, the Manager send all message
     * error into a tempory file otherwise, it uses the standard output error
     * stream.
     *
     * @param quiet If `quiet' is true, use a tempory file to store message,
     * false to use the standard error output stream.
     */
    Manager(bool quiet = false);

    /**
     * @brief Clean up the Manager: if in `quiet' mode, restore the std::cerr
     * streambuf and delete the `quiet' mode.
     */
    ~Manager();

    /**
     * @brief If in `quiet' mode, restore the std::cerr streambuf and delete
     * the `quiet' mode.
     */
    void close();

    //
    ///
    //

    /**
     * @brief read an experimental frame VPZ, build the complete condition
     * combinations and launch or connect to the vle::Simulator and send
     * experimental frame instance VPZi. vle::Manager can listening on
     * localhost port and wait for VPZ stream like a daemon.
     * @param nbProcessor number of processor.
     * @return true if manager is a success.
     */
    bool runManager(bool allInLocal, bool savevpz, int nbProcessor,
                    const CmdArgs& args);

    /**
     * @brief vle::Simulator: manage a number of processor and launch
     * simulation on it. vle::Simulator run in localhost daemon.
     * @param nbProcessor number of processor.
     * @param port The port [0..65535] to listen.
     * @return true if simulator is a success.
     */
    bool runSimulator(int nbProcessor, int port);

    /**
     * @brief vle::JustRun: start a simulation without analysis of the
     * complete condition combinations, just take the fist and run
     * simulation.
     * @return true if justRun simulation is a success.
     */
    bool justRun(int nbProcessor, const CmdArgs& args);

    /**
     * @brief Build a dictonnary of dependencies. For each vpz file in
     * experiment directory, we produce a list of dependencies.
     * @return The dictionnary.
     */
    VLE_MANAGER_EXPORT static std::map < std::string, Depends > depends();

    //
    ///
    //

    /**
     * @brief Get the mode of the Manager: quiet or not quiet.
     *
     * @return
     */
    bool quiet() const { return mQuiet; }

    /**
     * @brief Get the filename of the Manager's quiet mode.
     *
     * @return
     */
    const std::string& filename() const { return mFilename; }

    /**
     * @brief Get the success of the latest operation of this Manager.
     *
     * @return
     */
    bool success() const { return mSuccess; }

private:
    bool mQuiet;
    std::string mFilename; /**< The filename of the temporary file build in the
                             constructor if the user selects the `quiet'
                             mode. */
    std::ofstream* mFileError; /**< The std::ofstream of the temporary file
                                 build in the constructor if the user selects
                                 the `quiet' mode. */
    std::streambuf* mStreamBufError; /**< To store the streambuf of the
                                       std::cerr output stream if the user
                                       selects the `quiet' mode. */
    bool mSuccess;

    /**
     * @brief A simple functor to produce a vpz::Vpz object from the
     * std::string filename. To use with std::transform for example.
     */
    struct VLE_MANAGER_EXPORT BuildVPZ
    {
        vpz::Vpz* operator()(const std::string& filename) const
        { return new vpz::Vpz(filename); }
    };
};

/*
 *
 * initialisation and finalize functions of the system
 *
 */

/**
 * @brief Initialize the VLE system by:
 * - installling signal (segmentation fault etc.) to standard error
 *   function.
 * - initialize the user directory ($HOME/.vle/ etc.).
 * - initialize the WinSock
 * - initialize the thread system.
 * - initialize the singleton utils::Trace system.
 * - initialize the singleton \c boost::pool devs::Pools and value::Pools.
 */
VLE_MANAGER_EXPORT void init();

/**
 * @brief Delete all singleton from VLE system.
 * - kill the utils::Trace singleton.
 * - kill the utils::Path singleton.
 * - kill the value::Pools singleton.
 * - kill the devs::Pools singleton.
 */
VLE_MANAGER_EXPORT void finalize();

}} // namespace vle manager

#endif
