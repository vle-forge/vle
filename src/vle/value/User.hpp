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

#ifndef VLE_VALUE_USER_HPP
#define VLE_VALUE_USER_HPP 1

#include <vle/DllDefines.hpp>
#include <vle/value/Value.hpp>

namespace vle {
namespace value {

/**
 * @brief A Value to encapsulates user's data type.
 *
 * To provide your own value, you need to:
 * - override the @c vle::value::User class.
 * - provide @c clone, @c writeFile, @c writeString and @c writeXml virtual
 *   functions
 * - provide @c id function to identify your own data
 */
class VLE_API User : public Value
{
public:
    /**
     * @brief Build a User object.
     */
    User()
      : Value()
    {
    }

    /**
     * @brief Copy constructor.
     * @param value The value to copy.
     */
    User(const User& value)
      : Value(value)
    {
    }

    /**
     * @brief Nothing to delete.
     */
    virtual ~User()
    {
    }

    /**
     * @brief To identify your own data.
     */
    virtual size_t id() const = 0;

    ///
    ////
    ///

    /**
     * @brief Get the type of this class.
     * @return Return Value::User.
     */
    virtual Value::type getType() const override
    {
        return Value::USER;
    }
};

inline const User&
toUserValue(std::shared_ptr<Value> value)
{
    return value::reference(value).toUser();
}

inline const User&
toUserValue(std::shared_ptr<const Value> value)
{
    return value::reference(value).toUser();
}

inline const User&
toUserValue(const std::unique_ptr<Value>& value)
{
    return value::reference(value).toUser();
}

inline const User&
toUserValue(const Value& value)
{
    return value.toUser();
}

inline User&
toUserValue(Value& value)
{
    return value.toUser();
}
}
} // namespace vle value

#endif
