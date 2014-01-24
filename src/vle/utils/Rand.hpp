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


#ifndef VLE_UTILS_RAND_HPP
#define VLE_UTILS_RAND_HPP

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/lognormal_distribution.hpp>
#include <boost/random/exponential_distribution.hpp>
#include <boost/random/poisson_distribution.hpp>
#include <boost/random/gamma_distribution.hpp>
#include <boost/random/binomial_distribution.hpp>
#include <boost/random/geometric_distribution.hpp>
#include <boost/random/cauchy_distribution.hpp>
#include <boost/random/triangle_distribution.hpp>
#include <vle/DllDefines.hpp>

namespace vle { namespace utils {

    /**
     * @brief vle::utils::Rand is a pseudo-random number generator based on
     * boost random packages. vle::utils::Rand uses the mersene_twister PRNG.
     *
     * @note "Mersenne Twister: A 623-dimensionally equidistributed uniform
     * pseudo-random number generator", Makoto Matsumoto and Takuji Nishimura,
     * ACM Transactions on Modeling and Computer Simulation: Special Issue on
     * Uniform Random Number Generation, Vol. 8, No. 1, January 1998, pp. 3-30.
     *
     * @code
     * vle::utils::Rand r;
     * r.seed((uint32_t)12345);
     * r.getInt(); // uint32_t [0, 2^32-1]
     * r.getInt(1, 10); // int [1, 10]
     * r.getBool(); // bool [true, false]
     * r.getDouble(); // double [0.0, 1.0)
     * r.getDouble(0.0, 1.0); // double [0.0, 1.0)
     * r.getDoubleExcluded(0.0, 1.0); // double (0.0, 1.0)
     * r.normal(1.0, 0.0);
     * r.lognormal(1.0, 0.0);
     * r.poisson(1.0);
     * r.gamma(1.0);
     * r.binomial(1, 0.5);
     * r.geometric(0.5);
     * r.cauchy(0.0, 1.0);
     * r.triangle(0.0, 0.5, 1.0);
     * r.weibull(1.0, 1.0);
     * @endcode
     */
    class VLE_API Rand
    {
    public:
        typedef boost::mt19937::result_type result_type;

        /**
         * @brief Create a new PRNG mersene twister initializecd with a seed
         * equal to 5489.
         */
        Rand() {}

        /**
         * @brief Create a new PRNG mersene twister initialized with a seed
         * provide by parameters.
         * @param seed The seed to assign to the PRNG.
         */
        explicit Rand(result_type seed) :
            m_rand(seed)
        {}

        /**
         * @brief Set the seed for the random number generator.
         * @param seed a value to reinitialize the random number generator.
         */
        void seed(result_type seed)
        { m_rand.seed(seed); }

        /**
         * @brief Generate a boolean value [true, false] using the Bernoulli
         * distribition where p = 0.5. (P(true) = p, P(false) = 1 - p).
         * @return a random true or false boolean.
         */
        inline bool getBool()
        {
            boost::bernoulli_distribution < > distrib(0.5);
            boost::variate_generator < boost::mt19937&,
                boost::bernoulli_distribution < > >gen(m_rand, distrib);

            return gen();
        }

        /**
         * @brief Gererate an unsigned int value [0..2^32-1].
         * @return a random unsigned int
         */
        inline result_type getInt()
        {
            return m_rand();
        }

        /**
         * @brief Generate an integer from begin to end [begin..end].
         * @param begin The minimum value include.
         * @param end The limit of the range.
         * @return a random integer.
         */
        inline int getInt(int begin, int end)
        {
            boost::uniform_int < > distrib(begin, end);
            boost::variate_generator < boost::mt19937&,
                boost::uniform_int < > > gen(m_rand, distrib);

            return gen();
        }

        /**
         * @brief Generate a real value [0, 1)
         * @return a random real.
         */
        inline double getDouble()
        {
            boost::uniform_real < > distrib(0.0, 1.0);
            boost::variate_generator < boost::mt19937&,
                boost::uniform_real < > > gen(m_rand, distrib);

            return gen();
        }

        /**
         * @brief Generate a real from begin to end [begin..end).
         * @param begin The minimum value.
         * @param end The limit (exclude) of the range.
         * @return a random real.
         */
        inline double getDouble(double begin, double end)
        {
            boost::uniform_real < > distrib(begin, end);
            boost::variate_generator < boost::mt19937&,
                boost::uniform_real < > > gen(m_rand, distrib);

            return gen();
        }

        /**
         * @brief Generate a real value (0, 1);
         * 0 and 1 are excluded from the generated values.
         * @return a random real.
         */
        double getDoubleExcluded();

        /**
         * @brief Generate a real using the normal law.
         * @param mean
         * @param sigma
         * @return a real.
         */
        double normal(double mean, double sigma)
        {
            boost::normal_distribution < > distrib(mean, sigma);
            boost::variate_generator < boost::mt19937&,
                boost::normal_distribution < > > gen(m_rand, distrib);

            return gen();
        }

