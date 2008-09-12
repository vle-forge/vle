/**
 * @file vle/extension/DifferentialEquation.hpp
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

#ifndef VLE_EXTENSION_DIFFERENTIAL_EQUATION_HPP
#define VLE_EXTENSION_DIFFERENTIAL_EQUATION_HPP

#include <vle/devs/Dynamics.hpp>

namespace vle { namespace extension {

    class DifferentialEquation : public vle::devs::Dynamics
    {
    public:
        DifferentialEquation(const vle::graph::AtomicModel& model,
			     const vle::devs::InitEventList& events);

	virtual ~DifferentialEquation() { }

        inline double getValue() const
        { return mValue; }

	double getValue(const vle::devs::Time& time, double delay) const;

        double getValue(const std::string& name) const;

	double getValue(const std::string& name,
			const vle::devs::Time& time, double delay) const;

	virtual double getEstimatedValue(double e) const =0;

        /**
         * @brief The function to develop mathemacial expression like:
         * @code
         * return a * getValue() - b * getValue() * getValue("y");
         * @endcode
         */
        virtual double compute(const vle::devs::Time& time) const =0;

        enum state { INIT, POST_INIT, RUN, RUN2, POST, POST2, POST3 };

        virtual vle::devs::Time init(const devs::Time& time);

        virtual void output(
            const vle::devs::Time& time,
            vle::devs::ExternalEventList& output) const;

        virtual vle::devs::Time timeAdvance() const;

        virtual vle::devs::Event::EventType confluentTransitions(
            const vle::devs::Time& time,
            const vle::devs::ExternalEventList& extEventlist) const;

        virtual void internalTransition(
            const vle::devs::Time& event);

        virtual void externalTransition(
            const vle::devs::ExternalEventList& event,
            const vle::devs::Time& time);

        virtual vle::value::Value observation(
            const vle::devs::ObservationEvent& event) const;

        virtual void request(const vle::devs::RequestEvent& event,
                             const vle::devs::Time& time,
                             vle::devs::ExternalEventList& output) const;

    private:
	void updateExternalVariable(const vle::devs::Time& time);
	
    protected:
        enum thresholdType { UP, DOWN };
        typedef std::map < std::string, std::pair < double, thresholdType > >
            threshold;
	typedef std::deque < std::pair < devs::Time, double > > valueBuffer;

	void pushValue(const vle::devs::Time& time,
		       double value);
	void pushExternalValue(const std::string& name,
			       const vle::devs::Time& time,
			       double value);
        virtual void reset(const vle::devs::Time& time, double value) =0;
	virtual void updateGradient(bool external,
				    const vle::devs::Time& time) =0;
        virtual void updateSigma(const vle::devs::Time& time) =0;
        virtual void updateValue(bool external, const vle::devs::Time& time) =0;

        inline void setGradient(const std::string& name, double gradient)
        { mExternalVariableGradient[name] = gradient; }	

	bool mUseGradient;
        bool mActive;
        bool mDependance;

        const valueBuffer& externalValueBuffer(const std::string& name) const;
        valueBuffer& externalValueBuffer(const std::string& name);

        /** Internal variable */
        double mInitialValue;
        std::string mVariableName;
	double mValue;

        /** External variables */
        unsigned int mExternalVariableNumber;
        std::map < std::string , double > mExternalVariableValue;
        std::map < std::string , double > mExternalVariableGradient;
        std::map < std::string , bool > mIsGradient;
        bool mExternalValues;

	/** Thresholds **/
	double mPreviousValue;
	threshold mThresholds;

	/** Buffer */
	bool mBuffer;
	devs::Time mStartTime;
	double mDelay;
	int mSize;
	valueBuffer mValueBuffer;
	std::map < std::string, valueBuffer > mExternalValueBuffer;
	
        /** State */
        double mGradient;
        devs::Time mSigma;
        devs::Time mLastTime;
        state mState;
    };

}} // namespace vle extension

#endif
