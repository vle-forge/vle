/**
 * @file vle/extension/differential-equation/DESS.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2009 ULCO http://www.univ-littoral.fr
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


#include <vle/extension/differential-equation/DESS.hpp>
#include <vle/value/Map.hpp>
#include <vle/utils/Debug.hpp>
#include <cmath>

namespace vle { namespace extension { namespace DifferentialEquation {

using namespace devs;
using namespace value;

DESS::DESS(const DynamicsInit& model,
	   const InitEventList& events) :
    Base(model, events), mMethod(0)
{
    mUseGradient = false;
    mTimeStep = vle::value::toDouble(events.get("timeStep"));

    if (events.exist("method")) {
	std::string method(vle::value::toString(events.get("method")));

        if (method == "rk4") {
            mMethod = new RK4(*this);
        } else if (method == "euler") {
            mMethod = new Euler(*this);
        }

        if (not mMethod) {
            mMethod = new RK4(*this);
        }
    } else {
        mMethod = new RK4(*this);
    }
}

double DESS::getEstimatedValue(double /* e */) const
{
    return mValue;
}

void DESS::updateGradient(bool external, const vle::devs::Time& time)
{
    if (!external) {
	mLastTime = time;
    }
    updateSigma(time);
}

void DESS::updateSigma(const vle::devs::Time& time)
{
    mSigma = mTimeStep - (time - mLastTime).getValue();
}

void DESS::updateValue(bool external,
		       const vle::devs::Time& time)
{
    if (!external) {
	mPreviousValue = mValue;
	(*mMethod)(time);
    }
    pushValue(time, mValue);
}

void DESS::reset(const Time& time, double value)
{
    mPreviousValue = mValue;
    mValue = value;
    mLastTime = time;
    mGradient = compute(time);
    updateSigma(time);
}

void RK4::operator()(const Time& time)
{
    double init = dess.mValue;
    double x0, x1, x2, x3, gradient;

    dess.mGradient = dess.compute(time);
    x0 = dess.mGradient;

    dess.mValue = init + dess.mTimeStep * x0 / 2;
    gradient = dess.compute(time);
    x1 = gradient;

    dess.mValue = init + dess.mTimeStep * x1 / 2;
    gradient = dess.compute(time);
    x2 = gradient;

    dess.mValue = init + dess.mTimeStep * x2;
    gradient = dess.compute(time);
    x3 = gradient;

    dess.mValue = init + dess.mTimeStep * ((x0 + 2.0 * (x1 + x2) + x3) / 6.0);
}

void Euler::operator()(const Time& time)
{
    dess.mGradient = dess.compute(time);
    dess.mValue += dess.mTimeStep * dess.mGradient;
}

}}} // namespace vle extension DifferentialEquation
