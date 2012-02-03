/*
 * @file vle/manager/Manager.hpp
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


#ifndef VLE_MANAGER_RUNMANAGER_HPP
#define VLE_MANAGER_RUNMANAGER_HPP

#include <vle/manager/Types.hpp>
#include <vle/manager/ExperimentGenerator.hpp>
#include <vle/vpz/Vpz.hpp>

namespace vle { namespace manager {

/**
 * The @e RunManager allows to run experimental frames.
 *
 * The @e RunManager is no copyable and nonassignable and uses the Pimpl idiom.
 */
class VLE_EXPORT RunManager
{
public:
    /**
     * Build a RunManager object for local simulation.
     *
     * The RunManager executes in local process all experiments from the
     * ExperimentGenerator.
     */
    RunManager();

    /**
     * Build a RunManager object for local threaded simulations.
     *
     * The RunManager executes in local process but with several threads all
     * experiments from the ExperimentGenerator.
     *
     * @param threads Number of thread to be used.
     */
    RunManager(uint32_t threads);

    /**
     * Build a RunManager object for distributed simulations over MPI.
     *
     * The RunManager executes in local process a part of the experiments from
     * the ExperimentGenerator.
     *
     * @param rank The id of the process (for example: the MPI's rank
     * information).
     * @param size The number of process (for example: the MPI's world size
     * information).
     */
    RunManager(uint32_t rank, uint32_t size);

    ~RunManager();

    /**
     * Start the simulations.
     *
     * @param filename The VPZ to run.
     *
     * @return A pointer to a list of list of value matrix, can be NULL.
     */
    OutputSimulationList* run(const std::string& filename);

    /**
     * Start the simulations.
     *
     * @param exp An experiment file, cloned.
     *
     * @return A pointer to a list of list of value matrix, can be NULL.
     */
    OutputSimulationList* run(const vpz::Vpz& exp);

protected:
    RunManager(const RunManager& other);
    RunManager& operator=(const RunManager& other);

    class Pimpl;
    Pimpl *mPimpl;
};

}}  // namespace vle manager

#endif
