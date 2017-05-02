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

#include <boost/config.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <vle/utils/Algo.hpp>
#include <vle/utils/Array.hpp>
#include <vle/utils/Context.hpp>
#include <vle/utils/DateTime.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Rand.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/unit-test.hpp>
#include <vle/vle.hpp>

using namespace vle;

struct is_odd
{
    inline bool operator()(const int i) const
    {
        return i % 2;
    }
};

struct cout_map
{
    inline void operator()(int x) const
    {
        std::cout << x << "\n";
    }
};

struct clear_string
{
    inline void operator()(std::string& x) const
    {
        x.clear();
    }
};

struct append_string
{
    std::string str;

    void operator()(const std::string& x)
    {
        str.append(x);
    }
};

void
test_format()
{
    std::string A = vle::utils::format("%d is good!", 1);
    std::string B = vle::utils::format("%d is good!", 1);

    EnsuresEqual(A, B);

    std::string C{ "1 is good!" };
    EnsuresEqual(A, C);

    A[0] = 2;

    Ensures(A != B);

    std::string Big = vle::utils::format("%s %s %s %s %s %s %s %s %s %s\n",
                                         "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
                                         "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
                                         "cccccccccccccccccccccccccccccc",
                                         "dddddddddddddddddddddddddddddd",
                                         "eeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
                                         "ffffffffffffffffffffffffffffff",
                                         "gggggggggggggggggggggggggggggg",
                                         "hhhhhhhhhhhhhhhhhhhhhhhhhhhhhh",
                                         "iiiiiiiiiiiiiiiiiiiiiiiiiiiiii",
                                         "jjjjjjjjjjjjjjjjjjjjjjjjjjjjjj");

    Ensures(Big.size() > 256);
}

void
test_algo()
{
    std::vector<int> b(5);
    std::vector<int> out;
    b[0] = 1;
    b[1] = 2;
    b[2] = 2;
    b[3] = 5;
    b[4] = 6;

    utils::copyIf(b.begin(), b.end(), std::back_inserter(out), is_odd());

    EnsuresEqual(out.size(), (std::vector<int>::size_type)2);
    EnsuresEqual(out[0], 1);
    EnsuresEqual(out[1], 5);

    EnsuresEqual(b.size(), (std::vector<int>::size_type)5);
    EnsuresEqual(b[0], 1);
    EnsuresEqual(b[1], 2);
    EnsuresEqual(b[2], 2);
    EnsuresEqual(b[3], 5);
    EnsuresEqual(b[4], 6);

    std::map<std::string, int> m;
    m["toto"] = 1;
    m["tutu"] = 2;
    m["titi"] = 2;

    std::map<std::string, std::string> n;
    n["tutu"] = "ou";
    n["tati"] = "xu";
    n["tete"] = "re";
    n["tita"] = "nt";

    append_string x = utils::forEach(n.begin(), n.end(), append_string());
    EnsuresEqual(x.str, "xurentou");

    std::map<std::string, std::string>::iterator it;
    it = utils::findIf(
      n.begin(),
      n.end(),
      std::bind1st(std::equal_to<std::string>(), std::string("ou")));
    Ensures(it != n.end());
}

void
test_unary_function()
{
    typedef std::map<std::string, std::string> TwoStrings;
    TwoStrings n;
    n["tutu"] = "ou";
    n["tati"] = "xu";
    n["tete"] = "re";
    n["tita"] = "nt";

    {
        std::ostringstream out;

        std::transform(n.begin(),
                       n.end(),
                       std::ostream_iterator<std::string>(out),
                       utils::select1st<TwoStrings::value_type>());

        EnsuresEqual(out.str(), "tatitetetitatutu");
    }

    {
        std::ostringstream out;

        std::transform(n.begin(),
                       n.end(),
                       std::ostream_iterator<std::string>(out),
                       utils::select2nd<TwoStrings::value_type>());

        EnsuresEqual(out.str(), "xurentou");
    }
}

void
test_generator()
{
    vle::utils::Rand r(123456789);

    r.getBool();
    r.getInt();
    r.getInt(-100, 100);
    r.getDouble();
    r.getDouble(-1.0, 1.0);
}

