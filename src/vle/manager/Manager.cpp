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


#if defined _WIN32 || defined __CYGWIN__
# define BOOST_THREAD_USE_LIB
# define BOOST_THREAD_DONT_USE_CHRONO
#endif

#include <vle/manager/Manager.hpp>
#include <vle/manager/ExperimentGenerator.hpp>
#include <vle/manager/Simulation.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/vpz/BaseModel.hpp>
#include <boost/thread/thread.hpp>

namespace vle { namespace manager {

/**
 * Assign an new name to the experiment.
 *
 * This function assign a new name to the experiment file.
 *
 * @param destination The experiment where change the name.
 * @param name The base name of the experiment.
 * @param number The combination number.
 */
static void setExperimentName(vpz::Vpz           *destination,
                              const std::string&  name,
                              uint32_t            number)
{
    std::string result(name.size() + 12, '-');

    result.replace(0, name.size(), name);
    result.replace(name.size() + 1, std::string::npos,
                   utils::to < uint32_t >(number));


    destination->project().setInstance(number);
    destination->project().experiment().setName(result);
}

class Manager::Pimpl
{
public:
    Pimpl(LogOptions            logoptions,
          SimulationOptions     simulationoptions,
          std::ostream         *output)
        : mLogOption(logoptions),
          mSimulationOption(simulationoptions),
          mOutputStream(output)
    {
    }

    ~Pimpl()
    {
    }

    template <typename T >
    void writeSummaryLog(const T& fmt)
    {
        if (mLogOption & manager::LOG_SUMMARY) {
            if (mOutputStream) {
                *mOutputStream << fmt;
            } else {
                utils::Trace::send(fmt);
            }
        }
    }

    template <typename T >
    void writeRunLog(const T& fmt)
    {
        if (mLogOption & manager::LOG_RUN) {
            if (mOutputStream) {
                *mOutputStream << fmt;
            } else {
                utils::Trace::send(fmt);
            }
        }
    }

    /**
     * The @c worker is a boost thread functor to execute threaded
     * source code.
     *
     */
    struct worker
    {
        const vpz::Vpz       *vpz;
        ExperimentGenerator  &expgen;
        utils::ModuleManager &modulemgr;
        LogOptions            mLogOption;
        SimulationOptions     mSimulationOption;
        uint32_t              index;
        uint32_t              threads;
        value::Matrix        *result;
        Error                *error;

        worker(const vpz::Vpz        *vpz,
               ExperimentGenerator&   expgen,
               utils::ModuleManager&  modulemgr,
               LogOptions             logoptions,
               SimulationOptions      simulationoptions,
               uint32_t               index,
               uint32_t               threads,
               value::Matrix         *result,
               Error                 *error)
            : vpz(vpz), expgen(expgen), modulemgr(modulemgr),
              mLogOption(logoptions), mSimulationOption(simulationoptions),
              index(index), threads(threads), result(result), error(error)
        {
        }

        ~worker()
        {
        }

        void operator()()
        {
            std::string vpzname(vpz->project().experiment().name());

            for (uint32_t i = expgen.min() + index; i < expgen.max();
                 i += threads) {
                Simulation sim(mLogOption, mSimulationOption, NULL);
                Error err;
                vpz::Vpz *file = new vpz::Vpz(*vpz);
                setExperimentName(file, vpzname, i);
                expgen.get(i, &file->project().experiment().conditions());

                value::Map *simresult = sim.run(file, modulemgr, &err);

                if (err.code) {
                    // writeRunLog(err.message);

                    if (not error->code) {
                        error->code = -1;
                        error->message = _("Manager failure.");
                    }
                } else {
                    result->add(i, 0, simresult);
                }
            }
        }
    };

