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

#ifndef VLE_UTILS_DATETIME_HPP
#define VLE_UTILS_DATETIME_HPP

#include <string>
#include <vle/DllDefines.hpp>

namespace vle {
namespace utils {

enum DateTimeUnitOptions
{
    DATE_TIME_UNIT_NONE,
    DATE_TIME_UNIT_DAY,
    DATE_TIME_UNIT_WEEK,
    DATE_TIME_UNIT_MONTH,
    DATE_TIME_UNIT_YEAR
};

class VLE_API DateTime
{
public:
    /**
     * @brief Write the current date and time conform to RFC 822.
     * @code
     * std::cout << "currentDate(): `"
     *           << vle::utils::DateTime::currentDate()
     *           << "'\n";
     *
     * // Display: currentDate(): `2011-Jun-09 12:13:21'
     * @endcode
     * @return string representation of date.
     */
    static std::string currentDate();

    /* * * * */

    /**
     * @brief Get the year in the simulation time.
     * @code
     * vle::utils::DateTime::year(2451545) == 2000u;
     * @endcode
     * @param time The simulation time.
     * @return An unsigned int.
     */
    static unsigned int year(double time);

    /**
     * @brief Get the month in the simulation time.
     * @code
     * vle::utils::DateTime::month(2451545) == 1u;
     * @endcode
     * @param time The simulation time.
     * @return An unsigned int.
     */
    static unsigned int month(double time);

    /**
     * @brief Get the day of the month in the simulation time.
     * @code
     * vle::utils::DateTime::dayOfMonth((2451545)) == 1u;
     * @endcode
     * @param time The simulation time.
     * @return An unsigned int.
     */
    static unsigned int dayOfMonth(double time);

    /**
     * @brief Get the day in the week of the simulation time.
     * @code
     * vle::utils::DateTime::dayOfWeek((2451545)) == 6u;
     * @endcode
     * @param time The simulation time.
     * @return An unsigned int.
     */
    static unsigned int dayOfWeek(double time);

    /**
     * @brief Get the day in the year of the simulation time.
     * @code
     * vle::utils::DateTime::dayOfYear((2451545)) == 1u;
     * @endcode
     * @param time The simulation time.
     * @return An unsigned int.
     */
    static unsigned int dayOfYear(double time);

    /**
     * @brief Get the week in the year of the simulation time.
     * @code
     * vle::utils::DateTime::dayOfYear((2451545)) == 1u;
     * @endcode
     * @param time The simulation time.
     * @return An unsigned int.
     */
    static unsigned int weekOfYear(double time);

    /**
     * @brief Check if the simulation time is a leap year.
     * @param time The simulation time.
     * @return true if time is a leap year, false otherwise.
     */
    static bool isLeapYear(double time);

    /**
     * @brief Get the number of day in the year for the simulaton time.
     * @code
     * vle::utils::Datime::aYear(2451545) == 366;
     * @endcode
     * @param time The simulation time.
     * @return number of day.
     */
    static double aYear(double time);

    /**
     * @brief Get the number of day in the month for the simulation time.
     * @code
     * vle::utils::Datime::aMonth(2451545) == 31;
     * vle::utils::Datime::aMonth(2451576) == 29;
     * @endcode
     * @param time The simulation time.
     * @return number of day.
     */
    static double aMonth(double time);

    /**
     * @brief Get the number of day in a week.
     * @return Return 7.
     */
    static inline double aWeek()
    {
        return 7;
    }

    /**
     * @brief Get the number of day in a day.
     * @return Return 1.
     */
    static inline double aDay()
    {
        return 1;
    }

    /**
     * @brief Get number of days in n-years from the simulation time.
     * @code
     * vle::utils::DateTime::years((2451545), 1), 366);
     * vle::utils::DateTime::years((2451545), 2), 731);
     * @endcode
     * @param time The simulation time.
     * @param n The number of years.
     * @return The number of days in n-years.
     */
    static double years(double time, unsigned int n);

