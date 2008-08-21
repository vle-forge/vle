/**
 * @file vle/utils/Rand.cpp
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


#include <cmath>
#include <glib/gtypes.h>
#include <vle/utils/Rand.hpp>



namespace vle { namespace utils {

double Rand::normal(double standarddeviation, double average)
{
    double x, y;

    y = get_double_range_excluded(0.0, 1.0);

    x = std::sqrt(-2.0 * std::log(y)) *
        std::sin(G_PI_2) * get_double() *
	standarddeviation + average;

    return get_bool() ? 2 * average - x : x;
}

double Rand::log_normal(double standarddeviation, double average)
{
    double s, x, y;

    x = 0.0;
    y = std::log(((standarddeviation * standarddeviation) /
		  (average * average)) + 1.0);
    s = sqrt(y);

    for (int i=1; i<=12; i++)
	x += get_int();

    return std::exp(std::log(average) - 1.0 / 2 * y + s * ( x - 6.0));
}

double Rand::von_mises(const double kappa, const double mu)
{
    if (kappa <= 1e-6){
	return 2.*G_PI * get_double_included(); 
    }

    double a = 1.0 + sqrt(1.0 + 4.0 * kappa * kappa);
    double b = (a  - sqrt(2.0 * a))/(2.0 * kappa);
    double r = (1.0 + b * b)/(2.0 * b);
    double f;

    for (;;) {
        double u1 = get_double_included();
        double z = cos(G_PI * u1);

        f = (1.0 + r * z)/(r + z);
        double c = kappa * (r - f);
        double u2 = get_double_included();

        if (not (u2 >= c * (2.0 - c) and u2 > c * exp(1.0 - c)))
            break;
    }
    double u3 = get_double_included();

    return (u3 > 0.5) ? mu + acos(f) : mu - acos(f);
}

}} // namespace vle utils
