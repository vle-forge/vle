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


#include <vle/utils/DateTime.hpp>
#include <vle/utils/Exception.hpp>
#include <boost/date_time.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>
#include <cmath>

namespace vle { namespace utils {

std::string DateTime::currentDate()
{
    boost::posix_time::ptime current(
        boost::posix_time::second_clock::local_time());

    std::ostringstream out;
    out << current;

    return out.str();
}

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

DateTimeUnitOptions DateTime::convertUnit(const std::string& unit)
{
    if (unit == "day") {
        return DATE_TIME_UNIT_DAY;
    } else if (unit == "week") {
        return DATE_TIME_UNIT_WEEK;
    } else if (unit == "month") {
        return DATE_TIME_UNIT_MONTH;
    } else if (unit == "year") {
        return DATE_TIME_UNIT_YEAR;
    } else {
        return DATE_TIME_UNIT_DAY;
    }
}

double DateTime::duration(const double& time, double duration,
                          DateTimeUnitOptions unit)
{
    switch (unit) {
    case DATE_TIME_UNIT_NONE:
    case DATE_TIME_UNIT_DAY:
        return days(duration);
    case DATE_TIME_UNIT_WEEK:
        return weeks(duration);
    case DATE_TIME_UNIT_MONTH:
        return months(time, duration);
    case DATE_TIME_UNIT_YEAR:
        return years(time, duration);
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

bool DateTime::isValidYear(const double& date)
{
    namespace bg = boost::gregorian;

    try {
        bg::date mindate(bg::min_date_time);
        bg::date maxdate(bg::max_date_time);
        bg::date current(boost::numeric_cast < bg::date::date_int_type >(date));

        if (not current.is_special()) {
            return mindate < current and current < maxdate;
        } else {
            return false;
        }
    } catch(...) {
        return false;
    }
}

double DateTime::toTime(const double& date, long& year,
                        long& month, long& day,
                        long& hours, long& minutes,
                        long& seconds)
{
    namespace bg = boost::gregorian;

    bg::date d(boost::numeric_cast < bg::date::date_int_type >(date));

    if (not d.is_special()) {
        year = d.year();
        month = d.month();
        day = d.day();

        double f, e;
        f = std::modf(date, &e);

        f *= 24.0;
        hours = std::floor(f);
        f -= hours;

        f *= 60.0;
        minutes = std::floor(f);
        f -= minutes;

        f *= 60.0;
        seconds = std::floor(f);
        f -= seconds;

        return f;
    } else {
        throw utils::ArgError(fmt(
                _("Can not convert date '%1%' to gregorian calendar")) %
            date);
    }

    return 0.0;
}

void DateTime::currentDate(long& year,
                           long& month,
                           long& day)
{
    namespace bg = boost::gregorian;

    bg::date d(bg::day_clock::local_day());

    year = d.year();
    month = d.month();
    day = d.day();
}
}} // namespace vle utils