void
date_time()
{
    std::cout << "Test date_time " << vle::utils::DateTime::currentDate()
              << "\n";

    EnsuresEqual(vle::utils::DateTime::year((2451545)), 2000u);
    EnsuresEqual(vle::utils::DateTime::month((2451545)), 1u);
    EnsuresEqual(vle::utils::DateTime::dayOfMonth((2451545)), 1);
    EnsuresEqual(vle::utils::DateTime::dayOfYear((2451545)), 1);
    EnsuresEqual(vle::utils::DateTime::dayOfYear((2451545 + 31)), 32);
    EnsuresEqual(vle::utils::DateTime::dayOfYear((2451545 + 31 + 31)), 63);
    EnsuresEqual(vle::utils::DateTime::dayOfYear((2451911)), 1);
    EnsuresEqual(vle::utils::DateTime::dayOfYear((2451911 + 31 + 31)), 63);
    EnsuresEqual(vle::utils::DateTime::dayOfYear((2451607)), 63);
    EnsuresEqual(vle::utils::DateTime::dayOfWeek((2451545)), 6);
    EnsuresEqual(vle::utils::DateTime::weekOfYear((2451547)), 1);

    Ensures(vle::utils::DateTime::isLeapYear((2451545)));
    Ensures(not vle::utils::DateTime::isLeapYear((2451911)));

    Ensures(not vle::utils::DateTime::isValidYear((-1)));

    EnsuresEqual(vle::utils::DateTime::aYear((2451545)), 366);
    EnsuresEqual(vle::utils::DateTime::aMonth((2451545)), 31);
    EnsuresEqual(vle::utils::DateTime::aMonth((2451576)), 29);
    EnsuresEqual(vle::utils::DateTime::aMonth((2451942)), 28);
    EnsuresEqual(vle::utils::DateTime::aWeek(), 7);
    EnsuresEqual(vle::utils::DateTime::aDay(), 1);

    EnsuresEqual(vle::utils::DateTime::years((2451545), 1), 366);
    EnsuresEqual(vle::utils::DateTime::months((2451545), 2), 60);
    EnsuresEqual(vle::utils::DateTime::weeks(2), 14);
    EnsuresEqual(vle::utils::DateTime::days(2), 2);

    EnsuresEqual(vle::utils::DateTime::convertUnit("year"),
                 vle::utils::DATE_TIME_UNIT_YEAR);
    EnsuresEqual(vle::utils::DateTime::convertUnit("month"),
                 vle::utils::DATE_TIME_UNIT_MONTH);
    EnsuresEqual(vle::utils::DateTime::convertUnit("week"),
                 vle::utils::DATE_TIME_UNIT_WEEK);
    EnsuresEqual(vle::utils::DateTime::convertUnit("day"),
                 vle::utils::DATE_TIME_UNIT_DAY);

    EnsuresEqual(vle::utils::DateTime::duration(
                   (2451545), 1, vle::utils::DATE_TIME_UNIT_YEAR),
                 366);
    EnsuresEqual(vle::utils::DateTime::duration(
                   (2451545), 1, vle::utils::DATE_TIME_UNIT_MONTH),
                 31);
    EnsuresEqual(vle::utils::DateTime::duration(
                   (2451545), 1, vle::utils::DATE_TIME_UNIT_WEEK),
                 7);
    EnsuresEqual(vle::utils::DateTime::duration(
                   (2451545), 1, vle::utils::DATE_TIME_UNIT_DAY),
                 1);
}

void
julian_date()
{
    std::cout << "\nJulian day number\n";
    EnsuresEqual(2452192,
                 vle::utils::DateTime::toJulianDayNumber("2001-10-9"));

    std::cout << "\nJulian day\n";

    EnsuresEqual("2007-01-14 01:18:59",
                 vle::utils::DateTime::toJulianDay(2454115.05486));

    //
    // note: the value below is not the value above because
    // julianDay(2454115.05486) involves not null milliseconds, etc..
    // and toJulianDay only gives second precision
    //
    EnsuresEqual(2454115.0548495371,
                 vle::utils::DateTime::toJulianDay("2007-01-14 01:18:59"));

    EnsuresEqual(
      vle::utils::DateTime::toJulianDay(2454115.05486),
      vle::utils::DateTime::toJulianDay(vle::utils::DateTime::toJulianDay(
        vle::utils::DateTime::toJulianDay(2454115.05486))));

    EnsuresEqual(
      vle::utils::DateTime::toJulianDay(
        vle::utils::DateTime::toJulianDay(2454115.05486)),
      vle::utils::DateTime::toJulianDay(
        vle::utils::DateTime::toJulianDay(vle::utils::DateTime::toJulianDay(
          vle::utils::DateTime::toJulianDay(2454115.05486)))));
}

