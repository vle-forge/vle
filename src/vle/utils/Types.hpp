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

#ifndef VLE_UTILS_TYPES_HPP
#define VLE_UTILS_TYPES_HPP

#include <cstdint>

/*
 * \c Types.hpp defines a list of integer types based on the C99 library
 * wrapped into boost/cstdint.hpp
 */

namespace vle {

using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;

using std::int_fast8_t;
using std::int_fast16_t;
using std::int_fast32_t;
using std::uint_fast8_t;
using std::uint_fast16_t;
using std::uint_fast32_t;

using std::int_least8_t;
using std::int_least16_t;
using std::int_least32_t;
using std::uint_least8_t;
using std::uint_least16_t;
using std::uint_least32_t;

} // namespace vle utils

#endif
