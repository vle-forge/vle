/**
 * @file vle/extension/difference-equation/Generic.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2009 ULCO http://www.univ-littoral.fr
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


#ifndef VLE_EXTENSION_DIFFERENCE_EQUATION_GENERIC_HPP
#define VLE_EXTENSION_DIFFERENCE_EQUATION_GENERIC_HPP

#include <vle/extension/difference-equation/Simple.hpp>

namespace vle { namespace extension { namespace DifferenceEquation {

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

}}} // namespace vle extension DifferenceEquation

#endif
