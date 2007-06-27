/**
 * @file devs/Simulator.hpp
 * @author The VLE Development Team.
 * @brief Represent a couple devs::AtomicModel and devs::Dynamic class to
 * represent the DEVS simulator.
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

#ifndef DEVS_SIMULATOR_HPP
#define DEVS_SIMULATOR_HPP

#include <vle/devs/Time.hpp>
#include <vle/devs/InternalEvent.hpp>
#include <vle/devs/InitEvent.hpp>
#include <vle/devs/ExternalEventList.hpp>
#include <vle/devs/InstantaneousEventList.hpp>
#include <vle/devs/EventList.hpp>
#include <vle/devs/EventTable.hpp>
#include <vle/devs/DevsTypes.hpp>
#include <vle/graph/AtomicModel.hpp>

namespace vle { namespace devs {

    class Coordinator;
    class Model;
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
	const std::string & getName() const;
	Time getLastTime() const;
	graph::AtomicModel* getStructure() const;
	Model* findModel(const std::string & name) const;
	void setLastTime(const Time& time);

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void finish();

        void getOutputFunction(
            const Time& currentTime,
            ExternalEventList& output);

        Time getTimeAdvance();

        /** 
         * @brief Call the init function of the Dynamics plugin and add the time
         * parameter to the value returned by the init() function of Dynamics
         * plugin.
         * @param time the time to add to Dynamics plugin init() function.
         * @return a new InternalEvent.
         */
	InternalEvent* init(const Time& time);

        Event::EventType processConflict(
            const InternalEvent& internal,
            const ExternalEventList& extEventlist) const;

        void processInitEvents(
            const InitEventList& event);
        
        InternalEvent* processInternalEvent(
            const InternalEvent& event);
        
        InternalEvent* processExternalEvents(
            const ExternalEventList& event,
            const Time& time);
        
        void processInstantaneousEvent(
            const InstantaneousEvent& event,
            const Time& time,
            ExternalEventList& output);

        StateEvent* processStateEvent(
            const StateEvent& event) const;

    protected:
	InternalEvent* buildInternalEvent(const Time& currentTime);

    private:
	Time                    m_lastTime;
	Dynamics*               m_dynamics;
	graph::AtomicModel*     m_atomicModel;
    };

}} // namespace vle devs

#endif
