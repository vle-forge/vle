/*
 * @file vle/manager/Manager.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
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


#include <vle/manager/RunManager.hpp>
#include <vle/manager/Run.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/SharedLibraryManager.hpp>
#include <vle/version.hpp>
#include <boost/thread/thread.hpp>

namespace vle { namespace manager {

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

enum RunManagerType
{
    RUN_MANAGER_MONO,
    RUN_MANAGER_THREAD,
    RUN_MANAGER_MPI
};

struct RunManagerMono
{
    OutputSimulationList * run(const vpz::Vpz&       vpz,
                               utils::ModuleManager& modulemgr)
    {
        ExperimentGenerator expgen(vpz, 0, 1);
        OutputSimulationList *result = new OutputSimulationList(expgen.size());
        std::string vpzname(vpz.project().experiment().name());

        for (uint32_t i = expgen.min(); i <= expgen.max(); ++i) {
            vpz::Vpz *file = new vpz::Vpz(vpz);
            setExperimentName(file, vpzname, i);
            expgen.get(i, &file->project().experiment().conditions());

            RunQuiet r(modulemgr);
            r.start(file);
            result->operator[](i) = r.outputs();
        }

        return result;
    }

    OutputSimulationList *operator()(const std::string&    vpz,
                                     utils::ModuleManager& modulemgr)
    {
        vpz::Vpz exp(vpz);

        return run(exp, modulemgr);
    }

    OutputSimulationList *operator()(const vpz::Vpz&       vpz,
                                     utils::ModuleManager& modulemgr)
    {
        return run(vpz, modulemgr);
    }
};

struct RunManagerThread
{
    OutputSimulationList *operator()(const std::string&    vpz,
                                     utils::ModuleManager& modulemgr,
                                     uint32_t              threads)
    {
        vpz::Vpz exp(vpz);

        return operator()(exp, modulemgr, threads);
    }

    OutputSimulationList *operator()(const vpz::Vpz&       vpz,
                                     utils::ModuleManager& modulemgr,
                                     uint32_t              threads)
    {
        ExperimentGenerator expgen(vpz, 0, 1);
        OutputSimulationList *result = new OutputSimulationList(expgen.size());

        boost::thread_group gp;

        for (uint32_t i = 0; i < threads; ++i) {
            gp.create_thread(RunManagerThread::worker(vpz, modulemgr, i,
                                                      threads, result));
        }

        gp.join_all();

        return result;
    }

    struct worker
    {
        const vpz::Vpz& vpz;
        utils::ModuleManager& modules;
        uint32_t rank;
        uint32_t world;
        OutputSimulationList *result;

        worker(const vpz::Vpz& vpz, utils::ModuleManager& modulemgr,
               uint32_t rank, uint32_t world, OutputSimulationList *result)
            : vpz(vpz), modules(modulemgr), rank(rank), world(world),
            result(result)
        {
        }

        void operator()()
        {
            ExperimentGenerator expgen(vpz, rank, world);
            std::string vpzname(vpz.project().experiment().name());

            for (uint32_t i = expgen.min(); i <= expgen.max(); ++i) {
                vpz::Vpz *file = new vpz::Vpz(vpz);
                setExperimentName(file, vpzname, i);
                expgen.get(i, &file->project().experiment().conditions());

                RunQuiet r(modules);
                r.start(file);
                result->operator[](i) = r.outputs();
            }
        }
    };

};

struct RunManagerMpi
{
    OutputSimulationList * run(const vpz::Vpz&       vpz,
                               utils::ModuleManager& modulemgr,
                               uint32_t              rank,
                               uint32_t              world)
    {
        ExperimentGenerator expgen(vpz, rank, world);
        std::string vpzname(vpz.project().experiment().name());

        for (uint32_t i = expgen.min(); i <= expgen.max(); ++i) {
            vpz::Vpz *file = new vpz::Vpz(vpz);
            setExperimentName(file, vpzname, i);
            expgen.get(i, &file->project().experiment().conditions());

            RunQuiet r(modulemgr);
            r.start(file);
        }

        return NULL;
    }

    OutputSimulationList *operator()(const std::string&    vpz,
                                     utils::ModuleManager& modulemgr,
                                     uint32_t              rank,
                                     uint32_t              world)
    {
        vpz::Vpz exp(vpz);

        return run(exp, modulemgr, rank, world);
    }

    OutputSimulationList *operator()(const vpz::Vpz&       vpz,
                                     utils::ModuleManager& modulemgr,
                                     uint32_t              rank,
                                     uint32_t              world)
    {
        return run(vpz, modulemgr, rank, world);
    }
};

struct RunManager::Pimpl
{
    RunManagerType mType;
    uint32_t mThreads;
    uint32_t mRank;
    uint32_t mWorld;

    Pimpl(RunManagerType type)
        : mType(type)
    {
    }

    ~Pimpl()
    {
    }

private:
    Pimpl(const Pimpl& other);
    Pimpl& operator=(const Pimpl& other);
};

RunManager::RunManager()
    : mPimpl(new RunManager::Pimpl(RUN_MANAGER_MONO))
{
}

RunManager::RunManager(uint32_t threads)
    : mPimpl(new RunManager::Pimpl(RUN_MANAGER_THREAD))
{
    mPimpl->mThreads = threads;
}

RunManager::RunManager(uint32_t rank, uint32_t size)
    : mPimpl(new RunManager::Pimpl(RUN_MANAGER_MPI))
{
    mPimpl->mRank = rank;
    mPimpl->mWorld = size;
}

RunManager::~RunManager()
{
    delete mPimpl;
}

OutputSimulationList* RunManager::run(const std::string& filename)
{
    utils::SharedLibraryManager slm;
    utils::ModuleManager modulemgr;

    switch (mPimpl->mType) {
    case RUN_MANAGER_MONO:
        return RunManagerMono()(filename, modulemgr);
        break;

    case RUN_MANAGER_THREAD:
        return RunManagerThread()(filename, modulemgr, mPimpl->mThreads);
        break;

    case RUN_MANAGER_MPI:
        return RunManagerMpi()(filename, modulemgr, mPimpl->mRank,
                               mPimpl->mWorld);
        break;
    };

    return 0;
}

OutputSimulationList* RunManager::run(const vpz::Vpz& exp)
{
    utils::SharedLibraryManager slm;
    utils::ModuleManager modulemgr;

    switch (mPimpl->mType) {
    case RUN_MANAGER_MONO:
        return RunManagerMono()(exp, modulemgr);
        break;

    case RUN_MANAGER_THREAD:
        return RunManagerThread()(exp, modulemgr, mPimpl->mThreads);
        break;

    case RUN_MANAGER_MPI:
        return RunManagerMpi()(exp, modulemgr, mPimpl->mRank, mPimpl->mWorld);
        break;
    };

    return 0;
}

}}  // namespace vle manager
