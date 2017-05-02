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

#ifndef VLE_MANAGER_MANAGER_HPP
#define VLE_MANAGER_MANAGER_HPP

#include <chrono>
#include <vle/DllDefines.hpp>
#include <vle/manager/Types.hpp>
#include <vle/utils/Context.hpp>
#include <vle/vpz/Vpz.hpp>

namespace vle {
namespace manager {

/**
 * @c manager::Manager permits to run experimental frames.
 *
 * The @c manager::Manager returns a @c value::Matrix. The lines are
 * replicas and the columns are combination index from the @c
 * manager::ExperimentGenerator. A cell of the @c value::Matrix is a
 * @c value::Map.  The key is the name of the @c devs::View and the
 * value is a @c value::Matrix or NULL if the @c value::Matrix is
 * empty.
 *
 * @attention You are in charge to freed the manager result @c
 * value::Matrix.
 */
class VLE_API Manager
{
public:
    using result_type = std::unique_ptr<value::Matrix>;

    Manager(utils::ContextPtr context,
            LogOptions logoptions,
            SimulationOptions simulationoptions,
            std::ostream* output);

    Manager(utils::ContextPtr context,
            LogOptions logoptions,
            SimulationOptions simulationoptions,
            std::chrono::milliseconds timeout,
            std::ostream* output);

    ~Manager();

    Manager(const Manager& other) = delete;
    Manager& operator=(const Manager& other) = delete;
    Manager(Manager&& other) = delete;
    Manager& operator=(Manager&& other) = delete;

    /**
     * Run an part or a complete experimental frames with mono thread
     * or multi-thread.
     *
     * @param exp
     * @param thread
     * @param rank
     * @param world
     *
     * (1, 0, 1) defines one thread for the complete experimental
     * frame.  (4, 0, 1) defines four threads for the complete
     * experimental frame. (1, 0, 2) defines one thread for half of
     * the experimental frame. (4, 0, 2) defines four thread by half
     * of experimental frame.
     *
     * @return A @c value::Matrix to freed.
     */
    std::unique_ptr<value::Matrix> run(std::unique_ptr<vpz::Vpz> exp,
                                       uint32_t thread,
                                       uint32_t rank,
                                       uint32_t world,
                                       Error* error);

private:
    class Pimpl;
    std::unique_ptr<Pimpl> mPimpl;
};
}
} // namespace vle manager

#endif
