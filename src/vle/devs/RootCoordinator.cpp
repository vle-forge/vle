/**
 * @file devs/RootCoordinator.cpp
 * @author The VLE Development Team.
 * @brief Define the DEVS root coordinator. Manage a lot of DEVS Coordinators.
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

#include <vle/devs/RootCoordinator.hpp>
#include <vle/devs/Coordinator.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/Time.hpp>
#include <vle/graph/Model.hpp>
#include <vle/graph/TargetPort.hpp>
#include <vle/utils/XML.hpp>
#include <libxml++/libxml++.h>

namespace vle { namespace devs {

RootCoordinator::RootCoordinator() :
    m_currentTime(0)
{
}

RootCoordinator::~RootCoordinator()
{
    std::vector < Coordinator* >::iterator it = m_coordinatorList.begin();
    while (it != m_coordinatorList.end())
	delete *it++;
}

void RootCoordinator::load(vpz::Vpz& io)
{
    Coordinator* coor = new Coordinator(io, io.project().model());
    m_coordinatorList.push_back(coor);
}

bool RootCoordinator::init()
{
    m_currentTime = devs::Time(0);

    for (std::vector < Coordinator* >::iterator its = m_coordinatorList.begin();
         its != m_coordinatorList.end(); ++its) {
        (*its)->init();
    }
    return true;
}

bool RootCoordinator::run()
{
    Time min(Time::infinity);
    int index = -1;

    std::vector < Coordinator* >::iterator it = m_coordinatorList.begin();
    int i = 0;

    while (it != m_coordinatorList.end()) {
        const Time& time = (*it++)->getNextTime();

        if (time >= 0) {
            if (min > time) {
		min = time;
		index = i;
	    }
	}
	++i;
    }
    if (index != -1) {
	ExternalEventList *eventList;
	const Time& time = m_coordinatorList[index]->getNextTime();

	m_currentTime = time;
	eventList = m_coordinatorList[index]->run();
	if (eventList) {
	    while (not eventList->empty()) {
		ExternalEvent* ev = eventList->front();
		dispatchExternalEvent(ev);
		eventList->erase(0);
	    }
	    delete eventList;
	}
	return true;
    }
    return false;
}

void RootCoordinator::finish()
{
    std::vector < Coordinator* >::iterator its = m_coordinatorList.begin();

    while (its != m_coordinatorList.end()) {
	(*its)->finish();
	delete *its;
	++its;
    }
    m_coordinatorList.clear();
}

std::vector < TargetPort* > RootCoordinator::getTargetPortList(
                    const std::string&,
                    const std::string&)
{
    return std::vector < TargetPort* > ();
}

void RootCoordinator::addCoordinator(Coordinator* Coordinator)
{
    m_coordinatorList.push_back(Coordinator);
}

void RootCoordinator::dispatchExternalEvent(ExternalEvent *)
{
}

}} // namespace vle devs
