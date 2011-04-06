/*
 * @file vle/manager/VLE.cpp
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

Manager::Manager(bool quiet)
    : mQuiet(quiet), mSuccess(true)
{
    if (quiet) {
        mFilename = utils::Path::buildTemp("manager");
        mFileError = new std::ofstream(mFilename.c_str());

        if (not mFileError->is_open()) {
            delete mFileError;

            throw utils::InternalError(
                _("Manager can not build error file in quiet mode"));
        }

        mStreamBufError = std::cerr.rdbuf();
        std::cerr.rdbuf(mFileError->rdbuf());
    }
}

Manager::~Manager()
{
    close();
}

void Manager::close()
{
    if (mQuiet) {
        if (mStreamBufError) {
            std::cerr.rdbuf(mStreamBufError);
        }
        delete mFileError;

        mQuiet = false;
        mFileError = 0;
        mStreamBufError = 0;
    }
}

bool Manager::runManager(bool allInLocal, bool savevpz, int nbProcessor, const
                         CmdArgs& args)
{
    mSuccess = true;

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
                    "Manager all simulations in %1% processor\n")) %
                    nbProcessor;
                ManagerRunThread r(std::cerr, savevpz, nbProcessor);
                r.start(args.front());
            }
        } else {
            std::cerr << _("Manager with distant simulator\n");
            ManagerRunDistant r(std::cerr, savevpz);
            r.start(args.front());
        }
    } catch(const std::exception& e) {
        std::cerr << fmt(
            _("\n/!\\ vle manager error reported: %1%\n")) %
            utils::demangle(typeid(e)) << e.what();
        mSuccess = false;
    }

    return mSuccess;
}

bool Manager::runSimulator(int process, int port)
{
    mSuccess = true;

    try {
        std::cerr << _("Simulator start in daemon mode\n");

        if (utils::Trace::getLevel() != utils::Trace::DEVS) {
            utils::buildDaemon();
        }

        utils::Trace::setLogFile(
            utils::Trace::getLogFilename(boost::str(fmt(
                    "distant-%1%") % utils::DateTime::simpleCurrentDate())));

        SimulatorDistant sim(process, port);
        sim.start();
    } catch(const std::exception& e) {
        std::cerr << fmt(_("\n/!\\ vle distant simulator error "
                           " reported: %1%")) % utils::demangle(typeid(e)) <<
                e.what();
        mSuccess = false;
    }

    return mSuccess;
}

bool Manager::justRun(int nbProcessor, const CmdArgs& args)
{
    mSuccess = true;

    if (nbProcessor == 1) {
        try {
            JustRunMono jrm(std::cerr);
            mSuccess = jrm.operator()(args);
        } catch(const std::exception& e) {
            std::cerr << fmt(_(
                    "\n/!\\ vle mono simulator error reported: %1%")) %
                utils::demangle(typeid(e)) << e.what();
            mSuccess = false;
        }
    } else {
        try {
            JustRunThread jrt(std::cerr, nbProcessor);
            mSuccess = jrt.operator()(args);
        } catch(const std::exception& e) {
            std::cerr << fmt(_("\n/!\\ vle thread simulator error reported: "
                               " %1%")) % utils::demangle(typeid(e)) <<
                e.what();
            mSuccess = false;
        }
    }
    return mSuccess;
}

std::map < std::string, Depends > Manager::depends()
{
    std::map < std::string, Depends > result;

    utils::PathList vpz(utils::Path::path().getInstalledExperiments());
    std::sort(vpz.begin(), vpz.end());

    for (utils::PathList::iterator it = vpz.begin(); it != vpz.end(); ++it) {
        std::set < std::string > depends;
        try {
            vpz::Vpz vpz(*it);
            depends = vpz.depends();
        } catch (const std::exception& /*e*/) {
        }
        result[*it] = depends;
    }

    return result;
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


