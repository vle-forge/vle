/**
 * @file vle/utils/Rand.hpp
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


#ifndef UTILS_RAND_HPP
#define UTILS_RAND_HPP

#include <cmath>
#include <limits>
#include <glibmm/random.h>

namespace vle { namespace utils {

    /**
     * @brief Random Numbers - pseudo-random number generator based on
     * Glib::Rand ie. Mersene Twister. The Glib::Rand provides functions:
     * @code
     * // return true or false
     * bool get_bool();
     * // return a number between [0..2^32-1]
     * guint32 get_int();
     * // return a number between [begin..end-1]
     * gint32  get_int_range(gint32 begin, gint32 end);
     * // return a real [0..1)
     * double get_double();
     * // return a real [begin..end)
     * double get_double_range(double begin, double end);
     * @endcode
     *
     * For instance, to develop a DEVS atomic model:
     * @code
     * // in a devs::Dynamics model:
     * double d = rand().get_double();
     * int i = rand().get_int_range(0, 123);
     * @endcode
     * 
     */
    class Rand
    {
    public:
        /** 
         * @brief Create a new random number generator initializecd with a seed
         * takne either from /dev/urandom if exist or from the current time.
         */
        Rand() { }

        /** 
         * @brief Set the seed for the random number generator.
         * @param seed a value to reinitialize the random number generator.
         */
        void set_seed(guint32 seed)
        {
            m_rand.set_seed(seed);
        }

        /** 
         * @brief Generate a boolean value [true, false].
         * @return true of false.
         */
        inline bool get_bool()
        {
            return m_rand.get_bool();
        }

        /** 
         * @brief Gererate an unsigned int value [0..2^32-1].
         * @return a unsigned int
         */
        inline guint32 get_int()
        {
            return m_rand.get_int();
        }

        /** 
         * @brief Generate an integer from begin to end [begin..end-1].
         * @param begin The minimum value include.
         * @param end The limit (exclude) of the range.
         * @return an integer.
         */
        inline gint32  get_int_range(gint32 begin, gint32 end)
        {
            return m_rand.get_int_range(begin, end);
        }

        /** 
         * @brief Generate a real between 0 and 1 [0..1).
         * @return a real.
         */
        inline double get_double()
        {
            return m_rand.get_double();
        }

        /** 
         * @brief Generate a real between begin to end [begin..end).
         * @param begin The minimum value.
         * @param end The limit (exclude) of the range.
         * @return a real.
         */
        inline double get_double_range(double begin, double end)
        {
            return m_rand.get_double_range(begin, end);
        }

        /**
         * @brief Generate a real between 0 and 1 [0..1].
         * @return a real.
         */
	inline double get_double_included()
	{
            return (double) get_int() /
                (double) (std::numeric_limits < guint32 >::max() - 1.0);
	}

        /** 
         * @brief Generate a real between begin and end [begin..end].
         * @param begin The minimum value include.
         * @param end The maximum value include.
         * @return a real.
         */
	inline double get_double_included(double begin, double end)
	{
	    return get_double_included() * std::abs(end - begin) + begin;
	}

        /** 
         * @brief Generate a real between begin and end (begin..end).
         * @param begin The minimum value exclude.
         * @param end The maximum value exclude.
         * @return a real.
         */
        inline double get_double_range_excluded(double begin, double end)
        {
            double random;
            while (begin == (random = get_double_range(begin, end))) {}
            return random;
        }

        /** 
         * @brief Generate an integer between begin and end (begin..end).
         * @param begin The minimun value exclude.
         * @param end The maximum value exclude.
         * @return a integer.
         */
        inline gint32 get_int_range_excluded(gint32 begin, gint32 end)
        {
            gint32 random;
            while (begin == (random = get_int_range(begin, end))) {}
            return random;
        }

        ////
        //// Specific random generator method
        ////

	/**
	 * @brief Generate a real using the Normal law.
	 * @param standarddeviation
	 * @param average
	 * @return a real.
	 */
	double normal(double standarddeviation, double average);

	/**
	 * @brief Generate a real using the Log Normal law.
	 * @param standarddeviation
	 * @param average
	 * @return a real.
	 */
	double log_normal(double standarddeviation, double average);

	/**
         * @brief Generate a random number between 0 and 2*PI using the von
         * Mises law [0..2*PI]. mu is the mean mu is the mean angle, expressed
         * in radians between 0 and 2*pi, and kappa is the concentration
         * parameter, which must be greater than or equal to zero. If kappa is
         * equal to zero, this distribution reduces to a uniform random angle
         * over the range 0 to 2*pi.angle, expressed in radians between 0 and
         * 2*pi, and kappa is the concentration parameter, which must be greater
         * than or equal to zero.  If kappa is equal to zero, this distribution
         * reduces to a uniform random angle over the range 0 to 2*pi.
	 * @param kappa
	 * @param mu
	 * @return a random number [0;2PI]
	 */
	double von_mises(double kappa, double mu);

    private:
        Glib::Rand  m_rand;
    };

}} // namespace vle utils

#endif
