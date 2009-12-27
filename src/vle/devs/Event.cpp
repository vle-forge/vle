/**
 * @file vle/devs/Event.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
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


#include <vle/devs/Event.hpp>
#include <vle/devs/Pools.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/String.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/ObservationEvent.hpp>
#include <vle/devs/RequestEvent.hpp>

namespace vle { namespace devs {

Event::~Event()
{
    if (m_delete and m_attributes) {
        delete m_attributes;
    }
}

const std::string Event::getSourceModelName() const
{
    return m_source->getName();
}

void Event::putAttributes(const value::Map& mp)
{
    for (value::MapValue::const_iterator it = mp.value().begin();
         it != mp.value().end(); ++it) {
        putAttribute((*it).first, (*it).second->clone());
    }
}

void init()
{
#ifdef VLE_HAVE_POOL
    devs::Pools::init();
#endif
}

void finalize()
{
#ifdef VLE_HAVE_POOL
    devs::Pools::kill();
#endif
}

}} // namespace vle devs
