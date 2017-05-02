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

#ifndef VLE_DEVS_THREAD_HPP
#define VLE_DEVS_THREAD_HPP

#include <atomic>
#include <chrono>
#include <thread>
#include <vle/devs/Simulator.hpp>
#include <vle/utils/Context.hpp>
#include <vle/utils/ContextPrivate.hpp>
#include <vle/utils/i18n.hpp>

namespace vle {
namespace devs {

template <typename SimulatorT>
bool
simulator_process(SimulatorT* simulator, Time time) noexcept
{
    try {
        if (simulator->haveInternalEvent()) {
            if (not simulator->haveExternalEvents())
                simulator->internalTransition(time);
            else
                simulator->confluentTransitions(time);
        } else {
            simulator->externalTransition(time);
        }
    } catch (...) {
        return false;
    }

    return true;
}

class SimulatorProcessParallel
{
    std::vector<std::thread> m_workers;
    std::atomic<long int> m_block_id;
    std::atomic<long int> m_block_count;
    std::atomic<bool> m_running_flag;

    std::vector<Simulator*>* m_jobs;
    Time m_time;
    long m_block_size;

    void run()
    {
        while (m_running_flag.load(std::memory_order_relaxed)) {
            auto block = m_block_id.fetch_sub(1, std::memory_order_relaxed);

            if (block >= 0) {
                std::size_t begin = block * m_block_size;
                std::size_t begin_plus_b = begin + m_block_size;
                std::size_t end = std::min(m_jobs->size(), begin_plus_b);

                for (; begin < end; ++begin)
                    simulator_process((*m_jobs)[begin], m_time);

                m_block_count.fetch_sub(1, std::memory_order_relaxed);
            } else {
                //
                // TODO: Maybe we can use a yield instead of this
                // sleep_for function to reduce the overhead of the
                // current thread.
                //
                std::this_thread::sleep_for(std::chrono::nanoseconds(1));
            }
        }
    }

public:
    SimulatorProcessParallel(utils::ContextPtr context)
      : m_jobs(nullptr)
    {
        long block_size = 8;
        {
            context->get_setting("vle.simulation.block-size", &block_size);

            if (block_size <= 0)
                m_block_size = 8;
            else
                m_block_size = block_size;
        }

        long workers_count = 1;
        {
            context->get_setting("vle.simulation.thread", &workers_count);

            if (workers_count <= 0)
                workers_count = 0l;
        }

        vInfo(context,
              _("Simulation kernel: thread:%ld block-size:%ld\n"),
              workers_count,
              m_block_size);

        m_block_id.store(-1, std::memory_order_relaxed);
        m_block_count.store(-1, std::memory_order_relaxed);
        m_running_flag.store(true, std::memory_order_relaxed);

        try {
            m_workers.reserve(workers_count);
            for (long i = 0; i != workers_count; ++i)
                m_workers.emplace_back(&SimulatorProcessParallel::run, this);
        } catch (...) {
            m_running_flag.store(false, std::memory_order_relaxed);
            throw;
        }
    }

    ~SimulatorProcessParallel() noexcept
    {
        m_running_flag.store(false, std::memory_order_relaxed);

        for (auto& thread : m_workers)
            if (thread.joinable())
                thread.join();
    }

    bool parallelize() const noexcept
    {
        return not m_workers.empty();
    }

    bool for_each(std::vector<Simulator*>& simulators, Time time) noexcept
    {
        m_jobs = &simulators;
        m_time = time;

        auto sz = (simulators.size() / m_block_size) +
                  ((simulators.size() % m_block_size) ? 1 : 0);

        m_block_count.store(sz, std::memory_order_relaxed);
        m_block_id.store(sz, std::memory_order_relaxed);

        for (;;) {
            auto block = m_block_id.fetch_sub(1, std::memory_order_relaxed);

            if (block < 0)
                break;

            std::size_t begin = block * m_block_size;
            std::size_t begin_plus_b = begin + m_block_size;
            std::size_t end = std::min(m_jobs->size(), begin_plus_b);

            for (; begin < end; ++begin)
                simulator_process((*m_jobs)[begin], m_time);

            m_block_count.fetch_sub(1, std::memory_order_relaxed);
        }

        while (m_block_count.load(std::memory_order_relaxed) >= 0)
            std::this_thread::sleep_for(std::chrono::nanoseconds(1));

        m_jobs = nullptr;

        return true;
    }
};
}
}

#endif
