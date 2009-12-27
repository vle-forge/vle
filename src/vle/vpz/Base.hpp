/**
 * @file vle/vpz/Base.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
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


#ifndef VLE_VPZ_BASE_HPP
#define VLE_VPZ_BASE_HPP

#include <vle/vpz/DllDefines.hpp>
#include <ostream>

namespace vle { namespace vpz {

    /**
     * @brief A base to class to force all inheritance classes to load and
     * write from an XML reference.
     */
    class VLE_VPZ_EXPORT Base
    {
    public:
        /**
         * @brief The list of available sub classes of vpz::Base. Use in the
         * vpz::SaxStackVpz class to sort the type of class.
         */
        enum type { STRUCTURES, MODEL, SUBMODELS, CONNECTIONS,
            INTERNAL_CONNECTION, INPUT_CONNECTION, OUTPUT_CONNECTION, ORIGIN,
            DESTINATION, IN, OUT, INIT, STATE, PORT, DYNAMICS, DYNAMIC,
            EXPERIMENT, PROJECT, VIEWS, OBSERVABLES, OBSERVABLE, OBSERVABLEPORT,
            OUTPUTS, OUTPUT, VIEW, CONDITIONS, CONDITION, CLASSES, CLASS,
            REPLICAS, VPZ, TRAME, MODELTRAME };

        /**
         * @brief Default constructor.
         */
        Base()
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~Base()
        {}

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
        { return getType() == STRUCTURES; }

        /**
         * @return True if type is MODEL.
         */
        inline bool isModel() const
        { return getType() == MODEL; }

        /**
         * @return True if type is SUBMODELS.
         */
        inline bool isSubmodels() const
        { return getType() == SUBMODELS; }

        /**
         * @return True if type is CONNECTIONS.
         */
        inline bool isConnections() const
        { return getType() == CONNECTIONS; }

        /**
         * @return True if type is INTERNAL_CONNECTION.
         */
        inline bool isInternalConnection() const
        { return getType() == INTERNAL_CONNECTION; }

        /**
         * @return True if type is INPUT_CONNECTION.
         */
        inline bool isInputConnection() const
        { return getType() == INPUT_CONNECTION; }

        /**
         * @return True if type is OUTPUT_CONNECTION.
         */
        inline bool isOutputConnection() const
        { return getType() == OUTPUT_CONNECTION; }

        /**
         * @return True if type is ORIGIN.
         */
        inline bool isOrigin() const
        { return getType() == ORIGIN; }

        /**
         * @return True if type is DESTINATION.
         */
        inline bool isDestination() const
        { return getType() == DESTINATION; }

        /**
         * @return True if type is IN.
         */
        inline bool isIn() const
        { return getType() == IN; }

        /**
         * @return True if type is OUT.
         */
        inline bool isOut() const
        { return getType() == OUT; }

        /**
         * @return True if type is INIT.
         */
        inline bool isInit() const
        { return getType() == INIT; }

        /**
         * @return True if type is STATE.
         */
        inline bool isState() const
        { return getType() == STATE; }

        /**
         * @return True if type is PORT.
         */
        inline bool isPort() const
        { return getType() == PORT; }

        /**
         * @return True if type is DYNAMICS.
         */
        inline bool isDynamics() const
        { return getType() == DYNAMICS; }

        /**
         * @return True if type is DYNAMIC.
         */
        inline bool isDynamic() const
        { return getType() == DYNAMIC; }

        /**
         * @return True if type is EXPERIMENT.
         */
        inline bool isExperiment() const
        { return getType() == EXPERIMENT; }

        /**
         * @return True if type is PROJECT.
         */
        inline bool isProject() const
        { return getType() == PROJECT; }

        /**
         * @return True if type is VIEWS.
         */
        inline bool isViews() const
        { return getType() == VIEWS; }

        /**
         * @return True if type is VIEW.
         */
        inline bool isView() const
        { return getType() == VIEW; }

        /**
         * @return True if type is OBSERVABLES.
         */
        inline bool isObservables() const
        { return getType() == OBSERVABLES; }

        /**
         * @return True if type is OBSERVABLE.
         */
        inline bool isObservable() const
        { return getType() == OBSERVABLE; }

        /**
         * @return True if type is OBSERVABLEPORT.
         */
        inline bool isObservablePort() const
        { return getType() == OBSERVABLEPORT; }

        /**
         * @return True if type is OUTPUTS.
         */
        inline bool isOutputs() const
        { return getType() == OUTPUTS; }

        /**
         * @return True if type is OUTPUT.
         */
        inline bool isOutput() const
        { return getType() == OUTPUT; }

        /**
         * @return True if type is CONDITIONS.
         */
        inline bool isConditions() const
        { return getType() == CONDITIONS; }

        /**
         * @return True if type is CONDITION.
         */
        inline bool isCondition() const
        { return getType() == CONDITION; }

        /**
         * @return True if type is CLASSES.
         */
        inline bool isClasses() const
        { return getType() == CLASSES; }

        /**
         * @return True if type is CLASS.
         */
        inline bool isClass() const
        { return getType() == CLASS; }

        /**
         * @return True if type is REPLICAS.
         */
        inline bool isReplicas() const
        { return getType() == REPLICAS; }

        /**
         * @return True if type is VPZ.
         */
        inline bool isVpz() const
        { return getType() == VPZ; }

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
        { obj.write(out); return out; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Functors.
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Predicate functor. Check if the specified vpz::Base object is
         * a class.
         */
        struct IsClass {
            /**
             * @brief Check if the pointer is a Class.
             * @param base the pointer to check.
             * @return true if pointer is not null and is a Class.
             */
            inline bool operator()(const vpz::Base* base) const
            { return base != 0 and base->getType() == CLASS; }

            /**
             * @brief Check if the reference is a Class.
             * @param base the reference to check.
             * @return true if pointer if a Class.
             */
            inline bool operator()(const vpz::Base& base) const
            { return base.getType() == CLASS; }
        };
    };

}} // namespace vle vpz

#endif
