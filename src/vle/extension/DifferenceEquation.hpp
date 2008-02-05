/**
 * @file src/vle/extension/DifferenceEquation.hpp
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




#ifndef VLE_EXTENSION_DIFFERENCE_EQUATION_HPP
#define VLE_EXTENSION_DIFFERENCE_EQUATION_HPP

#include <vle/devs/Dynamics.hpp>

namespace vle { namespace extension {
    
    class DifferenceEquation : public devs::Dynamics
    {
	typedef std::map < std::string, std::deque < std::pair < double, double > > > valueList;

    public:
        DifferenceEquation(const graph::AtomicModel& model,
                           const devs::InitEventList& events);

        virtual ~DifferenceEquation() { }

	virtual double compute(const vle::devs::Time& /* time */) =0;
	virtual double initValue() { return 0; }
	double getTimeStep() const { return mTimeStep; }
	double getValue(const char* name, int shift = 0);
	double getValue(int shift = 0) const;

        virtual devs::Time init(const devs::Time& time);

        virtual void output(const devs::Time& time,
                            devs::ExternalEventList& output) const;

        virtual devs::Time timeAdvance() const;

        virtual devs::Event::EventType confluentTransitions(
	    const devs::Time& internal,
            const devs::ExternalEventList& extEventlist) const;

        virtual void internalTransition(
	    const devs::Time& event);

        virtual void externalTransition(
	    const devs::ExternalEventList& event,
            const devs::Time& time);

        virtual value::Value observation(
	    const devs::ObservationEvent& event) const;

        virtual void request(
            const devs::RequestEvent& /* event */,
            const devs::Time& /* time */,
            devs::ExternalEventList& /* output */) const;

    protected:
	void displayValues();
	
    private:
	
	void addValue(const std::string& /* name */,
		      const vle::devs::Time& /* time */,
		      double /* value */);
	
	enum state { PRE_INIT, PRE_INIT2, INIT, PRE, RUN, POST, POST2 }; 
	
	std::string mVariableName;
	state mState;
	double mTimeStep;
	double mDelta;
	unsigned int mMultiple;
	std::deque < double > mValue;
	valueList mValues;
	std::map < std::string, unsigned int > mMultiples;
	unsigned int mSyncs;
	bool mActive;
	bool mDependance;
	int mSize;
	unsigned int mReceive;
	vle::devs::Time mSigma;
	vle::devs::Time mSigma2;
	vle::devs::Time mLastTime;
	std::vector < std::string > mWait;
	bool mInitValue;
	double mInitialValue;
	bool mInvalid;
    };

}} // namespace vle extension

#endif
