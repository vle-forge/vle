/**
 * @file examples/equation/Executive.cpp
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


#include <examples/equation/Executive.hpp>
#include <boost/algorithm/string.hpp>

namespace vle { namespace examples { namespace equation {

Executive::Executive(const devs::ExecutiveInit& model,
		     const devs::InitEventList& events) :
    devs::Executive(model, events)
{
    mTimeStep = value::toDouble(events.get("time-step"));
    mView = value::toString(events.get("view"));
}

void Executive::buildInputConnections()
{
    std::map < std::string, std::pair < std::string, bool > >::iterator itv =
	mVariables.begin();

    while (itv != mVariables.end()) {
	if (not itv->second.second) {
	    std::vector < model* >::const_iterator itm = mModels.begin();

	    itv->second.second = true;
	    while (itm != mModels.end()) {
		std::string name((*itm)->name);
		value::VectorValue::const_iterator it = (*itm)->in.begin();

		while (it != (*itm)->in.end()) {
		    std::string varModel = (*it)->writeToString();

		    if (mVariables.find(varModel)->second.first ==
			itv->second.first) {
			std::string updatePort(
			    (fmt("update_%1%_%2%") % name %
			     varModel).str());

			coupledmodel().delInternalConnection(
			    "Executive", updatePort,
			    name, "update");
			coupledmodel().addInternalConnection(
			    varModel,"update",
			    name, "update");
		    }
		    ++it;
		}
		++itm;
	    }
	}
	++itv;
    }
}

void Executive::buildOutputConnections(model* mdl)
{
    std::string name(mdl->name);
    value::VectorValue::const_iterator it = mdl->out.begin();
    value::VectorValue::const_iterator it2 = mdl->generic.begin();

    while (it != mdl->out.end()) {
	// if generic
	if (value::toBoolean(**it2)) {
	    coupledmodel().findModel((*it)->writeToString())
		->addInputPort(name);
	    coupledmodel().addInternalConnection(name, "update",
						 (*it)->writeToString(),
						 name);
	} else {
	    coupledmodel().addInternalConnection(name, "update",
						 (*it)->writeToString(),
						 "update");
	}
	++it;
	++it2;
    }
}

void Executive::clearModels()
{
    std::vector < model* >::const_iterator it = mModels.begin();

    while (it != mModels.end()) {
	delete *it++;
    }
    mModels.clear();
}

void Executive::createModel(model* mdl)
{
    std::string modelName(mdl->name);

    // Load dynamics library
    std::vector < std::string > dyn;

    boost::split(dyn, mdl->dyn, boost::is_any_of(":"));

    std::string dynamicsName((fmt("dyn_%1%") % modelName).str());
    std::string library = dyn[0];
    std::string model = dyn[1];
    vpz::Dynamic dyns(dynamicsName);

    dyns.setLibrary(library);
    dyns.setModel(model);
    dyns.setLocalDynamics();
    try {
        dynamics().add(dyns);
    } catch (...) { }

    // Build condition
    std::string conditionName((fmt("cond_%1%") % modelName).str());
    vpz::Strings conditions;
    vpz::Condition condition(conditionName);

    condition.addValueToPort("name",value::String::create( mdl->var));
    condition.addValueToPort("time-step", value::Double::create(mTimeStep));
    Executive::conditions().add(condition);

    // Build observable
    std::string obsName((fmt("obs_%1%") % modelName).str());
    vpz::Observable obs(obsName);
    vpz::ObservablePort& port = obs.add(mdl->var);

    port.add(mView);
    observables().add(obs);

    // Build model
    graph::AtomicModel* atom(new graph::AtomicModel(modelName,
						    &coupledmodel()));

    atom->addInputPort("update");
    atom->addOutputPort("update");
    conditions.push_back(conditionName);
    coupledmodel().addInternalConnection(modelName, "update",
					 "Executive", "update");

    value::VectorValue::const_iterator it = mdl->in.begin();

    while (it != mdl->in.end()) {
	std::string updatePort(
	    (fmt("update_%1%_%2%") % modelName %
	     (*it)->writeToString()).str());

	if (not coupledmodel().findModel("Executive")
	    ->existOutputPort(updatePort)) {
	    coupledmodel().findModel("Executive")->addOutputPort(updatePort);
	}
	coupledmodel().addInternalConnection("Executive", updatePort,
					     modelName, "update");
	++it;
    }
    devs::Executive::createModel(atom, dynamicsName, conditions, obsName);
}

void Executive::removeInputConnections(model* mdl)
{
    std::string name(mdl->name);
    value::VectorValue::const_iterator it = mdl->in.begin();

    while (it != mdl->in.end()) {
	coupledmodel().delInternalConnection((*it)->writeToString(),
					     "update", name, "update");
	++it;
    }
}

void Executive::removeModel(model* mdl)
{
    std::string name(mdl->name);

    delModel(&coupledmodel(), name);
}

void Executive::removeOutputConnections(model* mdl)
{
    std::string name(mdl->name);
    value::VectorValue::const_iterator it = mdl->out.begin();
    value::VectorValue::const_iterator it2 =
	mdl->generic.begin();

    while (it != mdl->out.end()) {
	if (value::toBoolean(**it2)) {
	    coupledmodel().delInternalConnection(name, "update",
						 (*it)->writeToString(),
						 name);
	    coupledmodel().findModel((*it)->writeToString())
		->delInputPort(name);
	} else {
	    coupledmodel().delInternalConnection(name, "update",
						 (*it)->writeToString(),
						 "update");
	}
	++it;
	++it2;
    }
}

void Executive::removeTemporaryConnections(model* mdl)
{
    std::string name(mdl->name);
    value::VectorValue::const_iterator it = mdl->in.begin();

    while (it != mdl->in.end()) {
	std::string updatePort((fmt("update_%1%_%2%") % mdl->name %
				(*it)->writeToString()).str());

	coupledmodel().delInternalConnection("Executive", updatePort,
					     name, "update");
	++it;
    }
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

devs::Time Executive::init(const devs::Time& /* time */)
{
    mState = IDLE;
    mSigma = devs::Time::infinity;
    mReRun = false;
    mReceives = 0;
    mSent = false;
    if (getModel().existInputPort("update")) {
	graph::ModelPortList::const_iterator it =
	    getModel().getInPort("update").begin();

	while (it != getModel().getInPort("update").end()) {
	    mActiveModels.push_back(it->first->getName());
	    ++it;
	}
    }
    return devs::Time::infinity;
}

