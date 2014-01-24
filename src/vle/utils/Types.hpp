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


#ifndef VLE_UTILS_TYPES_HPP
#define VLE_UTILS_TYPES_HPP

#include <boost/cstdint.hpp>

/*
 * \c Types.hpp defines a list of integer types based on the C99 library wrapped
 * into boost/cstdint.hpp
 */

namespace vle {

    using boost::int8_t;
    using boost::int16_t;
    using boost::int32_t;
    using boost::uint8_t;
    using boost::uint16_t;
    using boost::uint32_t;

    using boost::int_fast8_t;
    using boost::int_fast16_t;
    using boost::int_fast32_t;
    using boost::uint_fast8_t;
    using boost::uint_fast16_t;
    using boost::uint_fast32_t;

    using boost::int_least8_t;
    using boost::int_least16_t;
    using boost::int_least32_t;
    using boost::uint_least8_t;
    using boost::uint_least16_t;
    using boost::uint_least32_t;

} // namespace vle utils

#endif
