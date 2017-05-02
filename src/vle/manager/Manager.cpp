/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#include <sstream>
#include <thread>
#include <vle/manager/ExperimentGenerator.hpp>
#include <vle/manager/Manager.hpp>
#include <vle/manager/Simulation.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/vpz/BaseModel.hpp>
#include <vle/vpz/Vpz.hpp>

namespace vle {
namespace manager {

struct vle_log_manager_thread : vle::utils::Context::LogFunctor
{
    FILE* fp;
    unsigned int th;

    vle_log_manager_thread(unsigned int _th)
      : fp(nullptr)
      , th(_th)
    {
    }

    virtual ~vle_log_manager_thread()
    {
        if (fp)
            fclose(fp);
    }

    virtual void write(const vle::utils::Context& /*ctx*/,
                       int priority,
                       const char* file,
                       int line,
                       const char* fn,
                       const char* format,
                       va_list args) noexcept override
    {
        if (not fp) {
            std::stringstream s;
            s.imbue(std::locale::classic());
            s << "vle_manager_thread_" << th << ".log";
            fp = fopen(s.str().c_str(), "w");
        }

        if (fp) {
            if (priority == 7)
                fprintf(fp, "[dbg] %s:%d %s: ", file, line, fn);
            else if (priority == 6)
                fprintf(fp, "%s: ", fn);
            else
                fprintf(fp, "[Error] %s: ", fn);

            vfprintf(fp, format, args);
        }
    }
};

/**
 * Assign an new name to the experiment.
 *
 * This function assign a new name to the experiment file.
 *
 * @param destination The experiment where change the name.
 * @param name The base name of the experiment.
 * @param number The combination number.
 */
static void
setExperimentName(const std::unique_ptr<vpz::Vpz>& destination,
                  const std::string& name,
                  uint32_t number)
{
    std::string result(name.size() + 12, '-');

    result.replace(0, name.size(), name);
    result.replace(
      name.size() + 1, std::string::npos, utils::to<uint32_t>(number));

    destination->project().setInstance(number);
    destination->project().experiment().setName(result);
}

class Manager::Pimpl
{
public:
    Pimpl(utils::ContextPtr context,
          LogOptions logoptions,
          SimulationOptions simulationoptions,
          std::chrono::milliseconds timeout,
          std::ostream* output)
      : mContext(context)
      , mTimeout(timeout)
      , mOutputStream(output)
      , mLogOption(logoptions)
      , mSimulationOption(simulationoptions)
    {
        if (timeout != std::chrono::milliseconds::zero())
            mSimulationOption |= vle::manager::SIMULATION_SPAWN_PROCESS;
    }

    template <typename T>
    void writeSummaryLog(const T& fmt)
    {
        if (mLogOption & manager::LOG_SUMMARY and mOutputStream)
            *mOutputStream << fmt;
    }

    template <typename T>
    void writeRunLog(const T& fmt)
    {
        if (mLogOption & manager::LOG_RUN and mOutputStream)
            *mOutputStream << fmt;
    }

    /**
     * The @c worker is a boost thread functor to execute threaded
     * source code.
     *
     */
    struct worker
    {
        utils::ContextPtr context;
        const std::unique_ptr<vpz::Vpz>& vpz;
        std::chrono::milliseconds mTimeout;
        ExperimentGenerator& expgen;
        LogOptions mLogOption;
        SimulationOptions mSimulationOption;
        uint32_t index;
        uint32_t threads;
        value::Matrix* result;
        Error* error;

        worker(utils::ContextPtr context,
               const std::unique_ptr<vpz::Vpz>& vpz,
               std::chrono::milliseconds timeout,
               ExperimentGenerator& expgen,
               LogOptions logoptions,
               SimulationOptions simulationoptions,
               uint32_t index,
               uint32_t threads,
               value::Matrix* result,
               Error* error)
          : context(context)
          , vpz(vpz)
          , mTimeout(timeout)
          , expgen(expgen)
          , mLogOption(logoptions)
          , mSimulationOption(simulationoptions)
          , index(index)
          , threads(threads)
          , result(result)
          , error(error)
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
                Simulation sim(
                  context, mLogOption, mSimulationOption, mTimeout, nullptr);
                Error err;

                auto file = std::unique_ptr<vpz::Vpz>(new vpz::Vpz(*vpz));
                setExperimentName(file, vpzname, i);
                expgen.get(i, &file->project().experiment().conditions());

                auto simresult = sim.run(std::move(file), &err);

                if (err.code) {
                    if (not error->code) {
                        error->code = -1;
                        error->message = err.message;
                    }
                } else {
                    result->add(i, 0, std::move(simresult));
                }
            }
        }
    };

