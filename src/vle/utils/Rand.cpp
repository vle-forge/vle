/**
 * @file Rand.cpp
 * @author The VLE Development Team.
 * @brief Random Numbers - pseudo-random number generator.
 */

/*
 * Copyright (c) 2004, 2005 The vle Development Team
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

#include <cmath>
#include <glib/gtypes.h>
#include <vle/utils/Rand.hpp>



namespace vle { namespace utils {

Rand* Rand::mRand = 0;

Rand::Rand()
{
}

Rand::~Rand()
{
}

double Rand::normal(double standarddeviation, double average)
{
    double x, y;

    y = utils::Rand::rand().get_double_range_excluded(0.0, 1.0);

    x = std::sqrt(-2.0 * std::log(y)) *
	std::sin(G_PI_2) * utils::Rand::rand().get_double() *
	standarddeviation + average;

    return utils::Rand::rand().get_bool() ? 2 * average - x : x;
}

double Rand::log_normal(double standarddeviation, double average)
{
    double s, x, y;

    x = 0.0;
    y = std::log(((standarddeviation * standarddeviation) /
		  (average * average)) + 1.0);
    s = sqrt(y);

    for (int i=1; i<=12; i++)
	x += utils::Rand::rand().get_int();

    return std::exp(std::log(average) - 1.0 / 2 * y + s * ( x - 6.0));
}

}} // namespace vle utils
