/**
 * @file sModel.cpp
 * @author The VLE Development Team.
 * @brief
 */

/*
 * Copyright (c) 2005 The VLE Development Team.
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

#include <vle/devs/sModel.hpp>
#include <vle/devs/Simulator.hpp>

namespace vle { namespace devs {

sModel::sModel(Simulator* p_simulator) :
    m_simulator(p_simulator)
{
}

sModel::~sModel()
{
}

Simulator* sModel::getSimulator() const
{
  return m_simulator;
}

long sModel::getSimulatorIndex() const
{
    return m_simulator->getIndex();
}

}} // namespace vle devs
