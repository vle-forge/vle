/**
 * @file devs/StateEvent.cpp
 * @author The VLE Development Team.
 * @brief State event use to get information from graph::Model using
 * TimedObserver or EventObserver.
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

#include <vle/devs/StateEvent.hpp>
#include <string>

namespace vle { namespace devs {

bool StateEvent::isExternal() const
{
    return false;
}

bool StateEvent::isInternal() const
{
    return false;
}

bool StateEvent::isInit() const
{
    return false;
}

bool StateEvent::isState() const
{
  return true;
}

}} // namespace vle devs
