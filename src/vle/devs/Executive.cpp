/** 
 * @file Executive.cpp
 * @brief 
 * @author The vle Development Team
 */

/*
 * Copyright (C) 2008 - The vle Development Team
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
