/**
 * @file src/vle/extension/QSS.cpp
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

#include <vle/extension/QSS.hpp>
#include <vle/value/Map.hpp>
#include <vle/utils/Debug.hpp>
#include <cmath>

namespace vle { namespace extension {

using namespace devs;
using namespace graph;
using namespace value;

qss::qss(const AtomicModel& model,
         const InitEventList& events) :
    DifferentialEquation(model, events)
{
    const Value& precision = events.get("precision");
    mPrecision = toDouble(precision);
    mEpsilon = mPrecision;

    const Value& threshold = events.get("threshold");
    mThreshold = toDouble(threshold);
}

double qss::getEstimatedValue(double e) const
{
    return mValue + e * mGradient;
}

void qss::updateGradient(bool external, const vle::devs::Time& time) 
{ 
    mLastTime = time;
    mGradient = compute(time);
    if (external) {
	mSigma = Time(0);
	mState = POST3;
    }
    else updateSigma(time);
}

void qss::updateSigma(const vle::devs::Time& /* time */)
{
    if (std::abs(mGradient) < mThreshold) {
        mSigma = Time::infinity;
    } else {
        if (mGradient > 0) {
            mSigma = Time((d(mIndex + 1)-mValue) / mGradient);
        } else {
            mSigma = Time(((d(mIndex)-mValue)-mEpsilon) / mGradient);
        }
    }
}

void qss::updateValue(bool external,
		      const vle::devs::Time& time)
{
    if (external) {
	pushValue(time, mValue + (time - mLastTime).getValue() * mGradient);
    }
    else {
	if (mGradient >= 0) ++mIndex;
	else --mIndex;
	mPreviousValue = mValue;
	pushValue(time, d(mIndex));
    }
}
	
void qss::reset(const Time& time, double value)
{
    mPreviousValue = mValue;
    mValue = value;
    mIndex = (long)(floor(mValue/mPrecision));
    mLastTime = time;
    mGradient = compute(time);
    updateSigma(time);    
}

// DEVS Methods
Time qss::init(const devs::Time& time)
{
    Time r = DifferentialEquation::init(time);
    
    mIndex = (long)(floor(mValue/mPrecision));
    return r;
}

}} // namespace vle extension
