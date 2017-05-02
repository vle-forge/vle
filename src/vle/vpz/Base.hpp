/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VLE_VPZ_BASE_HPP
#define VLE_VPZ_BASE_HPP

#include <ostream>
#include <vle/DllDefines.hpp>

namespace vle {
namespace vpz {

/**
 * @brief A base to class to force all inheritance classes to load and
 * write from an XML reference.
 */
class VLE_API Base
{
public:
    /**
     * @brief The list of available sub classes of vpz::Base. Use in the
     * vpz::SaxStackVpz class to sort the type of class.
     */
    enum type
    {
        VLE_VPZ_STRUCTURES,
        VLE_VPZ_MODEL,
        VLE_VPZ_SUBMODELS,
        VLE_VPZ_CONNECTIONS,
        VLE_VPZ_INTERNAL_CONNECTION,
        VLE_VPZ_INPUT_CONNECTION,
        VLE_VPZ_OUTPUT_CONNECTION,
        VLE_VPZ_ORIGIN,
        VLE_VPZ_DESTINATION,
        VLE_VPZ_IN,
        VLE_VPZ_OUT,
        VLE_VPZ_INIT,
        VLE_VPZ_STATE,
        VLE_VPZ_PORT,
        VLE_VPZ_DYNAMICS,
        VLE_VPZ_DYNAMIC,
        VLE_VPZ_EXPERIMENT,
        VLE_VPZ_PROJECT,
        VLE_VPZ_VIEWS,
        VLE_VPZ_OBSERVABLES,
        VLE_VPZ_OBSERVABLE,
        VLE_VPZ_OBSERVABLEPORT,
        VLE_VPZ_OUTPUTS,
        VLE_VPZ_OUTPUT,
        VLE_VPZ_VIEW,
        VLE_VPZ_CONDITIONS,
        VLE_VPZ_CONDITION,
        VLE_VPZ_CLASSES,
        VLE_VPZ_CLASS,
        VLE_VPZ_REPLICAS,
        VLE_VPZ_VPZ,
        VLE_VPZ_TRAME,
        VLE_VPZ_MODELTRAME
    };

    /**
     * @brief Default constructor.
     */
    Base()
    {
    }

