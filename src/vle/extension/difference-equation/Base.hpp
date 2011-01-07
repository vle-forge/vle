/*
 * @file vle/extension/difference-equation/Base.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#ifndef VLE_EXTENSION_DIFFERENCE_EQUATION_BASE_HPP
#define VLE_EXTENSION_DIFFERENCE_EQUATION_BASE_HPP 1

#include <vle/extension/DllDefines.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/utils/DateTime.hpp>

namespace vle { namespace extension { namespace DifferenceEquation {

enum Perturbation_t { ADD, SCALE, SET };

/**
 * @brief Base The class Base encapsules the general
 * mecanisms of DifferenceEquation extension.
 */
class VLE_EXTENSION_EXPORT Base : public devs::Dynamics
{
protected:
    typedef std::deque < double > Values;

    /**
     * @brief Variable The class represents a variable of
     * difference equation.
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
        Variable(std::string name, Base* equation) :
            mName(name), mEquation(equation)
        {}

        std::string mName;
        Base* mEquation;
    };

    struct VariableIterators
    {
        Values* mValues;
        Values* mInitValues;
        bool* mReceivedValues;
        bool mSynchro;
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
         * @brief Returns the value of variable.
         * @param shift the temporal shift.
         * @return the name of variable.
         */
        virtual double operator()(int shift = 0)
        {
            if (not mEquation) {
                throw utils::InternalError(
                    _("DifferenceEquation - variable not build"));
            }

            if (mEquation->init(mIterators)) {
                return mEquation->val(mName, mIterators.mInitValues,
                                      mIterators, shift);
            } else {
                return mEquation->val(mName, mIterators.mValues,
                                      mIterators, shift);
            }
        }

    protected:
        Ext(std::string name, Base* equation) :
            Variable(name, equation)
        { mEquation->createExternalVariable(name, mIterators); }

        VariableIterators mIterators;
    };

public:
    /**
     * @brief sync The class sync represents a synchronous
     * external variable
     */
    class Sync : public Ext
    {
    public:
        /**
         * @brief Default constructor
         */
        Sync() {}

        virtual ~Sync() {}

    private:
        Sync(std::string name, Base* equation) :
            Ext(name, equation)
        {
            equation->depends(name, true);
            mIterators.mSynchro = true;
        }

        friend class Base;
    };

    /**
     * @brief nosync The class nosync represents a asynchronous
     * external variable
     */
    class Nosync : public Ext
    {
    public:
        /**
         * @brief Default constructor
         */
        Nosync() {}

        virtual ~Nosync() {}

    private:
        Nosync(std::string name, Base* equation) :
            Ext(name, equation)
        {
            equation->depends(name, false);
            mIterators.mSynchro = false;
        }

        friend class Base;
    };

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

    static const unsigned int DEFAULT_SIZE;

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

    /**
     * @brief Create a synchronous external variable
     * @param name the name of the external variable
     * @return the synchronous external variable
     */
    inline Sync createSync(const std::string& name)
    { return Base::Sync(name, this); }

    /**
     * @brief Create a asynchronous external variable
     * @param name the name of the external variable
     * @return the asynchronous external variable
     */
    inline Nosync createNosync(const std::string& name)
    { return Base::Nosync(name, this); }

    void enableNosyncDependance()
    { mNosyncDependance = true; }

    int phase() const
    { return mPhase; }

    /**
     * @brief Set the size of value buffer of an external
     * variable. By default, this size is DEFAULT_SIZE.
     * @param name the name of external variable.
     * @param size the size of buffer.
     */
    void size(const std::string& name, int size);

    /**
     * @brief Get the size of value buffer of an external
     * variable.
     * @param name the name of external variable.
     */
    int size(const std::string& name)
    { return mSize.find(name)->second; }

    /**
     * @brief Returns the time step of equation.
     * @return the time step of equation.
     */
    double timeStep(const devs::Time& time) const
    {
        if (mTimeStepUnit == vle::utils::DateTime::None) {
            return mTimeStep;
        } else {
            return vle::utils::DateTime::duration(
                time, mTimeStep, mTimeStepUnit);
        }
    }

