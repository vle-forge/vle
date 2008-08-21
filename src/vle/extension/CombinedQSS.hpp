/**
 * @file vle/extension/CombinedQSS.hpp
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


#ifndef VLE_EXTENSION_COMBINEDQSS_HPP
#define VLE_EXTENSION_COMBINEDQSS_HPP

#include <vle/devs/Dynamics.hpp>
#include <vector>
#include <map>

namespace vle { namespace extension {
    
    class CombinedQss : public devs::Dynamics
    {
    public:
        enum state { INIT, POST_INIT, RUN, POST, POST2, POST3 };

        CombinedQss(const graph::AtomicModel& model,
                    const devs::InitEventList& event);

        virtual ~CombinedQss() { }

        virtual void finish();

        virtual devs::Time init(const devs::Time& /* time */);

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

        virtual void request(const devs::RequestEvent& /* event */,
                             const devs::Time& /* time */,
                             devs::ExternalEventList& /* output */) const;
        /**
         * @brief The function to develop mathemacial expression like:
         * @code
         * if (i == 0) { // the first variable
         *   return a * getValue(0) - b * getValue(0) * getValue(1);
         * } else {
         *   return c * getValue(1) - d * getValue(1) * getValue(0);
         * }
         * @endcode
         * @param i the index of the variable to compute.
         */
        virtual double compute(unsigned int i) const = 0;

        /**
         * @brief Get the value of the variable specified by index. Be carefull,
         * no check on the variable i: 0 <= i < m_functionNumber.
         * @param i index of the variable.
         */
        inline double getValue(unsigned int i) const
        { return mValue[i]; }

    private:
        /**
         * @brief Set the value of the variable specified by index. Be carefull,
         * no check on the variable i: 0 <= i < m_functionNumber.
         * @param i index of the variable.
         */
        inline void setValue(unsigned int i, double value)
        { mValue[i] = value; }

        inline double getExternalValue(const std::string& name) const
        { return mExternalVariableValue.find(mExternalVariableIndex.find(name)->second)->second; }	

        inline void setExternalValue(const std::string& name, double value)
        { mExternalVariableValue[mExternalVariableIndex[name]] = value; }	

        inline void setExternalGradient(const std::string& name, double gradient)
        { mExternalVariableGradient[mExternalVariableIndex[name]] = gradient; }	

        bool mActive;
        bool mDependance;
        /** Internal variables */
        std::vector < std::pair < unsigned int , double > > mInitialValueList;
        std::map < unsigned int , std::string > mVariableName;
        double* mValue;
        std::map < unsigned int , double > mVariablePrecision;
        std::map < unsigned int , double > mVariableEpsilon;
        std::map < std::string , unsigned int > mVariableIndex;
        unsigned int mVariableNumber;
        /** External variables */
        unsigned int mExternalVariableNumber;
        std::map < unsigned int , double > mExternalVariableValue;
        std::map < unsigned int , double > mExternalVariableGradient;
        std::map < unsigned int , bool > mIsGradient;
        std::map < std::string , unsigned int > mExternalVariableIndex;
        bool mExternalValues; // y-a-t-il des variables externes
        // sans gradient ? 
        /** State */
        long* mIndex;
        double* mGradient;
        devs::Time* mSigma;
        devs::Time* mLastTime;
        state* mState;

        /** Current model */
        unsigned int mCurrentModel;
        double mThreshold;

        inline double d(unsigned int i, long x)
        { return x * mVariablePrecision[i]; }

        int getVariable(const std::string& name) const;

        const std::string& getVariable(unsigned int index) const;

        inline double getGradient(unsigned int i) const
        { return mGradient[i]; }

        inline long getIndex(unsigned int i) const
        { return mIndex[i]; }

        inline const devs::Time& getLastTime(unsigned int i) const
        { return mLastTime[i]; }

        inline const devs::Time& getSigma(unsigned int i) const
        { return mSigma[i]; }

        inline state getState(unsigned int i) const
        { return mState[i]; }

        inline void decIndex(unsigned int i)
        { --mIndex[i]; }

        inline void incIndex(unsigned int i)
        { ++mIndex[i]; }

        inline void setIndex(unsigned int i, long index)
        { mIndex[i] = index; }

        inline void setGradient(unsigned int i,double gradient)
        { mGradient[i] = gradient; }

        inline void setLastTime(unsigned int i,const devs::Time & time)
        { mLastTime[i] = time; }

        inline void setSigma(unsigned int i,const devs::Time & time)
        { mSigma[i] = time; }

        inline void setState(unsigned int i,state state)
        { mState[i] = state; }

        void updateSigma(unsigned int i);

        void minSigma();

        void reset(const devs::Time& time, unsigned int i, double value);
    };

}} // namespace vle extension

#endif
