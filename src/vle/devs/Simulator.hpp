/**
 * @file vle/devs/Simulator.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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

#include <vle/devs/DllDefines.hpp>
#include <vle/devs/Time.hpp>
#include <vle/devs/InternalEvent.hpp>
#include <vle/devs/ExternalEventList.hpp>
#include <vle/devs/RequestEventList.hpp>
#include <vle/devs/EventList.hpp>
#include <vle/devs/EventTable.hpp>
#include <vle/graph/AtomicModel.hpp>

namespace vle { namespace devs {

    class Coordinator;
    class Dynamics;

    /**
     * @brief Represent a couple devs::AtomicModel and devs::Dynamic class to
     * represent the DEVS simulator.
     *
     */
    class VLE_DEVS_EXPORT Simulator
    {
    public:
        /**
         * @brief Build a new devs::Simulator with an empty devs::Dynamics, a
         * null last time but a graph::AtomicModel node.
         * @param a The atomic model.
         * @throw utils::InternalError if the atomic model does not exist.
         */
	Simulator(graph::AtomicModel* a);

        /**
         * @brief Delete the attached devs::Dynamics user's model.
         */
	~Simulator();

        /**
         * @brief Assign a new dynamics to the current Simulator. If a dynamic
         * already exists, it will be delete.
         * @param dynamics The new dynamics.
         */
	void addDynamics(Dynamics* dynamics);

        /**
         * @brief Get the name of the graph::AtomicModel node.
         * @return the name of the graph::AtomicModel.
         * @throw utils::InternalEvent if the model is destroyed.
         */
        const std::string& getName() const;

        /**
         * @brief Get a std::string which concatenate the names of the hierarchy
         * of graph::CoupledModel parent. Use the function
         * graph::Model::getParentName().
         * @return The concatenated names of the parents or "Deleted model" if
         * the model is destroy before observed.
         */
        const std::string& getParent();

        /**
         * @brief Get the atomic model attached to the Simulator.
         * @return A reference.
         */
        inline graph::AtomicModel* getStructure() const
        { return m_atomicModel; }

        /**
         * @brief Delete the dynamics and erase reference to the AtomicModel.
         */
        void clear();

        /**
         * @brief Return a constant reference to the devs::Dynamics.
         * @return
         */
        inline const Dynamics* dynamics() const
        { return m_dynamics; }

                             /*-*-*-*-*-*-*-*-*-*/

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


        Event::EventType confluentTransitions(
            const InternalEvent& ie,
            const ExternalEventList& ees) const;

        InternalEvent* internalTransition(const InternalEvent& event);

        InternalEvent* externalTransition(const ExternalEventList& event,
                                          const Time& time);

        /**
         * @brief Call to produce the conflict function with a priority to the
         * external event.
         * \f[
         * \delta_{conf}=\delta_{ext}(..,\delta_{int}, ..)
         * \f]
         * @param event the internal event.
         * @param events the externals events.
         * @return the new internal event.
         */
        InternalEvent* internalTransitionConflict(
            const InternalEvent& event,
            const ExternalEventList& events);

        /**
         * @brief Call to produce the conflict function with a priority to the
         * internal event.
         * \f[
         * \delta_{conf}=\delta_{int}(..,\delta_{ext}, ..)
         * \f]
         * @param event the internal event.
         * @param events the externals events.
         * @return the new internal event.
         */
        InternalEvent* externalTransitionConflict(
            const InternalEvent& event,
            const ExternalEventList& events);

        void request(const RequestEvent& event,
                     const Time& time,
                     ExternalEventList& output);

        ObservationEvent* observation(const ObservationEvent& event) const;

    private:
	Dynamics            *m_dynamics;
	graph::AtomicModel  *m_atomicModel;
	std::string          m_parents;

	InternalEvent* buildInternalEvent(const Time& currentTime);
    };

}} // namespace vle devs

#endif
