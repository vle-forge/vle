/**
 * @file devs/EventTable.hpp
 * @author The VLE Development Team.
 * @brief Scheduller class to manage internal, external and state events.
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

#ifndef DEVS_EVENTTABLE_HPP
#define DEVS_EVENTTABLE_HPP

#include <vle/devs/Event.hpp>
#include <vle/devs/InternalEvent.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/InstantaneousEvent.hpp>
#include <vle/devs/StateEvent.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/devs/DevsTypes.hpp>
#include <vle/devs/EventList.hpp>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <deque>

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
    inline bool stateLessThan(const StateEvent* e1,
                              const StateEvent* e2)
    { return (e1->getTime() > e2->getTime()); }

    ///////////////////////////////////////////////////////////////////////////

    /**
     * @brief Represent a bag of events for a specific model ie. the
     * internal event if exist and external event list if exist and
     * instantaneous event if exist.
     *
     */
    class EventBagModel
    {
    public:
	inline EventBagModel() :
	    _intev(0)
	{ }

        inline void addInternal(InternalEvent* ev)
        { _intev = ev; }

        inline InternalEvent* internal()
        { return _intev; }

        inline void delInternal()
        { _intev = 0; }


        inline void addExternal(const ExternalEventList& evs)
        { _extev = evs; }

        inline ExternalEventList& externals()
        { return _extev; }

        inline void delExternals()
        { _extev.clear(); }


        inline void addInstantaneous(const InstantaneousEventList& evs)
        { _instev = evs; }

        inline void addInstantaneous(InstantaneousEvent* evt)
        { _instev.addEvent(evt); }

        inline InstantaneousEventList& instantaneous()
        { return _instev; }

        inline void delInstantaneous()
        { _instev.clear(); }


        inline bool empty() const
        { return emptyInternal() and emptyExternal() and emptyInstantaneous(); }

        inline bool emptyInternal() const
        { return _intev == 0; }

        inline bool emptyExternal() const
        { return _extev.empty(); }

        inline bool emptyInstantaneous() const
        { return _instev.empty(); }

        inline void clear()
        { delete _intev; _intev = 0;
            _extev.deleteAndClear(); _extev.clear();
            _instev.deleteAndClear(); _instev.clear(); }

        friend std::ostream& operator<<(std::ostream& o, const EventBagModel& e)
        {
	    o << "[Internal: " << e._intev << "][Externals: ";
	    for (size_t i = 0; i < e._extev.size(); ++i)
		o << e._extev.at(i) << " ";
	    o << "] [Instantaneous: ";
	    for (size_t i = 0; i < e._instev.size(); ++i)
		o << e._instev.at(i) << " ";
	    o << "]\n";
	    return o;
	}

    private:
	InternalEvent*          _intev;
	ExternalEventList       _extev;
        InstantaneousEventList  _instev;
    };

    ///////////////////////////////////////////////////////////////////////////

    /**
     * @brief Represent a set of event bags for all model.
     *
     */
    class CompleteEventBagModel
    {
    public:
	CompleteEventBagModel()
        { }

	/**
	 * Return the bag for a specified model. If model is not present it is
	 * push into bags an return a reference to the new Bag.
	 *
	 * @param m the specified model to search or to add.
	 *
	 * @return a reference to the a bag or a new bag.
	 */
	inline EventBagModel& getBag(Simulator* m)
        { return _bags[m]; }

        inline void addInternal(Simulator* m, InternalEvent* ev)
        { getBag(m).addInternal(ev); }

        inline void addExternal(Simulator* m, const ExternalEventList& lst)
        { getBag(m).addExternal(lst); }

        inline void addInstantaneous(Simulator* m,
                                     const InstantaneousEventList& lst)
        { getBag(m).addInstantaneous(lst); }
        
        inline void addInstantaneous(Simulator* m,
                                     InstantaneousEvent* evt)
        { getBag(m).addInstantaneous(evt); }


        inline void addState(StateEvent* ev)
        { _states.push_back(ev); }

        
        inline bool empty()
        { return (_bags.empty() and _states.empty()); }

        inline bool emptyBag()
        { return _bags.empty(); }

        inline bool emptyStates()
        { return _states.empty(); }

    
        inline EventBagModel& topBag()
        { return (*_bags.begin()).second; }

        inline Simulator* topBagModel()
        { return (*_bags.begin()).first; }

        inline StateEvent* topStateEvent()
        { return _states.front(); }

        inline StateEventList& states()
        { return _states; }


        inline void popBag()
        { _bags.erase(_bags.begin()); }

        inline void popState()
        { _states.pop_front(); }

        inline void popStates()
        { _states.clear(); }

        void delModel(Simulator*);

        friend std::ostream& operator<<(std::ostream& o,
                                        const CompleteEventBagModel& c)
        {
            o << "Nb bags: " << c._bags.size() << " Nb states: " 
                << c._states.size();
            return o;
        }

    private:
        std::map < Simulator*, EventBagModel >  _bags;
        std::deque < StateEvent* >              _states;
    };

    ///////////////////////////////////////////////////////////////////////////

    /**
     * @brief Scheduller class to manage internal, external and state events.
     *
     */
    class EventTable
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
         * Put an instantaneous event into vector heap. Delete Internal event
         * from same model source if present in vector heap.
         *
         * @param event ExternalEvent to put into vector heap.
         * @return true.
         */
        bool putInstantaneousEvent(InstantaneousEvent* event);

        /**
         * Put a state event into vector heap.
         *
         * @param event state event to push into vector heap.
         */
        bool putStateEvent(StateEvent* event);

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
        typedef std::map < Simulator*,
                std::pair < ExternalEventList, InstantaneousEventList > >
                    ExternalEventModel;
        
	/**
	 * Delete the first event in Internal heap.
	 *
	 */
	void popInternalEvent();

	/**
	 * Delete the first event in State heap.
	 *
	 */
	void popStateEvent();

        /** 
         * @brief delete the first invalid InternalEvent from InternalEventList.
         */
        void cleanInternalEventList();

        /** 
         * @brief delete the first invalid StateEvent from StateEventList.
         */
        void cleanStateEventList();

	/// scheduller for internal event.
	InternalEventList mInternalEventList;

	/// scheduller for state events.
	StateEventList    mStateEventList;

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
