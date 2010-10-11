/*
 * @file vle/devs/RequestEvent.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#ifndef DEVS_REQUESTEVENT_HPP
#define DEVS_REQUESTEVENT_HPP

#include <vle/devs/DllDefines.hpp>
#include <vle/devs/ExternalEvent.hpp>

namespace vle { namespace devs {

    /**
     * @brief Request event based on the devs::ExternalEvent class and are
     * build by graph::Model to precess Request event.
     *
     */
    class VLE_DEVS_EXPORT RequestEvent : public ExternalEvent
    {
    public:
	RequestEvent(const std::string& sourcePortName) :
            ExternalEvent(sourcePortName)
        {}

	RequestEvent(RequestEvent* event,
                           Simulator* target,
                           const std::string& targetPortName,
                           bool deleter) :
            ExternalEvent(event, target, targetPortName, deleter)
        {}

	virtual ~RequestEvent()
        {}

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        virtual bool isRequest() const
        { return true; }
    };

}} // namespace vle devs

#endif