    /**
     * @brief Get number of days in n-months from the simulation time.
     * @code
     * vle::utils::DateTime::months((2451545), 2) = 60;
     * @endcode
     * @param time The simulation time.
     * @param n The number of weeks.
     * @return The number of days in n-months.
     */
    static double months(double time, unsigned int n);

    /**
     * @brief Get number of days in n-weeks.
     * @param n Number of weeks.
     * @return n * 7.
     */
    static inline double weeks(unsigned int n)
    {
        return (int)(7 * n);
    }

    /**
     * @brief Get number of days in n-days.
     * @param n Number of days.
     * @return n.
     */
    static inline double days(unsigned int n)
    {
        return (int)n;
    }

    /**
     * @brief Convert std::string unit ("day", "week", "month", "year") into
     * the DateTime::Unit type.
     * @param unit The std::string unit to convert.
     * @return The convertion of Day if error.
     */
    static DateTimeUnitOptions convertUnit(const std::string& unit);

    /**
     * @brief A easy function to call days(), weeks(), months() or years()
     * using a DateTime::Unit type.
     * @param time The simulation date (useless for Day, Week).
     * @param duration The number of DateTime::Unit.
     * @param unit The unit.
     * @return A number of day.
     */
    static double duration(double time,
                           double duration,
                           DateTimeUnitOptions unit);

    /* * * * */

    /**
     * @brief Convert an julian day number into a string.
     * @code
     * vle::utils::DateTime::toJulianDayNumber(2452192) = "2001-10-9";
     * @endcode
     * @param date The date to convert.
     * @return A string representation of the julian day.
     */
    static std::string toJulianDayNumber(unsigned long date);

    /**
     * @brief Convert a string into a julian day number;
     * @code
     * vle::utils::DateTime::toJulianDayNumber("2001-10-9") = 2452192;
     * @endcode
     * @param date The date to convert.
     * @return A julian day number.
     */
    static long toJulianDayNumber(const std::string& date);

    /**
     * @brief Convert a julian date into a string.
     * @code
     * vle::utils::DateTime::toJulianDay(2454115.05486)) = "2001-10-9
     * hh:mm:ss";
     * @endcode
     * @param date The date to convert.
     * @return A string representation of the julian day.
     */
    static std::string toJulianDay(double date);

    /**
     * @brief Convert a string into a julian day.
     * @code
     * vle::utils::DateTime::toJulianDay("2001-10-9 hh:mm:ss") = 2454115.05486;
     * @endcode
     * @param date The date to convert.
     * @return A string representation of the julian day.
     */
    static double toJulianDay(const std::string& date);

    /* * * * */

    /**
     * @brief Check if the date is a valid year in gregorian calendard.
     *
     * @param date The date to check.
     *
     * @return True if date is a valid year, false otherwise.
     */
    static bool isValidYear(double date);

    /**
     * @brief Explode the specified date attribute to year, month, day in the
     * month, hours, minutes and seconds.
     *
     * @param date The date to convert.
     * @param year Output parameter to represent year.
     * @param month Output parameter to represent month.
     * @param day Output parameter to represent day in a month (1..31).
     * @param hours Output parameter to represent hours in date.
     * @param minutes Output parameter to represent minutes in date.
     * @param seconds Output parameter to represent seconds in date.
     *
     * @throw utils::ArgError error to convert the date.
     *
     * @return The remainder of the conversion.
     */
    static double toTime(double date,
                         long& year,
                         long& month,
                         long& day,
                         long& hours,
                         long& minutes,
                         long& seconds);

    /**
     * @brief Explode current date to year, month, day in the
     * month, hours, minutes and seconds.
     *
     * @param year Output parameter to represent year.
     * @param month Output parameter to represent month.
     * @param day Output parameter to represent day in a month (1..31).
     *
     */
    static void currentDate(long& year, long& month, long& day);
};
}
} // namespace vle utils

#endif
