/**
 * @file Rand.hpp
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

#ifndef UTILS_RAND_HPP
#define UTILS_RAND_HPP

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
	 * Return a random number using Log Normal law.
	 *
	 * @param standarddeviation
	 * @param average
	 *
	 * @return a random number.
	 */
	double log_normal(double standarddeviation, double average);

    private:
        static Rand* mRand;

        Rand();

        ~Rand();
    };

}} // namespace vle utils

#endif
