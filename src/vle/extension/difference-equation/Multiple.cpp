/*
 * @file vle/extension/difference-equation/Multiple.cpp
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


#include <vle/extension/difference-equation/Multiple.hpp>
#include <vle/value/Tuple.hpp>

namespace vle { namespace extension { namespace DifferenceEquation {

using namespace devs;
using namespace value;

Multiple::Multiple(const DynamicsInit& model,
                   const InitEventList& events,
                   bool control) :
    Base(model, events, control)
{
    if (events.exist("variables")) {
        const value::Set& variables = events.getSet("variables");
        unsigned int index;

        for (index = 0; index < variables.size(); ++index) {
            const value::Set& tab(variables.getSet(index));
            std::string name = toString(tab.get(0));

            mVariableNames.push_back(name);
            mValues[name] = MultipleValues();
            mInitValues[name] = false;
            size(name, DEFAULT_SIZE);

            if (tab.size() > 1) {
                if (tab.get(1)->isDouble()) {
                    double init = toDouble(tab.get(1));

                    mInitialValues[name] = init;
                } else {
                    if (tab.get(1)->isSet()) {
                        const Set& init = tab.getSet(1);
                        unsigned int i;

                        for (i = 0; i < init.size(); ++i) {
                            mValues[name] = toDouble(init.get(i));
                        }
                    }
                }
                if (tab.size() == 3) {
                    const Set& init = tab.getSet(2);
                    unsigned int i;

                    for (i = 0; i < init.size(); ++i) {
                        mValues[name] = toDouble(init.get(i));
                    }
                }
            }
        }
    }
}

void Multiple::addValue(double value, const std::string& name)
{
    mValues[name] = value;
    while (mSize[name] > 0
        and mValues[name].size() > (unsigned int)mSize[name]) {
        mValues[name].pop_back();
    }
}

bool Multiple::check()
{
    bool ok = true;
    std::map < std::string, bool >::const_iterator it = mInitValues.begin();

    while (ok and it != mInitValues.end()) {
        ok = it->second;
        ++it;
    }
    return ok;
}

void Multiple::computeInit(const Time& time)
{
    compute(time);

    {
        std::map < std::string, bool >::iterator it = mInitValues.begin();

        while (it != mInitValues.end()) {
            it->second = true;
            ++it;
        }
    }

    unset();
}

void Multiple::create(const std::string& name,
                      MultipleVariableIterators& iterators)
{
    if (mValues.find(name) == mValues.end()) {
        mVariableNames.push_back(name);
        mValues[name] = MultipleValues();
        mInitValues[name] = false;
        size(name, DEFAULT_SIZE);
    }
    mSetValues.insert(std::make_pair(name, false)).first;
    iterators.mMultipleValues = &mValues[name];
    iterators.mSetValues = &mSetValues[name];
    iterators.mInitValues = &mInitValues[name];
}

void Multiple::initValue(const Time& /* time */)
{
}

void Multiple::initValues(const Time& time)
{
    {
        std::vector < std::string >::const_iterator it = mVariableNames.begin();

        while (it != mVariableNames.end()) {
            std::map < std::string, double >::const_iterator iti =
                mInitialValues.find(*it);

            if (iti != mInitialValues.end()) {
                addValue(iti->second, *it);
                mInitValues[*it] = true;
                mSetValues[*it] = true;
            }
            ++it;
        }
    }

    initValue(time);

    {
        std::vector < std::string >::const_iterator it = mVariableNames.begin();

        while (it != mVariableNames.end()) {
            std::map < std::string, bool >::const_iterator itv =
                mInitValues.find(*it);

            if (not itv->second) {
                std::map < std::string, double >::const_iterator iti =
                    mInitialValues.find(*it);

                if (iti != mInitialValues.end()) {
                    addValue(iti->second, *it);
                } else {
                    addValue(0.0, *it);
                }
                mInitValues[*it] = true;
                mSetValues[*it] = true;
            }
            ++it;
        }
    }

    {
        std::map < std::string, bool >::iterator it = mInitValues.begin();

        while (it != mInitValues.end()) {
            it->second = true;
            ++it;
        }
    }
    unset();
}

void Multiple::invalidValues()
{
    unset(true);
}

void Multiple::lockValue(const std::string& name)
{
    mSetValues[name] = true;
}

void Multiple::removeValue(const std::string& name)
{
    mValues[name].pop_front();
}

void Multiple::removeValues()
{
    MultipleValuesMap::iterator it = mValues.begin();
    std::map < std::string, bool >::iterator it2 = mSetValues.begin();

    while (it != mValues.end()) {
        if (not it2->second) {
            it->second.pop_front();
        }
        ++it;
        ++it2;
    }
}

