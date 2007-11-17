/**
 * @file extension/DifferenceEquation.hpp
 * @author The VLE Development Team.
 */

/*
 * Copyright (C) 2007 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef VLE_EXTENSION_DIFFERENCE_EQUATION_HPP
#define VLE_EXTENSION_DIFFERENCE_EQUATION_HPP

#include <vle/devs/Dynamics.hpp>

namespace vle { namespace extension {
    
    class DifferenceEquation : public devs::Dynamics
    {
    public:
        enum state { INIT, POST_INIT, RUN, POST, POST2, POST3 };

        DifferenceEquation(const graph::AtomicModel& model);

        virtual ~DifferenceEquation() { }

        virtual void finish();

        virtual devs::Time init();

        virtual void getOutputFunction(
	    const devs::Time& time,
            devs::ExternalEventList& output);

        virtual devs::Time getTimeAdvance();

        virtual devs::Event::EventType processConflict(
	    const devs::InternalEvent& internal,
            const devs::ExternalEventList& extEventlist) const;

        virtual void processInitEvents(
	    const devs::InitEventList& event);

        virtual void processInternalEvent(
	    const devs::InternalEvent& event);

        virtual void processExternalEvents(
	    const devs::ExternalEventList& event,
            const devs::Time& time);

        virtual value::Value processStateEvent(
	    const devs::StateEvent& event) const;

        virtual void processInstantaneousEvent(
            const devs::InstantaneousEvent& /* event */,
            const devs::Time& /* time */,
            devs::ExternalEventList& /* output */) const;

    protected:
	inline double getValue(const std::string& name) const
        { return mExternalVariableValue.find(
                mExternalVariableIndex.find(name)->second)->second; }	

	inline void setValue(const std::string& name, double value)
        { mExternalVariableValue[mExternalVariableIndex[name]] = value; }	

        /**
         * @brief The function to develop mathemacial expression like:
         * @code
         * return a * mValue - b * mValue * getValue("y");
         * @endcode
         */
        virtual double compute() = 0;

        bool mActive;
        bool mDependance;
        devs::Time mTimeStep;

        /** Internal variable */
        double mInitialValue;
        std::string mVariableName;
        double mValue;

        /** External variables */
	std::map < unsigned int , double > mExternalVariableValue;
	std::map < std::string , unsigned int > mExternalVariableIndex;

        /** State */
        devs::Time mLastTime;
        state mState;
	devs::Time mSigma;

    private:
	void reset(const devs::Time& time, double value);
};

}} // namespace vle extension

#endif
