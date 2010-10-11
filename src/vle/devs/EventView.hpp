/*
 * @file vle/devs/EventView.hpp
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


#ifndef VLE_DEVS_EVENT_VIEW_HPP
#define VLE_DEVS_EVENT_VIEW_HPP

#include <vle/devs/DllDefines.hpp>
#include <vle/devs/View.hpp>
#include <vle/devs/ObservationEvent.hpp>

namespace vle { namespace devs {

    /**
     * @brief Define a Event View base on devs::View class. This class
     * build state event when event are push.
     */
    class VLE_DEVS_EXPORT EventView : public View
    {
    public:
        EventView(const std::string& name, StreamWriter* stream);

        virtual ~EventView() { }

        virtual bool isEvent() const
        { return true; }

        virtual devs::ObservationEvent*
            processObservationEvent(devs::ObservationEvent* event);
    };

    typedef std::map < std::string, devs::EventView* > EventViewList;

}} // namespace vle devs

#endif
