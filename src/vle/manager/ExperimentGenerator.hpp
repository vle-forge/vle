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

#ifndef VLE_MANAGER_EXPERIMENTGENERATOR_HPP
#define VLE_MANAGER_EXPERIMENTGENERATOR_HPP

#include <vle/DllDefines.hpp>

#include <string>
#include <vle/vpz/Conditions.hpp>
#include <vle/vpz/Vpz.hpp>

namespace vle {
namespace manager {

/**
 * ExperimentGenerator build @e vpz::Conditions from an experimental frame.
 *
 * For example:
 * @code
 * ExperimentGenerator expgen("gens.vpz", 0, 10);
 *
 * assert(expgen.size() == 100);
 * assert(expgen.min() == 0);
 * assert(expgen.max() == 9);
 *
 * ExperimentGenerator expgen("gens.vpz", 9, 10);
 * assert(expgen.size() == 100);
 * assert(expgen.min() == 90);
 * assert(expgen.max() == 99);
 *
 * for (uint32_t i = expgen.min(); i < expgen.max(); ++i) {
 *   vpz::Conditions conds;
 *   expgen.get(i, &conds);
 * }
 * @endcode
 *
 * The class ExperimentGenerator is no copyable and nonassignable and uses the
 * Pimpl idiom.
 */
class VLE_API ExperimentGenerator
{
public:
    /**
     * Prepare the ExperimentGenerator to build @e vpz::Conditions.
     *
     * @param vpz The filename of a VPZ file.
     * @param rank The id of the worker.
     * @param size The number of workers for this experimental frame.
     *
     * @throw utils::Exception if the filename does not exist.
     */
    ExperimentGenerator(const std::string& vpz, uint32_t rank, uint32_t size);

    /**
     * Prepare the ExperimentGenerator to build @e vpz::Conditions.
     *
     * @param vpz A constant reference of a VPZ file (internally, cloned).
     * @param rank The id of the worker.
     * @param size The number of workers for this experimental frame.
     */
    ExperimentGenerator(const vpz::Vpz& vpz, uint32_t rank, uint32_t size);

    ~ExperimentGenerator();

    /**
     * Get the conditions of the specified index.
     *
     * The @e index parameter would be greater or equal to @e min() and lower
     * the @e max().
     *
     * @param[in] index The index in the experiment generator table.
     * @param[out] conditions Conditions to fill with new conditions.
     */
    void get(uint32_t index, vpz::Conditions* conditions);

    /**
     * The minimal index of experiences produce by the object.
     *
     * @return An integer lower than @e max().
     */
    uint32_t min() const;

    /**
     * The maximal index of experiences produce by the object.
     *
     * @return An integer greater than @e min().
     */
    uint32_t max() const;

    /**
     * Get the total number of experiences to be produced.
     *
     * @return The total number of experiences.
     */
    uint32_t size() const;

private:
    ExperimentGenerator(const ExperimentGenerator& other);
    ExperimentGenerator& operator=(const ExperimentGenerator& other);

    class Pimpl;
    Pimpl* mPimpl;
};
}
} // namespace vle manager

#endif
