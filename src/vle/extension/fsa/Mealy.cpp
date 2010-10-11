/*
 * @file vle/extension/fsa/Mealy.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#include <vle/extension/fsa/Mealy.hpp>

namespace vle { namespace extension { namespace fsa {

void Mealy::process(const devs::Time& time,
		    const devs::ExternalEvent* event)
{
    if (mTransitions.find(currentState()) !=
        mTransitions.end() and
        (mTransitions[currentState()].find(event->
                                                    getPortName())
         != mTransitions[currentState()].end())) {
        ActionsIterator ita = mActions.find(currentState());

        if (ita != mActions.end() and
            ita->second.find(event->getPortName()) != ita->second.end()) {

            (ita->second.find(event->getPortName())->second)(time, event);
        }

        currentState(mTransitions[currentState()]
                              [event->getPortName()]);
    }
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

void Mealy::output(const devs::Time& time,
		   devs::ExternalEventList& output) const
{
    if (mPhase == PROCESSING) {
        devs::ExternalEventList* events = mToProcessEvents.front();
        devs::ExternalEvent* event = events->front();
        OutputFuncsIterator itof = mOutputFuncs.find(currentState());

        if (itof != mOutputFuncs.end()  and
            itof->second.find(event->getPortName()) != itof->second.end()) {
            (itof->second.find(event->getPortName())->second)(time, output);
        } else {
            OutputsIterator ito = mOutputs.find(currentState());

            if (ito != mOutputs.end() and
                ito->second.find(event->getPortName()) != ito->second.end()) {

                output.addEvent(
                    buildEvent(ito->second.find(event->getPortName())->second));
            }
        }
    }
}

devs::Time Mealy::init(const devs::Time& /* time */)
{
    if (not isInit()) {
        throw utils::InternalError(
            _("FSA::Mealy model, initial state not defined"));
    }

    currentState(initialState());
    mPhase = IDLE;
    return 0;
}

void Mealy::externalTransition(const devs::ExternalEventList& events,
			       const devs::Time& /* time */)
{
    if (events.size() > 1) {
        devs::ExternalEventList sortedEvents = select(events);
        devs::ExternalEventList* clonedEvents =
            new devs::ExternalEventList;
        devs::ExternalEventList::const_iterator it = sortedEvents.begin();

        while (it != sortedEvents.end()) {
            clonedEvents->addEvent(cloneExternalEvent(*it));
            ++it;
        }
        mToProcessEvents.push_back(clonedEvents);
    } else {
        devs::ExternalEventList::const_iterator it = events.begin();
        devs::ExternalEventList* clonedEvents =
            new devs::ExternalEventList;

        clonedEvents->addEvent(cloneExternalEvent(*it));
        mToProcessEvents.push_back(clonedEvents);
    }
    mPhase = PROCESSING;
}

devs::Time Mealy::timeAdvance() const
{
    if (mPhase == IDLE) {
        return devs::Time::infinity;
    } else {
        return 0;
    }
}

void Mealy::internalTransition(const devs::Time& time)
{
    if (mPhase == PROCESSING) {
        devs::ExternalEventList* events = mToProcessEvents.front();
        devs::ExternalEvent* event = events->front();

        process(time, event);

        events->erase(events->begin());
        delete event;

        if (events->empty()) {
            mToProcessEvents.pop_front();
            delete events;
        }
        if (mToProcessEvents.empty()) {
            mPhase = IDLE;
        }
    }
}

}}} // namespace vle extension fsa
