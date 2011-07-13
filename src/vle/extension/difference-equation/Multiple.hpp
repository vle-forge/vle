/*
 * @file vle/extension/difference-equation/Multiple.hpp
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


#ifndef VLE_EXTENSION_DIFFERENCE_EQUATION_MULTIPLE_HPP
#define VLE_EXTENSION_DIFFERENCE_EQUATION_MULTIPLE_HPP 1

#include <vle/extension/difference-equation/Base.hpp>

namespace vle { namespace extension { namespace DifferenceEquation {

class VLE_EXTENSION_EXPORT Multiple : public Base
{
    typedef std::map < std::string, bool > SetValuesMap;

    class MultipleValues
    {
    public:
        MultipleValues() {}
        virtual ~MultipleValues() {}

        Values::const_iterator begin() const
        { return mDeque.begin(); }

        bool empty() const
        { return mDeque.empty(); }

        Values::const_iterator end() const
        { return mDeque.end(); }

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
        bool* mInitValues;
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
                throw utils::InternalError(
                    fmt(_("DifferenceEquation::multiple - " \
                          "variable %1% not build"))
                    % name());
            }

            if (not *mIterators.mSetValues) {
                *mIterators.mMultipleValues = value;
                {
                    int s = mEquation->size(name());

                    if (s > 0
                        and mIterators.mMultipleValues->size() > (size_t)s) {
                        mIterators.mMultipleValues->pop_back();
                    }
                }
                *mIterators.mSetValues = true;
                *mIterators.mInitValues = true;
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
                throw utils::InternalError(
                    _("DifferenceEquation::multiple - variable not build"));
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
     * @brief Initialize the value of internal variable. This
     * method is not abstract. The default value is null if
     * no initialization event provide an initial value.
     * @param time the time of the computation.
     * @return the initial value of internal variable.
     */
    virtual void initValue(const vle::devs::Time& time);

protected:
    const MultipleValuesMap& values() const
    { return mValues; }

private:
    void create(const std::string& name,
                MultipleVariableIterators& iterator);

    void unset(bool all = false);

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

}}} // namespace vle extension DifferenceEquation

#endif
