/**
 * @file examples/random/NormalGenerator.cpp
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


#include <NormalGenerator.hpp>
#include <vle/utils.hpp>
#include <glib/gtypes.h>
#include <cmath>

namespace vle { namespace examples { namespace generator {

double NormalGenerator::generate()
{
    double x, y, z;

    y = m_rand.get_double_range_excluded(0.0, 1.0);

    x = std::sqrt(-2.0 * std::log(y)) *
        std::sin(G_PI_2) * m_rand.get_double() *
        m_standardDeviation + m_average;

    z = m_rand.get_bool() ? 2 * m_average - x : x;
    return z;
}

}}} // namespace vle examples generator
