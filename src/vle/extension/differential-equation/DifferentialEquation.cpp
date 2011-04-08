/*
 * @file vle/extension/differential-equation/DifferentialEquation.cpp
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


#include <vle/extension/differential-equation/DifferentialEquation.hpp>

namespace vle { namespace extension { namespace DifferentialEquation {

using namespace devs;
using namespace value;

Base::Base(const DynamicsInit& model,
					   const InitEventList& events) :
    Dynamics(model, events),
    mUseGradient(true),
    mActive(true),
    mDependance(true),
    mExternalVariableNumber(0),
    mExternalValues(false),
    mBuffer(false)
{
    if (events.exist("active")) {
        mActive = events.getBoolean("active");
    }

    if (events.exist("dependance")) {
        mDependance = events.getBoolean("dependance");
    }

    mInitialValue = events.getDouble("value");

    mVariableName = events.getString("name");

    if (events.exist("thresholds")) {
        const MapValue& thresholds =
            toMap(events.get("thresholds"));

        for (MapValue::const_iterator it = thresholds.begin();
             it != thresholds.end(); it++) {
            const Set& tab(toSetValue(*it->second));
            double value = toDouble(tab.get(0));
            std::string type = toString(tab.get(1));

            if (type == "up") {
                mThresholds[it->first] = std::make_pair(value, UP);
            } else if (type == "down") {
                mThresholds[it->first] = std::make_pair(value, DOWN);
            }
        }
    }
}

double Base::getValue(const Time& now,
                                      double delay) const
{
    if (not (delay <= 0 and (mSize < 0 or -delay <= (int)mSize))) {
        throw utils::InternalError(fmt(_(
                "DifferentialEquation: invalid delay: %1%")) % delay);
    }

    if (delay == 0) {
        return mValue;
    } else {
        if (mSize > 0) {
            if ((now - mStartTime).getValue() < -delay * mDelay) {
                return mValueBuffer.back().second;
            } else {
                valueBuffer::const_reverse_iterator it = mValueBuffer.rbegin();
                Time time = now.getValue() + delay * mDelay;
                double value = it->second;

                while (it != mValueBuffer.rend() and it->first < time) {
                    value = it->second;
                    ++it;
                }
                if (it->first == time)
                    return it->second;
                else
                    return value;
            }
        } else {
            if ((now - mStartTime).getValue() < -delay) {
                return mValueBuffer.back().second;
            } else {
                valueBuffer::const_iterator it = mValueBuffer.begin();
                Time time = now.getValue() + delay;
                double value = it->second;

                while (it != mValueBuffer.end() and it->first > time) {
                    value = it->second;
                    ++it;
                }
                if (it->first == time)
                    return it->second;
                else
                    return value;
            }
        }
    }
}

double Base::getValue(const std::string& name) const
{
    std::map < std::string, double >::const_iterator it(
        mExternalVariableValue.find(name));

    if (it == mExternalVariableValue.end()) {
        throw utils::InternalError(fmt(_(
                "DifferentialEquation: unknow variable %1%")) % name);
    }

    return it->second;
}


double Base::getValue(const std::string& name,
                                      const Time& now,
                                      double delay) const
{
    if (not (delay <= 0 and (mSize < 0 or -delay <= (int)mSize))) {
        throw utils::InternalError(fmt(_(
           "DifferentialEquation: invalid delay: %1%")) % delay);
    }

    if (delay == 0) {
        return getValue(name);
    } else {
        if (mSize > 0) {
            if ((now - mStartTime).getValue() < -delay * mDelay) {
                return externalValueBuffer(name).back().second;
            } else {
                valueBuffer::const_reverse_iterator it =
                    externalValueBuffer(name).rbegin();
                Time time = now.getValue() + delay * mDelay;
                double value = it->second;

                while (it != externalValueBuffer(name).rend() and
                       it->first < time) {
                    value = it->second;
                    ++it;
                }
                if (it->first == time)
                    return it->second;
                else
                    return value;
            }
        } else {
            if ((now - mStartTime).getValue() < -delay) {
                return externalValueBuffer(name).back().second;
            } else {
                valueBuffer::const_iterator it =
                    externalValueBuffer(name).begin();
                Time time = now.getValue() + delay;
                double value = it->second;

                while (it != externalValueBuffer(name).end() and
                       it->first > time) {
                    value = it->second;
                    ++it;
                }
                if (it->first == time)
                    return it->second;
                else
                    return value;
            }
        }
    }
}

void Base::pushValue(const Time& now,
                                     double value)
{
    mValue = value;
    if (mBuffer) {
        if (mSize < 0 or (now - mStartTime).getValue() < mSize * mDelay) {
            mValueBuffer.push_front(std::make_pair(now, mValue));
        } else {
            Time last = now.getValue() - mSize * mDelay;
            std::pair < Time, double > value;
            bool remove = false;

            mValueBuffer.push_front(std::make_pair(now, mValue));
            while (mValueBuffer.back().first < last) {
                value = mValueBuffer.back();
                mValueBuffer.pop_back();
                remove = true;
            }
            if (remove)
                mValueBuffer.push_back(value);
        }
    }
}

void Base::pushExternalValue(const std::string& name,
                                             const Time& now,
                                             double value)
{
    mExternalVariableValue[name] = value;
    if (mBuffer) {
        if (mSize < 0 or (now - mStartTime).getValue() < mSize * mDelay) {
            externalValueBuffer(name).push_front(
                std::make_pair(now, getValue(name)));
        } else {
            Time last = now.getValue() - mSize * mDelay;
            std::pair < Time, double > value2;
            bool remove = false;

            externalValueBuffer(name).push_front(
                std::make_pair(now, getValue(name)));
            while (externalValueBuffer(name).back().first < last) {
                value2 = externalValueBuffer(name).back();
                externalValueBuffer(name).pop_back();
                remove = true;
            }
            if (remove)
                externalValueBuffer(name).push_back(value2);
        }
    }
}

void Base::updateExternalVariable(const Time& time)
{
    if (mExternalVariableNumber > 1) {
        std::map < std::string , double >::iterator it =
            mExternalVariableValue.begin();

        while (it != mExternalVariableValue.end()) {
            mExternalVariableValue[it->first] +=
                (time - mLastTime).getValue() *
                mExternalVariableGradient[it->first];
            ++it;
        }
    }
}

Time Base::init(const Time& time)
{
    mStartTime = time;
    mPreviousValue = mInitialValue;
    pushValue(time, mInitialValue);
    mGradient = 0.0;
    mSigma = Time(0);
    mLastTime = time;
    mState = INIT;
    return Time(0);
}

void Base::output(const Time& time,
                                  ExternalEventList& output) const
{
    // change value outputs
    if ((mState == INIT and mActive) or
        ((mState == POST3 and mExternalValues) or
         (mState == RUN and mActive))) {
        ExternalEvent* ee = new ExternalEvent("update");
        double e = (time - mLastTime).getValue();

        ee << attribute("name", mVariableName);
        ee << attribute("value", getEstimatedValue(e));
        if (mUseGradient) {
            ee << attribute("gradient", mGradient);
        }
        output.addEvent(ee);
    }
    // threshold outputs
    if (mState == RUN2 or mState == POST2) {
        threshold::const_iterator it = mThresholds.begin();
        bool found = false;
        std::string name;

        while (it != mThresholds.end() and !found) {
            double value = it->second.first;

            if (it->second.second == DOWN)
                found = (mPreviousValue >= value) and (value >= mValue);
            if (it->second.second == UP)
                found = (mPreviousValue <= value) and (value <= mValue);
            if (found) name = it->first;
            ++it;
        }
        if (found) {
            ExternalEvent* ee = new ExternalEvent("out");

            ee << attribute("name", name);
            output.addEvent(ee);
        }
    }
}

Time Base::timeAdvance() const
{
    return mSigma;
}

void Base::confluentTransitions(
    const Time& time,
    const ExternalEventList& ext)
{
    externalTransition(ext, time);
    internalTransition(time);
}

void Base::internalTransition(const Time& time)
{
    switch (mState) {
    case INIT:
        if (mDependance) {
            mState = POST_INIT;
            mSigma = Time::infinity;
        } else {
            mState = RUN;
            mGradient = compute(time);
            updateSigma(time);
        }
        break;
    case POST2:
        // update the gradient after receive of value of external
        // variable
        mState = RUN;
        updateGradient(false, time);
        break;
    case POST3:
        mState = RUN;
        updateSigma(time);
        break;
    case RUN:
        updateValue(false, time);
        // broadcast or not the new value
        if (mActive and mExternalValues) {
            // then wait the new values of all my external variables
            mState = POST;
            mSigma = Time::infinity;
        } else {
            mState = RUN2;
            mSigma = Time(0);
            // else next step
        }
        break;
    case RUN2:
        mState = RUN;
        updateGradient(false, time);
        break;
    case POST:
    case POST_INIT:
        break;
    }
}

void Base::externalTransition(const ExternalEventList& event,
                                              const Time& time)
{
    if (mState == POST_INIT) {
        ExternalEventList::const_iterator it = event.begin();
        unsigned int index = 0;
        unsigned int linear = 0;

        while (it != event.end()) {
            std::string name = (*it)->getStringAttributeValue("name");
            double value = (*it)->getDoubleAttributeValue("value");

            mExternalValueBuffer[name] = valueBuffer();
            pushExternalValue(name, time, value);
            if ((mIsGradient[name] = (*it)->existAttributeValue("gradient"))) {
                mExternalVariableGradient[name] =
                    (*it)->getDoubleAttributeValue("gradient");
                ++linear;
            } else {
                mExternalVariableGradient[name] = 0;
            }
            ++index;
            ++it;
        }
        mExternalValues =
            (mExternalVariableNumber + linear < mExternalVariableValue.size());
        mExternalVariableNumber += index;
        if (mExternalVariableValue.size() == mExternalVariableNumber) {
            mState = RUN;
            mGradient = compute(time);
            updateSigma(time);
        }
    } else {
        ExternalEventList::const_iterator it = event.begin();
        bool _reset = false;

        while (it != event.end()) {
            std::string name = (*it)->getStringAttributeValue("name");
            double value = (*it)->getDoubleAttributeValue("value");

            // it is a numerical external variable
            if ((*it)->onPort("update")) {
                if (name == mVariableName) {
                    throw utils::InternalError(
                        fmt(_("DifferentialEquation update, invalid variable " \
                              "name: %1%")) % name);
                }

                pushExternalValue(name, time, value);
                if (mIsGradient[name]){
                    setGradient(name,
                                (*it)->getDoubleAttributeValue("gradient"));
                }
            }
            // it is a perturbation on an internal variable
            else if ((*it)->onPort("perturb")) {
                if (name != mVariableName) {
                    throw utils::InternalError(
                        fmt(_("DifferentialEquation perturbation, invalid " \
                              "variable name: %1%")) % name);
                }

                reset(time, value);
                _reset = true;
            } else {
                throw utils::InternalError(
                    fmt(_("DifferentialEquation, wrong port name: %1%")) %
                    (*it)->getPortName());
            }
            ++it;
        }
        if (mState == POST) {
            mState = POST2;
            mSigma = Time(0);
        } else if (mState == RUN or mState == RUN2) {
            if (_reset) mSigma = Time(0);
            else {
                updateValue(true, time);
                updateExternalVariable(time);
                updateGradient(true, time);
            }
        }
    }
}

Value* Base::observation(const ObservationEvent& event) const
{
    if (event.getPortName() != mVariableName) {
        throw utils::InternalError(fmt(_(
                "DifferentialEquation model, invalid variable name: %1%")) %
            event.getStringAttributeValue("name"));
    }

    double e = (event.getTime() - mLastTime).getValue();
    return Double::create(getEstimatedValue(e));
}

void Base::request(const RequestEvent& event,
                                   const Time& time,
                                   ExternalEventList& output) const
{
    if (event.getStringAttributeValue("name") != mVariableName) {
        throw utils::InternalError(fmt(_(
                "DifferentialEquation model, invalid variable name: %1%")) %
            event.getStringAttributeValue("name"));
    }

    double e = (time - mLastTime).getValue();
    ExternalEvent* ee = new ExternalEvent("response");

    ee << attribute("name", event.getStringAttributeValue("name"));
    ee << attribute("value", getEstimatedValue(e));
    if (mUseGradient) {
        ee << attribute("gradient", mGradient);
    }
    output.addEvent(ee);
}

const Base::valueBuffer& Base::externalValueBuffer(
    const std::string& name) const
{
    std::map < std::string, valueBuffer >::const_iterator it(
        mExternalValueBuffer.find(name));

    if (it == mExternalValueBuffer.end()) {
        throw utils::InternalError(fmt(_(
               "DifferentialEquation: unknow value buffer '%1%'")) % name);
    }

    return it->second;
}

Base::valueBuffer& Base::externalValueBuffer(const std::string& name)
{
    std::map < std::string, valueBuffer >::iterator it(
        mExternalValueBuffer.find(name));

    if (it == mExternalValueBuffer.end()) {
        throw utils::InternalError(fmt(_(
                "DifferentialEquation: unknow value buffer '%1%'")) % name);
    }

    return it->second;
}

}}} // namespace vle extension DifferentialEquation
