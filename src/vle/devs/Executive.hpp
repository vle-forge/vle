/**
 * @file devs/Dynamics.hpp
 * @author The VLE Development Team.
 * @brief Dynamics class for the Barros DEVS extension. Provite graph
 * manipulation at runtime using namespace graph and the coordinator. By
 * default, this model does nothing and it will be inherit to build simulation
 * components.
 */

/*
 * Copyright (C) 2007 - The vle Development Team
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef VLE_DEVS_EXECUTIVE_HPP
#define VLE_DEVS_EXECUTIVE_HPP

#include <vle/devs/Coordinator.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/graph/CoupledModel.hpp>

namespace vle { namespace devs {

    class Simulator;

    /**
     * @brief Dynamics class for the Barros DEVS extension. Provite graph
     * manipulation at runtime using namespace graph and the coordinator. By
     * default, this model does nothing and it will be inherit to build
     * simulation components.
     */
    class Executive : public Dynamics
    {
    public:
	/**
	 * @brief Constructor of the Executive of an atomic model
	 * @param model the atomic model to which belongs the dynamics
	 */
        Executive(const graph::AtomicModel& model,
                  const devs::InitEventList& events) : 
            Dynamics(model, events),
            m_coordinator(0)
        { }

	/**
	 * @brief Destructor (nothing to do)
	 * @return none
	 */
        virtual ~Executive()
        { }

        /** 
         * @brief If this function return true, then a cast to an Executive
         * object is produce and the set_coordinator function is call. Executive
         * permit to manipulate graph::CoupledModel and devs::Coordinator at
         * runtime of the simulation.
         * @return false if Dynamics is not an Executive.
         */
        inline virtual bool isExecutive() const
        { return true; }

        /** 
         * @brief Assign a coordinator to the Executive model to give access to
         * vle::graph, vle::devs and vle::vpz API.
         * @param coordinator A reference to the coordinator.
         */
        friend void Coordinator::setCoordinator(Executive& exe);

    protected:
        Coordinator& coordinator() const;

        graph::CoupledModel& coupledmodel() const;

        Coordinator& coordinator();

        graph::CoupledModel& coupledmodel();

    private:
        Coordinator*    m_coordinator; /** Reference to the coordinator object
                                         to build, delete or construct new
                                         models at runtime. */

        void isInitialized() const;
    };

}} // namespace vle devs

#endif
