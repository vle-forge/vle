/*
 * @file examples/petrinet/Simple.cpp
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


#include <vle/extension/PetriNet.hpp>

namespace vle { namespace examples { namespace petrinet {

    /**
     * @brief A DEVS generator which send an empty message on its "out" port
     * every 1.0 time. The generator start from the StartTime parameter from
     * VPZ.
     */
    class PetrinetBeep : public devs::Dynamics
    {
    public:
        PetrinetBeep(const devs::DynamicsInit& model,
                     const devs::InitEventList& events) :
            devs::Dynamics(model, events)
        {
            mStartTime = value::toDouble(events.get("start"));
            mTimeStep = value::toDouble(events.get("timestep"));
        }

        virtual ~PetrinetBeep()
        { }

        virtual devs::Time init(const devs::Time& /* time */)
        { return mStartTime; }

        virtual void output(const devs::Time& /* time */,
                            devs::ExternalEventList& output) const
        { output.addEvent(new devs::ExternalEvent("out")); }

        virtual devs::Time timeAdvance() const
        { return mTimeStep; }

    private:
        devs::Time mStartTime;
        devs::Time mTimeStep;
    };

}}} // namespace vle examples petrinet

DECLARE_DYNAMICS(vle::examples::petrinet::PetrinetBeep)
