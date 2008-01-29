/** 
 * @file manager/VLE.cpp
 * @brief Classes to start simulator, manager or distributor.
 * @author The vle Development Team
 * @date lun, 23 jan 2006 14:09:01 +0100
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

#include <vle/devs/Coordinator.hpp>
#include <vle/manager/Run.hpp>
#include <vle/manager/SimulatorDistant.hpp>
#include <vle/manager/Manager.hpp>
#include <vle/manager/JustRun.hpp>
#include <vle/manager/VLE.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Socket.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Rand.hpp>
#include <vle/vpz/Vpz.hpp>

#include <glibmm/optioncontext.h>
#include <glibmm/thread.h>
#include <iostream>
#include <fstream>
#include <string>



namespace vle { namespace manager {

VLE::VLE(int port) :
    mPort(port)
{
    utils::install_signal();
    utils::initUserDirectory();
    utils::net::Base::init();
}

VLE::~VLE()
{
    utils::Path::kill();
    utils::Rand::kill();
    utils::Trace::kill();
}

bool VLE::runManager(bool allInLocal, bool savevpz, int nbProcessor, const
                     CmdArgs& args)
{
    CmdArgs::const_iterator it = args.begin();

    try {
        Glib::thread_init();
        if (allInLocal) {
            if (nbProcessor == 1) {
                std::cerr << boost::format(
                    "Manager all simulations in one thread\n");
                ManagerRunMono r(std::cerr, savevpz);
                r.start(args.front());
            } else {
                std::cerr << boost::format(
                    "Manager all simulations in %1% processor\n") % nbProcessor;
                ManagerRunThread r(std::cerr, savevpz, nbProcessor);
                r.start(args.front());
            }
        } else {
            std::cerr << "Manager with distant simulator\n";
            ManagerRunDistant r(std::cerr, savevpz);
            r.start(args.front());
        }
    } catch(const std::exception& e) {
        std::cerr << "Manager error: "
            << utils::demangle(e.what()) << std::endl;
        return false;
    }
    return true;
}

bool VLE::runSimulator(int process)
{
    try {
        std::cerr << "Simulator start in daemon mode\n";
        Glib::thread_init();

        if (utils::Trace::trace().getLevel() != utils::Trace::DEBUG) {
            utils::buildDaemon();
        }

        SimulatorDistant sim(std::cerr, process, mPort);
        sim.start();
    } catch(const std::exception& e) {
        std::cerr << "Simulator error: " << e.what() << std::endl;
        return false;
    }
    return true;
}

bool VLE::justRun(int nbProcessor, const CmdArgs& args)
{
    if (nbProcessor == 1) {
        try {
            JustRunMono jrm(std::cerr);
            jrm.operator()(args);
        } catch(const std::exception& e) {
            std::cerr << "Simulation error: " << e.what() << std::endl;
            return false;
        }
    } else {
        try {
            Glib::thread_init();
            JustRunThread jrt(std::cerr, nbProcessor);
            jrt.operator()(args);
        } catch(const std::exception& e) {
            std::cerr << "Simulation error: " << e.what() << std::endl;
            return false;
        }
    }
    return true;
}

void VLE::printInformations(std::ostream& out)
{
    out <<
        "Virtual Laboratory Environment - " << VLE_PACKAGE_VERSION << "\n"
        "Copyright (C) 2004, 05, 06, 07 VLE Development Team.\n"
        "VLE comes with ABSOLUTELY NO WARRANTY.\n"
        "You may redistribute copies of VLE\n"
        "under the terms of the GNU General Public License.\n"
        "For more information about these matters, see the file named COPYING."
        << std::endl;
}

void VLE::printVersion(std::ostream& out)
{
    out << VLE_PACKAGE_VERSION << std::endl;
}

}} // namespace vle manager