devs::Time Executive::timeAdvance() const
{
    return mSigma;
}

void Executive::output(const devs::Time& /* time */,
		       devs::ExternalEventList& output) const
{
    if (mState == POST_ADD) {
	{
	    std::map < std::string,
		std::pair < std::string, bool > >::const_iterator itv =
		mVariables.begin();

	    while (itv != mVariables.end()) {
		if (not itv->second.second) {
		    std::vector < model* >::const_iterator itm =
			mModels.begin();

		    while (itm != mModels.end()) {
			std::string name((*itm)->name);
			value::VectorValue::const_iterator it =
			    (*itm)->in.begin();

			while (it != (*itm)->in.end()) {
			    std::string varModel = (*it)->writeToString();

			    if (mVariables.find(varModel)->second.first ==
				itv->second.first) {
				std::string updatePort(
				    (fmt("update_%1%_%2%") % name %
				     varModel).str());
				devs::ExternalEvent* ee =
				    new devs::ExternalEvent(updatePort);
				std::string var = itv->second.first;

				ee << devs::attribute(
				    "name",
				    value::String::create(var));
				ee << devs::attribute(
				    "value",
				    value::Double::create(
					mValues.find(
					    std::make_pair(varModel,
							   var))->second));
				output.addEvent(ee);
			    }
			    ++it;
			}
			++itm;
		    }
		}
		++itv;
	    }
	}

	if (not mSent) {
	    std::vector < model* >::const_iterator it = mModels.begin();

	    while (it != mModels.end()) {
		std::string name((*it)->name);
		devs::ExternalEvent* ee =
		    new devs::ExternalEvent("add");

		ee << devs::attribute("name",
					   value::String::create(name));
		output.addEvent(ee);
		++it;
	    }
	}
    } else if (mState == POST_REMOVE) {
	std::vector < model* >::const_iterator it = mModels.begin();

	while (it != mModels.end()) {
	    std::string name((*it)->name);
	    devs::ExternalEvent* ee =
		new devs::ExternalEvent("remove");

	    ee << devs::attribute("name",
				       value::String::create(name));
	    output.addEvent(ee);
	    ++it;
	}
    }
}

