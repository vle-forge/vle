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

#ifndef DEVS_EXECUTIVE_HPP
#define DEVS_EXECUTIVE_HPP

#include <vle/devs/Dynamics.hpp>
#include <vle/graph/CoupledModel.hpp>

namespace vle { namespace devs {

    class Simulator;
    class Coordinator;

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
	 * Constructor of the dynamics of an atomic model
	 *
	 * @param model the atomic model to which belongs the dynamics
	 */
        Executive(const graph::AtomicModel& model) : 
            Dynamics(model)
        { }

	/**
	 * Destructor (nothing to do)
	 *
	 * @return none
	 */
        virtual ~Executive()
        { }


        inline virtual bool is_executive() const
        { return true; }

        inline void set_coordinator(Coordinator* coordinator)
        { m_coordinator = coordinator; }

    protected:
        inline Coordinator& coordinator() const
        { return *m_coordinator; }

        inline graph::CoupledModel* coupledmodel() const
        { return getModel().getParent(); }

        inline Coordinator& coordinator()
        { return *m_coordinator; }

        inline graph::CoupledModel* coupledmodel()
        { return getModel().getParent(); }

    private:
        Coordinator*    m_coordinator; /** Reference to the coordinator object
                                         to build, delete or construct new
                                         models at runtime. */
    };

}} // namespace vle devs

#endif
