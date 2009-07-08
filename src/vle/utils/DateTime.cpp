/**
 * @file vle/utils/DateTime.cpp
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


#include <vle/utils/DateTime.hpp>
#include <vle/utils/Exception.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <cmath>

namespace vle { namespace utils {

unsigned int DateTime::year(const double& time)
{
    boost::gregorian::date d(
	boost::numeric_cast <
	boost::gregorian::date::date_int_type >(time));

    return d.year();
}

unsigned int DateTime::month(const double& time)
{
    boost::gregorian::date d(
	boost::numeric_cast <
	boost::gregorian::date::date_int_type >(time));

    return d.month();
}

unsigned int DateTime::dayOfMonth(const double& time)
{
    boost::gregorian::date d(
	boost::numeric_cast <
	boost::gregorian::date::date_int_type >(time));

    return d.day();
}

unsigned int DateTime::dayOfWeek(const double& time)
{
    boost::gregorian::date d(
	boost::numeric_cast <
	boost::gregorian::date::date_int_type >(time));

    return d.day_of_week();
}

unsigned int DateTime::dayOfYear(const double& time)
{
    boost::gregorian::date d(
	boost::numeric_cast <
	boost::gregorian::date::date_int_type >(time));

    return d.day_of_year();
}

unsigned int DateTime::weekOfYear(const double& time)
{
    boost::gregorian::date d(
	boost::numeric_cast <
	boost::gregorian::date::date_int_type >(time));

    return d.week_number();
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

bool DateTime::isLeapYear(const double& time)
{
    return boost::gregorian::gregorian_calendar::is_leap_year(year(time));
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

double DateTime::aYear(const double& time)
{
    return isLeapYear(time) ? 366 : 365;
}

double DateTime::aMonth(const double& time)
{
    boost::gregorian::date d(
	boost::numeric_cast <
	boost::gregorian::date::date_int_type >(time));
    boost::gregorian::months duration(1);

    return (int)((d + duration).julian_day() - d.julian_day());
}

double DateTime::years(const double& time, unsigned int n)
{
    boost::gregorian::date d(
	boost::numeric_cast <
	boost::gregorian::date::date_int_type >(time));
    boost::gregorian::years duration(n);

    return (int)((d + duration).julian_day() - d.julian_day());
}

double DateTime::months(const double& time, unsigned int n)
{
    boost::gregorian::date d(
	boost::numeric_cast <
	boost::gregorian::date::date_int_type >(time));
    boost::gregorian::months duration(n);

    return (int)((d + duration).julian_day() - d.julian_day());
}

DateTime::Unit DateTime::convertUnit(const std::string& unit)
{
    if (unit == "day") return Day;
    if (unit == "week") return Week;
    if (unit == "month") return Month;
    if (unit == "year") return Year;
    return Day;
}

double DateTime::duration(const double& time, double duration, Unit unit)
{
    switch (unit) {
    case None:
    case Day: return days(duration);
    case Week: return weeks(duration);
    case Month: return months(time, duration);
    case Year: return years(time, duration);
    }
    return 0;
}

std::string DateTime::toJulianDayNumber(unsigned long date)
{
    boost::gregorian::date dmin(boost::gregorian::min_date_time);
    boost::gregorian::days d(date - dmin.julian_day());

    boost::gregorian::date result(dmin);
    result = result + d;

    return boost::gregorian::to_iso_extended_string(result);
}

long DateTime::toJulianDayNumber(const std::string& date)
{
    boost::gregorian::date d;

    try {
	d = boost::gregorian::from_simple_string(date);
	return d.julian_day();
    } catch (...) {
	try {
	    d = boost::gregorian::from_undelimited_string(date);
	    return d.julian_day();
	} catch (...) {
            throw utils::InternalError(fmt(
                    _("Date time error: error to convert '%1%' into julian"
                      " day number")) % date);
	}
    }

    return -1.0;
}

std::string DateTime::toJulianDay(double date)
{
    double f, e;
    f = std::modf(date, &e);

    f *= 24.0;
    long hours = std::floor(f);
    f -= hours;

    f *= 60.0;
    long minutes = std::floor(f);
    f -= minutes;

    f *= 60.0;
    long seconds = std::floor(f);
    f -= seconds;

    boost::posix_time::time_duration td(hours, minutes, seconds, f);
    boost::posix_time::ptime d(boost::gregorian::date(e), td);
    return boost::posix_time::to_simple_string(d);
}

double DateTime::toJulianDay(const std::string& date)
{
    boost::posix_time::ptime d;

    try {
	d = boost::posix_time::time_from_string(date);
	switch (d.time_of_day().resolution()) {
	case boost::date_time::milli:
	    return d.date().julian_day() +
		d.time_of_day().total_milliseconds() / 86400000.0;
	case boost::date_time::micro:
	    return d.date().julian_day() +
		d.time_of_day().total_microseconds() / 86400000000.0;
	case boost::date_time::nano:
	    return d.date().julian_day() +
		d.time_of_day().total_nanoseconds() / 86400000000000.0;
	default:
	    return d.date().julian_day() +
		d.time_of_day().total_seconds() / 86400.0;
	}
    } catch (...) {
	try {
	    d = boost::posix_time::from_iso_string(date);
	    switch (d.time_of_day().resolution()) {
	    case boost::date_time::milli:
		return d.date().julian_day() +
		    d.time_of_day().total_milliseconds() / 86400000.0;
	    case boost::date_time::micro:
		return d.date().julian_day() +
		    d.time_of_day().total_microseconds() / 86400000000.0;
	    case boost::date_time::nano:
		return d.date().julian_day() +
		    d.time_of_day().total_nanoseconds() / 86400000000000.0;
	    default:
		return d.date().julian_day() +
		    d.time_of_day().total_seconds() / 86400.0;
	    }
	} catch (...) {
            throw utils::InternalError(fmt(
                    _("Date time error: error to convert '%1%' into julian"
                      " day")) % date);
	}
    }
    return -1.0;
}

}} // namespace vle utils
