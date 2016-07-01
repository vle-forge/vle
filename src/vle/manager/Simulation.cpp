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
#include <vle/utils/ContextPrivate.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/manager/Simulation.hpp>
#include <boost/timer.hpp>
#include <boost/progress.hpp>
#include <sstream>

namespace vle { namespace manager {

class Simulation::Pimpl
{
public:
    utils::ContextPtr  m_context;
    std::ostream      *m_out;
    LogOptions         m_logoptions;
    SimulationOptions  m_simulationoptions;

    Pimpl(utils::ContextPtr  context,
          LogOptions         logoptions,
          SimulationOptions  simulationoptionts,
          std::ostream      *output)
        : m_context(context)
        , m_out(output)
        , m_logoptions(logoptions)
        , m_simulationoptions(simulationoptionts)
    {
        if (m_simulationoptions & manager::SIMULATION_SPAWN_PROCESS)
            vInfo(m_context, _("Simulation: SIMULATION_SPAWN_PROCESS is not "
                               " yet implemented\n"));
    }

    template <typename T>
    void write(const T& t)
    {
        if (m_out)
            (*m_out) << t;
    }

    std::unique_ptr<value::Map>
    runVerboseRun(std::unique_ptr<vpz::Vpz>   vpz,
                  Error                      *error)
    {
        std::unique_ptr<value::Map> result;
        boost::timer  timer;

        try {
            devs::RootCoordinator root(m_context);

            const double duration = vpz->project().experiment().duration();
            const double begin    = vpz->project().experiment().begin();

            write(fmt(_("[%1%]\n")) % vpz->filename());
            write(_(" - Coordinator load models ......: "));

            root.load(*vpz);

            write(_("ok\n"));

            write(_(" - Clean project file ...........: "));
            vpz->clear();
            vpz.reset(nullptr);
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

    std::unique_ptr<value::Map>
    runVerboseSummary(std::unique_ptr<vpz::Vpz>   vpz,
                      Error                      *error)
    {
        std::unique_ptr<value::Map> result;
        boost::timer  timer;

        try {
            devs::RootCoordinator root(m_context);

            write(fmt(_("[%1%]\n")) % vpz->filename());
            write(_(" - Coordinator load models ......: "));

            root.load(*vpz);

            write(_("ok\n"));

            write(_(" - Clean project file ...........: "));
            vpz->clear();
            vpz.reset(nullptr);
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

    std::unique_ptr<value::Map>
    runQuiet(std::unique_ptr<vpz::Vpz>   vpz,
             Error                      *error)
    {
        std::unique_ptr<value::Map> result;

        try {
            devs::RootCoordinator root(m_context);
            root.load(*vpz);
            vpz->clear();
            vpz.reset(nullptr);

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

Simulation::Simulation(utils::ContextPtr  context,
                       LogOptions         logoptions,
                       SimulationOptions  simulationoptionts,
                       std::ostream      *output)
    : mPimpl(std::make_unique<Simulation::Pimpl>(context, logoptions,
                                                 simulationoptionts, output))
{
}

Simulation::~Simulation() = default;

std::unique_ptr<value::Map>
Simulation::run(std::unique_ptr<vpz::Vpz>  vpz,
                Error                      *error)
{
    error->code = 0;
    std::unique_ptr<value::Map> result;

    if (mPimpl->m_logoptions != manager::LOG_NONE) {
        if (mPimpl->m_logoptions & manager::LOG_RUN and mPimpl->m_out) {
            result = mPimpl->runVerboseRun(std::move(vpz), error);
        } else {
            result = mPimpl->runVerboseSummary(std::move(vpz), error);
        }

    } else {
        result = mPimpl->runQuiet(std::move(vpz), error);
    }

    if (mPimpl->m_simulationoptions & manager::SIMULATION_NO_RETURN) {
        return {};
    } else {
        return result;
    }
}

}}