    std::unique_ptr<value::Matrix> runManagerThread(
      std::unique_ptr<vpz::Vpz> vpz,
      uint32_t threads,
      uint32_t rank,
      uint32_t world,
      Error* error)
    {
        ExperimentGenerator expgen(*vpz, rank, world);
        std::string vpzname(vpz->project().experiment().name());

        auto result = std::unique_ptr<value::Matrix>(
          new value::Matrix(expgen.size(), 1, expgen.size(), 1));

        std::vector<std::thread> gp;
        for (uint32_t i = 0; i < threads; ++i) {
            utils::ContextPtr ctx = mContext->clone();
            ctx->set_log_function(std::unique_ptr<utils::Context::LogFunctor>(
              new vle_log_manager_thread(i)));
            gp.emplace_back(worker(ctx,
                                   vpz,
                                   mTimeout,
                                   expgen,
                                   mLogOption,
                                   mSimulationOption,
                                   i,
                                   threads,
                                   result.get(),
                                   error));
        }

        for (uint32_t i = 0; i < threads; ++i)
            gp[i].join();

        return result;
    }

    std::unique_ptr<value::Matrix> runManagerMono(
      std::unique_ptr<vpz::Vpz> vpz,
      uint32_t rank,
      uint32_t world,
      Error* error)
    {
        Simulation sim(
          mContext, mLogOption, mSimulationOption, mTimeout, nullptr);
        ExperimentGenerator expgen(*vpz, rank, world);
        std::string vpzname(vpz->project().experiment().name());
        std::unique_ptr<value::Matrix> result;

        error->code = 0;
        error->message.clear();

        if (mSimulationOption == manager::SIMULATION_NO_RETURN) {
            for (uint32_t i = expgen.min(); i < expgen.max(); ++i) {
                Error err;
                auto file = std::unique_ptr<vpz::Vpz>(new vpz::Vpz(*vpz));
                setExperimentName(file, vpzname, i);
                expgen.get(i, &file->project().experiment().conditions());

                sim.run(std::move(file), &err);

                if (err.code) {
                    writeRunLog(err.message);

                    if (not error->code) {
                        error->code = -1;
                        error->message = err.message;
                    }
                }
            }
        } else {
            result = std::unique_ptr<value::Matrix>(
              new value::Matrix(expgen.size(), 1, expgen.size(), 1));

            for (uint32_t i = expgen.min(); i < expgen.max(); ++i) {
                Error err;
                auto file = std::unique_ptr<vpz::Vpz>(new vpz::Vpz(*vpz));
                setExperimentName(file, vpzname, i);
                expgen.get(i, &file->project().experiment().conditions());

                auto simresult = sim.run(std::move(file), &err);

                if (err.code) {
                    writeRunLog(err.message);

                    if (not error->code) {
                        error->code = -1;
                        error->message = err.message;
                    }
                } else {
                    result->add(i, 0, std::move(simresult));
                }
            }
        }

        return result;
    }

    utils::ContextPtr mContext;
    std::chrono::milliseconds mTimeout;
    std::ostream* mOutputStream;
    LogOptions mLogOption;
    SimulationOptions mSimulationOption;
};

Manager::Manager(utils::ContextPtr context,
                 LogOptions logoptions,
                 SimulationOptions simulationoptions,
                 std::ostream* output)
  : mPimpl(std::make_unique<Manager::Pimpl>(context,
                                            logoptions,
                                            simulationoptions,
                                            std::chrono::milliseconds(0),
                                            output))
{
}

Manager::Manager(utils::ContextPtr context,
                 LogOptions logoptions,
                 SimulationOptions simulationoptions,
                 std::chrono::milliseconds timeout,
                 std::ostream* output)
  : mPimpl(std::make_unique<Manager::Pimpl>(context,
                                            logoptions,
                                            simulationoptions,
                                            timeout,
                                            output))
{
}

Manager::~Manager() = default;

std::unique_ptr<value::Matrix>
Manager::run(std::unique_ptr<vpz::Vpz> exp,
             uint32_t thread,
             uint32_t rank,
             uint32_t world,
             Error* error)
{
    std::unique_ptr<value::Matrix> result;

    if (thread <= 0) {
        throw vle::utils::ArgError(
          (fmt(_("Manager error: thread must be superior to 0 (%1%)")) %
           thread)
            .str());
    }

    if (world <= rank) {
        throw vle::utils::ArgError(
          (fmt(_("Manager error: rank (%1%) must be inferior"
                 " to world (%2%)")) %
           rank % world)
            .str());
    }

    if (world <= 0) {
        throw vle::utils::ArgError(
          (fmt(_("Manager error: world (%1%) must be superior to 0.")) % world)
            .str());
    }

    mPimpl->writeSummaryLog(_("Manager started"));

    if (thread > 1) {
        result =
          mPimpl->runManagerThread(std::move(exp), thread, rank, world, error);
    } else {
        result = mPimpl->runManagerMono(std::move(exp), rank, world, error);
    }

    mPimpl->writeSummaryLog(_("Manager ended"));

    return result;
}
}
} // namespace vle manager
