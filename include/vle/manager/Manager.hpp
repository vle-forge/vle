/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2003-2018 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2018 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2018 INRA http://www.inra.fr
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
#include <vle/utils/Rand.hpp>
#include <vle/vpz/Vpz.hpp>

namespace vle {
namespace manager {

/**
 * TODO
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

enum ParallelOptions {PARALLEL_MONO, PARALLEL_THREADS, PARALLEL_MPI};

class VLE_API Manager
{
public:
    using result_type = std::unique_ptr<value::Matrix>;

    Manager(utils::ContextPtr context,
            ParallelOptions paralleloption,      // type of parallelisation
            unsigned int nbslots,                // nbslots for parallelization
            SimulationOptions simulationoption,  // for 1 simulation
            std::chrono::milliseconds timeout,   // for 1 simulation
            bool removeMPIfiles,                 // for cvle
            bool generateMPIhost,                // for cvle
            const std::string& workingDir);      // for cvle

    Manager(utils::ContextPtr context);

    ~Manager();

    Manager(const Manager& other) = delete;
    Manager& operator=(const Manager& other) = delete;
    Manager(Manager&& other) = delete;
    Manager& operator=(Manager&& other) = delete;

    /**
     * get access to the random number generator
     */
    utils::Rand& random_number_generator();

    /**
     * @brief Parse a string of the type that should identify an input, eg. :
     *   input_cond.port
     *   replicate_cond.port
     *   cond.port
     *
     * @param[in]  conf, the string to parse
     * @param[out] cond, the condition of the input or empty if not parsed
     * @param[out] port, the port of the input or empty if not parsed
     * @param[in] tells the prefix to parse.
     *
     * @return if parsing was successfull
     */
    static bool parseInput(const std::string& conf, std::string& cond,
        std::string& port, const std::string& prefix ="input_");

    /**
     * @brief Parse a string of the type output_idout that should identify
     * an output
     *
     * @param[in]  conf, the string to parse
     * @param[out] idout, id of the output
     *
     * @return if parsing was successfull
     */
    static bool parseOutput(const std::string& conf, std::string& idout);

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
    /**
     * @brief Simulates the experiment plan
     *
     * @param[in/optional] exp, the model to simulate, if not available, it
     *                          is built from the init structure for
     *                          configurations
     * @param[in] init, structure for the configuration of simulations
     * @param[out] err, an error structure
     *
     * @return the simulated values
     */
    std::unique_ptr<value::Map> runPlan(
            std::shared_ptr<vle::value::Map> init,
            Error& err);
    std::unique_ptr<value::Map> runPlan(
            const vle::value::Map& init,
            Error& err);
    std::unique_ptr<value::Map> runPlan(
            const devs::InitEventList& init,
            Error& err);
    std::unique_ptr<value::Map> runPlan(
            std::unique_ptr<vpz::Vpz> exp,
            std::shared_ptr<vle::value::Map> init,
            Error& err);
    std::unique_ptr<value::Map> runPlan(
            std::unique_ptr<vpz::Vpz> exp,
            const vle::value::Map& init,
            Error& err);
    std::unique_ptr<value::Map> runPlan(
            std::unique_ptr<vpz::Vpz> exp,
            const devs::InitEventList& init,
            Error& err);
    /**
     * @brief Get the embedded model configured for specific input and
     * replicate values
     *
     * @param[in/optional] exp, the model to simulate, if not available, it
     *                          is built from the init structure for
     *                          configurations
     * @param[in] init, structure for the configuration of simulations
     * @param[out] err, an error structure
     * @param[in] input_index, index (between 0 and N-1) of
     *                         the input to set
     * @param[in] replicate_index, index (between 0 and M-1) of
     *                             the replicate to set
     *
     * @return the simulated values
     */
    std::unique_ptr<vpz::Vpz> getEmbedded(
            std::shared_ptr<vle::value::Map> init,
            Error& err,
            unsigned int input_index = 0,
            unsigned int replicate_index = 0);
    std::unique_ptr<vpz::Vpz> getEmbedded(
            const vle::value::Map& init,
            Error& err,
            unsigned int input_index = 0,
            unsigned int replicate_index = 0);
    std::unique_ptr<vpz::Vpz> getEmbedded(
            const devs::InitEventList& events,
            Error& err,
            unsigned int input_index = 0,
            unsigned int replicate_index = 0);
    std::unique_ptr<vpz::Vpz> getEmbedded(
            std::unique_ptr<vpz::Vpz> exp,
            std::shared_ptr<vle::value::Map> init,
            Error& err,
            unsigned int input_index = 0,
            unsigned int replicate_index = 0);
    std::unique_ptr<vpz::Vpz> getEmbedded(
            std::unique_ptr<vpz::Vpz> exp,
            const vle::value::Map& init,
            Error& err,
            unsigned int input_index = 0,
            unsigned int replicate_index = 0);
    std::unique_ptr<vpz::Vpz> getEmbedded(
            std::unique_ptr<vpz::Vpz> exp,
            const devs::InitEventList& events,
            Error& err,
            unsigned int input_index = 0,
            unsigned int replicate_index = 0);

private:
    class Pimpl;
    std::unique_ptr<Pimpl> mPimpl;
};
}
} // namespace vle manager

#endif
