/**
 * @file devs/Coordinator.cpp
 * @author The VLE Development Team.
 * @brief Define the DEVS root coordinator. Manage a lot of DEVS Simulators.
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

#include <vle/devs/Coordinator.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/devs/sCoupledModel.hpp>
#include <vle/devs/Time.hpp>
#include <vle/graph/Model.hpp>
#include <vle/graph/TargetPort.hpp>
#include <vle/utils/XML.hpp>
#include <libxml++/libxml++.h>

namespace vle { namespace devs {

Coordinator::Coordinator() :
    m_currentTime(0)
{
}

Coordinator::~Coordinator()
{
    std::vector < Simulator* >::iterator it = m_simulatorList.begin();
    while (it != m_simulatorList.end())
	delete *it++;
}

void Coordinator::load(const vpz::Vpz& io)
{
    Simulator* simu = new Simulator(0, io);
    m_simulatorList.push_back(simu);
}

bool Coordinator::init()
{
    m_currentTime = devs::Time(0);

    for (std::vector < Simulator* >::iterator its = m_simulatorList.begin();
         its != m_simulatorList.end(); ++its) {
        (*its)->init();
    }
    return true;
}

bool Coordinator::run()
{
    Time min(Time::infinity);
    int index = -1;

    std::vector < Simulator* >::iterator it = m_simulatorList.begin();
    int i = 0;

    while (it != m_simulatorList.end()) {
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
	const Time& time = m_simulatorList[index]->getNextTime();

	m_currentTime = time;
	eventList = m_simulatorList[index]->run();
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

void Coordinator::finish()
{
    std::vector < Simulator* >::iterator its = m_simulatorList.begin();

    while (its != m_simulatorList.end()) {
	(*its)->finish();
	delete *its;
	++its;
    }
    m_simulatorList.clear();
}

std::vector < TargetPort* > Coordinator::getTargetPortList(
                    const std::string&,
                    const std::string&)
{
    return std::vector < TargetPort* > ();
}

void Coordinator::addSimulator(Simulator* simulator)
{
    m_simulatorList.push_back(simulator);
}

void Coordinator::dispatchExternalEvent(ExternalEvent *)
{
}

}} // namespace vle devs
