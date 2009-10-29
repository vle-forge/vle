/**
 * @file vle/extension/DifferentialEquation.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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

#include <vle/extension/DllDefines.hpp>
#include <vle/devs/Dynamics.hpp>
#include <deque>

namespace vle { namespace extension {

    class VLE_EXTENSION_EXPORT DifferentialEquation : public vle::devs::Dynamics
    {
    protected:
        /**
         * @brief Variable The class represents a variable of
         * differential equation.
         */
        class Variable
        {
        public:
            /**
             * @brief Default constructor
             */
            Variable() : mEquation(0)
            {}

            /**
             * @brief Returns the name of variable
             * @return the name of variable
             */
            const std::string& name() const
            { return mName; }

        protected:
            Variable(const std::string& name, DifferentialEquation* equation) :
                mName(name), mEquation(equation)
            {
            }

            std::string mName;
            DifferentialEquation* mEquation;
        };

    public:

        /**
         * @brief var The class var represents an internal
         * variable of the equation
         */
        class Var : public Variable
        {
        public:
            /**
             * @brief Default constructor
             */
            Var() {}

            virtual ~Var() {}

            /**
             * @brief Returns the current value of variable.
             * @return the current value of variable.
             */
            virtual double operator()() const
            {
                if (not mEquation) {
                    throw utils::InternalError(_(
                        "DifferentialEquation - variable not create"));
                }
                return mEquation->getValue();
            }

            /**
             * @brief Returns the value of variable.
             * @param delay the temporal delay.
             * @return the value of variable.
             */
            virtual double operator()(const vle::devs::Time& time,
                                      int delay) const
            {
                if (not mEquation) {
                    throw utils::InternalError(_(
                        "DifferentialEquation - variable not create"));
                }
                return mEquation->getValue(time, delay);
            }

        private:
            Var(const std::string& name, DifferentialEquation* equation) :
                Variable(name, equation)
            {
                if (name != equation->mVariableName) {
                    throw utils::InternalError(fmt(_(
                            "DifferentialEquation - wrong variable " \
                            "name: %1% in %2%")) % name %
                        equation->mVariableName);
                }
            }

            friend class DifferentialEquation;
        };

        /**
         * @brief ext The class represents an external variable of
         * difference equation.
         */
        class Ext : public Variable
        {
        public:
            /**
             * @brief Default constructor
             */
            Ext() {}

            virtual ~Ext() {}

            /**
             * @brief Returns the value of external variable.
             * @return the value of external variable.
             */
            virtual double operator()() const
            {
                if (not mEquation) {
                    throw utils::InternalError(_(
                        "DifferentialEquation - variable not create"));
                }
                return mEquation->getValue(mName);
            }

            /**
             * @brief Returns the value of variable.
             * @param delay the temporal delay.
             * @return the value of variable.
             */
            virtual double operator()(const vle::devs::Time& time,
                                      int delay) const
            {
                if (not mEquation) {
                    throw utils::InternalError(_(
                        "DifferentialEquation - variable not create"));
                }
                return mEquation->getValue(mName, time, delay);
            }

        protected:
            Ext(const std::string& name, DifferentialEquation* equation) :
                Variable(name, equation)
            {}

            friend class DifferentialEquation;
        };

        /**
         * @brief Create an external variable
         * @param name the name of the external variable
         * @return the external variable
         */
        inline Ext createExt(const std::string& name)
        {
            return Ext(name, this);
        }

        /**
         * @brief Create the internal variable
         * @param name the name of the internal variable
         * @return the internal variable
         */
        inline Var createVar(const std::string& name)
        {
            return Var(name, this);
        }

        /**
         * @brief The function to develop mathematical expression like:
         * @code
         * return a * x() - b * x() * y();
         * @endcode
         */
        virtual double compute(const vle::devs::Time& time) const =0;

        /**
         * @brief Activate the buffering mechanism of external
         * variable and set the size of value buffer of external
         * variables. By default, the external variables are not bufferized.
         * @param size the size of buffer.
         * @param delay the value of delay quantum.
         */
        void size(int size, double delay)
        {
            mBuffer = true;
            mSize = size;
            mDelay = delay;
        }

        /**
         * @brief Activate the buffering mechanism of external
         * variables with an unlimited size for the buffer.
         * By default, the external variables are not bufferized.
         */
        void buffer()
        {
            mBuffer = true;
            mSize = -1;
        }

    protected:
        DifferentialEquation(const vle::devs::DynamicsInit& model,
                             const vle::devs::InitEventList& events);

        virtual ~DifferentialEquation() {}

        /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

        virtual double getEstimatedValue(double e) const =0;
        virtual void reset(const vle::devs::Time& time, double value) =0;
        virtual void updateGradient(bool external,
                                    const vle::devs::Time& time) =0;
        virtual void updateSigma(const vle::devs::Time& time) =0;
        virtual void updateValue(bool external, const vle::devs::Time& time) =0;

        /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

        void pushValue(const vle::devs::Time& time,
                       double value);

        enum state { INIT, POST_INIT, RUN, RUN2, POST, POST2, POST3 };

        /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

        virtual vle::devs::Time init(const devs::Time& time);

    private:
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

        virtual vle::value::Value* observation(
            const vle::devs::ObservationEvent& event) const;

        virtual void request(const vle::devs::RequestEvent& event,
                             const vle::devs::Time& time,
                             vle::devs::ExternalEventList& output) const;

        /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

        enum thresholdType { UP, DOWN };
        typedef std::map < std::string, std::pair < double, thresholdType > >
            threshold;
        typedef std::deque < std::pair < devs::Time, double > > valueBuffer;

        const valueBuffer& externalValueBuffer(const std::string& name) const;
        valueBuffer& externalValueBuffer(const std::string& name);

        inline double getValue() const
        { return mValue; }

        double getValue(const vle::devs::Time& time, double delay) const;

        double getValue(const std::string& name) const;

        double getValue(const std::string& name,
                        const vle::devs::Time& time, double delay) const;

        void pushExternalValue(const std::string& name,
                               const vle::devs::Time& time,
                               double value);

        inline void setGradient(const std::string& name, double gradient)
        { mExternalVariableGradient[name] = gradient; }

    protected:
        bool mUseGradient;
        bool mActive;
        bool mDependance;

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

    private:
        void updateExternalVariable(const vle::devs::Time& time);

        friend class Var;
        friend class Ext;
    };

}} // namespace vle extension

#endif
