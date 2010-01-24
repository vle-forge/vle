/**
 * @file vle/extension/differential-equation/QSS.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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


#ifndef VLE_EXTENSION_DIFFERENTIAL_EQUATION_QSS_HPP
#define VLE_EXTENSION_DIFFERENTIAL_EQUATION_QSS_HPP 1

#include <vle/extension/differential-equation/DifferentialEquation.hpp>

namespace vle { namespace extension { namespace QSS {

class VLE_EXTENSION_EXPORT Simple :
        public vle::extension::DifferentialEquation::Base
{
public:
    Simple(const vle::devs::DynamicsInit& model,
           const vle::devs::InitEventList& events);
    virtual ~Simple() { }

private:
    virtual double getEstimatedValue(double e) const;
    virtual vle::devs::Time init(const devs::Time& time);
    virtual void reset(const vle::devs::Time& /* time */,
                       double /* value */);
    virtual void updateGradient(bool external, const vle::devs::Time& time);
    virtual void updateSigma(const vle::devs::Time& time);
    virtual void updateValue(bool external, const vle::devs::Time& time);

    unsigned long mIndex;
    /** Constantes */
    double mPrecision;
    double mEpsilon;
    double mThreshold;

    inline double d(unsigned long x)
    { return x * mPrecision; }
};

class VLE_EXTENSION_EXPORT Multiple : public devs::Dynamics
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
        { }

        /**
         * @brief Returns the name of variable
         * @return the name of variable
         */
        const std::string& name() const
        { return mName; }

    protected:
        Variable(const std::string& name, Multiple* equation) :
            mName(name), mEquation(equation)
        {
        }

        std::string mName;
        Multiple* mEquation;
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
        Var() { }

        virtual ~Var() {}

        /**
         * @brief Returns the value of internal variable.
         * @return the value of internal variable.
         */
        virtual double operator()() const
        {
            if (not mEquation) {
                throw utils::InternalError(
                    _("QSS::Multiple - variable not create"));
            }
            return mEquation->getValue(mIndex);
        }

    private:
        Var(unsigned int index, const std::string& name,
            Multiple* equation) :
            Variable(name, equation), mIndex(index)
        { }

        unsigned int mIndex;

        friend class Multiple;
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
        Ext() { }

        virtual ~Ext() {}

        /**
         * @brief Returns the value of external variable.
         * @return the value of external variable.
         */
        virtual double operator()() const
        {
            if (not mEquation) {
                throw utils::InternalError(
                    _("QSS::Multiple - variable not create"));
            }
            return mEquation->getExternalValue(mName);
        }

    protected:
        Ext(const std::string& name, Multiple* equation) :
            Variable(name, equation)
        { }

        friend class Multiple;
    };

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

    Multiple(const devs::DynamicsInit& model,
             const devs::InitEventList& event);
    virtual ~Multiple();

    /**
     * @brief Create an external variable
     * @param name the name of the external variable
     * @return the external variable
     */
    inline Ext createExt(const std::string& name)
    { return Ext(name, this); }

    /**
     * @brief Create the internal variable
     * @param name the name of the internal variable
     * @return the internal variable
     */
    inline Var createVar(unsigned int index,
                         const std::string& name)
    { return Var(index, name, this); }

    /**
     * @brief The function to develop mathematical expression like:
     * @code
     * if (i == 0) { // the first variable
     *   return a * x() - b * x() * y();
     * } else {
     *   return c * y() - d * y() * x();
     * }
     * @endcode
     * @param i the index of the variable to compute.
     * @param time the time of the computation.
     */
    virtual double compute(unsigned int i,
                           const devs::Time& time) const = 0;

private:
    virtual devs::Time init(const devs::Time& /* time */);

    virtual void output(const devs::Time& time,
                        devs::ExternalEventList& output) const;

    virtual devs::Time timeAdvance() const;

    virtual devs::Event::EventType confluentTransitions(
        const devs::Time& internal,
        const devs::ExternalEventList& extEventlist) const;

    virtual void internalTransition(const devs::Time& event);

    virtual void externalTransition(const devs::ExternalEventList& event,
                                    const devs::Time& time);

    virtual value::Value* observation(
        const devs::ObservationEvent& event) const;

    virtual void request(const devs::RequestEvent& /* event */,
                         const devs::Time& /* time */,
                         devs::ExternalEventList& /* output */) const;

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

    inline double getValue(unsigned int i) const
    { return mValue[i]; }

    inline void setValue(unsigned int i, double value)
    { mValue[i] = value; }

    inline double getExternalValue(const std::string& name) const
    {
        return mExternalVariableValue.find(mExternalVariableIndex.find(name)
                                           ->second)->second;
    }

    inline void setExternalValue(const std::string& name, double value)
    { mExternalVariableValue[mExternalVariableIndex[name]] = value; }

    inline void setExternalGradient(const std::string& name, double gradient)
    { mExternalVariableGradient[mExternalVariableIndex[name]] = gradient; }

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

    enum state { INIT, POST_INIT, RUN, POST, POST2, POST3 };

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

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

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

    friend class Var;
    friend class Ext;
};

}}} // namespace vle extension QSS

#endif