    value::Matrix * runManagerThread(vpz::Vpz              *vpz,
                                     utils::ModuleManager&  modulemgr,
                                     uint32_t               threads,
                                     uint32_t               rank,
                                     uint32_t               world,
                                     Error                 *error)
    {
        ExperimentGenerator expgen(*vpz, rank, world);
        std::string vpzname(vpz->project().experiment().name());
        boost::thread_group gp;
        value::Matrix *result = new value::Matrix(expgen.size(), 1, expgen.size(), 1);

        for (uint32_t i = 0; i < threads; ++i) {
            gp.create_thread(worker(vpz, expgen, modulemgr,
                                    mLogOption, mSimulationOption,
                                    i, threads, result, error));
        }

        gp.join_all();

         delete vpz->project().model().model();
         delete vpz;

         return result;
    }

    value::Matrix * runManagerMono(vpz::Vpz             *vpz,
                                   utils::ModuleManager &modulemgr,
                                   uint32_t              rank,
                                   uint32_t              world,
                                   Error                *error)
    {
        Simulation sim(mLogOption, mSimulationOption, NULL);
        ExperimentGenerator expgen(*vpz, rank, world);
        std::string vpzname(vpz->project().experiment().name());
        value::Matrix *result = 0;

        error->code = 0;
        error->message.clear();

        if (mSimulationOption == manager::SIMULATION_NO_RETURN) {
            for (uint32_t i = expgen.min(); i < expgen.max(); ++i) {
                Error err;
                vpz::Vpz *file = new vpz::Vpz(*vpz);
                setExperimentName(file, vpzname, i);
                expgen.get(i, &file->project().experiment().conditions());

                sim.run(file, modulemgr, &err);

                if (err.code) {
                    writeRunLog(err.message);

                    if (not error->code) {
                        error->code = -1;
                        error->message = _("Manager failure.");
                    }
                }
            }
        } else {
            result = new value::Matrix(expgen.size(), 1, expgen.size(), 1);

            for (uint32_t i = expgen.min(); i < expgen.max(); ++i) {
                Error err;
                vpz::Vpz *file = new vpz::Vpz(*vpz);
                setExperimentName(file, vpzname, i);
                expgen.get(i, &file->project().experiment().conditions());

                value::Map *simresult = sim.run(file, modulemgr, &err);

                if (err.code) {
                    writeRunLog(err.message);

                    if (not error->code) {
                        error->code = -1;
                        error->message = _("Manager failure.");
                    }
                } else {
                    result->add(i, 0, simresult);
                }
            }
        }

        delete vpz->project().model().model();
        delete vpz;

        return result;
    }

    LogOptions            mLogOption;
    SimulationOptions     mSimulationOption;
    std::ostream         *mOutputStream;
    uint32_t              mCurrentTime;
    uint32_t              mduration;
};

Manager::Manager(LogOptions            logoptions,
                 SimulationOptions     simulationoptions,
                 std::ostream         *output)
    : mPimpl(new Manager::Pimpl(logoptions, simulationoptions, output))
{
}

Manager::~Manager()
{
    delete mPimpl;
}

value::Matrix * Manager::run(vpz::Vpz             *exp,
                             utils::ModuleManager &modulemgr,
                             uint32_t              thread,
                             uint32_t              rank,
                             uint32_t              world,
                             Error                *error)
{
    value::Matrix *result = 0;

    if (thread <= 0) {
        throw vle::utils::ArgError(
            fmt(_("Manager error: thread must be superior to 0 (%1%)"))
            % thread);
    }

    if (world <= rank) {
        throw vle::utils::ArgError(
            fmt(_("Manager error: rank (%1%) must be inferior"
                  " to world (%2%)"))  % rank % world);
    }

    if (world <= 0) {
        throw vle::utils::ArgError(
            fmt(_("Manager error: world (%1%) must be superior to 0."))
            % world);
    }

    mPimpl->writeSummaryLog(_("Manager started"));

    if (thread > 1) {
        result = mPimpl->runManagerThread(exp, modulemgr, thread, rank,
                                          world, error);
    } else {
        result = mPimpl->runManagerMono(exp, modulemgr, rank, world, error);
    }

    mPimpl->writeSummaryLog(_("Manager ended"));

    return result;
}

}} // namespace vle manager
