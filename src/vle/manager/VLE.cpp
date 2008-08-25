/**
 * @file vle/manager/VLE.cpp
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


#include <vle/devs/Coordinator.hpp>
#include <vle/manager/Run.hpp>
#include <vle/manager/SimulatorDistant.hpp>
#include <vle/manager/Manager.hpp>
#include <vle/manager/JustRun.hpp>
#include <vle/manager/VLE.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Rand.hpp>
#include <vle/vpz/Vpz.hpp>

#include <glibmm/optioncontext.h>
#include <iostream>
#include <fstream>
#include <string>



namespace vle { namespace manager {

VLE::VLE(int port) :
    mPort(port)
{
    utils::init();
}

VLE::~VLE()
{
    utils::Path::kill();
    utils::Trace::kill();
}

bool VLE::runManager(bool allInLocal, bool savevpz, int nbProcessor, const
                     CmdArgs& args)
{
    CmdArgs::const_iterator it = args.begin();

    try {
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
        std::cerr << "\n/!\\ vle manager error reported: "
            << utils::demangle(typeid(e)) << "\n" << e.what();
        return false;
    }
    return true;
}

bool VLE::runSimulator(int process)
{
    try {
        std::cerr << "Simulator start in daemon mode\n";

        if (utils::Trace::trace().getLevel() != utils::Trace::DEBUG) {
            utils::buildDaemon();
        }
        
        utils::Trace::trace().setLogFile(
            utils::Trace::getLogFilename(boost::str(boost::format(
                    "distant-%1%") % utils::get_simple_current_date())));

        SimulatorDistant sim(utils::Trace::trace().output(), process, mPort);
        sim.start();
    } catch(const std::exception& e) {
            std::cerr << "\n/!\\ vle distant simulator error reported: "
                << utils::demangle(typeid(e)) << "\n" << e.what();
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
            std::cerr << "\n/!\\ vle mono simulator error reported: "
                << utils::demangle(typeid(e)) << "\n" << e.what();
            return false;
        }
    } else {
        try {
            JustRunThread jrt(std::cerr, nbProcessor);
            jrt.operator()(args);
        } catch(const std::exception& e) {
            std::cerr << "\n/!\\ vle thread simulator error reported: "
                << utils::demangle(typeid(e)) << "\n" << e.what();
            return false;
        }
    }
    return true;
}

}} // namespace vle manager

