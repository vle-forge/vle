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


#ifndef DEVS_ATTRIBUTE_HPP
#define DEVS_ATTRIBUTE_HPP

#include <vle/DllDefines.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Set.hpp>
#include <string>

namespace vle { namespace devs {

    /**
     * @brief The Event classes need a pair string value, to construct
     * information. Attribute is this class and define some function to quickly
     * build Attribute.
     */
    typedef std::pair < std::string, value::Value* > Attribute;

    /**
     * Build an attribute with a specified name and integer value.
     *
     * @param name the name of the attribute.
     * @param value the integer value.
     *
     * @return a new Attribute.
     */
    inline Attribute attribute(const std::string& name, int value)
    { return Attribute(name, value::Integer::create(value)); }

    /**
     * Build an attribute with a specified name and double value.
     *
     * @param name the name of the attribute.
     * @param value the double value.
     *
     * @return a new Attribute.
     */
    inline Attribute attribute(const std::string& name, double value)
    { return Attribute(name, value::Double::create(value)); }

    /**
     * Build an attribute with a specified name and boolean value.
     *
     * @param name the name of the attribute.
     * @param value the boolean value.
     *
     * @return a new Attribute.
     */
    inline Attribute attribute(const std::string& name, bool value)
    { return Attribute(name, value::Boolean::create(value)); }

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
    { return Attribute(name, value::String::create(value)); }

    /**
     * @brief Build an attribute with a specified name and string value.
     *
     * @param name the name of the attribute.
     * @param value the string value.
     *
     * @return a new Attribute.
     */
    inline Attribute attribute(const std::string& name, const char* value)
    { return Attribute(name, value::String::create(std::string(value))); }

    /**
     * Build an attribute with a specified name and set value. Be carreful, the
     * set is not clone.
     *
     * @param name the name of the attribute.
     * @param value the value. The value is not cloned.
     *
     * @return a new Attribute.
     */
    inline Attribute attribute(const std::string& name, value::Value* value)
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
    inline Attribute attribute(const std::string& name, value::Set* value)
    { return Attribute(name, value); }

}} // namespace vle devs

#endif
