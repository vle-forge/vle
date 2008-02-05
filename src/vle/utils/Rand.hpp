/**
 * @file src/vle/utils/Rand.hpp
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
#include <glibmm/random.h>

namespace vle { namespace utils {

    /**
     * @brief Random Numbers - pseudo-random number generator based on
     * Glib::Rand ie. Mersene Twister.
     *
     * @code
     * double d = utils::Rand::rand().get_double();
     * int i = utils::Rand::rand().get_int_range(0, 123);
     * @endcode
     * 
     */
    class Rand : public Glib::Rand
    {
    public:
        Rand();

        ~Rand();

        /**
         * Returns a random double uniformaly distributed over the range
         * [0..1].
	 *
         * @return A random number.
         */
	inline double get_double_included()
	{
	    return get_int()/(G_MAXUINT32-1.0);   
	}

        /**
         * Returns a random double uniformaly distributed over the range
         * [begin..end].
	 *
         * @param begin lower closed bound of the interval.
         * @param end upper closed bound of the interval.
	 *
         * @return A random number.
         */
	inline double get_double_included(double begin, double end)
	{
	    return get_double_included() * std::abs(end - begin) + begin;
	}

        /**
         * Returns a random double equally distributed over the range
         * ]begin..end[.
	 *
         * @param begin lower open bound of the interval.
         * @param end upper open bound of the interval.
	 *
         * @return A random number.
         */
        inline double get_double_range_excluded(double begin, double end)
        {
            double random;
            while (begin == (random = get_double_range(begin, end)));
            return random;
        }

        /**
         * Returns a random gint32 equally distributed over the range
         * ]begin..end[.
	 *
         * @param begin lower open bound of the interval.
         * @param end upper open bound of the interval.
	 *
         * @return A random number.
         */
        inline gint32 get_int_range_excluded(gint32 begin, gint32 end)
        {
            gint32 random;
            while (begin == (random = get_int_range(begin, end)));
            return random;
        }

        /**
         * Return a reference to the unique Random generator. Show Glib::Rand
         * class for more documnetation.
	 *
         * @return A reference to a Rand object.
         */
        inline static Rand& rand()
        {
            return (mRand) ? *mRand : *(mRand = new Rand);
        }

        /**
         * Destroy unique Random generator if it exist.
	 *
         */
        inline static void kill()
        {
            if (mRand) {
                delete mRand;
                mRand = 0;
            }
        }

	/**
	 * Return a random number using Normal law.
	 *
	 * @param standarddeviation
	 * @param average
	 *
	 * @return a random number.
	 */
	double normal(double standarddeviation, double average);

	/**
	 * Return a random number using the Log Normal law.
	 *
	 * @param standarddeviation
	 * @param average
	 *
	 * @return a random number.
	 */
	double log_normal(double standarddeviation, double average);

	/**
	 * Return a random number between 0 and 2*PI using the von Mises law.
	 *
	 * mu is the mean mu is the mean angle, expressed in radians between 
	 * 0 and 2*pi, and kappa is the concentration parameter, which must 
	 * be greater than or equal to zero. 
	 * If kappa is equal to zero, this distribution reduces to a uniform
	 * random angle over the range 0 to 2*pi.angle, expressed in radians 
	 * between 0 and 2*pi, and kappa is the concentration parameter, which
	 * must be greater than or equal to zero.  If kappa is equal to zero,
	 * this distribution reduces to a uniform random angle over the range
	 * 0 to 2*pi.
	 *
	 * @param kappa
	 * @param mu
	 *
	 * @return a random number [0;2PI]
	 */
	double von_mises(double kappa, double mu);
    private:
        static Rand* mRand;
    };

}} // namespace vle utils

#endif
