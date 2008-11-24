/**
 * @file vle/devs/EventTable.cpp
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


#include <vle/devs/EventTable.hpp>
#include <vle/devs/InternalEvent.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/ExternalEventList.hpp>
#include <vle/devs/RequestEventList.hpp>
#include <vle/devs/ObservationEvent.hpp>
#include <vle/utils/Debug.hpp>



namespace vle { namespace devs {

std::map < Simulator*, EventBagModel >::value_type&
    CompleteEventBagModel::topBag()
{
    while (_itbags != _bags.end()) {
        if ((*_itbags).first->dynamics()->isExecutive()) {
            _exec.push_back(&(*_itbags));
            _itexec = _exec.begin();
            _itbags++;
        } else {
            std::map < Simulator*, EventBagModel >::iterator r = _itbags;
            ++_itbags;
            return *r;
        }
    }

    while (_itexec != _exec.end()) {
        std::list < std::map < Simulator*, EventBagModel >::value_type*
            >::iterator r = _itexec;
        ++_itexec;
        return **r;
    }

    Throw(utils::InternalError, "Top bag problem");
}

void CompleteEventBagModel::invalidateModel(Simulator* mdl)
{
    std::map < Simulator*, EventBagModel >::iterator it = _bags.find(mdl);
    if (it != _bags.end()) {
        it->second.clear();
    }

    std::for_each(_states.begin(), _states.end(),
                  Event::InvalidateSimulator(mdl));
}

void CompleteEventBagModel::delModel(Simulator* mdl)
{
    std::map < Simulator*, EventBagModel >::iterator it = _bags.find(mdl);
    if (it != _bags.end()) {
        it->second.clear();
        _bags.erase(it);
    }

    for (std::vector < ObservationEvent* >::iterator it = _states.begin();
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
	ObservationEventList::iterator it = mObservationEventList.begin();
	while (it != mObservationEventList.end()) {
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
    size_t sum = mObservationEventList.size() + mInternalEventList.size();

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

void EventTable::cleanObservationEventList()
{
    while (not mObservationEventList.empty() and
           not mObservationEventList[0]->isValid()) {
        delete mObservationEventList[0];
        std::pop_heap(mObservationEventList.begin(), mObservationEventList.end(),
                      stateLessThan);
        mObservationEventList.pop_back();
    }
}

const Time& EventTable::topEvent()
{
    if (not mExternalEventModel.empty()) {
        return mCurrentTime;
    } else {
        cleanInternalEventList();
        if (not mInternalEventList.empty()) {
            cleanObservationEventList();
            if (not mObservationEventList.empty()) {
                if (mInternalEventList[0]->getTime() <=
                    mObservationEventList[0]->getTime()) {
                    return mInternalEventList[0]->getTime();
                } else {
                    return mObservationEventList[0]->getTime();
                }
            } else {
                return mInternalEventList[0]->getTime();
            }
        } else {
            cleanObservationEventList();
            if (not mObservationEventList.empty()) {
                return mObservationEventList[0]->getTime();
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
            EventBagModel& bagmodel = mCompleteEventBagModel.getBag(mdl);
            bagmodel.addExternal((*mExternalEventModel.begin()).second.first);
            bagmodel.addRequest((*mExternalEventModel.begin()).second.second);
	    mExternalEventModel.erase(mExternalEventModel.begin());
	}

	if (mCompleteEventBagModel.emptyBag())
	  while (not mObservationEventList.empty() and
		 mObservationEventList[0]->getTime() == mCurrentTime) {
	    mCompleteEventBagModel.addState(mObservationEventList[0]);
	    popObservationEvent();
	  }
    }
    mCompleteEventBagModel.init();
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

bool EventTable::putRequestEvent(RequestEvent* event)
{
    Simulator* mdl = event->getTarget();
    Assert(utils::InternalError, mdl, "Put Request event.");

    mExternalEventModel[mdl].second.addEvent(event);
    return true;
}

bool EventTable::putObservationEvent(ObservationEvent* event)
{
    mObservationEventList.push_back(event);
    std::push_heap(mObservationEventList.begin(), mObservationEventList.end(),
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

void EventTable::popObservationEvent()
{
    if (not mObservationEventList.empty()) {
        std::pop_heap(mObservationEventList.begin(), mObservationEventList.end(),
                      stateLessThan);
        mObservationEventList.pop_back();
    }
}

void EventTable::invalidateModel(Simulator* mdl)
{
    {
        InternalEventModel::iterator it = mInternalEventModel.find(mdl);
        if (it != mInternalEventModel.end()) {
            if ((*it).second) {
                (*it).second->invalidate();
                (*it).second = 0;
            }
        }
    }

    {
        ExternalEventModel::iterator it = mExternalEventModel.find(mdl);
        if (it != mExternalEventModel.end()) {
            (*it).second.first.deleteAndClear();
            (*it).second.second.deleteAndClear();
        }
    }

    std::for_each(mObservationEventList.begin(), mObservationEventList.end(),
                  Event::InvalidateSimulator(mdl));

    mCompleteEventBagModel.invalidateModel(mdl);
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

    std::for_each(mObservationEventList.begin(), mObservationEventList.end(),
                  Event::InvalidateSimulator(mdl));

    mCompleteEventBagModel.delModel(mdl);
}

}} // namespace vle devs
