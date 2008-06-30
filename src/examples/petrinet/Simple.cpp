/**
 * @file src/examples/petrinet/Logical.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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


#include <vle/extension.hpp>
#include <vle/devs.hpp>

namespace vle { namespace examples { namespace petrinet {

    /**
     * @brief A DEVS generator which send an empty message on its "out" port
     * every 1.0 time. The generator start from the StartTime parameter from
     * VPZ.
     */
    class PetrinetBeep : public devs::Dynamics
    {
    public:
        PetrinetBeep(const graph::AtomicModel& model,
                     const devs::InitEventList& events) :
            devs::Dynamics(model, events)
        {
            mStartTime = value::toDouble(events.get("start"));
            mTimeStep = value::toDouble(events.get("timestep"));
        }

        virtual ~PetrinetBeep()
        { }

        virtual devs::Time init(const vle::devs::Time& /* time */)
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

    /** 
     * @brief A DEVS counter which store the date of the latest event.
     */
    class PetrinetCounter : public devs::Dynamics
    {
    public:
        PetrinetCounter(const graph::AtomicModel& model,
                        const devs::InitEventList& events) :
            devs::Dynamics(model, events),
            mDate(0),
            mNumber(0)
        { }

        virtual ~PetrinetCounter()
        { }

        virtual void externalTransition(const devs::ExternalEventList& evts,
                                        const devs::Time& time)
        { mDate = time; mNumber += evts.size(); }

        virtual value::Value observation(const devs::ObservationEvent& e) const
        { return e.onPort("date") ?
            value::DoubleFactory::create(mDate.getValue()) :
                value::DoubleFactory::create(mNumber); }

    private:
        devs::Time mDate;
        int mNumber;
    };

    /**
     * @brief A DEVS implementation of the extension::PetriNet.
     */
    class PetrinetOrdinary : public extension::PetriNet
    {
    public:
        PetrinetOrdinary(const graph::AtomicModel& model,
                         const devs::InitEventList& events) :
            extension::PetriNet(model, events)
        { }

        virtual ~PetrinetOrdinary()
        { }

    };

}}} // namespace vle examples petrinet

DECLARE_NAMED_DYNAMICS(Beep, vle::examples::petrinet::PetrinetBeep)
DECLARE_NAMED_DYNAMICS(Ordinary, vle::examples::petrinet::PetrinetOrdinary)
DECLARE_NAMED_DYNAMICS(Counter, vle::examples::petrinet::PetrinetCounter)