void
to_time_function()
{
    long year, month, day, hours, minutes, seconds;
    double date = 2452192.191273148;

    vle::utils::DateTime::toTime(
      date, year, month, day, hours, minutes, seconds);

    EnsuresEqual(year, 2001);
    EnsuresEqual(month, 10);
    EnsuresEqual(day, 9);
    EnsuresEqual(hours, 4);
    EnsuresEqual(minutes, 35);
    EnsuresEqual(seconds, 26);
}

void
localized_conversion()
{
    namespace vu = vle::utils;

    /* convert a C locale real */
    EnsuresApproximatelyEqual(
      vu::convert<double>("123456789"), 123456789., 0.1);
    EnsuresApproximatelyEqual(
      vu::convert<double>("12.3456789"), 12.3456789, 0.1);
    EnsuresApproximatelyEqual(
      vu::convert<double>("12345678.9"), 12345678., 0.1);
    EnsuresApproximatelyEqual(vu::convert<double>("-12345e5"), -12345e5, 0.1);
    EnsuresApproximatelyEqual(vu::convert<double>("12345e5"), 12345e5, 0.1);
    EnsuresApproximatelyEqual(vu::convert<double>("12345."), 12345., 0.1);

    /* convert a fr_FR locale real */
    if (vu::isLocaleAvailable("fr_FR")) {
        EnsuresApproximatelyEqual(
          vu::convert<double>("123 456 789", true, "fr_FR"), 123456789., 0.1);
        EnsuresApproximatelyEqual(
          vu::convert<double>("12,3456789", true, "fr_FR"), 12.3456789, 0.1);
        EnsuresApproximatelyEqual(
          vu::convert<double>("12345678,9", true, "fr_FR"), 12345678., 0.1);
        EnsuresApproximatelyEqual(
          vu::convert<double>("-12345,0e5", true, "fr_FR"), -12345e5, 0.1);
        EnsuresApproximatelyEqual(
          vu::convert<double>("12345,0e5", true, "fr_FR"), 12345e5, 0.1);
        EnsuresApproximatelyEqual(
          vu::convert<double>("12345,", true, "fr_FR"), 12345., 0.1);
    }
}

void
to_scientific_string_function()
{
    namespace vu = vle::utils;

    EnsuresEqual(vu::toScientificString(0.0), "0");
    EnsuresEqual(vu::toScientificString(-1504), "-1504");
    EnsuresEqual(vu::toScientificString(3022), "3022");
#ifdef BOOST_WINDOWS
    EnsuresEqual(vu::toScientificString(123456789123456789.0),
                 "1.23456789123457e+017");
    EnsuresEqual(vu::toScientificString(0.00000000000000000000982),
                 "9.82e-021");
#else
    EnsuresEqual(vu::toScientificString(123456789123456789.0),
                 "1.23456789123457e+17");
    EnsuresEqual(vu::toScientificString(0.00000000000000000000982),
                 "9.82e-21");
#endif
    EnsuresEqual(vu::toScientificString(-0.12345), "-0.12345");
    EnsuresEqual(vu::toScientificString(0.12345), "0.12345");
    EnsuresEqual(vu::toScientificString(0.0001), "0.0001");
    EnsuresEqual(vu::toScientificString(1000.0001), "1000.0001");
}

