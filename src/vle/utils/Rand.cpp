/**
 * @file vle/utils/Rand.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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

double Rand::vonMises(const double kappa, const double mu)
{
    // FIXME: get_double_included instead of getDouble()
    if (kappa <= 1e-6){
	return 2.*G_PI * getDouble();
    }

    double a = 1.0 + sqrt(1.0 + 4.0 * kappa * kappa);
    double b = (a  - sqrt(2.0 * a))/(2.0 * kappa);
    double r = (1.0 + b * b)/(2.0 * b);
    double f;

    for (;;) {
        double u1 = getDouble();
        double z = cos(G_PI * u1);

        f = (1.0 + r * z)/(r + z);
        double c = kappa * (r - f);
        double u2 = getDouble();

        if (not (u2 >= c * (2.0 - c) and u2 > c * exp(1.0 - c)))
            break;
    }
    double u3 = getDouble();

    return (u3 > 0.5) ? mu + acos(f) : mu - acos(f);
}

}} // namespace vle utils
