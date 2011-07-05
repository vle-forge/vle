/**
 * @file src/examples/DifferenceEquationAdaptor.cpp
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
#include <vle/extension/difference-equation/Base.hpp>

namespace vle { namespace examples { namespace qss {

class DifferenceEquationAdaptor : public vle::devs::Dynamics
{
    typedef std::map < std::string, double > values;

public:
    DifferenceEquationAdaptor(const vle::devs::DynamicsInit& init,
                              const vle::devs::InitEventList& events) :
	vle::devs::Dynamics(init, events)
    {
        mTimeStep = vle::value::toDouble(events.get("time-step"));
    }

    virtual ~DifferenceEquationAdaptor()
    { }

    virtual vle::devs::Time init(const vle::devs::Time& time)
    {
        {
            const vle::graph::ConnectionList& in_list =
                getModel().getInputPortList();

            for (vle::graph::ConnectionList::const_iterator it =
                     in_list.begin(); it != in_list.end(); ++it) {
                if (not getModel().existOutputPort(it->first)) {
                    throw utils::InternalError(
                        fmt(_(
                                "[%1%] DifferenceEquation adaptor - "   \
                                "%2% output port is missing")) %
                        getModelName() % it->first);
                }
                mValues[it->first] = 0.;
            }
        }

        {
            const vle::graph::ConnectionList& out_list =
                getModel().getOutputPortList();

            for (vle::graph::ConnectionList::const_iterator it =
                     out_list.begin(); it != out_list.end(); ++it) {
                if (not getModel().existInputPort(it->first))
                    throw utils::InternalError(
                        fmt(_(
                                "[%1%] DifferenceEquation adaptor - "   \
                                "%2% input port is missing or "         \
                                "%2% output port is unused")) %
                        getModelName() % it->first);
            }
        }

        mPhase = INIT;
        mSigma = 0;
        mLastTime = time;
        mStepNumber = 0;
        return vle::devs::Time::infinity;
    }

    virtual void output(const vle::devs::Time& /* time */,
			vle::devs::ExternalEventList& output) const
    {
        if (mPhase == SEND) {
            for (values::const_iterator it = mValues.begin();
                 it != mValues.end(); ++it) {
                output << (vle::extension::DifferenceEquation::Var(it->first) =
                           it->second);
            }
        }
    }

    virtual vle::devs::Time timeAdvance() const
    {
        return mSigma;
    }

    virtual void internalTransition(const vle::devs::Time& time)
    {
        if (mPhase == STEP) {
            ++mStepNumber;
            mPhase = SEND;
            mSigma = 0;
        } else if (mPhase == SEND) {
            mPhase = STEP;
            mSigma = mTimeStep;
        }
        mLastTime = time;
    }

    virtual void externalTransition(
	const vle::devs::ExternalEventList& events,
	const vle::devs::Time& time)
    {
        vle::devs::ExternalEventList::const_iterator it = events.begin();

        while (it != events.end()) {
            if (mPhase == INIT or mPhase == STEP or mPhase == SEND) {
                std::string name = (*it)->getPortName();
                values::iterator itv = mValues.find(name);

                if (itv != mValues.end() and
                    (*it)->getStringAttributeValue("name") == name) {
                    itv->second = (*it)->getDoubleAttributeValue("value");
                } else {
                    throw utils::InternalError(
                        fmt(_(
                                "[%1%] DifferenceEquation adaptor - "     \
                                "wrong variable name on %2% port: %3%")) %
                        getModelName() % name %
                        (*it)->getStringAttributeValue("name"));
                }

                if (mPhase == INIT) {
                    mPhase = SEND;
                    mSigma = 0;
                } else {
                    if (mSigma > 0) {
                        mSigma -= time - mLastTime;

                        double error = (time + mSigma).getValue() -
                            ((mStepNumber + 1) * mTimeStep);

                        if (error > 0 and error < 1e-12) {
                            mSigma -= 1.001 * error;
                        }
                    }
                }
            }
            ++it;
        }
        mLastTime = time;
    }

    virtual void confluentTransitions(
        const vle::devs::Time& time,
        const vle::devs::ExternalEventList& extEventlist)
    {
        externalTransition(extEventlist, time);
        internalTransition(time);
    }

    virtual vle::value::Value* observation(
        const vle::devs::ObservationEvent& event) const
    {
        values::const_iterator it = mValues.find(event.getPortName());

        if (it != mValues.end()) {
            return vle::value::Double::create(it->second);
        } else {
            return 0;
        }
    }

    enum phase { INIT, STEP, SEND };

    // parameters
    double mTimeStep;

    // state
    phase mPhase;
    vle::devs::Time mSigma;
    vle::devs::Time mLastTime;
    unsigned long mStepNumber;

    values mValues;
};

}}} // namespace vle examples qss

DECLARE_NAMED_DYNAMICS(DifferenceEquationAdaptor,
                       vle::examples::qss::DifferenceEquationAdaptor)
