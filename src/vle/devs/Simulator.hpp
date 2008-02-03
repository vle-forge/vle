/**
 * @file vle/devs/Simulator.hpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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


#ifndef DEVS_SIMULATOR_HPP
#define DEVS_SIMULATOR_HPP

#include <vle/devs/Time.hpp>
#include <vle/devs/InternalEvent.hpp>
#include <vle/devs/ExternalEventList.hpp>
#include <vle/devs/RequestEventList.hpp>
#include <vle/devs/EventList.hpp>
#include <vle/devs/EventTable.hpp>
#include <vle/devs/DevsTypes.hpp>
#include <vle/graph/AtomicModel.hpp>

namespace vle { namespace devs {

    class Coordinator;
    class Dynamics;

    /**
     * @brief Represent a couple devs::AtomicModel and devs::Dynamic class to
     * represent the DEVS simulator.
     *
     */
    class Simulator
    {
    public:
	Simulator(graph::AtomicModel* a);
	~Simulator();

	void addDynamics(Dynamics* dynamics);
	const std::string getName() const;
	Time getLastTime() const;
	graph::AtomicModel* getStructure() const;
        graph::Model* findModel(const std::string & name) const;
	void setLastTime(const Time& time);

        void clear();

        /** 
         * @brief Return a constant reference to the devs::Dynamics.
         * @return 
         */
        inline const Dynamics* dynamics() const
        { return m_dynamics; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /** 
         * @brief Call the init function of the Dynamics plugin and add the time
         * parameter to the value returned by the init() function of Dynamics
         * plugin.
         * @param time the time to add to Dynamics plugin init() function.
         * @return a new InternalEvent.
         */
	InternalEvent* init(const Time& time);

        Time timeAdvance();

	void finish();

        void output(const Time& currentTime, ExternalEventList& output);


        Event::EventType confluentTransitions(const InternalEvent& ie,
                                              const ExternalEventList& ees) const;

        InternalEvent* internalTransition(const InternalEvent& event);
        
        InternalEvent* externalTransition(const ExternalEventList& event,
                                          const Time& time);
        
        void request(const RequestEvent& event,
                     const Time& time,
                     ExternalEventList& output);

        ObservationEvent* observation(const ObservationEvent& event) const;

    protected:
	InternalEvent* buildInternalEvent(const Time& currentTime);

    private:
	Time                    m_lastTime;
	Dynamics*               m_dynamics;
	graph::AtomicModel*     m_atomicModel;
    };

}} // namespace vle devs

#endif
