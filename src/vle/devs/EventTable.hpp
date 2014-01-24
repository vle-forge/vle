/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
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


#ifndef DEVS_EVENTTABLE_HPP
#define DEVS_EVENTTABLE_HPP

#include <vle/DllDefines.hpp>
#include <vle/devs/InternalEvent.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/ViewEvent.hpp>
#include <vle/devs/Simulator.hpp>
#include <list>
#include <set>

namespace vle { namespace devs {

    /**
     * Compare two internals events with devs::Time like comparator.
     *
     * @param e1 first event to compare.
     * @param e2 second event to compare.
     * @return true if devs::Time e1 is more recent than devs::Time e2.
     */
    inline bool internalLessThan(const InternalEvent* e1,
                                 const InternalEvent* e2)
    { return (e1->getTime() > e2->getTime()); }

    /**
     * Compare two states events with devs::Time like comparator.
     *
     * @param e1 first event to compare.
     * @param e2 second event to compare.
     * @return true if devs::Time e1 is more recent than devs::Time e2.
     */
    inline bool viewEventLessThan(const ViewEvent* e1,
                                  const ViewEvent* e2)
    { return (e1->getTime() > e2->getTime()); }

    ///////////////////////////////////////////////////////////////////////////

    /**
     * @e EventBagModel represents a bag or a set of internal and
     * external events for a specific model.
     */
    class VLE_API EventBagModel
    {
    public:
	inline EventBagModel() :
	    _intev(0)
	{}

        inline ~EventBagModel() { clear(); }

        inline void addInternal(InternalEvent* ev)
        { _intev = ev; }

        inline InternalEvent* internal()
        { return _intev; }

        inline const InternalEvent* internal() const
        { return _intev; }

        inline void delInternal()
        { _intev = 0; }


        inline void addExternal(const ExternalEventList& evs)
        { _extev = evs; }

        inline ExternalEventList& externals()
        { return _extev; }

        inline const ExternalEventList& externals() const
        { return _extev; }

        inline void delExternals()
        { _extev.clear(); }


        inline bool empty() const
        { return emptyInternal() and emptyExternal(); }

        inline bool emptyInternal() const
        { return _intev == 0; }

        inline bool emptyExternal() const
        { return _extev.empty(); }

        inline void clear()
        {
            delete _intev;
            _intev = 0;

            std::for_each(_extev.begin(), _extev.end(),
                          boost::checked_deleter < ExternalEvent >());
            _extev.clear();
        }

        friend std::ostream& operator<<(std::ostream& o, const EventBagModel& e)
        {
	    o << "[Internal: " << e._intev << "][Externals: ";
	    for (size_t i = 0; i < e._extev.size(); ++i)
		o << e._extev.at(i) << " ";
	    return o;
	}

    private:
	InternalEvent*          _intev;
	ExternalEventList       _extev;
    };

    ///////////////////////////////////////////////////////////////////////////

    /**
     * @brief Represent a set of event bags for all model.
     *
     */
    class VLE_API CompleteEventBagModel
    {
    public:
	CompleteEventBagModel()
        { init(); }

        ~CompleteEventBagModel()
        { clear(); }

	/**
	 * Return the bag for a specified model.
	 * @param m the specified model to search or to add.
	 * @return a reference to the a bag or a new bag.
	 */
        inline EventBagModel& getBag(Simulator* m)
        { return _bags[m]; }

        /**
         * @brief Return true if the Simulator already exist in the bag.
         * @param m A reference to the simulator.
         * @return True if Simulator was find, false otherwise.
         */
        inline bool exist(Simulator* m) const
        { return _bags.find(m) != _bags.end(); }

        inline void addInternal(Simulator* m, InternalEvent* ev)
        { getBag(m).addInternal(ev); }

        inline void addExternal(Simulator* m, const ExternalEventList& lst)
        { getBag(m).addExternal(lst); }


        inline void addState(ViewEvent* ev)
        { _states.add(ev); }


        inline bool empty()
        { return (_bags.empty() and _states.empty()); }

        inline bool emptyBag()
        { return _itbags == _bags.end() and _itexec == _exec.end(); }

        inline bool emptyStates()
        { return _states.empty(); }

        /**
         * @brief Return a bag with the priority to the Executive model ie. the
         * first bag for a non-executive model of this CompleteEventBagModel. If
         * the size of the CompleteEventBagModel is equal to the list of
         * Excutive, all executive are send.
         * @return A reference to the Bag of a simulator.
         */
        std::map < Simulator*, EventBagModel >::value_type& topBag();

