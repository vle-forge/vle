/**
 * @file vle/extension/DifferenceEquation.hpp
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


#ifndef VLE_EXTENSION_DIFFERENCE_EQUATION_HPP
#define VLE_EXTENSION_DIFFERENCE_EQUATION_HPP

#include <vle/extension/DllDefines.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/utils/DateTime.hpp>
#include <set>

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
            {
            }

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
                    throw utils::InternalError(_(
                            "DifferenceEquation - variable not build"));
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
        {
            return Base::Sync(name, this);
        }

        /**
         * @brief Create a asynchronous external variable
         * @param name the name of the external variable
         * @return the asynchronous external variable
         */
        inline Nosync createNosync(const std::string& name)
        {
            return Base::Nosync(name, this);
        }

        /**
         * @brief Set the size of value buffer of an external
         * variable. By default, this size is DEFAULT_SIZE.
         * @param name the name of external variable.
         * @param size the size of buffer.
         */
        void size(const std::string& /* name */,
                  int /* size */);

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

        bool init(const VariableIterators& it) const
        { return (mState == INIT and
                  not it.mInitValues->empty()); }

        void initExternalVariable(const std::string& name);

        void processUpdate(const std::string& name,
                           const vle::devs::ExternalEvent& event,
                           bool begin, bool end,
                           const vle::devs::Time& time);

        void pushNoEDValues();

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

        virtual devs::Event::EventType confluentTransitions(
            const devs::Time& internal,
            const devs::ExternalEventList& extEventlist) const;

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
        SizeMap mSize;
        mode mMode;
        Mapping mMapping;

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
    };

    class VLE_EXTENSION_EXPORT Simple : public Base
    {
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
             * @brief Returns the value of variable.
             * @param shift the temporal shift.
             * @return the name of variable.
             */
            virtual double operator()(int shift = 0)
            { return ((Simple*)mEquation)->val(shift); }

        private:
            Var(std::string name, Base* equation) :
                Variable(name, equation)
            {
                if (name != ((Simple*)equation)->mVariableName) {
                    throw utils::InternalError(fmt(_(
                                "DifferenceEquation::simple - wrong variable" \
                                " name: %1% in %2%")) % name %
                        ((Simple*)equation)->mVariableName);
                }
            }

            friend class Simple;
        };

        /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

        /**
         * @brief Constructor of the dynamics of an
         * DifferenceEquation::Simple model. Process the
         * init events: these events occurs only at the beginning of the
         * simulation and initialize the state of the model.
         * @param model the atomic model to which belongs the dynamics.
         * @param event the init event list.
         * @param control if true then the external variables are
         * declared before use.
         */
        Simple(const devs::DynamicsInit& model,
               const devs::InitEventList& events,
               bool control = true);

        /**
         * @brief Destructor
         */
        virtual ~Simple() {}

        /**
         * @brief Compute the value of internal variable. This
         * method is abstract.
         * @param time the time of the computation.
         * @return the new value of internal variable.
         */
        virtual double compute(const vle::devs::Time& time) =0;

        /**
         * @brief Initialize the value of internal variable. This
         * method is not abstract. The default value is null if
         * no initialization event provides an initial value.
         * @param time the time of the computation.
         * @return the initial value of internal variable.
         */
        virtual double initValue(const vle::devs::Time& /* time */)
        { return 0.0; }

        /**
         * @brief Create an internal variable (the variable
         * managed by the equation)
         * @param name the name of the internal variable
         * @return the internal variable
         */
        Var createVar(const std::string& name)
        { return Var(name, this); }

        void size(int size);

    private:
        double val() const;

        double val(int shift) const;

        /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

        virtual void addValue(double value, const std::string& name);

        virtual bool check()
        { return mInitValue; }

        virtual double getValue(const std::string& /* name */)
        { return val(); }

        virtual unsigned int getVariableNumber() const
        { return 1; }

        virtual void initValues(const vle::devs::Time& time);

        virtual void invalidValues() {}

        virtual void lockValue(const std::string& /* name */) {}

        virtual void removeValue(const std::string& name);

        virtual void removeValues();

        virtual void updateValues(const vle::devs::Time& time);

        /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

        virtual void output(const devs::Time& time,
                            devs::ExternalEventList& output) const;

        virtual value::Value* observation(
            const devs::ObservationEvent& event) const;

        virtual void request(
            const devs::RequestEvent& /* event */,
            const devs::Time& /* time */,
            devs::ExternalEventList& /* output */) const;

        /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

        std::string mVariableName;
        Values mValues;
        double mInitialValue;
        bool mInitValue;
        int mSize;
        bool mSetValue;
    };

    class VLE_EXTENSION_EXPORT Multiple : public Base
    {
        typedef std::map < std::string, bool > SetValuesMap;

        class MultipleValues
        {
        public:
            MultipleValues() {}
            virtual ~MultipleValues() {}

            bool empty() const
            { return mDeque.empty(); }

            double front() const
            { return mDeque.front(); }

            void operator=(double value)
            { mDeque.push_front(value); }

            double operator[](unsigned int index) const
            { return mDeque[index]; }

            void pop_back()
            { mDeque.pop_back(); }

            void pop_front()
            { mDeque.pop_front(); }

            size_t size() const
            { return mDeque.size(); }

        private:
            Values mDeque;
        };

        typedef std::map < std::string, MultipleValues > MultipleValuesMap;

        struct MultipleVariableIterators
        {
            MultipleValues* mMultipleValues;
            bool* mSetValues;
        };

    public:
        /**
         * @brief var The class var represents an internal
         * variable of the system of equations
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
             * @brief Set the value of variable.
             * @param value the new value of variable
             */
            virtual void operator=(double value)
            {
                if (not mEquation) {
                    throw utils::InternalError(fmt(_(
                                "DifferenceEquation::multiple - variable %1% not build"))
                        % name());
                }

                if (not *mIterators.mSetValues) {
                    *mIterators.mMultipleValues = value;
                    *mIterators.mSetValues = true;
                }
            }

            /**
             * @brief Returns the value of variable.
             * @param shift the temporal shift.
             * @return the name of variable.
             */
            virtual double operator()(int shift = 0)
            {
                if (not mEquation) {
                    throw utils::InternalError(_(
                            "DifferenceEquation::multiple - variable not build"));
                }

                return ((Multiple*)mEquation)->val(mName, mIterators, shift);
            }

        private:
            Var(std::string name, Base* equation) :
                Variable(name, equation)
            {
                ((Multiple*)mEquation)->create(name, mIterators);
            }

            MultipleVariableIterators mIterators;

            friend class Multiple;
        };

        /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

        /**
         * @brief Constructor of the dynamics of an
         * DifferenceEquation::Multiple model. Process the
         * init events: these events occurs only at the beginning of the
         * simulation and initialize the state of the model.
         * @param model the atomic model to which belongs the dynamics.
         * @param event the init event list.
         * @param control if true then the external variables are
         * declared before use.
         */
        Multiple(const devs::DynamicsInit& model,
                 const devs::InitEventList& events,
                 bool control = true);

        /**
         * @brief Destructor
         */
        virtual ~Multiple() {}

        /**
         * @brief Compute the value of all internal variables. This
         * method is abstract.
         * @param time the time of the computation.
         */
        virtual void compute(const vle::devs::Time& time) =0;

        /**
         * @brief Call the compute method at beginning and
         * declare that all internal variables are initialized.
         */
        void computeInit(const vle::devs::Time& time);

        /**
         * @brief Create an internal variable (the variable
         * managed by the equation).
         * @param name the name of the internal variable.
         * @return the internal variable.
         */
        Var createVar(const std::string& name)
        { return Var(name, this); }

        /**
         * @brief Initialize an internal variable.
         * @param var the internal variable.
         * @param value the initial value of the internal variable.
         */
        void init(const Var& variable, double value);

        /**
         * @brief Initialize the value of internal variable. This
         * method is not abstract. The default value is null if
         * no initialization event provide an initial value.
         * @param time the time of the computation.
         * @return the initial value of internal variable.
         */
        virtual void initValue(const vle::devs::Time& time);

    private:
        void create(const std::string& name,
                    MultipleVariableIterators& iterator);

        void unset();

        double val(const std::string& name) const;

        double val(const std::string& name,
                   const MultipleVariableIterators& iterators,
                   int shift) const;

        /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

        virtual void addValue(double value, const std::string& name);

        virtual bool check();

        virtual double getValue(const std::string& name)
        { return val(name); }

        virtual unsigned int getVariableNumber() const
        { return mVariableNames.size(); }

        virtual void initValues(const vle::devs::Time& time);

        virtual void invalidValues();

        virtual void lockValue(const std::string& name);

        virtual void removeValue(const std::string& name);

        virtual void removeValues();

        virtual void updateValues(const vle::devs::Time& time);

        /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

        virtual devs::Time init(const devs::Time& time);

        virtual void output(const devs::Time& time,
                            devs::ExternalEventList& output) const;

        virtual value::Value* observation(
            const devs::ObservationEvent& event) const;

        virtual void request(
            const devs::RequestEvent& /* event */,
            const devs::Time& /* time */,
            devs::ExternalEventList& /* output */) const;

        /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

        std::vector < std::string > mVariableNames;
        MultipleValuesMap mValues;
        std::map < std::string, double > mInitialValues;
        std::map < std::string, bool > mInitValues;
        SetValuesMap mSetValues;
    };

    class VLE_EXTENSION_EXPORT Generic : public Simple
    {
    public:
        /**
         * @brief Constructor of the dynamics of an
         * DifferenceEquation::Generic model. Process the
         * init events: these events occurs only at the beginning of the
         * simulation and initialize the state of the model.
         * @param model the atomic model to which belongs the dynamics.
         * @param event the init event list.
         * @param control if true then the external variables are
         * declared before use.
         */
        Generic(const devs::DynamicsInit& model,
                const devs::InitEventList& events);

        /**
         * @brief Destructor
         */
        virtual ~Generic() {}

        /**
         * @brief Specify that all external variables are
         * synchronous. By default, the external variables are
         * not synchronous.
         */
        void allSync();

        /**
         * @brief The iterator points to the beginning of the
         * list of external variables.
         */
        void beginExt();

        /**
         * @brief Returns true if the iterator is at the ending
         * of the list of external variables.
         * @return true if the iterator is at the ending
         * of the list of external variables.
         */
        bool endExt();

        /**
         * @brief Return the value of current external variable.
         * @return the value of current variable.
         */
        double valueExt(int shift);

        /**
         * @brief Return the name of current external variable.
         * @return the name of current variable.
         */
        std::string nameExt();

        /**
         * @brief The iterator points to the next external variable.
         */
        void nextExt();

        /**
         * @brief Return the number of external variables.
         * @return the number of external variables.
         */
        size_t sizeExt() const
        { return mExternalValues.size(); }

    protected:
        virtual void externalTransition(
            const devs::ExternalEventList& event,
            const devs::Time& time);

    private:
        ValuesMapIterator mValuesIt;
        ValuesMapIterator mNamesIt;
    };

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

struct VLE_EXTENSION_EXPORT Var
{
    Var(const std::string& name) : name(name)  { }
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
