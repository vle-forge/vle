/**
 * @file src/examples/examples/random/LogNormalGenerator.hpp
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


#ifndef EXAMPLES_RANDOM_LOG_NORMAL_GENERATOR_HPP
#define EXAMPLES_RANDOM_LOG_NORMAL_GENERATOR_HPP

#include <RandomGenerator.hpp>

namespace examples { 

    class LogNormalGenerator : public RandomGenerator
    {
    public:
        LogNormalGenerator(vle::utils::Rand& rnd,
                           double average,
                           double standardDeviation) :
            RandomGenerator(rnd),
            m_average(average),
            m_standardDeviation(standardDeviation) { }

        virtual ~LogNormalGenerator() { }

        virtual double generate();

    private:
        double m_average;
        double m_standardDeviation;
    };

}

#endif
