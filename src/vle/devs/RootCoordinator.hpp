/**
 * @file devs/RootCoordinator.hpp
 * @author The VLE Development Team.
 * @brief Define the DEVS root coordinator. Manage a lot of DEVS Coordinators.
 */

/*
 * Copyright (c) 2004, 2005 The VLE Development Team.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef DEVS_ROOTCOORDINATOR_HPP
#define DEVS_ROOTCOORDINATOR_HPP

#include <vle/devs/Event.hpp>
#include <vle/devs/Time.hpp>
#include <vle/vpz/Vpz.hpp>

namespace vle { namespace devs {

    class ExternalEvent;
    class Coordinator;
    class TargetPort;
    class ExperimentGenerator;

    /**
     * @brief Define the DEVS root coordinator. Manage a lot of DEVS
     * Coordinators.
     *
     */
    class RootCoordinator
    {
    public:
	RootCoordinator();

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
	void finish();

        /** 
         * @brief Return the current time of the simulation.
         * @return A constant reference to the current time.
         */
        inline const Time& getCurrentTime()
        { return m_currentTime; }

    private:
	devs::Time      m_currentTime;
        devs::Time      m_duration;
	Coordinator*    m_coordinator;
    };

}} // namespace vle devs

#endif
