/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#ifndef VLE_DEVS_VIEWEVENT_HPP
#define VLE_DEVS_VIEWEVENT_HPP 1

#include <cassert>
#include <vle/DllDefines.hpp>
#include <vle/devs/View.hpp>

namespace vle {
namespace devs {

/**
 * ViewEvent is used in scheduller to store the date to launch observation of
 * atomic models.
 */
struct VLE_LOCAL ViewEvent
{
    ViewEvent(View* view, Time currenttime, Time timestep)
      : mView(view)
      , mTime(currenttime)
      , mTimestep(timestep)
    {
        assert(view && "ViewEvent: view is null");
        assert(not isInfinity(currenttime) && "ViewEvent: bad current time");
        assert(not isInfinity(timestep) && "ViewEvent: bad timestep");
    }

    /**
     * Call for each \e devs::Dynamics attached to this view, the
     * observation function.
     */
    void run()
    {
        assert(mView &&
               "ViewEvent::run(Time) was called previously. Mistake.");

        mView->run(mTime);
    }

    /**
     * Call for each \e devs::Dynanics attached to this view, the
     * observation function for a specified time.
     *
     * \note To be use in final simulation loop.
     *
     * \e time The current date to observe.
     */
    void run(Time time)
    {
        assert(mView &&
               "ViewEvent::run(Time) was called previously. Mistake.");

        mView->run(time);
        mView = nullptr;
    }

    /**
     * Update the next wake-up time.
     */
    void update()
    {
        mTime += mTimestep;
    }

    View* mView;
    Time mTime;
    Time mTimestep;
};
}
} // namespace vle devs

#endif