        inline ViewEvent* topObservationEvent()
        { return _states.front(); }

        inline ViewEventList& states()
        { return _states; }


        inline void popBag()
        { _bags.erase(_bags.begin()); }

        inline void clearStates()
        { _states.clear(); }

        inline void deleteStates()
        { _states.erase(); }

        void delModel(Simulator*);

        void clear()
        { _bags.clear(); _itbags = _bags.end(); _exec.clear(); _itexec =
            _exec.end(); }

        inline void init()
        { _itbags = _bags.begin(); _itexec = _exec.end(); }

        friend std::ostream& operator<<(std::ostream& o,
                                        const CompleteEventBagModel& c)
        {
            o << "Nb bags: " << c._bags.size() << " Nb states: "
                << c._states.size();
            return o;
        }

    private:
        std::map < Simulator*, EventBagModel >              _bags;
        std::map < Simulator*, EventBagModel >::iterator    _itbags;
        std::list < std::map < Simulator*, EventBagModel >::value_type* > _exec;
        std::list < std::map < Simulator*, EventBagModel >::value_type* >::iterator _itexec;

        ViewEventList _states;
    };

    ///////////////////////////////////////////////////////////////////////////

    /**
     * @brief Scheduller class to manage internal, external and state events.
     *
     */
    class VLE_API EventTable
    {
    public:
        /**
         * Initialisation of events vectors with specified size, and NULL
         * value.
         *
         * @param sz minimum size to initialise vectors (Default size if 4096).
         */
        EventTable(size_t sz = 4096);

        /**
         * Delete all existing events in vectors internal, external, state
         * and init. Be carreful, don't delete Event that you have put.
         */
        ~EventTable();

        /**
         * Return number of events managed by all EventTable vector.
         *
         * @return number of events.
         */
        size_t getEventNumber() const;

        /**
         * Get first next event from vectors i.e. most recent event. If event
         * have same date, return InternalEvent or ExternalEvent in priority.
         *
         * @return next event found or null otherwise.
         */
        const Time& topEvent();

        /**
         * Get next events (more recent event) with same date from vectors.
         *
         * @return list of event found or null otherwise.
         */
        CompleteEventBagModel& popEvent();

        /**
         * Put an external event into vector heap. Delete event and return
         * false if same model have already external model.
         *
         * @param event InternalEvent to put into vector heap.
         * @return true if InternalEvent is put into vector, otherwise false.
         */
        bool putInternalEvent(InternalEvent* event);

        /**
         * Put an external event into vector heap. Delete Internal event from
         * same model source if present in vector heap.
         *
         * @param event ExternalEvent to put into vector heap.
         * @return true.
         */
        bool putExternalEvent(ExternalEvent* event);

        /**
         * Put a state event into vector heap.
         *
         * @param event state event to push into vector heap.
         */
        bool putObservationEvent(ViewEvent* event);

        /**
         * Return the current simulation Time ie. during the latest popEvent.
         *
         * @return the current simulation Time ie. during the latest popEvent.
         */
        inline const Time& getCurrentTime() const
        { return mCurrentTime; }

        /**
         * @brief Delete all event from Simulator.
         *
         * @param mdl the model to delete events.
         */
        void delModelEvents(Simulator* mdl);

    private:
        typedef std::map < Simulator*, InternalEvent* > InternalEventModel;
        typedef std::map < Simulator*, ExternalEventList > ExternalEventModel;

	/**
	 * Delete the first event in Internal heap.
	 *
	 */
	void popInternalEvent();

	/**
	 * Delete the first event in State heap.
	 *
	 */
	void popObservationEvent();

        /**
         * @brief delete the first invalid InternalEvent from InternalEventList.
         */
        void cleanInternalEventList();

	/// scheduller for internal event.
	InternalEventList mInternalEventList;

	/// scheduller for state events.
	ViewEventList mObservationEventList;

	/// table to quick found event.
	InternalEventModel mInternalEventModel;

	/// table to conserve external event.
	ExternalEventModel mExternalEventModel;

	/// the bag to send with popEvent function.
        CompleteEventBagModel mCompleteEventBagModel;

	/// current time.
        Time                                mCurrentTime;
    };

}} // namespace vle devs

#endif
