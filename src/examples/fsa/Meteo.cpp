/*
 * @file examples/fsa/Meteo.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
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


#include <vle/devs/Dynamics.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace vle { namespace examples { namespace fsa {

namespace vd = vle::devs;

class Meteo : public vd::Dynamics
{
public:
    Meteo(const vd::DynamicsInit& init,
          const vd::InitEventList& events) :
        vd::Dynamics(init, events)
    { }

    virtual ~Meteo()
    { }

    virtual vd::Time init(const vd::Time& /* time */)
    {
        mPhase = INIT;
        mValue = rand().getDouble(5, 30);
        return 0.0;
    }

    virtual vd::Time timeAdvance() const
    {
        if (mPhase == RUN) return 1.0;
        if (mPhase == SEND) return 0.0;
        return vd::Time::infinity;
    }

    virtual void output(const vd::Time& /* time */,
                        vd::ExternalEventList& outputs) const
    {
        if (mPhase == SEND or mPhase == INIT) {
            vd::ExternalEvent* ee =
                new vd::ExternalEvent("out");

            ee << vd::attribute("name", "Tmoy");
            ee << vd::attribute("value", mValue);
            outputs.addEvent(ee);
        }
    }

    virtual void internalTransition(const vd::Time& /* time */)
    {
        if (mPhase == INIT or mPhase == SEND) {
            mPhase = RUN;
        } else if (mPhase == RUN) {
            mValue = rand().getDouble(5, 30);
            mPhase = SEND;
        }
    }

    virtual vle::value::Value* observation(
        const vd::ObservationEvent& event) const
    {
        if (event.onPort("state")) {
            return buildDouble(mValue);
        }
        return 0;
    }

private:
    enum Phase { INIT, RUN, SEND };

    Phase mPhase;
    double mValue;
};

DECLARE_NAMED_DYNAMICS(Meteo, Meteo)

}}} // namespace vle examples fsa