void Executive::internalTransition(const devs::Time& /* time */)
{
    switch (mState) {
    case ADD:
    {
	std::vector < model* >::const_iterator it = mModels.begin();

	while (it != mModels.end()) {
	    createModel(*it);
	    buildOutputConnections(*it);
	    ++it;
	}
	mState = POST_ADD;
	mSigma = 0;
	break;
    }
    case POST_ADD:
    {
	if (not mReRun) {
	    buildInputConnections();
	    mSent = true;
	    if (mReceives == mActiveModels.size()) {
		clearModels();
		mVariables.clear();
		mValues.clear();
		mReceives = 0;
		mSent = false;
		mState = IDLE;
		mSigma = devs::Time::infinity;
	    } else {
		mState = POST_ADD;
		mSigma = devs::Time::infinity;
	    }
	} else {
	    mReRun = false;
	}
	break;
    }
    case IDLE:
    {
	break;
    }
    case REMOVE:
    {
	{
	    std::vector < model* >::const_iterator it = mModels.begin();

	    while (it != mModels.end()) {
		removeInputConnections(*it);
		removeOutputConnections(*it);
		++it;
	    }
	}
	{
	    std::vector < model* >::const_iterator it = mModels.begin();

	    while (it != mModels.end()) {
		removeModel(*it);
		++it;
	    }
	}

	mState = POST_REMOVE;
	mSigma = 0;
	break;
    }
    case POST_REMOVE:
    {
	if (not mReRun) {
	    clearModels();
	    mState = IDLE;
	    mSigma = devs::Time::infinity;
	} else {
	    mReRun = false;
	}
	break;
    }
    }
}

void Executive::externalTransition(const devs::ExternalEventList& event,
				   const devs::Time& /* time */)
{
    devs::ExternalEventList::const_iterator it = event.begin();

    while (it != event.end()) {
	if ((*it)->onPort("update")) {
	    std::string name = (*it)->getStringAttributeValue("name");
	    double value = (*it)->getDoubleAttributeValue("value");
	    std::string source = (*it)->getModel()->getName();

	    mValues[std::make_pair(source, name)] = value;
	    mVariables[source] = std::make_pair(name, false);
	    ++mReceives;
	    if (mState == POST_ADD or mState == POST_REMOVE) {
		mReRun = true;
		mSigma = 0;
	    } else {
		mReRun = false;
	    }
	} else if ((*it)->onPort("add")) {
	    model* m = new model((*it)->getStringAttributeValue("name"),
				 (*it)->getStringAttributeValue("var"),
				 (*it)->getStringAttributeValue("dyn"),
				 (*it)->getSetAttributeValue("in"),
				 (*it)->getSetAttributeValue("out"),
				 (*it)->getSetAttributeValue("generic"));

	    mModels.push_back(m);
	    mActiveModels.push_back(m->name);
	    mState = ADD;
	    mSigma = 0;
	} else if ((*it)->onPort("remove")) {
	    model* m = new model(
		(*it)->getStringAttributeValue("name"),
		"", "",
		(*it)->getSetAttributeValue("in"),
		(*it)->getSetAttributeValue("out"),
		(*it)->getSetAttributeValue("generic"));

	    mModels.push_back(m);
	    mState = REMOVE;
	    mSigma = 0;
	}
	++it;
    }
}

}}} // namespace vle examples equation
