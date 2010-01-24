/**
 * @file vle/devs/Pools.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
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


#include <vle/version.hpp>
#include <vle/value/Pools.hpp>
#include <vle/devs/InternalEvent.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/ObservationEvent.hpp>
#include <vle/devs/RequestEvent.hpp>

namespace vle { namespace devs {


#ifdef VLE_HAVE_POOL
Pools* Pools::m_pool = 0;

Pools::Pools() :
    m_pools(
        std::max(sizeof(InternalEvent), std::max(sizeof(ExternalEvent),
        std::max(sizeof(ObservationEvent), sizeof(RequestEvent)))) + 1)
{
}

Pools::Pools(const Pools& /* other */) :
    m_pools(
        std::max(sizeof(InternalEvent), std::max(sizeof(ExternalEvent),
        std::max(sizeof(ObservationEvent), sizeof(RequestEvent)))) + 1)
{
}
#endif

}} // namespace vle value

