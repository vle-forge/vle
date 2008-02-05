/**
 * @file src/vle/vpz/Base.hpp
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




#ifndef VLE_VPZ_BASE_HPP
#define VLE_VPZ_BASE_HPP

#include <ostream>
#include <libxml++/libxml++.h>

namespace vle { namespace vpz {

    /** 
     * @brief A base to class to force all inheritance classes to load and
     * write from an XML reference.
     */
    class Base
    {
    public:
        enum type { STRUCTURES, MODEL, SUBMODELS, CONNECTIONS,
            INTERNAL_CONNECTION, INPUT_CONNECTION, OUTPUT_CONNECTION, ORIGIN,
            DESTINATION, IN, OUT, INIT, STATE, PORT, DYNAMICS, DYNAMIC,
            EXPERIMENT, PROJECT, VIEWS, OBSERVABLES, OBSERVABLE, OBSERVABLEPORT,
            OUTPUTS, OUTPUT, VIEW, CONDITIONS, CONDITION, CLASSES, CLASS,
            REPLICAS, VPZ, TRAME, MODELTRAME };

        Base() { }

        virtual ~Base() { }


        //
        // virtuals functions 
        //


        /** 
         * @brief write object information into the stream.
         * 
         * @param out an output stream to write xml.
         */
        virtual void write(std::ostream& out) const = 0;

        /** 
         * @brief Return the Base::type of the Vpz object.
         * 
         * @return the Base::type of the object.
         */
        virtual Base::type getType() const = 0;


        //
        // inlines functions
        //


        inline bool isStructures() const { return getType() == STRUCTURES; }
        inline bool isModel() const { return getType() == MODEL; }
        inline bool isSubmodels() const { return getType() == SUBMODELS; }
        inline bool isConnections() const { return getType() == CONNECTIONS; }
        inline bool isInternalConnection() const { return getType() ==
            INTERNAL_CONNECTION; }
        inline bool isInputConnection() const { return getType() ==
            INPUT_CONNECTION; }
        inline bool isOutputConnection() const { return getType() ==
            OUTPUT_CONNECTION; }
        inline bool isOrigin() const { return getType() == ORIGIN; }
        inline bool isDestination() const { return getType() == DESTINATION; }
        inline bool isIn() const { return getType() == IN; }
        inline bool isOut() const { return getType() == OUT; }
        inline bool isInit() const { return getType() == INIT; }
        inline bool isState() const { return getType() == STATE; }
        inline bool isPort() const { return getType() == PORT; }
        inline bool isDynamics() const { return getType() == DYNAMICS; }
        inline bool isDynamic() const { return getType() == DYNAMIC; }
        inline bool isExperiment() const { return getType() == EXPERIMENT; }
        inline bool isProject() const { return getType() == PROJECT; }
        inline bool isViews() const { return getType() == VIEWS; }
        inline bool isView() const { return getType() == VIEW; }
        inline bool isObservables() const { return getType() == OBSERVABLES; }
        inline bool isObservable() const { return getType() == OBSERVABLE; }
        inline bool isObservablePort() const { return getType() ==
            OBSERVABLEPORT; }
        inline bool isOutputs() const { return getType() == OUTPUTS; }
        inline bool isOutput() const { return getType() == OUTPUT; }
        inline bool isConditions() const { return getType() == CONDITIONS; }
        inline bool isCondition() const { return getType() == CONDITION; }
        inline bool isClasses() const { return getType() == CLASSES; }
        inline bool isClass() const { return getType() == CLASS; }
        inline bool isReplicas() const { return getType() == REPLICAS; }
        inline bool isVpz() const { return getType() == VPZ; }
        inline bool isTrame() const { return getType() == TRAME; }
        inline bool isModelTrame() const { return getType() == MODELTRAME; }


        //
        // friend stream
        //


        /** 
         * @brief Operator << for std::stream to help building trame.
         * @param out the output where data are writed.
         * @param tr trame base class.
         * @return the std::ostream.
         */
        friend std::ostream& operator<<(std::ostream& out, const Base& obj)
        { obj.write(out); return out; }

    };

}} // namespace vle vpz

#endif
