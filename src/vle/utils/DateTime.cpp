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

#include <cmath>
#include <ctime>
#include <regex>
#include <sstream>
#include <vle/utils/DateTime.hpp>

namespace vle {
namespace utils {

bool
intern_isLeapYear(long year) noexcept
{
    if (year % 4 != 0)
        return false;

    if (year % 100 != 0)
        return true;

    if (year % 400 != 0)
        return false;

    return true;
}

long
intern_aYear(long year) noexcept
{
    if (intern_isLeapYear(year))
        return 366;

    return 365;
}

long
intern_aMonth(long year, long month) noexcept
{
    switch (month) {
    case 1:
        return 31;
    case 2:
        if (intern_isLeapYear(year))
            return 29;
        return 28;
    case 3:
        return 31;
    case 4:
        return 30;
    case 5:
        return 31;
    case 6:
        return 30;
    case 7:
        return 31;
    case 8:
        return 31;
    case 9:
        return 30;
    case 10:
        return 31;
    case 11:
        return 30;
    case 12:
        return 31;
    default:
        return std::numeric_limits<long>::min();
    }
}

struct intern_date
{
    long myear;
    long mmonth;
    long mday;
    long mhours;
    long mminutes;
    long mseconds;

    enum STR_FORMAT
    {
        extended, //"%Y-%m-%d %H:%M:%S"
        ymd,      //"%Y-%m-%d"
        hms       //"%H:%M:%S"
    };

    intern_date() noexcept : myear(1400),
                             mmonth(1),
                             mday(1),
                             mhours(0),
                             mminutes(0),
                             mseconds(0)
    {
    }

    intern_date(long year, long month, long day, double partofday) noexcept
      : myear(year),
        mmonth(month),
        mday(day),
        mhours(0),
        mminutes(0),
        mseconds(0)
    {
        initPartOfDay(partofday);
    }

    intern_date(const intern_date& d) = default;
    intern_date& operator=(const intern_date& d) = default;
    intern_date(intern_date&& d) = default;
    intern_date& operator=(intern_date&& d) = default;

    void initPartOfDay(double partofday) noexcept // between 0 and 1
    {
        double f = partofday * 24.0;
        mhours = std::floor(f);
        f -= mhours;

        f *= 60.0;
        mminutes = std::floor(f);
        f -= mminutes;

        f *= 60.0;
        mseconds = std::floor(f);
    }

    std::string toString(STR_FORMAT fmt) noexcept
    {
        std::stringstream ss;

        if (fmt != hms) {
            ss << myear << "-";
            if (mmonth < 10) {
                ss << "0";
            }
            ss << mmonth << "-";
            if (mday < 10) {
                ss << "0";
            }
            ss << mday;
        }
        if (fmt == extended) {
            ss << " ";
        }
        if (fmt != ymd) {
            if (mhours < 10) {
                ss << "0";
            }
            ss << mhours << ":";
            if (mminutes < 10) {
                ss << "0";
            }
            ss << mminutes << ":";
            if (mseconds < 10) {
                ss << "0";
            }
            ss << mseconds;
        }

        return ss.str();
    }

    // format : not extended = "%Y-%m-%d"
    //         extended     = "%Y-%m-%d %H:%M:%S"
    // return true if no error
    bool fromString(const std::string& date, STR_FORMAT toparse) noexcept
    {
        bool error = false;

        if (toparse == extended) {
            // parse "%Y-%m-%d %H:%M:%S"
            try {
                std::regex regex("([^\\s]+)\\s([^\\s]+)");
                std::sregex_iterator next(date.begin(), date.end(), regex);
                std::sregex_iterator end;

                if (next != end) {

                    std::smatch match = *next;

                    if (match.size() == 3) {
                        fromString(match[1].str(), ymd);
                        fromString(match[2].str(), hms);
                    } else {
                        error = true;
                    }

                } else {
                    error = true;
                }
            } catch (const std::exception& e) {
                error = true;
            }
        } else {
            // parse "%Y-%m-%d" or "%H:%M:%S"
            unsigned int nbmatches = 0;
            try {
                std::regex regex("[0-9]+|[^0-9]");
                std::sregex_iterator next(date.begin(), date.end(), regex);
                std::sregex_iterator end;
                while (next != end) {
                    std::smatch match = *next;
                    nbmatches++;
                    if (nbmatches == 1) {
                        if (toparse == ymd) {
                            myear = std::stol(match.str());
                        } else {
                            mhours = std::stol(match.str());
                        }
                    } else if (nbmatches == 3) {
                        if (toparse == ymd) {
                            mmonth = std::stol(match.str());
                        } else {
                            mminutes = std::stol(match.str());
                        }
                    } else if (nbmatches == 5) {
                        if (toparse == ymd) {
                            mday = std::stol(match.str());
                        } else {
                            mseconds = std::stol(match.str());
                        }
                    }
                    next++;
                }
            } catch (const std::exception& e) {
                error = true;
            }
            error = error or (nbmatches != 5);
        }
        return error;
    }

