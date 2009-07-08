/**
 * @file vle/utils/DateTime.hpp
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


#ifndef VLE_UTILS_DATETIME_HPP
#define VLE_UTILS_DATETIME_HPP

#include <boost/date_time.hpp>

namespace vle { namespace utils {

    struct DateTime
    {
        enum Unit { None, Day, Week, Month, Year };

        static unsigned int year(const double& time);
        static unsigned int month(const double& time);
        static unsigned int dayOfMonth(const double& time);
        static unsigned int dayOfWeek(const double& time);
        static unsigned int dayOfYear(const double& time);
        static unsigned int weekOfYear(const double& time);

        static bool isLeapYear(const double& time);

        static double aYear(const double& time);
        static double aMonth(const double& time);

        static inline double aWeek()
        {
            return 7;
        }

        static inline double aDay()
        {
            return 1;
        }

        static double years(const double& time, unsigned int n);
        static double months(const double& time, unsigned int n);
        static inline double weeks(unsigned int n)
        {
            return (int)(7 * n);
        }

        static inline double days(unsigned int n)
        {
            return (int)n;
        }

        static DateTime::Unit convertUnit(const std::string& unit);
        static double duration(const double& time, double duration, DateTime::Unit unit);
        static std::string toJulianDayNumber(unsigned long date);
        static long toJulianDayNumber(const std::string& date);
        static std::string toJulianDay(double date);
        static double toJulianDay(const std::string& date);

};

}} // namespace vle utils

#endif