    /**
     * @brief Nothing to delete.
     */
    virtual ~Base()
    {
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Pure virtual functions.
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief write object information into the stream.
     * @param out an output stream to write xml.
     */
    virtual void write(std::ostream& out) const = 0;

    /**
     * @brief Return the Base::type of the Vpz object.
     * @return the Base::type of the object.
     */
    virtual Base::type getType() const = 0;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Inline.
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @return True if type is STRUCTURES.
     */
    inline bool isStructures() const
    {
        return getType() == VLE_VPZ_STRUCTURES;
    }

    /**
     * @return True if type is MODEL.
     */
    inline bool isModel() const
    {
        return getType() == VLE_VPZ_MODEL;
    }

    /**
     * @return True if type is SUBMODELS.
     */
    inline bool isSubmodels() const
    {
        return getType() == VLE_VPZ_SUBMODELS;
    }

    /**
     * @return True if type is CONNECTIONS.
     */
    inline bool isConnections() const
    {
        return getType() == VLE_VPZ_CONNECTIONS;
    }

    /**
     * @return True if type is INTERNAL_CONNECTION.
     */
    inline bool isInternalConnection() const
    {
        return getType() == VLE_VPZ_INTERNAL_CONNECTION;
    }

    /**
     * @return True if type is INPUT_CONNECTION.
     */
    inline bool isInputConnection() const
    {
        return getType() == VLE_VPZ_INPUT_CONNECTION;
    }

    /**
     * @return True if type is OUTPUT_CONNECTION.
     */
    inline bool isOutputConnection() const
    {
        return getType() == VLE_VPZ_OUTPUT_CONNECTION;
    }

    /**
     * @return True if type is ORIGIN.
     */
    inline bool isOrigin() const
    {
        return getType() == VLE_VPZ_ORIGIN;
    }

    /**
     * @return True if type is DESTINATION.
     */
    inline bool isDestination() const
    {
        return getType() == VLE_VPZ_DESTINATION;
    }

    /**
     * @return True if type is IN.
     */
    inline bool isIn() const
    {
        return getType() == VLE_VPZ_IN;
    }

    /**
     * @return True if type is OUT.
     */
    inline bool isOut() const
    {
        return getType() == VLE_VPZ_OUT;
    }

    /**
     * @return True if type is INIT.
     */
    inline bool isInit() const
    {
        return getType() == VLE_VPZ_INIT;
    }

    /**
     * @return True if type is STATE.
     */
    inline bool isState() const
    {
        return getType() == VLE_VPZ_STATE;
    }

    /**
     * @return True if type is PORT.
     */
    inline bool isPort() const
    {
        return getType() == VLE_VPZ_PORT;
    }

    /**
     * @return True if type is DYNAMICS.
     */
    inline bool isDynamics() const
    {
        return getType() == VLE_VPZ_DYNAMICS;
    }

    /**
     * @return True if type is DYNAMIC.
     */
    inline bool isDynamic() const
    {
        return getType() == VLE_VPZ_DYNAMIC;
    }

    /**
     * @return True if type is EXPERIMENT.
     */
    inline bool isExperiment() const
    {
        return getType() == VLE_VPZ_EXPERIMENT;
    }

    /**
     * @return True if type is PROJECT.
     */
    inline bool isProject() const
    {
        return getType() == VLE_VPZ_PROJECT;
    }

    /**
     * @return True if type is VIEWS.
     */
    inline bool isViews() const
    {
        return getType() == VLE_VPZ_VIEWS;
    }

    /**
     * @return True if type is VIEW.
     */
    inline bool isView() const
    {
        return getType() == VLE_VPZ_VIEW;
    }

    /**
     * @return True if type is OBSERVABLES.
     */
    inline bool isObservables() const
    {
        return getType() == VLE_VPZ_OBSERVABLES;
    }

    /**
     * @return True if type is OBSERVABLE.
     */
    inline bool isObservable() const
    {
        return getType() == VLE_VPZ_OBSERVABLE;
    }

    /**
     * @return True if type is OBSERVABLEPORT.
     */
    inline bool isObservablePort() const
    {
        return getType() == VLE_VPZ_OBSERVABLEPORT;
    }

    /**
     * @return True if type is OUTPUTS.
     */
    inline bool isOutputs() const
    {
        return getType() == VLE_VPZ_OUTPUTS;
    }

    /**
     * @return True if type is OUTPUT.
     */
    inline bool isOutput() const
    {
        return getType() == VLE_VPZ_OUTPUT;
    }

    /**
     * @return True if type is CONDITIONS.
     */
    inline bool isConditions() const
    {
        return getType() == VLE_VPZ_CONDITIONS;
    }

    /**
     * @return True if type is CONDITION.
     */
    inline bool isCondition() const
    {
        return getType() == VLE_VPZ_CONDITION;
    }

    /**
     * @return True if type is CLASSES.
     */
    inline bool isClasses() const
    {
        return getType() == VLE_VPZ_CLASSES;
    }

    /**
     * @return True if type is CLASS.
     */
    inline bool isClass() const
    {
        return getType() == VLE_VPZ_CLASS;
    }

    /**
     * @return True if type is REPLICAS.
     */
    inline bool isReplicas() const
    {
        return getType() == VLE_VPZ_REPLICAS;
    }

    /**
     * @return True if type is VPZ.
     */
    inline bool isVpz() const
    {
        return getType() == VLE_VPZ_VPZ;
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Output stream.
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Stream operator for std::stream to help building trame.
     * @param out the output where data are writed.
     * @param obj the sub class to write.
     * @return the std::ostream.
     */
    friend std::ostream& operator<<(std::ostream& out, const Base& obj)
    {
        obj.write(out);
        return out;
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Functors.
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Predicate functor. Check if the specified vpz::Base object is
     * a class.
     */
    struct IsClass
    {
        /**
         * @brief Check if the pointer is a Class.
         * @param base the pointer to check.
         * @return true if pointer is not null and is a Class.
         */
        inline bool operator()(const vpz::Base* base) const
        {
            return base != 0 and base->getType() == VLE_VPZ_CLASS;
        }

        /**
         * @brief Check if the reference is a Class.
         * @param base the reference to check.
         * @return true if pointer if a Class.
         */
        inline bool operator()(const vpz::Base& base) const
        {
            return base.getType() == VLE_VPZ_CLASS;
        }
    };
};
}
} // namespace vle vpz

#endif
