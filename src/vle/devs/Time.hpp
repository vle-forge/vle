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

#ifndef VLE_DEVS_TIME_HPP
#define VLE_DEVS_TIME_HPP

#include <cmath> /* for isnan */
#include <limits>
#include <string>
#include <vle/DllDefines.hpp>

namespace vle {
namespace devs {

/**
 * @e Time represents the definition of the simulation time.
 *
 * The @e Time is used internally in the coordinator scheduller and in
 * the user's API in all the function of the @e Dynamics or @e
 * Executive classes.
 *
 * The infinity is represented by the constant HUGE_VAL and the
 * negative infinity by -HUGE_VAL. HUGE_VAL is the largest
 * representable double value. This value is returned by many run-time
 * math functions when an error occurs.
 */
typedef double Time;

static_assert(std::numeric_limits<Time>::has_quiet_NaN == true,
              "Use an iec559 standard OS/Compiler");

static const Time infinity = std::numeric_limits<Time>::infinity();
static const Time negativeInfinity =
  -1 * std::numeric_limits<Time>::infinity();
static const Time nan = std::numeric_limits<double>::quiet_NaN();

/**
 * Test if the @e time is Nan.
 *
 * @param time The double to check.
 *
 * @return true if @e time is NaN.
 */
inline static bool
isNan(Time time)
{
    return std::isnan(time);
}

/**
 * Test if the @e Time is infinity or -infinity.
 *
 * @param time The double to check.
 *
 * @return true if @e time is infinity or -infinity.
 */
inline static bool
isInfinity(Time time)
{
    return time == infinity or time == negativeInfinity;
}

/**
 * Test if the @e Time is infinity.
 *
 * @param time The double to check.
 *
 * @return true if @e time is infinity.
 */
inline static bool
isPositiveInfinity(Time time)
{
    return time == infinity;
}

/**
 * Test if the @e Time is -infinity.
 *
 * @param time The double to check.
 *
 * @return true if @e time is -infinity.
 */
inline static bool
isNegativeInfinity(Time time)
{
    return time == negativeInfinity;
}

/**
 * Transform the @e Time time into @e an std::string.
 *
 * If @e Time equals infinity, the toString function returns "+infinity",
 * if @e time equals negative infinity, the toString fuction
 * return "-infinity" else the string representation of the double
 * time is returned using the C locale.
 *
 * @param time The @e time to convert.
 *
 * @return An @e std::string representation of the @e Time time.
 */
VLE_API std::string convertTimeToString(Time time);
}
} // namespace vle devs

#endif
