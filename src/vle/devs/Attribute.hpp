/**
 * @file devs/Attribute.hpp
 * @author The VLE Development Team.
 * @brief The Event classes need a pair string value, to construct
 * information. Attribute is this class and define some function to quickly
 * build Attribute.
 */

/*
 * Copyright (c) 2005 The VLE Development Team.
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

#ifndef DEVS_ATTRIBUTE_HPP
#define DEVS_ATTRIBUTE_HPP

#include <vle/value/Value.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Set.hpp>
#include <utility>
#include <string>

namespace vle { namespace devs {

    /**
     * @brief The Event classes need a pair string value, to construct
     * information. Attribute is this class and define some function to quickly
     * build Attribute.
     */
    typedef std::pair < std::string, value::Value > Attribute;

    /**
     * Build an attribute with a specified name and integer value.
     *
     * @param name the name of the attribute.
     * @param value the integer value.
     *
     * @return a new Attribute.
     */
    inline Attribute attribute(const std::string& name, int value)
    { return Attribute(name, value::IntegerFactory::create(value)); }

    /**
     * Build an attribute with a specified name and double value.
     *
     * @param name the name of the attribute.
     * @param value the double value.
     *
     * @return a new Attribute.
     */
    inline Attribute attribute(const std::string& name, double value)
    { return Attribute(name, value::DoubleFactory::create(value)); }

    /**
     * Build an attribute with a specified name and boolean value.
     *
     * @param name the name of the attribute.
     * @param value the boolean value.
     *
     * @return a new Attribute.
     */
    inline Attribute attribute(const std::string& name, bool value)
    { return Attribute(name, value::BooleanFactory::create(value)); }
    
    /**
     * Build an attribute with a specified name and string value.
     *
     * @param name the name of the attribute.
     * @param value the string value.
     *
     * @return a new Attribute.
     */
    inline Attribute attribute(const std::string& name,
                               const std::string& value)
    { return Attribute(name, value::StringFactory::create(value)); }

    /** 
     * @brief Build an attribute with a specified name and string value.
     * 
     * @param name the name of the attribute.
     * @param value the string value.
     * 
     * @return a new Attribute.
     */
    inline Attribute attribute(const std::string& name,
                               const char* value)
    { return attribute(name,
                       value::StringFactory::create(std::string(value))); }
    
    /**
     * Build an attribute with a specified name and set value. Be carreful, the
     * set is not clone.
     *
     * @param name the name of the attribute.
     * @param value the value. The value is not cloned.
     *
     * @return a new Attribute.
     */
    inline Attribute attribute(const std::string& name,
                               value::Value value)
    { return Attribute(name, value); }

    /**
     * Build an attribute with a specified name and set value. Be carreful, the
     * set is not clone.
     *
     * @param name the name of the attribute.
     * @param value the value. The value is not cloned.
     *
     * @return a new Attribute.
     */
    inline Attribute attribute(const std::string& name,
                               value::Set value)
    { return Attribute(name, value); }

}} // namespace vle devs
    
#endif
