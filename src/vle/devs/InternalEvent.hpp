/**
 * @file InternalEvent.hpp
 * @author The VLE Development Team.
 * @brief Internal event based on the devs::Event class and are build by
 * graph::Model when internal transition are called.
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

#ifndef DEVS_INTERNALEVENT_HPP
#define DEVS_INTERNALEVENT_HPP

#include <vle/devs/Event.hpp>

namespace vle { namespace devs {

    class sAtomicModel;

    class InternalEvent : public Event
    {
    public:
	InternalEvent(const Time& time, sAtomicModel* model) :
	    Event(time, model)
	    { }

	virtual ~ InternalEvent()
	    { }

	virtual bool isExternal() const;

	virtual bool isInit() const;

	virtual bool isInternal() const;

	virtual bool isState() const;
    };

}} // namespace vle devs

#endif
