/**
 * @file examples/equation/Controller.cpp
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


#include <examples/equation/Controller.hpp>

namespace vle { namespace examples { namespace equation {

Controller::Controller(const graph::AtomicModel& model,
		       const devs::InitEventList& events) :
    devs::Dynamics(model, events)
{
    mTimeStep = value::toDouble(events.get("time-step"));
}

devs::ExternalEvent* Controller::makeAddEvent(
    std::string modelName,
    std::string var,
    std::string dyn,
    value::Set* in,
    value::Set* out,
    value::Set* generic) const
{
    devs::ExternalEvent* ee = new devs::ExternalEvent("add");

    ee << devs::attribute("name",
			       value::String::create(modelName));
    ee << devs::attribute("var",
			       value::String::create(var));
    ee << devs::attribute("dyn",
			       value::String::create(dyn));
    ee << devs::attribute("in", in);
    ee << devs::attribute("out", out);
    ee << devs::attribute("generic", generic);
    return ee;
}

devs::ExternalEvent* Controller::makeRemoveEvent(
    std::string modelName,
    value::Set* in,
    value::Set* out,
    value::Set* generic) const
{
    devs::ExternalEvent* ee = new devs::ExternalEvent("remove");

    ee << devs::attribute("name", value::String::create(modelName));
    ee << devs::attribute("in", in);
    ee << devs::attribute("out", out);
    ee << devs::attribute("generic", generic);
    return ee;
}

void Controller::output(const devs::Time& /* time */,
			devs::ExternalEventList& output) const
{
    std::string nameB((boost::format("b%1%") % mModelNumber).str());
    value::Set* inB = value::Set::create();
    value::Set* outB = value::Set::create();
    value::Set* genericB = value::Set::create();

    inB->add(value::String::create("A"));
    outB->add(value::String::create("sum"));
    genericB->add(value::Boolean::create(true));

    std::string nameC((boost::format("c%1%") % mModelNumber).str());
    value::Set* inC = value::Set::create();
    value::Set* outC = value::Set::create();
    value::Set* genericC = value::Set::create();

    inC->add(value::String::create(nameB));
    outC->add(value::String::create("sum"));
    genericC->add(value::Boolean::create(true));

    std::string nameD((boost::format("d%1%") % mModelNumber).str());
    value::Set* inD = value::Set::create();
    value::Set* outD = value::Set::create();
    value::Set* genericD = value::Set::create();

    inD->add(value::String::create("A"));
    inD->add(value::String::create(nameC));
    outD->add(value::String::create("sum"));
    genericD->add(value::Boolean::create(true));

    if (mMode) {
	output.addEvent(makeAddEvent(nameB, "b", "equation1:B1",
				     inB, outB, genericB));
	output.addEvent(makeAddEvent(nameC, "c", "equation1:C1",
				     inC, outC, genericC));
	output.addEvent(makeAddEvent(nameD, "d", "equation1:D1",
				     inD, outD, genericD));
    } else {
	output.addEvent(makeRemoveEvent(nameB, inB, outB, genericB));
	output.addEvent(makeRemoveEvent(nameC, inC, outC, genericC));
	output.addEvent(makeRemoveEvent(nameD, inD, outD, genericD));
    }
}

devs::Time Controller::timeAdvance() const
{
    return mSigma;
}

devs::Time Controller::init(const devs::Time& /* time */)
{
    mModelNumber = 0;
    mMode = true;
    mSigma = mTimeStep;
    return 0;
}

void Controller::internalTransition(const devs::Time& /* event */)
{
    if (mMode) ++mModelNumber;
    else --mModelNumber;

    if (mModelNumber == 5) {
	mMode = false;
	--mModelNumber;
    } else if (mModelNumber < 0) {
	mSigma = devs::Time::infinity;
    }
}

}}} // namespace vle examples equation
