/**
 * @file src/vle/extension/QSS.hpp
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




#ifndef VLE_EXTENSION_QSS_HPP
#define VLE_EXTENSION_QSS_HPP

#include <vle/devs/Dynamics.hpp>

namespace vle { namespace extension {

    class qss : public vle::devs::Dynamics
    {
    public:
        enum state { INIT, POST_INIT, RUN, POST, POST2, POST3 };

        qss(const vle::graph::AtomicModel& model,
            const vle::devs::InitEventList& events);

        virtual ~qss() { }

        virtual void finish();

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

        virtual void request(const vle::devs::RequestEvent& /* event */,
                             const vle::devs::Time& /* time */,
                             vle::devs::ExternalEventList& /* output */) const;

    protected:
        inline double getValue(const std::string& name) const
        { return mExternalVariableValue.find(
                mExternalVariableIndex.find(name)->second)->second; }

        inline void setValue(const std::string& name, double value)
        { mExternalVariableValue[mExternalVariableIndex[name]] = value; }	

        inline void setGradient(const std::string& name, double gradient)
        { mExternalVariableGradient[mExternalVariableIndex[name]] = gradient; }	

        /**
         * @brief The function to develop mathemacial expression like:
         * @code
         * return a * getValue(0) - b * getValue(0) * getValue(1);
         * @endcode
         */
        virtual double compute() = 0;

        bool mActive;
        bool mDependance;
        
        /** Internal variable */
        double mInitialValue;
        std::string mVariableName;
        double mValue;

        /** External variables */
        unsigned int mExternalVariableNumber;
        std::map < unsigned int , double > mExternalVariableValue;
        std::map < unsigned int , double > mExternalVariableGradient;
        std::map < unsigned int , bool > mIsGradient;
        std::map < std::string , unsigned int > mExternalVariableIndex;
        bool mExternalValues; // y-a-t-il des variables externes
                              // sans gradient ? 
        /** State */
        unsigned long mIndex;	
        double mGradient;
        devs::Time mSigma;
        devs::Time mLastTime;
        state mState;

        /** Constantes */
        double mPrecision;
        double mEpsilon;

    private:
        double mThreshold;

        inline double d(unsigned long x)
        { return x * mPrecision; }

        void updateSigma();
        void reset(const vle::devs::Time& time, double value);
    };

}} // namespace vle extension

#endif
