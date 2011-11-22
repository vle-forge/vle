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

enum RunManagerType
{
    RUN_MANAGER_MONO,
    RUN_MANAGER_THREAD,
    RUN_MANAGER_MPI
};

struct RunManagerMono
{
    OutputSimulationList *operator()(const std::string& vpz,
                                     utils::ModuleManager& modulemgr)
    {
        ExperimentGenerator expgen(vpz, 0, 1);
        OutputSimulationList *result = new OutputSimulationList(expgen.size());

        for (uint32_t i = expgen.min(); i <= expgen.max(); ++i) {
            vpz::Vpz file(vpz);
            expgen.get(i, &file.project().experiment().conditions());

            RunVerbose r(modulemgr, std::cout);
            r.start(file);
            result->operator[](i) = r.outputs();
        }

        return result;
    }
};

struct RunManagerThread
{
    OutputSimulationList *operator()(const std::string& vpz,
                                     utils::ModuleManager& modulemgr,
                                     uint32_t threads)
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
        const std::string& vpz;
        utils::ModuleManager& modules;
        uint32_t rank;
        uint32_t world;
        OutputSimulationList *result;

        worker(const std::string& vpz, utils::ModuleManager& modulemgr,
               uint32_t rank, uint32_t world, OutputSimulationList *result)
            : vpz(vpz), modules(modulemgr), rank(rank), world(world),
            result(result)
        {
        }

        void operator()()
        {
            ExperimentGenerator expgen(vpz, rank, world);

            for (uint32_t i = expgen.min(); i <= expgen.max(); ++i) {
                vpz::Vpz file(vpz);
                expgen.get(i, &file.project().experiment().conditions());

                RunQuiet r(modules);
                r.start(file);
                result->operator[](i) = r.outputs();
            }
        }
    };

};

struct RunManagerMpi
{
    OutputSimulationList *operator()(const std::string& vpz,
                                     utils::ModuleManager& modulemgr,
                                     uint32_t rank,
                                     uint32_t world)
    {
        ExperimentGenerator expgen(vpz, rank, world);

        for (uint32_t i = expgen.min(); i <= expgen.max(); ++i) {
            vpz::Vpz file(vpz);
            expgen.get(i, &file.project().experiment().conditions());

            RunQuiet r(modulemgr);
            r.start(file);
        }

        return NULL;
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

}}  // namespace vle manager
