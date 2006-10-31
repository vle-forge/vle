/**
 * @file sAtomicModel.hpp
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

#ifndef DEVS_SATOMIC_MODEL_HPP
#define DEVS_SATOMIC_MODEL_HPP

#include <vle/devs/sModel.hpp>
#include <vle/devs/Time.hpp>
#include <vle/devs/InternalEvent.hpp>
#include <vle/devs/ExternalEventList.hpp>
#include <vle/devs/InstantaneousEventList.hpp>
#include <vle/devs/EventList.hpp>
#include <vle/devs/EventTable.hpp>
#include <vle/devs/DevsTypes.hpp>
#include <vle/graph/AtomicModel.hpp>

namespace vle { namespace devs {

    class Simulator;
    class Model;
    class Dynamics;

    /**
     * @brief Represent a couple devs::AtomicModel and devs::Dynamic class to
     * represent the DEVS simulator.
     *
     */
    class sAtomicModel : public sModel
    {
    public:
	sAtomicModel(graph::AtomicModel* a, Simulator * simulator);
	virtual ~sAtomicModel();

	void addDynamics(Dynamics* dynamics);
	long getModelIndex() const;
	const std::string & getName() const;
	Time getLastTime() const;
	graph::AtomicModel* getStructure() const;
	virtual bool isAtomic() const;
	virtual bool isCoupled() const;
	virtual Model* findModel(const std::string & name) const;
	void setIndex(long index);
	void setLastTime(const Time& time);

	virtual void finish();
	virtual ExternalEventList* getOutputFunction(const Time& currentTime);
	virtual Time getTimeAdvance();
	virtual InternalEvent *init();

	/**
	 * Parse a XML tree which define the parameters of the model
	 *
	 * @return true if the XML parsing is correct
	 */
        virtual bool parseXML(xmlpp::Element*);


        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * Compute the selected event when an external event, instantaneous
         * event and an internal event occurs at the same time. Default, take
         * Internal event Ask modeller to make a choice between the internal
         * event or external event list. 
         *
         * @param internal the internal event.
         * @param extEventlist the external events list.
         * @param instEventlist the instantaneous events list.
         *
         * @return Event::INTERNAL if internal is priority, Event::EXTERNAL if
         * external event is priority or Event::INSTANTANEOUS if the priority is
         * to Instantaneous event.
         */
        virtual Event::EventType processConflict(
                                const InternalEvent& internal,
                                const ExternalEventList& extEventlist,
                                const InstantaneousEventList& instEventlist);

        /**
         * Compute the selected event when an internal event and external events
         * occurs at the same time. Default take internal.
         * 
         * @param internal the internal event.
         * @param eventlist the external events list.
         *
         * @return true if priority to Internal Event, false to External events.
         */
        virtual bool  processConflict(
                                const InternalEvent& internal,
                                const ExternalEventList& extEventlist);

        /**
         * Compute the selected event when an internal event and
         * Instantaneous events occurs at the same time. Default take internal.
         * 
         * @param internal the internal event.
         * @param eventlist the instantaneous events list.
         *
         * @return true if priority to Internal Event, false to
         * Instantaneous events.
         */
        virtual bool processConflict(
                            const InternalEvent& internal,
                            const InstantaneousEventList& instEventlist);

        /**
         * Compute the selected event when an external event and instantaneous
         * events occurs at the same time. Default take external.
         * 
         * @param extEventlist the external events list.
         * @param instEventlist the instantaneous event list.
         *
         * @return true if priority to External Event, false to
         * Instantaneous events.
         */
        virtual bool processConflict(
                            const ExternalEventList& extEventlist,
                            const InstantaneousEventList& instEventlist);

	/**
	 * Ask to modeller to make a choice between all external event list.
         * Simulator want a valid index.  Default return the first
         * ExternalEvent.
	 *
	 * @param eventlist the list of external events.
	 *
	 * @return the index of element to get.
	 */
	virtual size_t processConflict(const ExternalEventList& eventlist);

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	virtual void processInitEvent(InitEvent* event);
        
        virtual InternalEvent *processInternalEvent(InternalEvent* event);
        
        virtual InternalEvent *processExternalEvent(ExternalEvent* event);
        
        virtual ExternalEventList* processInstantaneousEvent(
                                            InstantaneousEvent* event);
	virtual StateEvent* processStateEvent(StateEvent* event) const;

    protected:
	InternalEvent* buildInternalEvent(const Time& currentTime);

    private:
	long                    m_modelIndex;
	Time                    m_lastTime;
	Dynamics*               m_dynamics;
	graph::AtomicModel*     m_atomicModel;
    };

}} // namespace vle devs

#endif
