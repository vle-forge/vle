/**
 * @file vle/extension/difference-equation/Simple.hpp
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


#ifndef VLE_EXTENSION_DIFFERENCE_EQUATION_SIMPLE_HPP
#define VLE_EXTENSION_DIFFERENCE_EQUATION_SIMPLE_HPP 1

#include <vle/extension/difference-equation/DifferenceEquation.hpp>

namespace vle { namespace extension { namespace DifferenceEquation {

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
                throw utils::InternalError(
                    fmt(_("DifferenceEquation::simple - wrong variable" \
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

    /**
     * @brief Specify a new size of history of internal variable
     * @param new size of history of internal variable
     */
    void size(int size);

    /**
     * @brief Get the name of internal variable
     * @return the name of internal variable
     */
    std::string name() const
    { return mVariableName; }

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
    std::string mPortName;
    Values mValues;
    double mInitialValue;
    bool mInitValue;
    int mSize;
    bool mSetValue;
};

}}} // namespace vle extension DifferenceEquation

#endif
