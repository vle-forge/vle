/**
 * @file vle/manager/VLE.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
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
#define VLE_MANAGER_VLE_HPP

#include <vle/manager/DllDefines.hpp>
#include <vle/manager/Types.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vector>

namespace vle { namespace manager {

    typedef std::set < std::string > Depends;

    /**
     * @brief read an experimental frame VPZ, build the complete condition
     * combinations and launch or connect to the vle::Simulator and send
     * experimental frame instance VPZi. vle::Manager can listening on
     * localhost port and wait for VPZ stream like a daemon.
     * @param nbProcessor number of processor.
     * @return true if manager is a success.
     */
    VLE_MANAGER_EXPORT bool runManager(bool allInLocal, bool savevpz,
                                       int nbProcessor, const CmdArgs& args);

    /**
     * @brief vle::Simulator: manage a number of processor and launch
     * simulation on it. vle::Simulator run in localhost daemon.
     * @param nbProcessor number of processor.
     * @param port The port [0..65535] to listen.
     * @return true if simulator is a success.
     */
    VLE_MANAGER_EXPORT bool runSimulator(int nbProcessor, int port);

    /**
     * @brief vle::JustRun: start a simulation without analysis of the
     * complete condition combinations, just take the fist and run
     * simulation.
     *
     * @return true if justRun simulation is a success.
     */
    VLE_MANAGER_EXPORT bool justRun(int nbProcessor, const CmdArgs& args);

    /**
     * @brief Build a dictonnary of dependencies. For each vpz file in
     * experiment directory, we produce a list of dependencies.
     * @return The dictionnary.
     */
    VLE_MANAGER_EXPORT std::map < std::string, Depends > depends();

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

    /**
     * @brief A simple functor to produce a vpz::Vpz object from the std::string
     * filename. To use with std::transform for example.
     */
    struct VLE_MANAGER_EXPORT BuildVPZ
    {
        vpz::Vpz* operator()(const std::string& filename) const
        { return new vpz::Vpz(filename); }
    };

}} // namespace vle manager

#endif
