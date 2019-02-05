/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2003-2018 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2018 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2018 INRA http://www.inra.fr
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

#include <string>
#include <vle/DllDefines.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/vle.hpp>

#if defined _WIN32 || defined __CYGWIN__
#define VLE_MODULE __declspec(dllexport)
#else
#if __GNUC__ >= 4
#define VLE_MODULE __attribute__((visibility("default")))
#else
#define VLE_MODULE
#endif
#endif

#define DECLARE_DYNAMICSWRAPPER(mdl)                                          \
    extern "C"                                                                \
    {                                                                         \
        VLE_MODULE vle::devs::Dynamics* vle_make_new_dynamics_wrapper(        \
          const vle::devs::DynamicsWrapperInit& init,                         \
          const vle::devs::InitEventList& events)                             \
        {                                                                     \
            return new mdl(init, events);                                     \
        }                                                                     \
                                                                              \
        VLE_MODULE void vle_api_level(std::uint32_t* major,                   \
                                      std::uint32_t* minor,                   \
                                      std::uint32_t* patch)                   \
        {                                                                     \
            auto version = vle::version();                                    \
            *major = std::get<0>(version);                                    \
            *minor = std::get<1>(version);                                    \
            *patch = std::get<2>(version);                                    \
        }                                                                     \
    }

namespace vle {
namespace devs {

struct DynamicsWrapperInit;

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
                    const devs::InitEventList& events);

    /**
     * @brief Destructor (nothing to do).
     * @return none
     */
    ~DynamicsWrapper() override = default;

    /**
     * @brief If this function return true, then a cast to a DynamicsWrapper
     * object is produce and the set_model and set_library function are
     * call.
     * @return false if Dynamics is not a DynamicsWrapper.
     */
    inline bool isWrapper() const override
    {
        return true;
    }

protected:
    std::string m_library;
};
}
} // namespace vle devs

#endif