    // init from julian day eg: 2454115.05486
    void fromJulianDay(double julianDay) noexcept
    {

        double partofday, J;
        partofday = std::modf(julianDay, &J);

        initPartOfDay(partofday);

        // parameters for gregorian calendar (cf wikipedia)
        int y = 4716;
        int j = 1401;
        int m = 2;
        int n = 12;
        int r = 4;
        int p = 1461;
        int v = 3;
        int u = 5;
        int s = 153;
        int w = 2;
        int B = 274277;
        int C = -38;

        long f = J + j + (((4 * J + B) / 146097) * 3) / 4 + C;
        long e = r * f + v;
        long g = (e % p) / r;
        long h = u * g + w;
        mday = (h % s) / u + 1;
        mmonth = ((h / s + m) % n) + 1;
        myear = (e / p) - y + (n + m - mmonth) / n;
    }

    bool equals(const intern_date& d) const noexcept
    {
        return (myear == d.myear and mmonth == d.mmonth and mday == d.mday);
    }

    bool inf(const intern_date& d) const noexcept
    {
        if (myear < d.myear)
            return true;

        if (myear > d.myear)
            return false;

        if (mmonth < d.mmonth)
            return true;

        if (mmonth > d.mmonth)
            return false;

        return mday < d.mday;
    }

    bool sup(const intern_date& d) const noexcept
    {
        if (myear < d.myear)
            return false;

        if (myear > d.myear)
            return true;

        if (mmonth < d.mmonth)
            return false;

        if (mmonth > d.mmonth)
            return true;

        return mday > d.mday;
    }

    // tells if a date is valid
    bool isValid() const noexcept
    {
        if (1 > mmonth or mmonth > 12)
            return false;

        if (1 > mday or mday > intern_aMonth(myear, mmonth))
            return false;

        if (0 > mhours or mhours > 23)
            return false;

        if (0 > mminutes or mminutes > 60)
            return false;

        if (0 > mseconds or mseconds > 60)
            return false;

        return true;
    }

    // add months to the current date
    void addMonths(unsigned int months) noexcept
    {
        mmonth += months;
        while (mmonth > 12) {
            myear++;
            mmonth -= 12;
            long nbDaysInMonth = intern_aMonth(mmonth, myear);
            if (mday > nbDaysInMonth) {
                mmonth++;
                mday -= nbDaysInMonth;
            }
        }
    }

    // daynNumber as computed in boost gregorian calendar.. (in wikipedia)
    // 12h Jan 1, 4713 BC (-4713-01-BC)
    long julianDayNumber() const noexcept
    {
        unsigned short a = static_cast<unsigned short>((14 - mmonth) / 12);
        unsigned short y = static_cast<unsigned short>(myear + 4800 - a);
        unsigned short m = static_cast<unsigned short>(mmonth + 12 * a - 3);

        return mday + ((153 * m + 2) / 5) + 365 * y + (y / 4) - (y / 100) +
               (y / 400) - 32045;
    }

    double julianDay() const noexcept
    {
        double res = static_cast<double>(julianDayNumber());
        res += mhours / 24.0;
        res += mminutes / 1440.0;
        res += mseconds / 86400.0;
        return res;
    }

    // day of the year (1<>366)
    long dayOfyear() const noexcept
    {
        long ret = 0;

        for (long m = 1; m < mmonth; m++)
            ret += intern_aMonth(myear, m);

        ret += mday;

        return ret;
    }

    // days between a date and end of year (1<>366)
    long daysUntilEndOfyear() const noexcept
    {
        return intern_aYear(myear) - dayOfyear();
    }

    // from boost date-time library
    unsigned short dayOfWeek() noexcept
    {
        unsigned short a = static_cast<unsigned short>((14 - mmonth) / 12);
        unsigned short y = static_cast<unsigned short>(myear - a);
        unsigned short m = static_cast<unsigned short>(mmonth + 12 * a - 2);
        unsigned short d = static_cast<unsigned short>(
          (mday + y + (y / 4) - (y / 100) + (y / 400) + (31 * m) / 12) % 7);

        return d;
    }

    // from boost date-time library
    unsigned short weekOfYear() noexcept
    {
        unsigned long julianbegin = intern_date(myear, 1, 1, 0).julianDay();
        unsigned long juliantoday = julianDay();
        unsigned long day = (julianbegin + 3) % 7;
        unsigned long week = (juliantoday + day - julianbegin + 4) / 7;

        if ((week >= 1) && (week <= 52))
            return week;

        if (week == 53) {
            if ((day == 6) || (day == 5 && intern_isLeapYear(myear))) {
                return week; // under these circumstances week == 53.
            } else {
                return 1; // monday - wednesday is in week 1 of next year
            }
        }
        // if the week is not in current year recalculate using the previous
        // year as the beginning year
        else if (week == 0) {
            julianbegin = intern_date(myear - 1, 1, 1, 0).julianDay();
            day = (julianbegin + 3) % 7;
            week = (juliantoday + day - julianbegin + 4) / 7;
            return week;
        }

        return week; // not reachable -- well except
        // if day == 5 and is_leap_year != true
    }