void
test_format_copy()
{
    namespace vu = vle::utils;

    std::ostringstream out;
    int myints[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    vu::formatCopy(myints, myints + 9, out, ", ", ";", "\"");

    EnsuresEqual(out.str(),
                 "\"1\", \"2\", \"3\", \"4\", \"5\", \"6\", \"7\", "
                 "\"8\", \"9\";");
}

void
test_array()
{
    {
        vle::utils::Array<bool> a(2, 5);

        Ensures(a.size() == 2 * 5);
        Ensures(a.columns() == 2);
        Ensures(a.rows() == 5);

        bool affect = true;
        for (size_t c = 0, end_c = a.columns(); c != end_c; ++c) {
            for (size_t r = 0, end_r = a.rows(); r != end_r; ++r) {
                a(c, r) = affect;
                affect = !affect;
            }
        }

        Ensures(a(0, 0) == true);
        Ensures(a(0, 1) == false);
        Ensures(a(0, 2) == true);
        Ensures(a(0, 3) == false);
        Ensures(a(0, 4) == true);
        Ensures(a(1, 0) == false);
        Ensures(a(1, 1) == true);
        Ensures(a(1, 2) == false);
        Ensures(a(1, 3) == true);
        Ensures(a(1, 4) == false);
    }

    {
        vle::utils::Array<int> a(5, 2, -1);

        Ensures(a.size() == 5 * 2);
        Ensures(a.columns() == 5);
        Ensures(a.rows() == 2);

        std::iota(std::begin(a), std::end(a), 0);

        Ensures(a(0, 0) == 0);
        Ensures(a(1, 0) == 1);
        Ensures(a(2, 0) == 2);
        Ensures(a(3, 0) == 3);
        Ensures(a(4, 0) == 4);
        Ensures(a(0, 1) == 5);
        Ensures(a(1, 1) == 6);
        Ensures(a(2, 1) == 7);
        Ensures(a(3, 1) == 8);
        Ensures(a(4, 1) == 9);

        a.resize(2, 2);

        EnsuresEqual(a.size(), 2 * 2);
        Ensures(a.columns() == 2);
        Ensures(a.rows() == 2);

        Ensures(a(0, 0) == 0);
        Ensures(a(1, 0) == 1);
        Ensures(a(0, 1) == 5);
        Ensures(a(1, 1) == 6);

        a.resize(3, 3);

        EnsuresEqual(a.size(), 3 * 3);
        Ensures(a.columns() == 3);
        Ensures(a.rows() == 3);

        Ensures(a(0, 0) == 0);
        Ensures(a(1, 0) == 1);
        Ensures(a(2, 0) == 0);
        Ensures(a(0, 1) == 5);
        Ensures(a(1, 1) == 6);
        Ensures(a(2, 1) == 0);
        Ensures(a(0, 2) == 0);
        Ensures(a(1, 2) == 0);
        Ensures(a(2, 2) == 0);

        a.resize(4, 4, -1);

        EnsuresEqual(a.size(), 4 * 4);
        Ensures(a.columns() == 4);
        Ensures(a.rows() == 4);

        std::find_if_not(
          std::begin(a), std::end(a), [](int i) { return i == -1; });
    }

    {
        vle::utils::Array<double> c(10, 10);

        EnsuresThrow(c(10, 10), std::exception);
        EnsuresThrow(c(10, 9), std::exception);
        EnsuresThrow(c(9, 10), std::exception);
    }
}

void
test_tokenize()
{
    {
        std::vector<std::string> tok;
        vle::utils::tokenize("c:iiu::e", tok, ":", true);
        Ensures(tok.size() == 3);
        Ensures(tok[0] == "c");
        Ensures(tok[1] == "iiu");
        Ensures(tok[2] == "e");
    }
    {
        std::vector<std::string> tok;
        vle::utils::tokenize(" c iiu  e ", tok, " ", false);
        Ensures(tok.size() == 6);
        Ensures(tok[0] == "");
        Ensures(tok[1] == "c");
        Ensures(tok[2] == "iiu");
        Ensures(tok[3] == "");
        Ensures(tok[4] == "e");
        Ensures(tok[5] == "");
    }
}

int
main()
{
    vle::Init app;

    Ensures(true == true);
    EnsuresEqual(10, 10);
    EnsuresNotEqual(10, 11);

    test_format();
    test_algo();
    test_generator();
    date_time();
    julian_date();
    to_time_function();
    localized_conversion();
    to_scientific_string_function();
    test_format_copy();
    test_array();
    test_tokenize();

    return unit_test::report_errors();
}
