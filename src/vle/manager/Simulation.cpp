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


#include <vle/DllDefines.hpp>
#include <vle/utils/ModuleManager.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/manager/Simulation.hpp>
#include <boost/timer.hpp>
#include <boost/progress.hpp>

namespace vle { namespace manager {

class Simulation::Pimpl
{
public:
    std::ostream      *m_out;
    LogOptions         m_logoptions;
    SimulationOptions  m_simulationoptions;

    Pimpl(LogOptions         logoptions,
          SimulationOptions  simulationoptionts,
          std::ostream      *output)
        : m_out(output),
          m_logoptions(logoptions),
          m_simulationoptions(simulationoptionts)
    {
        if (m_simulationoptions & manager::SIMULATION_SPAWN_PROCESS)
            TraceAlways(
                _("Simulation: SIMULATION_SPAWN_PROCESS is not yet"
                    " implemented"));
    }

    ~Pimpl()
    {
    }

    template <typename T>
    void write(const T& t)
    {
        if (m_out) {
            (*m_out) << t;
        } else {
            utils::Trace::send(t);
        }
    }

    value::Map * runVerboseRun(vpz::Vpz                   *vpz,
                               const utils::ModuleManager &modulemgr,
                               Error                      *error)
    {
        value::Map   *result = 0;
        boost::timer  timer;

        try {
            devs::RootCoordinator root(modulemgr);

            const double duration = vpz->project().experiment().duration();
            const double begin    = vpz->project().experiment().begin();

            write(fmt(_("[%1%]\n")) % vpz->filename());
            write(_(" - Coordinator load models ......: "));

            root.load(*vpz);

            write(_("ok\n"));

            write(_(" - Clean project file ...........: "));
            vpz->clear();
            delete vpz;
            write(_("ok\n"));

            write(_(" - Coordinator initializing .....: "));
            root.init();
            write(_("ok\n"));

            write(_(" - Simulation run................: "));

            boost::progress_display display(100, *m_out, "\n   ", "   ", "   ");
            long                    previous = 0;

            while (root.run()) {
                long pc = std::floor(100. * (root.getCurrentTime() - begin) /
                                     duration);

                display  += pc - previous;
                previous  = pc;
            }

            display += 100 - previous;

            write(_(" - Coordinator cleaning .........: "));
            root.finish();
            write(_("ok\n"));

            result = root.outputs();

            write(fmt(_(" - Time spent in kernel .........: %1% s"))
                  % timer.elapsed());

            error->code    = 0;
        } catch(const std::exception& e) {
            error->message = (fmt(_("\n/!\\ vle error reported: %1%\n%2%"))
                              % utils::demangle(typeid(e))
                              % e.what()).str();
            error->code    = -1;
        }

        return result;
    }

    value::Map * runVerboseSummary(vpz::Vpz                   *vpz,
                                   const utils::ModuleManager &modulemgr,
                                   Error                      *error)
    {
        value::Map   *result = 0;
        boost::timer  timer;

        try {
            devs::RootCoordinator root(modulemgr);

            write(fmt(_("[%1%]\n")) % vpz->filename());
            write(_(" - Coordinator load models ......: "));

            root.load(*vpz);

            write(_("ok\n"));

            write(_(" - Clean project file ...........: "));
            vpz->clear();
            delete vpz;
            write(_("ok\n"));

            write(_(" - Coordinator initializing .....: "));
            root.init();
            write(_("ok\n"));

            write(_(" - Simulation run................: "));

            while (root.run());
            write(_("ok\n"));

            write(_(" - Coordinator cleaning .........: "));
            root.finish();
            write(_("ok\n"));

            result = root.outputs();

            write(fmt(_(" - Time spent in kernel .........: %1% s"))
                  % timer.elapsed());

            error->code    = 0;
        } catch(const std::exception& e) {
            error->message = (fmt(_("\n/!\\ vle error reported: %1%\n%2%"))
                              % utils::demangle(typeid(e))
                              % e.what()).str();
            error->code    = -1;
        }

        return result;
    }

    value::Map * runQuiet(vpz::Vpz                   *vpz,
                          const utils::ModuleManager &modulemgr,
                          Error                      *error)
    {
        value::Map *result = 0;

        try {
            devs::RootCoordinator root(modulemgr);
            root.load(*vpz);
            vpz->clear();
            delete vpz;

            root.init();
            while (root.run()) {}
            root.finish();

            error->code    = 0;
            result         = root.outputs();
        } catch(const std::exception& e) {
            error->message = (fmt(_("\n/!\\ vle error reported: %1%\n%2%"))
                              % utils::demangle(typeid(e))
                              % e.what()).str();
            error->code    = -1;
        }

        return result;
    }

};

Simulation::Simulation(LogOptions         logoptions,
                       SimulationOptions  simulationoptionts,
                       std::ostream      *output)
    : mPimpl(new Simulation::Pimpl(logoptions, simulationoptionts, output))
{
}

Simulation::~Simulation()
{
    delete mPimpl;
}

value::Map * Simulation::run(vpz::Vpz                   *vpz,
                             const utils::ModuleManager &modulemgr,
                             Error                      *error)
{
    error->code = 0;
    value::Map *result = NULL;

    if (mPimpl->m_logoptions != manager::LOG_NONE) {
        if (mPimpl->m_logoptions & manager::LOG_RUN and mPimpl->m_out) {
            result = mPimpl->runVerboseRun(vpz, modulemgr, error);
        } else {
            result = mPimpl->runVerboseSummary(vpz, modulemgr, error);
        }

    } else {
        result = mPimpl->runQuiet(vpz, modulemgr, error);
    }

    if (mPimpl->m_simulationoptions & manager::SIMULATION_NO_RETURN) {
        delete result;
        return NULL;
    } else {
        return result;
    }
}

}}
