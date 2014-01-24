/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
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


#ifndef DEVS_DYNAMICSWRAPPER_HPP
#define DEVS_DYNAMICSWRAPPER_HPP

#include <vle/DllDefines.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/version.hpp>
#include <string>

#define DECLARE_DYNAMICSWRAPPER(mdl)                                    \
    extern "C" {                                                        \
        VLE_MODULE vle::devs::Dynamics*                                 \
        vle_make_new_dynamics_wrapper(                                  \
            const vle::devs::DynamicsWrapperInit& init,                 \
            const vle::devs::InitEventList& events)                     \
        {                                                               \
            return new mdl(init, events);                               \
        }                                                               \
                                                                        \
        VLE_MODULE void                                                 \
        vle_api_level(vle::uint32_t* major,                             \
                      vle::uint32_t* minor,                             \
                      vle::uint32_t* patch)                             \
        {                                                               \
            *major = VLE_MAJOR_VERSION;                                 \
            *minor = VLE_MINOR_VERSION;                                 \
            *patch = VLE_PATCH_VERSION;                                 \
        }                                                               \
    }

namespace vle { namespace devs {

    class VLE_API DynamicsWrapperInit : public DynamicsInit
    {
    public:
        DynamicsWrapperInit(const vpz::AtomicModel& atom,
                            PackageId packageid,
                            const std::string& library)
            : DynamicsInit(atom, packageid), m_library(library)
        {}

        virtual ~DynamicsWrapperInit()
        {}

        const std::string& library() const { return m_library; }

    private:
        const std::string& m_library;
    };

    /**
     * @brief DynamicsWrapper class represent a part of the DEVS simulator. This
     * class must be inherits to build simulation components into another
     * programming language.
     *
     */
    class VLE_API DynamicsWrapper : public Dynamics
    {
    public:
	/**
	 * @brief Constructor of the dynamics wrapper of an atomic model
	 * @param model the atomic model to which belongs the dynamics
	 */
        DynamicsWrapper(const DynamicsWrapperInit& init,
                        const devs::InitEventList& events) :
            Dynamics(init, events)
        {}

	/**
	 * @brief Destructor (nothing to do).
	 * @return none
	 */
        virtual ~DynamicsWrapper()
        {}

        /**
         * @brief If this function return true, then a cast to a DynamicsWrapper
         * object is produce and the set_model and set_library function are
         * call.
         * @return false if Dynamics is not a DynamicsWrapper.
         */
        inline virtual bool isWrapper() const
        { return true; }

    protected:
      std::string m_model;
      std::string m_library;
    };

}} // namespace vle devs

#endif
