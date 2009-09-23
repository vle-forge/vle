/**
 * @file examples/fsa/Meteo.cpp
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

#include <vle/devs/Dynamics.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace vle { namespace examples { namespace fsa {

class Meteo : public vle::devs::Dynamics
{
public:
    Meteo(const vle::devs::DynamicsInit& init,
	  const vle::devs::InitEventList& events) :
	vle::devs::Dynamics(init, events)
        { }

    virtual ~Meteo()
        { }

    virtual vle::devs::Time init(const vle::devs::Time& /* time */)
	{
	    mPhase = INIT;
	    mValue = rand().getDouble(5, 30);
	    return 0.0;
	}

    virtual vle::devs::Time timeAdvance() const
	{
	    if (mPhase == RUN) return 1.0;
	    if (mPhase == SEND) return 0.0;
	    return vle::devs::Time::infinity;
	}

    virtual void output(const vle::devs::Time& /* time */,
			vle::devs::ExternalEventList& outputs) const
	{
	    if (mPhase == SEND or mPhase == INIT) {
		vle::devs::ExternalEvent* ee =
		    new vle::devs::ExternalEvent("out");

		ee << vle::devs::attribute("name", "Tmoy");
		ee << vle::devs::attribute("value", mValue);
		outputs.addEvent(ee);
	    }
	}

    virtual void internalTransition(const vle::devs::Time& /* time */)
	{
	    if (mPhase == INIT or mPhase == SEND) {
		mPhase = RUN;
	    } else if (mPhase == RUN) {
		mValue = rand().getDouble(5, 30);
		mPhase = SEND;
	    }
	}

    virtual vle::value::Value* observation(
	const vle::devs::ObservationEvent& event) const
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
