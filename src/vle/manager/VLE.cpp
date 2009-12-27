/**
 * @file vle/manager/VLE.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
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
#include <vle/utils/DateTime.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Rand.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/value/Value.hpp>
#include <vle/devs/Event.hpp>
#include <vle/utils/Path.hpp>

#include <glibmm/optioncontext.h>
#include <iostream>
#include <fstream>
#include <string>



namespace vle { namespace manager {

VLE::VLE(int port) :
    mPort(port)
{
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
                std::cerr << fmt(_(
                    "Manager all simulations in one thread\n"));
                ManagerRunMono r(std::cerr, savevpz);
                r.start(args.front());
            } else {
                std::cerr << fmt(_(
                    "Manager all simulations in %1% processor\n")) % nbProcessor;
                ManagerRunThread r(std::cerr, savevpz, nbProcessor);
                r.start(args.front());
            }
        } else {
            std::cerr << _("Manager with distant simulator\n");
            ManagerRunDistant r(std::cerr, savevpz);
            r.start(args.front());
        }
    } catch(const std::exception& e) {
        std::cerr << fmt(_("\n/!\\ vle manager error reported: %1%\n")) %
            utils::demangle(typeid(e)) << e.what();
        return false;
    }
    return true;
}

bool VLE::runSimulator(int process)
{
    try {
        std::cerr << _("Simulator start in daemon mode\n");

        if (utils::Trace::trace().getLevel() != utils::Trace::DEVS) {
            utils::buildDaemon();
        }

        utils::Trace::trace().setLogFile(
            utils::Trace::getLogFilename(boost::str(fmt(
                    "distant-%1%") % utils::DateTime::simpleCurrentDate())));

        SimulatorDistant sim(utils::Trace::trace().output(), process, mPort);
        sim.start();
    } catch(const std::exception& e) {
        std::cerr << fmt(_("\n/!\\ vle distant simulator error "
                           " reported: %1%")) % utils::demangle(typeid(e)) <<
                e.what();
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
            std::cerr << fmt(_("\n/!\\ vle mono simulator error reported: %1%"))
                % utils::demangle(typeid(e)) << e.what();
            return false;
        }
    } else {
        try {
            JustRunThread jrt(std::cerr, nbProcessor);
            jrt.operator()(args);
        } catch(const std::exception& e) {
            std::cerr << fmt(_("\n/!\\ vle thread simulator error reported: "
                               " %1%")) % utils::demangle(typeid(e)) << e.what();
            return false;
        }
    }
    return true;
}

void init()
{
    value::init();
    utils::init();
    devs::init();
}

void finalize()
{
    value::finalize();
    utils::finalize();
    value::finalize();
    devs::finalize();
}

}} // namespace vle manager

