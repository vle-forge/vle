/**
 * @file src/vle/devs/Executive.cpp
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




#include <vle/devs/Executive.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace devs {

Coordinator& Executive::coordinator() const
{
    isInitialized();
    return *m_coordinator;
}

graph::CoupledModel& Executive::coupledmodel() const
{
    isInitialized();
    return *getModel().getParent();
}

Coordinator& Executive::coordinator()
{
    isInitialized();
    return *m_coordinator;
}

graph::CoupledModel& Executive::coupledmodel()
{
    isInitialized();
    return *getModel().getParent();
}

void Executive::isInitialized() const
{
    Assert(utils::ModellingError, m_coordinator, boost::format(
            "Executive model: do not use constructor to manage " \
            "the DEVS graph (model '%1%')") % getModel().getName());
}

}} // namespace vle devs