    // days between a date and end of month (0<>31)
    long idaysUntilEndOfMonth() const noexcept
    {
        return intern_aMonth(myear, mmonth) - mday;
    }

    // nb days between two dates (negative if this is before)
    long daysUntil(const intern_date& d) const noexcept
    {
        if (equals(d))
            return 0;

        if (sup(d))
            return -d.daysUntil(*this);

        return d.julianDay() - julianDay();
    }
};

// output format `2011-Jun-09 12:13:21'
std::string
DateTime::currentDate()
{
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    static const char mon_name[][4] = { "Jan", "Feb", "Mar", "Apr",
                                        "May", "Jun", "Jul", "Aug",
                                        "Sep", "Oct", "Nov", "Dec" };

    char result[26];
    sprintf(result,
            "%d-%.3s-%d %.2d:%.2d:%.2d \n",
            1900 + timeinfo->tm_year,
            mon_name[timeinfo->tm_mon],
            timeinfo->tm_mday,
            timeinfo->tm_hour,
            timeinfo->tm_min,
            timeinfo->tm_sec);

    return result;
}

unsigned int
DateTime::year(double time)
{
    intern_date d;
    d.fromJulianDay(time);
    return d.myear;
}

unsigned int
DateTime::month(double time)
{
    intern_date d;
    d.fromJulianDay(time);
    return d.mmonth;
}

unsigned int
DateTime::dayOfMonth(double time)
{
    intern_date d;
    d.fromJulianDay(time);
    return d.mday;
}

unsigned int
DateTime::dayOfWeek(double time)
{
    intern_date d;
    d.fromJulianDay(time);
    return d.dayOfWeek();
}

unsigned int
DateTime::dayOfYear(double time)
{
    intern_date d;
    d.fromJulianDay(time);
    return d.dayOfyear();
}

unsigned int
DateTime::weekOfYear(double time)
{
    intern_date d;
    d.fromJulianDay(time);
    return d.weekOfYear();
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

bool
DateTime::isLeapYear(double time)
{
    intern_date d;
    d.fromJulianDay(time);
    return intern_isLeapYear(d.myear);
    ;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

double
DateTime::aYear(double time)
{
    return isLeapYear(time) ? 366 : 365;
}

double
DateTime::aMonth(double time)
{
    intern_date d;
    d.fromJulianDay(time);
    return (intern_aMonth(d.myear, d.mmonth));
}

double
DateTime::years(double time, unsigned int n)
{

    intern_date d1;
    d1.fromJulianDay(time);
    intern_date d2(d1);
    d2.myear += n;
    return d2.julianDay() - d1.julianDay();
}

double
DateTime::months(double time, unsigned int n)
{

    intern_date d1;
    d1.fromJulianDay(time);
    intern_date d2(d1);
    d2.addMonths(n);
    return d2.julianDay() - d1.julianDay();
}

DateTimeUnitOptions
DateTime::convertUnit(const std::string& unit)
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

double
DateTime::duration(double time, double duration, DateTimeUnitOptions unit)
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

std::string
DateTime::toJulianDayNumber(unsigned long date)
{
    intern_date d;
    d.fromJulianDay(static_cast<double>(date));
    return d.toString(intern_date::ymd);
}

// parsing "2001-10-9"
long
DateTime::toJulianDayNumber(const std::string& date)
{
    intern_date d;
    d.fromString(date, intern_date::ymd);
    return d.julianDayNumber();
}

std::string
DateTime::toJulianDay(double date)
{
    intern_date d;
    d.fromJulianDay(date);
    return d.toString(intern_date::extended);
}

// parsing "2001-10-9 hh:mm:ss"
// if error, then try parsing "2001-10-9"
double
DateTime::toJulianDay(const std::string& date)
{

    intern_date d;
    bool error = d.fromString(date, intern_date::extended);
    if (error) {
        d.fromString(date, intern_date::ymd);
    }
    return d.julianDay();
}

bool
DateTime::isValidYear(double date)
{
    intern_date d;
    d.fromJulianDay(date);
    bool valid = (1399 < d.myear and d.myear < 10000); // boost rule ?
    return valid;
}

double
DateTime::toTime(double date,
                 long& year,
                 long& month,
                 long& day,
                 long& hours,
                 long& minutes,
                 long& seconds)
{
    intern_date d;
    d.fromJulianDay(date);
    year = d.myear;
    month = d.mmonth;
    day = d.mday;

    hours = d.mhours;
    minutes = d.mminutes;
    seconds = d.mseconds;

    return 0.0;
}

void
DateTime::currentDate(long& year, long& month, long& day)
{

    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    year = timeinfo->tm_year;
    month = timeinfo->tm_mon;
    day = timeinfo->tm_mday;
}
}
} // namespace vle utils
