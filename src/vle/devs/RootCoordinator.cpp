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
#include <vle/devs/ModelFactory.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/Time.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Rand.hpp>
#include <vle/graph/Model.hpp>

namespace vle { namespace devs {

RootCoordinator::RootCoordinator() :
    m_currentTime(0),
    m_coordinator(0),
    m_modelfactory(0),
    m_root(0)
{
}

RootCoordinator::~RootCoordinator()
{
    delete m_coordinator;
    delete m_modelfactory;
    delete m_root;
}

void RootCoordinator::load(const vpz::Vpz& io)
{
    if (m_coordinator) {
        delete m_coordinator;
        delete m_modelfactory;
        delete m_root;
    }

    m_rand.set_seed(io.project().experiment().seed());
    m_duration = io.project().experiment().duration();

    m_modelfactory = new ModelFactory(io.project().dynamics(),
                                      io.project().classes(),
                                      io.project().experiment(),
                                      io.project().model().atomicModels(),
                                      *this);

    m_coordinator = new Coordinator(*m_modelfactory);
    m_coordinator->init(io.project().model());

    m_root = io.project().model().model();
}

void RootCoordinator::init()
{
    m_currentTime = devs::Time(0);
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
    if (m_coordinator) {
        m_coordinator->finish();
        m_outputs = m_coordinator->outputs();
        delete m_coordinator;
        m_coordinator = 0;
    }

    if (m_modelfactory) {
        delete m_modelfactory;
        m_modelfactory = 0;
    }

    if (m_root) {
        delete m_root;
        m_root = 0;
    }
}

void RootCoordinator::setRand(Dynamics& dyn)
{
    dyn.m_rand = &m_rand;
}

}} // namespace vle devs
