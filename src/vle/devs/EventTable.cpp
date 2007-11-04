/**
 * @file devs/EventTable.cpp
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

#include <vle/devs/EventTable.hpp>
#include <vle/devs/InternalEvent.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/DevsTypes.hpp>
#include <vle/devs/ExternalEventList.hpp>
#include <vle/devs/InstantaneousEventList.hpp>
#include <vle/devs/StateEvent.hpp>
#include <vle/devs/InitEvent.hpp>
#include <vle/utils/Debug.hpp>



namespace vle { namespace devs {

void CompleteEventBagModel::delModel(Simulator* mdl)
{
    std::map < Simulator*, EventBagModel >::iterator it = _bags.find(mdl);
    if (it != _bags.end())
        _bags.erase(it);

    for (std::deque < StateEvent* >::iterator it = _states.begin();
         it != _states.end(); ++it) {
        if ((*it)->getModel() == mdl) {
            _states.erase(it);
        }
    }
}

EventTable::EventTable(size_t sz)
{
    mInternalEventList.reserve(sz);
}

EventTable::~EventTable()
{
    {
	InternalEventList::iterator it = mInternalEventList.begin();
	while (it != mInternalEventList.end()) {
	    delete (*it);
	    ++it;
	}
    }

    {
	StateEventList::iterator it = mStateEventList.begin();
	while (it != mStateEventList.end()) {
	    delete (*it);
	    ++it;
	}
    }

    {
	for (ExternalEventModel::iterator it = mExternalEventModel.begin();
	     it != mExternalEventModel.end(); ++it) {
	    (*it).second.first.deleteAndClear();
	    (*it).second.second.deleteAndClear();
	}
    }
}

size_t EventTable::getEventNumber() const
{
    size_t sum = mStateEventList.size() + mInternalEventList.size();

    for (ExternalEventModel::const_iterator it = mExternalEventModel.begin();
	     it != mExternalEventModel.end(); ++it) {
	sum += (*it).second.first.size() + (*it).second.second.size();
    }

    return sum;
}

void EventTable::cleanInternalEventList()
{
    while (not mInternalEventList.empty() and
	   not mInternalEventList[0]->isValid()) {
	delete mInternalEventList[0];
        std::pop_heap(mInternalEventList.begin(), mInternalEventList.end(),
                      internalLessThan);
        mInternalEventList.pop_back();
    }

}

void EventTable::cleanStateEventList()
{
    while (not mStateEventList.empty() and
           not mStateEventList[0]->isValid()) {
        delete mStateEventList[0];
        std::pop_heap(mStateEventList.begin(), mStateEventList.end(),
                      stateLessThan);
        mStateEventList.pop_back();
    }
}

const Time& EventTable::topEvent()
{   
    if (not mExternalEventModel.empty()) {
        return mCurrentTime;
    } else {
        cleanInternalEventList();
        if (not mInternalEventList.empty()) {
            cleanStateEventList();
            if (not mStateEventList.empty()) {
                if (mInternalEventList[0]->getTime() <=
                    mStateEventList[0]->getTime()) {
                    return mInternalEventList[0]->getTime();
                } else {
                    return mStateEventList[0]->getTime();
                }
            } else {
                return mInternalEventList[0]->getTime();
            }
        } else {
            cleanStateEventList();
            if (not mStateEventList.empty()) {
                return mStateEventList[0]->getTime();
            } else {
                return Time::infinity;
            }
        }
    }
}

CompleteEventBagModel& EventTable::popEvent()
{
    mCurrentTime = topEvent();

    if (mCurrentTime != Time::infinity) {
	while (not mInternalEventList.empty() and
               mInternalEventList[0]->getTime() == mCurrentTime) {
            if (mInternalEventList[0]->isValid()) {
                Simulator* mdl = mInternalEventList[0]->getModel();
                EventBagModel& bagmodel = mCompleteEventBagModel.getBag(mdl);
                bagmodel.addInternal(mInternalEventList[0]);
            }
            popInternalEvent();
	}

        while (not mExternalEventModel.empty()) {
            Simulator* mdl = (*mExternalEventModel.begin()).first;
	    mCompleteEventBagModel.getBag(mdl).addExternal(
		(*mExternalEventModel.begin()).second.first);
	    mCompleteEventBagModel.getBag(mdl).addInstantaneous(
		(*mExternalEventModel.begin()).second.second);
	    mExternalEventModel.erase(mExternalEventModel.begin());
	}

	if (mCompleteEventBagModel.emptyBag())
	  while (not mStateEventList.empty() and
		 mStateEventList[0]->getTime() == mCurrentTime) {
	    mCompleteEventBagModel.addState(mStateEventList[0]);
	    popStateEvent();
	  }
    }
    return mCompleteEventBagModel;
}

bool EventTable::putInternalEvent(InternalEvent* event)
{
    mInternalEventList.push_back(event);
    std::push_heap(mInternalEventList.begin(), mInternalEventList.end(),
                   internalLessThan);

    Assert(utils::InternalError, event->getModel(), "Put internal event.");

    if (mInternalEventModel[event->getModel()])
      mInternalEventModel[event->getModel()]->invalidate();

    mInternalEventModel[event->getModel()] = event;
    return true;
}

bool EventTable::putExternalEvent(ExternalEvent* event)
{
    Simulator* mdl = event->getTarget();
    Assert(utils::InternalError, mdl, "Put external event.");

    mExternalEventModel[mdl].first.addEvent(event);
    InternalEventModel::iterator it = mInternalEventModel.find(mdl);
    if (it != mInternalEventModel.end() and (*it).second and
        (*it).second->getTime() > getCurrentTime()) {
	(*it).second->invalidate();
	(*it).second = 0;
    }
    return true;
}

bool EventTable::putInstantaneousEvent(InstantaneousEvent* event)
{
    Simulator* mdl = event->getTarget();
    Assert(utils::InternalError, mdl, "Put instantaneous event.");

    mExternalEventModel[mdl].second.addEvent(event);
    return true;
}

bool EventTable::putStateEvent(StateEvent* event)
{
    mStateEventList.push_back(event);
    std::push_heap(mStateEventList.begin(), mStateEventList.end(),
                   stateLessThan);

    return true;
}

void EventTable::popInternalEvent()
{
    if (not mInternalEventList.empty()) {
        InternalEvent* evt = mInternalEventList[0];
        std::pop_heap(mInternalEventList.begin(), mInternalEventList.end(),
                      internalLessThan);
        mInternalEventList.pop_back();
	if (evt->isValid()) {
	    mInternalEventModel[evt->getModel()] = 0;
	} else {
	    delete evt;
	}
    }
}

void EventTable::popStateEvent()
{
    if (not mStateEventList.empty()) {
        std::pop_heap(mStateEventList.begin(), mStateEventList.end(),
                      stateLessThan);
        mStateEventList.pop_back();
    }
}

void EventTable::delModelEvents(Simulator* mdl)
{
    {
        InternalEventModel::iterator it = mInternalEventModel.find(mdl);
        if (it != mInternalEventModel.end()) {
            if ((*it).second)
                (*it).second->invalidate();
            mInternalEventModel.erase(it);
        }
    }

    {
        ExternalEventModel::iterator it = mExternalEventModel.find(mdl);
        if (it != mExternalEventModel.end()) {
            (*it).second.first.deleteAndClear();
            (*it).second.second.deleteAndClear();
            mExternalEventModel.erase(it);
        }
    }

    {
        for (StateEventList::iterator it = mStateEventList.begin();
             it != mStateEventList.end(); ++it) {
            if  ((*it)->getModel() == mdl) {
                (*it)->invalidate();
            }
        }
    }

    mCompleteEventBagModel.delModel(mdl);
}

}} // namespace vle devs
