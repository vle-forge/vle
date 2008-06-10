/**
 * @file src/examples/random/LogNormalGenerator.cpp
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


#include <LogNormalGenerator.hpp>
#include <vle/utils.hpp>
#include <cmath>

namespace examples { 

double LogNormalGenerator::generate()
{
    double s, x, y, z;

    x = 0.0;
    y = std::log(((m_standardDeviation * m_standardDeviation) / 
                  (m_average * m_average)) + 1.0);
    s = sqrt(y);

    for (int i=1; i<=12; i++)
        x += m_rand.get_int();

    z = std::exp(std::log(m_average) - 1.0 / 2 * y + s * ( x - 6.0));
    return z;
}

}
