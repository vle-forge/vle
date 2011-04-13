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
#include <vle/devs.hpp>

namespace vle { namespace examples { namespace petrinet {

    /**
     * @brief A DEVS model which simulate a Meteo. Each time step, it build an
     * event with true or false.
     */
    class PetrinetMeteo : public devs::Dynamics
    {
    public:
        PetrinetMeteo(const devs::DynamicsInit& model,
              const devs::InitEventList& events) :
            devs::Dynamics(model, events)
        {
            mInit = value::toBoolean(events.get("init"));
            mMin = value::toInteger(events.get("min"));
            mMax = value::toInteger(events.get("max"));
        }

        virtual ~PetrinetMeteo()
        { }

        virtual void output(const devs::Time& /* time */,
                            devs::ExternalEventList& output) const
        {
            if (mActive) {
                output.addEvent(buildEvent("yes"));
            } else {
                output.addEvent(buildEvent("no"));
            }
        }

        virtual devs::Time timeAdvance() const
        {
            return devs::Time(1);
        }

        virtual devs::Time init(const devs::Time& /* time */)
        {
            mActive = mInit;
            mNextTime = rand().getInt(mMin, mMax);
            return devs::Time(0);
        }

        virtual void internalTransition(const devs::Time& /* event */)
        {
            if (mNextTime == 0) {
                mActive = not mActive;
                mNextTime = rand().getInt(mMin, mMax);
            } else {
                --mNextTime;
            }
        }

        virtual value::Value* observation(
            const devs::ObservationEvent& event) const
        {
            if (event.onPort("state")) {
                return buildBoolean(mActive);
            }
            return 0;
        }

    private:
        bool mInit;
        unsigned int mMin;
        unsigned int mMax;
        unsigned int mNextTime;
        bool mActive;
    };

}}} // namespace vle examples petrinet

DECLARE_DYNAMICS(vle::examples::petrinet::PetrinetMeteo)
