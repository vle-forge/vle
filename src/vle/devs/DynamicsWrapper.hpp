/**
 * @file devs/DynamicsWrapper.hpp
 * @author The VLE Development Team.
 * @brief DynamicsWrapper class represent a part of the DEVS simulator. This class
 * must be inherits to build simulation components into another programming language.
 */

/*
 * Copyright (c) 2004, 2005 The VLE Development Team.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef DEVS_DYNAMICSWRAPPER_HPP
#define DEVS_DYNAMICSWRAPPER_HPP

#include <vle/devs/Dynamics.hpp>
#include <string>

namespace vle { namespace devs {

    /**
     * @brief DynamicsWrapper class represent a part of the DEVS simulator. This class
     * must be inherits to build simulation components into another programming language.
     *
     */
    class DynamicsWrapper: public Dynamics
    {
    public:
	/**
	 * Constructor of the dynamics wrapper of an atomic model
	 *
	 * @param model the atomic model to which belongs the dynamics
	 */
        DynamicsWrapper(const graph::AtomicModel& model) : 
            Dynamics(model)
        { }

	/**
	 * Destructor (nothing to do)
	 *
	 * @return none
	 */
        virtual ~DynamicsWrapper()
        { }

        /** 
         * @brief If this function return true, then a cast to a DynamicsWrapper
         * object is produce and the set_model and set_library function are call.
         * 
         * @return false if Dynamics is not a DynamicsWrapper.
         */
        inline virtual bool is_wrapper() const
        { return true; }

        /** 
         * @brief Set the name of library
         */
        inline virtual void set_library(const std::string& library)
        { m_library = library; }

        /** 
         * @brief Set the name of model
         */
        inline virtual void set_model(const std::string& model)
        { m_model = model; }

    protected:
      std::string m_model;
      std::string m_library;
    };

}} // namespace vle devs

#endif
