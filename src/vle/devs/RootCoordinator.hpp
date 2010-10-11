/*
 * @file vle/devs/RootCoordinator.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
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


#ifndef DEVS_ROOTCOORDINATOR_HPP
#define DEVS_ROOTCOORDINATOR_HPP

#include <vle/devs/DllDefines.hpp>
#include <vle/utils/Rand.hpp>
#include <vle/devs/Time.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/oov/OutputMatrix.hpp>

namespace vle { namespace graph {

    class Model;

}} // namespace vle graph

namespace vle { namespace devs {

    class Coordinator;
    class ModelFactory;
    class Dynamics;

    /**
     * @brief Define the DEVS root coordinator. Manage a lot of DEVS
     * Coordinators.
     *
     */
    class VLE_DEVS_EXPORT RootCoordinator
    {
    public:
        RootCoordinator();

        ~RootCoordinator();

        /**
         * @brief initialiase a new Coordinator with the specified vpz::Vpz
         * reference and intitialise the simulation time.
         * @param vp a reference to a structure.
         */
        void load(const vpz::Vpz& vp);

        /**
         * @brief Initialise RootCoordinator and his Coordinator: initiale time
         * is define, coordinator init function is call.
         */
        void init();

        /**
         * @brief Call the coordinator run function and test if current time is
         * the end of the simulation.
         * @return false when simulation is finished, true otherwise.
         */
        bool run();

        /**
         * @brief Call the coordinator finish function and delete the
         * coordinator and all attached data.
         */
        void finish();

        /**
         * @brief Return the current time of the simulation.
         * @return A constant reference to the current time.
         */
        inline const Time& getCurrentTime()
        { return m_currentTime; }

        /**
         * @brief Return a constant reference to the list of view plugins.
         * @return  Return a constant reference to the list of view plugins.
         */
        inline const oov::OutputMatrixViewList& outputs() const
        { return m_outputs; }

        /**
         * @brief Return a reference to the list of view plugins.
         * @return  Return a reference to the list of view plugins.
         */
        inline oov::OutputMatrixViewList& outputs()
        { return m_outputs; }

        /**
         * @brief Update the output matrix of result in the RootCoordinator by
         * getting from coordinator, all matrix of result.
         */
        void refreshOutputs();

        /**
         * @brief Return a reference to the random generator.
         * @return Return a reference to the random generator.
         */
        utils::Rand& rand() { return m_rand; }

    private:
        utils::Rand         m_rand;

        /** @brief Store the beginning of the simulation. */
        devs::Time          m_begin;

        /** @brief Store the current date of the simulator. */
        devs::Time          m_currentTime;

        /** @brief Store the end date of the simulation. */
        devs::Time          m_end;

        Coordinator*        m_coordinator;
        ModelFactory*       m_modelfactory;
        graph::Model*       m_root;

        ///! the output of the simulation if plugin are Storage.
        oov::OutputMatrixViewList m_outputs;
    };

}} // namespace vle devs

#endif
