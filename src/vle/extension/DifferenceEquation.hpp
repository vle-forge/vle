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
        typedef std::map < std::string,
                std::deque < std::pair < double, double > > > valueList;

    public:
        /**
         * @brief Constructor of Difference Equation extension. A
         * difference equation is composed to a internal variable
         * defined by the equation and some external
         * variables. The external variables can be synchronous
         * or not. The computation of internal variable waits the
         * value of synchronous variables.
         * @param model the atomic model to which belongs the dynamics.
         * @param event the init event list.
         */
        DifferenceEquation(const graph::AtomicModel& model,
                           const devs::InitEventList& events);

        virtual ~DifferenceEquation() { }

        /**
         * @brief Compute the value of internal variable. This
         * method is abstract.
         * @param time the time of the computation.
         * @return the new value of internal variable.
         */
        virtual double compute(const vle::devs::Time& /* time */) const =0;

        /**
         * @brief Initialize the value of internal variable. This
         * method is not abstract. The default value is null if
         * no initialization event provide an initial value.
         * @return the initial value of internal variable.
         */
        virtual double initValue() const { return 0.0; }

        /**
         * @brief Return the time step of equation.
         * @return the time step of equation.
         */
        double getTimeStep() const { return mTimeStep; }

        /**
         * @brief Return the value of an external variable. By
         * default, this method returns the current value. If the
         * parameter shift is specified, it returns the value of
         * external variable at current time minus shift. Shift
         * is the number of time step.
         * @param name the name of external variable.
         * @param shift the temporal shift.
         * @return the value of external variable.
         */
        double getValue(const char* /* name */,
                        int /* shift */ = 0) const;

        /**
         * @brief Return the value of the internal variable. By
         * default, this method returns the current value. If the
         * parameter shift is specified, it returns the value of
         * internal variable at current time minus shift. Shift
         * is the number of time step.
         * @param shift the temporal shift.
         * @return the value of the internal variable.
         */
        double getValue(int /* shift */ = 0) const;

        /**
         * @brief Specify if the external variable is
         * synchronous. By default, the external variables are
         * not synchronous.
         * @param name the name of external variable.
         */
        void setSynchronizedVariable(const std::string& /* name */);

        /**
         * @brief Specify that all external variables are
         * synchronous. By default, the external variables are
         * not synchronous.
         */
        void allSynchronizedVariable();

        /**
         * @brief Set the size of value buffer of an external
         * variable. By default, this size is infinity.
         * @param name the name of external variable.
         * @param name the size of buffer.
         */
        void setSize(const std::string& /* name */,
                     int /* size */);

        // iteration of dependance variables
        void beginNames();
        bool endNames();
        void nextName();
        std::string name() const;

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
        bool initExternalVariable(const std::string& /* name */,
                                  const vle::devs::ExternalEvent& /* event */,
                                  double& timeStep);

        enum state { PRE_INIT, PRE_INIT2, INIT, PRE, RUN, POST, POST2 };
        enum mode { NAME, PORT, MAPPING };

        std::string mVariableName;
        state mState;
        double mTimeStep;
        std::deque < double > mValue;
        valueList mValues;

        bool mSynchro;
        std::map < std::string, bool > mSynchros;
        unsigned int mSyncs;
        bool mAllSynchro;

        bool mActive;
        bool mDependance;
        std::map < std::string , int > mSize;
        unsigned int mReceive;
        vle::devs::Time mSigma;
        vle::devs::Time mSigma2;
        vle::devs::Time mLastTime;
        int mWaiting;
        std::vector < std::string > mWait;
        bool mInitValue;
        double mInitialValue;
        bool mInvalid;
        mode mMode;
        std::map < std::string, std::string > mMapping;

        valueList::const_iterator valuesIt;
        valueList::const_iterator namesIt;

    };

}} // namespace vle extension

#endif