protected:
    typedef Values::const_iterator ValuesIterator;
    typedef std::map < std::string, Values> ValuesMap;
    typedef ValuesMap::const_iterator ValuesMapIterator;
    typedef std::map < std::string, bool > ReceivedMap;
    typedef ReceivedMap::const_iterator ReceivedMapIterator;
    typedef std::map < std::string, devs::Time > NosyncReceivedMap;
    typedef NosyncReceivedMap::const_iterator NosyncReceivedMapIterator;
    typedef std::map < std::string, int > SizeMap;
    typedef SizeMap::const_iterator SizeMapIterator;
    typedef std::map < std::string, std::string > Mapping;
    typedef std::set < std::string > SynchroSet;
    typedef SynchroSet::const_iterator SynchroSetIterator;
    typedef std::set < std::string > DependanceSet;
    typedef DependanceSet::const_iterator DependanceSetIterator;
    struct Perturbation
    {
        std::string name;
        double value;
        Perturbation_t type;

        Perturbation(const std::string& name,
                     double value,
                     Perturbation_t type) :
            name(name), value(value), type(type) {}
    };
    typedef std::vector < Perturbation > Perturbations;
    typedef std::vector < std::string > LockedVariables;

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

    enum state { SEND_INIT, POST_SEND_INIT, PRE_INIT, PRE_INIT2, INIT,
                 PRE, RUN, POST, POST2, POST3 };
    enum mode { NAME, PORT, MAPPING };

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

    Base(const devs::DynamicsInit& model,
         const devs::InitEventList& events,
         bool control = true);

    virtual ~Base() {}

    void addExternalValue(double value,
                          const std::string& name,
                          bool init = false);

    void applyPerturbations(const vle::devs::Time& time, bool update);

    void clearReceivedValues();

    void createExternalVariable(const std::string& name,
                                VariableIterators& iterators);

    void depends(const std::string& name,
                 bool synchronized);

    const ValuesMap& externalValues() const
    { return mExternalValues; }

    bool init(const VariableIterators& it) const
    { return (mState == INIT and not it.mInitValues->empty()); }

    void initExternalVariable(const std::string& name);

    void processUpdate(const std::string& name,
                       const vle::devs::ExternalEvent& event,
                       bool begin, bool end,
                       const vle::devs::Time& time);

    void pushNoEDValues();

    const ReceivedMap& receivedValues() const
    { return mReceivedValues; }

    virtual double val(const std::string& name,
                       const Values* it,
                       const VariableIterators& iterators,
                       int shift) const;

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

    virtual void addValue(double value, const std::string& name) =0;

    virtual bool check() =0;

    virtual double getValue(const std::string& name) =0;

    virtual unsigned int getVariableNumber() const =0;

    virtual void initValues(const vle::devs::Time& time) =0;

    virtual void invalidValues() =0;

    virtual void lockValue(const std::string& name) =0;

    virtual void removeValue(const std::string& name) =0;

    virtual void removeValues() =0;

    virtual void updateValues(const vle::devs::Time& time) =0;

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

    virtual devs::Time init(const devs::Time& time);

    virtual devs::Time timeAdvance() const;

    virtual void confluentTransitions(
        const devs::Time& internal,
        const devs::ExternalEventList& extEventlist);

    virtual void internalTransition(
        const devs::Time& time);

    virtual void externalTransition(
        const devs::ExternalEventList& event,
        const devs::Time& time);

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

    state mState;
    double mTimeStep;
    vle::utils::DateTime::Unit mTimeStepUnit;

    // external variable info section
    ValuesMap mExternalValues;
    ValuesMap mInitExternalValues;
    ValuesMap mNoEDValues;
    ReceivedMap mReceivedValues;
    NosyncReceivedMap mNosyncReceivedValues;
    SizeMap mSize;
    mode mMode;
    Mapping mMapping;
    bool mNosyncDependance; // if nosync variable use in initvalue()

    // perturbation info section
    Perturbations mPerturbations;
    LockedVariables mLockedVariables;

    // dependance and synchronous info section
    DependanceSet mDepends;
    bool mControl;
    bool mSynchro;
    SynchroSet mSynchros;
    unsigned int mSyncs;
    bool mAllSynchro;

    bool mActive;
    bool mDependance;
    unsigned int mReceive;
    vle::devs::Time mSigma;
    vle::devs::Time mSigma2;
    vle::devs::Time mLastTime;
    vle::devs::Time mLastComputeTime;
    vle::devs::Time mLastClearTime;
    int mWaiting;

    // phase
    int mPhase;
};

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

struct VLE_EXTENSION_EXPORT Var
{
    Var(const std::string& name) : name(name), value(0.0)  { }

    Var(const std::string& name,
        const vle::devs::ExternalEvent* event) :
        name(name),
        value(event->getDoubleAttributeValue("value")) { }
    virtual Var& operator=(double v) { value = v; return *this; }

    std::string name;
    double value;
};

double VLE_EXTENSION_EXPORT
operator<<(double& value, const Var& var);

vle::devs::ExternalEventList& VLE_EXTENSION_EXPORT
operator<<(vle::devs::ExternalEventList& output, const Var& var);

}}} // namespace vle extension DifferenceEquation

#endif