void Multiple::unset(bool all)
{
    std::map < std::string, bool >::iterator it = mSetValues.begin();

    while (it != mSetValues.end()) {
        if (all or
            std::find(mLockedVariables.begin(), mLockedVariables.end(),
                      it->first) == mLockedVariables.end()) {
            it->second = false;
        }
        ++it;
    }
}

void Multiple::updateValues(const Time& time)
{
    compute(time);
    unset();
}

double Multiple::val(const std::string& name,
                     const MultipleVariableIterators& iterators,
                     int shift) const
{
    if (shift > 0) {
        throw utils::ModellingError(
            fmt(_("[%1%] DifferenceEquation::getValue - " \
                  "positive shift on %2%")) %
            getModelName() % name);
    }

    if (shift == 0) {

        if (not *iterators.mSetValues) {
            throw utils::InternalError(
                fmt(_("[%1%] - forbidden to use %2%() before computing of %2%"))
                % getModelName() % name);
        }

        return iterators.mMultipleValues->front();
    } else {
        if (not *iterators.mSetValues) {
            ++shift;
        }

        if ((int)(iterators.mMultipleValues->size() - 1) < -shift) {
            throw utils::InternalError(
                fmt(_("[%1%] - %2%[%3%] - shift too large")) % getModelName() %
                name % shift);
        }

        return (*iterators.mMultipleValues)[-shift];
    }
}

double Multiple::val(const std::string& name) const
{
    MultipleValuesMap::const_iterator it =
        mValues.find(name);

    if (it == mValues.end()) {
        throw utils::InternalError(fmt(_(
                    "[%1%] DifferenceEquation::getValue: invalid variable" \
                    " name: %2%")) % getModelName() % name);
    }
    return it->second.front();
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

Time Multiple::init(const Time& time)
{

    {
        MultipleValuesMap::const_iterator it = mValues.begin();
        bool ok = true;

        while (ok and it != mValues.end()) {
            if (mSetValues.find(it->first) == mSetValues.end()) {
                ok = false;
            } else {
                ++it;
            }
        }
        if (not ok) {
            throw utils::InternalError(
                fmt(_("[%1%] DifferenceEquation::Multiple: undeclared " \
                      "variable: %2%")) % getModelName() % it->first);
        }
    }

    Time t = Base::init(time);

    {
        std::vector < std::string >::const_iterator it = mVariableNames.begin();
        bool ok = false;

        while (not ok and it != mVariableNames.end()) {
            ok = getModel().existOutputPort(*it);
            ++it;
        }
        mActive = ok;
    }
    unset();
    return t;
}

void Multiple::output(const Time& /* time */,
                      ExternalEventList& output) const
{
    MultipleValuesMap::const_iterator it =
        mValues.begin();

    while (it != mValues.end()) {
        if (mState == SEND_INIT and not it->second.empty()) {
            ExternalEvent* ee = new ExternalEvent(it->first);

            Set* values = Set::create();
            unsigned int i = 0;

            while (i < it->second.size()) {
                values->addDouble(it->second[i]);
                ++i;
            }
            ee << attribute("name", it->first);
            ee << attribute("init", values);
            output.addEvent(ee);
        }
        else if (mActive and (mState == PRE_INIT or mState == PRE_INIT2
                              or mState == POST or mState == POST2)) {
            if (getModel().existOutputPort(it->first)) {
                ExternalEvent* ee = new ExternalEvent(it->first);

                if (mState == PRE_INIT or mState == PRE_INIT2
                    or mState == POST or mState == POST2) {
                    ee << attribute("name", it->first);
                    ee << attribute("value", val(it->first));
                }
                output.addEvent(ee);
            }
        }
        ++it;
    }
}

Value* Multiple::observation(const ObservationEvent& event) const
{
    if (event.getPortName() != "all") {
        return Double::create(val(event.getPortName()));
    } else {
        Tuple* values = Tuple::create();
        MultipleValuesMap::const_iterator it =
            mValues.begin();

        while (it != mValues.end()) {
            values->add(it->second[0]);
            ++it;
        }
        return values;
    }
}

void Multiple::request(const RequestEvent& event,
                       const Time& /*time*/,
                       ExternalEventList& output) const
{
    ExternalEvent* ee = new ExternalEvent("response");

    ee << attribute("name", event.getStringAttributeValue("name"));
    ee << attribute("value", val(event.getStringAttributeValue("name")));
    output.addEvent(ee);
}

}}} // namespace vle extension DifferenceEquation
