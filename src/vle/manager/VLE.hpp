/** 
 * @file VLE.hpp
 * @brief Classes to start simulator, manager or distributor.
 * @author The vle Development Team
 * @date lun, 29 mai 2006 09:52:59 +0200
 */

/*
 * Copyright (C) 2006 - The vle Development Team
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

#ifndef VLE_VLE_HPP
#define VLE_VLE_HPP

#include <vector>

namespace vle { namespace manager {

    /** 
     * @brief A class to start the simulation. This class initialises
     * the user directory and install signal. The desctructor clear all
     * singleton classes.
     */
    class VLE
    {
    public:
        /** 
         * @brief Define a list of filename from command line arguments argv and
         * argc.
         */
        typedef std::vector < const char* > CmdArgs;

        /** 
         * @brief Initialise signal and user directory.
         */
        explicit VLE(int port = 8000);

        /** 
         * @brief Delete all singleton classes.
         */
        ~VLE();

        /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

        /** 
         * @brief read an experimental frame VPZ, build the complete condition
         * combinations and launch or connect to the vle::Simulator and send
         * experimental frame instance VPZi. vle::Manager can listening on
         * localhost port and wait for VPZ stream like a daemon.
         * 
         * @return true if manager is a success.
         */
        bool runManager(bool daemon, bool allInLocal, const CmdArgs& args);

        /** 
         * @brief vle::Simulator: manage a number of processor and launch
         * simulation on it. vle::Simulator run in localhost daemon.
         * 
         * @return true if simulator is a success.
         */
        bool runSimulator(int process);

        /** 
         * @brief vle::JustRun: start a simulation without analysis of the
         * complete condition combinations, just take the fist and run
         * simulation.
         * 
         * @return true if justRun simulation is a success.
         */
        bool justRun(const CmdArgs& args);
        
        /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

        /** 
         * @brief Print the informations of VLE framework.
         * 
         * @param out stream to put information.
         */
        static void printInformations(std::ostream& out);

        /** 
         * @brief Print the version of VLE framework.
         * 
         * @param out stream to put information.
         */
        static void printVersion(std::ostream& out);

    private:
        int             mPort;
    };

}} // namespace vle manager

#endif
