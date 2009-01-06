/**
 * @file examples/lifegame/Cell.cpp
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


#include <Cell.hpp>
#include <vle/utils.hpp>

using namespace vle::devs;
using namespace vle::extension;

namespace vle { namespace examples { namespace lifegame {

//***********************************************************************
//***********************************************************************
//
//  DEVS Methods
//
//***********************************************************************
//***********************************************************************

Time Cell::init(const vle::devs::Time& /* time */)
{
// Parameters
    mTimeStep = vle::value::toDouble(m_parameters["TimeStep"]);

// States
    initBooleanNeighbourhood("s",false);
    if (!existState("s")) {
	double colour = rand().getDouble();

	if (colour > 0.5) initBooleanState("s", true);
	else initBooleanState("s", false);
    }

//     std::cout << getModelName() << " : " << getBooleanState("s") << std::endl;

    mState = INIT;
    neighbourModify();
    setSigma(Time(0));
    return Time(0);
}

void Cell::internalTransition(const Time& time)
{
    CellDevs::internalTransition(time);
    switch (mState) {
    case INIT:
	mState = IDLE;
	setSigma(Time(0));
	break;
    case IDLE:
	setLastTime(time);
	mState = NEWSTATE;
	setSigma(Time(0));
	break;
    case NEWSTATE:
	bool v_state = getBooleanState("s");
	unsigned int n = getBooleanNeighbourStateNumber("s", true);

// 	std::cout << "[" << time << "] " << getModelName() << " : " << getBooleanState("s")
// 		  << " - " << n << std::endl;

	if (v_state && (n < 2 || n > 3)) {
	    setBooleanState("s",false);
	    modify();
	    mState = INIT;
	    setSigma(mTimeStep);
	}
	else if (!v_state && (n == 3)) {
	    setBooleanState("s",true);
	    modify();
	    mState = INIT;
	    setSigma(mTimeStep);
	}
	else {
	    mState = IDLE;
	    setSigma(vle::devs::Time::infinity);
	}
	break;
    }
}

}}} // namespace vle examples lifegame

