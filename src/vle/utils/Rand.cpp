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

#include <boost/math/distributions/gamma.hpp>
#include <boost/math/distributions/normal.hpp>
#include <vle/utils/Rand.hpp>

#define _USE_MATH_DEFINES
#include <cmath>
#undef _USE_MATH_DEFINES

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace vle {
namespace utils {

Rand::Rand(result_type seed)
  : m_rand(seed)
{
}

void
Rand::seed(result_type seed)
{
    m_rand.seed(seed);
}

bool
Rand::getBool()
{
    std::bernoulli_distribution distrib(0.5);
    return distrib(m_rand);
}

Rand::result_type
Rand::getInt()
{
    return m_rand();
}

int
Rand::getInt(int begin, int end)
{
    std::uniform_int_distribution<int> distrib(begin, end);
    return distrib(m_rand);
}

double
Rand::getDouble()
{
    std::uniform_real_distribution<double> distrib(0.0, 1.0);
    return distrib(m_rand);
}

double
Rand::getDouble(double begin, double end)
{
    std::uniform_real_distribution<double> distrib(begin, end);
    return distrib(m_rand);
}

double
Rand::normal(double mean, double sigma)
{
    std::normal_distribution<double> distrib(mean, sigma);
    return distrib(m_rand);
}

double
Rand::normal_cdf(double mean, double sigma, double x)
{
    boost::math::normal_distribution<> norm_distr(mean, sigma);
    return boost::math::cdf(norm_distr, x);
}

double
Rand::logNormal(double mean, double sigma)
{
    std::lognormal_distribution<double> distrib(mean, sigma);
    return distrib(m_rand);
}

double
Rand::exponential(double rate)
{
    std::exponential_distribution<double> distrib(rate);
    return distrib(m_rand);
}

double
Rand::poisson(double mean)
{
    std::poisson_distribution<int> distrib(mean);
    return distrib(m_rand);
}

double
Rand::gamma(double alpha)
{
    std::gamma_distribution<double> distrib(alpha);
    return distrib(m_rand);
}

double
Rand::gamma_quantile(double shape, double scale, double x)
{
    boost::math::gamma_distribution<> gamma_distrib(shape, scale);
    return boost::math::quantile(gamma_distrib, x);
}

double
Rand::binomial(int t, double p)
{
    std::binomial_distribution<int> distrib(t, p);
    return distrib(m_rand);
}

double
Rand::geometric(double p)
{
    std::geometric_distribution<int> distrib(p);
    return distrib(m_rand);
}

double
Rand::cauchy(double median, double sigma)
{
    std::cauchy_distribution<double> distrib(median, sigma);
    return distrib(m_rand);
}

double
Rand::vonMises(const double kappa, const double mu)
{
    // FIXME: get_double_included instead of getDouble()
    if (kappa <= 1e-6) {
        return 2. * M_PI * getDouble();
    }

    double a = 1.0 + sqrt(1.0 + 4.0 * kappa * kappa);
    double b = (a - sqrt(2.0 * a)) / (2.0 * kappa);
    double r = (1.0 + b * b) / (2.0 * b);
    double f;

    for (;;) {
        double u1 = getDouble();
        double z = cos(M_PI * u1);

        f = (1.0 + r * z) / (r + z);
        double c = kappa * (r - f);
        double u2 = getDouble();

        if (not(u2 >= c * (2.0 - c) and u2 > c * exp(1.0 - c)))
            break;
    }
    double u3 = getDouble();

    return (u3 > 0.5) ? mu + acos(f) : mu - acos(f);
}

double
Rand::getDoubleExcluded()
{
    double x;
    do {
        x = getDouble();
    } while (x == 0);

    return x;
}

double
Rand::triangle(double a, double b, double c)
{
    double u = getDouble();
    double f = (c - a) / (b - a);

    if (u <= f)
        return a + std::sqrt(u * (b - a) * (c - a));

    return b - std::sqrt((1 - u) * (b - a) * (b - c));
}

double
Rand::weibull(const double a, const double b)
{
    std::weibull_distribution<double> distrib(a, b);
    return distrib(m_rand);
}

double
Rand::weibull3(const double a, const double b, const double c)
{
    double x = pow(-log(getDoubleExcluded()), 1.0 / a);

    return c + b * x;
}
}
} // namespace vle utils
