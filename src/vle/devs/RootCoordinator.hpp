/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2016 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2016 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2016 INRA http://www.inra.fr
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


#ifndef DEVS_ROOTCOORDINATOR_HPP
#define DEVS_ROOTCOORDINATOR_HPP

#include <vle/DllDefines.hpp>
#include <vle/devs/Coordinator.hpp>
#include <vle/utils/Context.hpp>
#include <vle/utils/Rand.hpp>
#include <vle/devs/Time.hpp>
#include <vle/vpz/Vpz.hpp>
#include <memory>

namespace vle { namespace vpz {

class BaseModel;

}} // namespace vle graph

namespace vle { namespace devs {

class Coordinator;
class Dynamics;

/**
 * @brief Define the DEVS root coordinator. Manage a lot of DEVS
 * Coordinators.
 *
 * @example
 * utils::ModuleManager mm;
 * vle::RootCoordinator rc(mm);
 *
 * m.load(...);
 * ...
 * value::Map *res = rc.outputs();
 * delete res;
 * @endexample
 *
 */
class VLE_LOCAL RootCoordinator
{
public:
    RootCoordinator(utils::ContextPtr context);

    ~RootCoordinator();

    /**
     * @brief initialiase a new Coordinator with the specified vpz::Vpz
     * reference and intitialise the simulation time.
     * @param vp a reference to a structure.
     */
    void load(vpz::Vpz& vp);

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
    std::unique_ptr<value::Map>  finish();

    /**
     * @brief Return the current time of the simulation.
     * @return A constant reference to the current time.
     */
    inline const Time& getCurrentTime()
    { return m_currentTime; }

    /**
     * Return the simulation results.
     *
     * This function build a new \c value::Map based on the observation of
     * the simulation. This function can return NULL lists of view is empty
     * or no storage are used.
     *
     * @return Return a pointer to the data of the list of plug-ins.
     */
    std::unique_ptr<value::Map> outputs() const;

    /**
     * @brief Return a reference to the random generator.
     * @return Return a reference to the random generator.
     */
    utils::Rand& rand() { return m_rand; }

private:
    RootCoordinator(const RootCoordinator& other);
    RootCoordinator& operator=(const RootCoordinator& other);

    utils::ContextPtr   m_context;
    utils::Rand         m_rand;

    /** @brief Store the beginning of the simulation. */
    devs::Time          m_begin;

    /** @brief Store the current date of the simulator. */
    devs::Time          m_currentTime;

    /** @brief Store the end date of the simulation. */
    devs::Time          m_end;

    std::unique_ptr<Coordinator> m_coordinator;
    std::unique_ptr<vpz::BaseModel> m_root;
};

}} // namespace vle devs

#endif
