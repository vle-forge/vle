/**
 * @file devs/ExternalEvent.cpp
 * @author The VLE Development Team.
 * @brief External event based on the devs::Event class and are build by
 * graph::Model when output function are called.
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

#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/Simulator.hpp>

namespace vle { namespace devs {

const std::string& ExternalEvent::getTargetModelName() const
{
    return m_target->getName();
}

bool ExternalEvent::isExternal() const
{
    return true;
}

bool ExternalEvent::isInternal() const
{
    return false;
}

bool ExternalEvent::isInit() const
{
    return false;
}

bool ExternalEvent::isState() const
{
  return false;
}

bool ExternalEvent::isInstantaneous() const
{
    return false;
}

}} // namespace vle devs
