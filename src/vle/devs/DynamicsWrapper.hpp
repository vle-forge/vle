/**
 * @file vle/devs/DynamicsWrapper.hpp
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


#ifndef DEVS_DYNAMICSWRAPPER_HPP
#define DEVS_DYNAMICSWRAPPER_HPP

#include <vle/devs/DllDefines.hpp>
#include <vle/devs/Dynamics.hpp>
#include <string>

#define DECLARE_DYNAMICSWRAPPER(mdl) \
    extern "C" { \
        vle::devs::Dynamics* \
        makeNewDynamicsWrapper(const vle::devs::DynamicsWrapperInit& init, \
                               const vle::devs::InitEventList& events) \
        { return new mdl(init, events); } \
    }

#define DECLARE_NAMED_DYNAMICSWRAPPER(name, mdl) \
    extern "C" { \
        vle::devs::Dynamics* \
        makeNewDynamicsWrapper##name(const vle::devs::DynamicsWrapperInit& init, \
                                     const vle::devs::InitEventList& events) \
        { return new mdl(init, events); } \
    }

namespace vle { namespace devs {

    class VLE_DEVS_EXPORT DynamicsWrapperInit : public DynamicsInit
    {
    public:
        DynamicsWrapperInit(const graph::AtomicModel& atom,
                            utils::Rand& rnd,
                            PackageId packageid,
                            const std::string& model,
                            const std::string& library)
            : DynamicsInit(atom, rnd, packageid), m_model(model),
            m_library(library)
        {}

        virtual ~DynamicsWrapperInit()
        {}

        const std::string& model() const { return m_model; }
        const std::string& library() const { return m_library; }

    private:
        const std::string& m_model;
        const std::string& m_library;
    };

    /**
     * @brief DynamicsWrapper class represent a part of the DEVS simulator. This
     * class must be inherits to build simulation components into another
     * programming language.
     *
     */
    class VLE_DEVS_EXPORT DynamicsWrapper : public Dynamics
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
