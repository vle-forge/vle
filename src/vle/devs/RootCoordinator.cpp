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
#include <vle/utils/XML.hpp>
#include <vle/utils/Rand.hpp>
#include <vle/graph/Model.hpp>
#include <libxml++/libxml++.h>

namespace vle { namespace devs {

RootCoordinator::RootCoordinator() :
    m_currentTime(0),
    m_coordinator(0)
{
}

RootCoordinator::~RootCoordinator()
{
    delete m_coordinator;
}

void RootCoordinator::load(vpz::Vpz& io)
{
    if (m_coordinator) {
        delete m_coordinator;
    }

    utils::Rand::rand().set_seed(io.project().experiment().seed());
    m_duration = io.project().experiment().duration();
    m_coordinator = new Coordinator(io, io.project().model());
}

void RootCoordinator::init()
{
    m_currentTime = devs::Time(0);
    m_coordinator->init();
}

bool RootCoordinator::run()
{
    m_currentTime = m_coordinator->getNextTime();
    if (m_currentTime == Time::infinity) {
        return false;
    } else if (m_currentTime > m_duration) {
        return false;
    }
    m_coordinator->run();
    return true;
}

void RootCoordinator::finish()
{
    m_coordinator->finish();
    delete m_coordinator;
    m_coordinator = 0;
}

}} // namespace vle devs
