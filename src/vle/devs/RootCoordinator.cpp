/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2003-2018 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2018 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2018 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "devs/RootCoordinator.hpp"
#include <utility>

#include <cassert>

namespace vle {
namespace devs {

RootCoordinator::RootCoordinator(utils::ContextPtr context)
  : m_context(std::move(context))
  , m_rand(0)
  , m_begin(0)
  , m_currentTime(0)
  , m_end(1.0)
  , m_coordinator(nullptr)
  , m_root(nullptr)
{}

RootCoordinator::~RootCoordinator() = default;

void
RootCoordinator::load(vpz::Vpz& io)
{
    m_begin = io.project().experiment().begin();
    m_end = m_begin + io.project().experiment().duration();
    m_currentTime = m_begin;

    m_coordinator = std::make_unique<Coordinator>(m_context,
                                                  io.project().dynamics(),
                                                  io.project().classes(),
                                                  io.project().experiment());

    m_coordinator->init(
      io.project().model(), m_currentTime, m_end, io.project().instance());

    m_root = io.project().model().graph();
}

void
RootCoordinator::init()
{
    m_currentTime = m_begin;
}

bool
RootCoordinator::run()
{
    m_currentTime = m_coordinator->getCurrentTime();

    if (isInfinity(m_currentTime))
        return false;

    if ((m_end - m_currentTime) < 0)
        return false;

    m_coordinator->run();

    return true;
}

std::unique_ptr<value::Map>
RootCoordinator::finish()
{
    if (m_coordinator) {
        return m_coordinator->finish();
    }
    return {};
}

std::unique_ptr<value::Map>
RootCoordinator::outputs() const
{
    if (m_coordinator) {
        return m_coordinator->getMap();
    }
    return {};
}
}
} // namespace vle devs