        /**
         * @brief Generate a real using the Log Normal law.
         * @param mean
         * @param sigma
         * @return a real.
         */
        double logNormal(double mean, double sigma)
        {
            boost::lognormal_distribution < > distrib(mean, sigma);
            boost::variate_generator < boost::mt19937&,
                boost::lognormal_distribution < > > gen(m_rand, distrib);

            return gen();
        }

        /**
         * @brief Generate a real using an exponential distribution.
         * @param rate
         * @return a real between 0 and infinite.
         */
        double exponential(double rate)
        {
            boost::exponential_distribution < > distrib(rate);
            boost::variate_generator < boost::mt19937&,
                boost::exponential_distribution < > > gen(m_rand, distrib);

            return gen();
        }

        /**
         * @brief Generate a random number between 0 and 2*PI using the von
         * Mises law [0..2*PI]. mu is the mean angle, expressed in radians
         * between 0 and 2*pi, and kappa is the concentration parameter, which
         * must be greater than or equal to zero. If kappa is equal to zero,
         * this distribution reduces to a uniform random angle over the range 0
         * to 2*pi.angle, expressed in radians between 0 and 2*pi, and kappa is
         * the concentration parameter, which must be greater than or equal to
         * zero.  If kappa is equal to zero, this distribution reduces to a
         * uniform random angle over the range 0 to 2*pi.
         * @param kappa
         * @param mu
         * @return a random number [0,2PI]
         */
        double vonMises(double kappa, double mu);

        /**
         * @brief Generate a real using the Poisson distribution.
         * @param mean
         * @return a real.
         */
        double poisson(double mean)
        {
            boost::poisson_distribution < > distrib(mean);
            boost::variate_generator < boost::mt19937&,
                boost::poisson_distribution < > > gen(m_rand, distrib);

            return gen();
        }

        /**
         * @brief Generate a real using the Gamma distribution.
         * @param alpha
         * @return a real.
         */
        double gamma(double alpha)
        {
            boost::gamma_distribution < > distrib(alpha);
            boost::variate_generator < boost::mt19937&,
                boost::gamma_distribution < > > gen(m_rand, distrib);

            return gen();
        }

        /**
         * @brief Generate a real using the Binomial distribution.
         * @param t
         * @param p
         * @return a real.
         */
        double binomial(int t, double p)
        {
            boost::binomial_distribution < > distrib(t, p);
            boost::variate_generator < boost::mt19937&,
                boost::binomial_distribution < > > gen(m_rand, distrib);

            return gen();
        }

        /**
         * @brief Generate a real using the Geometric distribution.
         * @param p
         * @return a real.
         */
        double geometric(double p)
        {
            boost::geometric_distribution < > distrib(p);
            boost::variate_generator < boost::mt19937&,
                boost::geometric_distribution < > > gen(m_rand, distrib);

            return gen();
        }

        /**
         * @brief Generate a real using the Cauchy law.
         * @param median
         * @param sigma
         * @return a real.
         */
        double cauchy(double median, double sigma)
        {
            boost::cauchy_distribution < > distrib(median, sigma);
            boost::variate_generator < boost::mt19937&,
                boost::cauchy_distribution < > > gen(m_rand, distrib);

            return gen();
        }

        /**
         * @brief Generate a real using the triangular law.
         * @param a
         * @param b
         * @param c
         * @return a real.
         */
        double triangle(double a, double b, double c)
        {
            boost::triangle_distribution < > distrib(a, b, c);
            boost::variate_generator < boost::mt19937&,
                boost::triangle_distribution < > > gen(m_rand, distrib);

            return gen();
        }

        /**
         * @brief Generate a real using the Weibull law. This version is a
         * two-parameter Weibull distribution (where c or gamma = 0)
         * @param a continuous shape parameter (beta > 0)
         * @param b continuous scale parameter (alpha > 0)
         * @return a real.
         *
         * @note: Several notations exist where:
         * a = beta = k to represent the shape;
         * b = alpha = lambda to represent the scale.
         */
        double weibull(const double a, const double b);

        /**
         * @brief Generate a real using the Weibull law. This version is a
         * three-parameter Weibull distribution
         * @param a continuous shape parameter (beta > 0)
         * @param b continuous scale parameter (alpha > 0)
         * @param c continuous location parameter (gamma > 0)
         * @return a real.
         *
         * @note: Several notations exist where:
         * a = beta = k to represent the shape;
         * b = alpha = lambda to represent the scale;
         * c = gamma to represent the location.
         */
        double weibull3(const double a, const double b, const double c);

        /**
         * @brief Get a reference to the Mersenne Twister PRNG.
         * @code
         * vle::utils::Rand r(123456789);
         * boost::uniform_real < > d(0., 100.); // [0., 100.)
         * boost::variate_generator < boost::mt19937&,
         *                            boost::uniform_real < > > gen(r, d);
         * std::cout << gen() << "\n";
         *
         * std::vector < uint32_t > vec(1000);
         * vle::utils::generate(vec.begin(), vec.end(), gen);
         * @endcode
         * @return A reference to the PRNG.
         */
        boost::mt19937& gen() { return m_rand; }

    private:
        boost::mt19937  m_rand;
    };

}} // namespace vle utils

#endif
